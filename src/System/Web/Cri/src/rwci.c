#include "rwci.h"
#include "cvfs.h"
#include "ut.h"
#include "err.h"

#include "cri_adxf.h"

#include "rwcore.h"
#include "rtfsyst.h"

#include <memory.h>
#include <string.h>


typedef enum
{
    RWCI_REQTYPE_NONE = 0,
    RWCI_REQTYPE_OPEN,
    RWCI_REQTYPE_CLOSE,
    RWCI_REQTYPE_READ,
    RWCI_REQTYPE_WRITE,
} rwCiReqType;


typedef struct
{
    CriChar8    path[CVFS_PATH_LEN];
    CVFS_RW     rw;
    void*       fhandle;
} rwCiOpenParam;


typedef struct
{
    void* dummy;
} rwCiCloseParam;


typedef union
{
    const void*     write;
    void*           read;
} rwCiIOBuf;


typedef struct
{
    rwCiIOBuf       buf;
    CriSint32       size;
    CriSint32       offset;
    CriBool         cancelrq;
    CriSint32       nsct;
} rwCiIOParam;


typedef struct
{
    CriBool         inuse;
    CriSint32       stat;
    CriSint32       reqtype;
    CriChar8        path[CVFS_PATH_LEN];
    void*           fhandle;
    CriSint32       fsize;
    CriSint32       fnsct;
    CriSint32       skpos;
    CriSint32       ionsct;
    CriSint32       numtr;
    rwCiOpenParam   open;
    rwCiCloseParam  close;
    rwCiIOParam     io;
} rwCiFobj;


static rwCiFobj rwci_fobj[16];
static CriChar8 rwci_root_dir[CVFS_PATH_LEN];


static const CriChar8*
rwci_PathConvert(CriChar8* filename)
{
    CriChar8* ptr = filename;

    while (*ptr)
    {
#ifdef CRI_TARGET_WIN
        if (*ptr == '/')
            *ptr = '\\';
#else /* CRI_TARGET_WIN */
        if (*ptr == '\\')
            *ptr = '/';
#endif /* CRI_TARGET_WIN */
        ++ptr;
    };

    return filename;
};


static const CriChar8*
rwci_PathCorrection(const CriChar8* filename)
{
    static CriChar8 newfilename[256];
    newfilename[0] = '\0';

    if (rwci_root_dir[0] != '\0')
    {
        const CriChar8* filenameStart = strchr(filename, ':');
        if (filenameStart)
            filename = filenameStart + 1;

        strcpy(newfilename, rwci_root_dir);

        if (filename[0] != '/')
            strcat(newfilename, "/");

        strcat(newfilename, filename);

        return rwci_PathConvert(newfilename);
    };

    return filename;
};


static void
rwci_Lock(void)
{
    ;
};


static void
rwci_Unlock(void)
{
    ;
};


static void
rwci_FreeFobj(rwCiFobj* fobj)
{
    memset(fobj, 0, sizeof(*fobj));
};


static rwCiFobj*
rwci_AllocFobj(void)
{
    for (CriSint32 i = 0; i < CRI_COUNTOF(rwci_fobj); ++i)
    {
        if (!rwci_fobj[i].inuse)
            return &rwci_fobj[i];
    };

    return CRI_NULL;
};


static CriBool
rwci_IsOpenReq(rwCiFobj* fobj)
{
    return (fobj->reqtype == RWCI_REQTYPE_OPEN);
};


static CriBool
rwci_IsFileOpened(rwCiFobj* fobj)
{
    return (fobj->fhandle != CRI_NULL);
};


static CriSint32
rwci_GetFileSizeByHandle(rwCiFobj* fobj)
{
    return fobj->fsize;
};


static CriBool
rwci_IsNeedDeviceName(void)
{
    if (rwci_root_dir[0] != '\0')
        return CRI_FALSE;
    
    return CRI_TRUE;
};


static CriBool
rwci_IsSharedFhandle(rwCiFobj* fobj)
{
    for (CriSint32 i = 0; i < CRI_COUNTOF(rwci_fobj); ++i)
    {
        rwCiFobj* other = &rwci_fobj[i];

        if (other == fobj)
            continue;

        if ((other->inuse == CRI_TRUE) &&
            (other->fhandle == fobj->fhandle))
        {
            return CRI_TRUE;
        };
    };

    return CRI_FALSE;
};


static void
rwci_ShareFhandle(const CriChar8* path, CriSint32* fnsct, CriSint32* fsize, void** fhandle)
{
    *fnsct = 0;
    *fsize = 0;
    *fhandle = CRI_NULL;

    for (CriSint32 i = 0; i < CRI_COUNTOF(rwci_fobj); ++i)
    {
        rwCiFobj* fobj = &rwci_fobj[i];

        if (fobj->inuse != CRI_TRUE)
            continue;

        if (fobj->fhandle == CRI_NULL)
            continue;

        if (!strcmp(fobj->path, path))
        {
            *fnsct = fobj->fnsct;
            *fsize = fobj->fsize;
            *fhandle = fobj->fhandle;

            break;
        };
    };
};


static CriBool
rwci_ExecRead(rwCiFobj* fobj)
{
    if (fobj->ionsct == 0)
    {
        RwFseek(fobj->fhandle, fobj->io.offset, SEEK_SET);
        RwFread(fobj->io.buf.read, (size_t)fobj->io.size, (size_t)1, fobj->fhandle);

        fobj->ionsct = fobj->io.nsct;
        fobj->skpos += fobj->ionsct;
    }
    else
    {
        RtFileStatus status = RtFileSystemGetFileStatus(fobj->fhandle);

        if (status == RTFILE_STATUS_READY)
        {
            fobj->numtr = CRI_NSCT_TO_SIZE(fobj->ionsct, ADXF_DEF_SCT_SIZE);

            return CRI_TRUE;
        }
        else if (status == RTFILE_STATUS_ERROR)
        {
            return CRI_TRUE;
        };
    };

    return CRI_FALSE;
};


static CriBool
rwci_ExecClose(rwCiFobj* fobj)
{
    if (!rwci_IsSharedFhandle(fobj))
    {
        if (fobj->fhandle)
        {
            RwFclose(fobj->fhandle);
            fobj->fhandle = CRI_NULL;
        };
    };

    return CRI_TRUE;
};


static CriBool
rwci_ExecOpen(rwCiFobj* fobj)
{
    if (fobj->open.fhandle == CRI_NULL)
    {
        rwci_ShareFhandle(fobj->open.path, &fobj->fnsct, &fobj->fsize, &fobj->fhandle);

        if (fobj->fhandle == CRI_NULL)
        {
            const RwChar* path = rwci_PathCorrection(fobj->open.path);

            fobj->open.fhandle = RwFopen(path, (fobj->open.rw == CVFS_RW_RD ? "rb" : "wb"));

            if (fobj->open.fhandle == CRI_NULL)
            {
                fobj->stat = CVFS_STAT_ERR;
                return CRI_TRUE;
            };
        }
        else
        {
            strcpy(fobj->path, fobj->open.path);
            return CRI_TRUE;
        };
    }
    else
    {
        RtFileStatus status = RtFileSystemGetFileStatus(fobj->open.fhandle);

        if (status == RTFILE_STATUS_READY)
        {
            CriSint32 fsize = (CriSint32)RtFileSystemGetFileSize(fobj->open.fhandle);
            CriSint32 fnsct = (CriSint32)(fsize / ADXF_DEF_SCT_SIZE);

            if (fsize == 0)
            {
                RwFclose(fobj->open.fhandle);
                fobj->open.fhandle = CRI_NULL;

                fobj->stat = CVFS_STAT_ERR;

                return CRI_TRUE;
            };

            if ((fsize % ADXF_DEF_SCT_SIZE) > 0)
                fnsct += 1;

            fobj->fnsct = fnsct;
            fobj->fsize = fsize;
            fobj->fhandle = fobj->open.fhandle;
            strcpy(fobj->path, fobj->open.path);

            return CRI_TRUE;
        }
        else if (status == RTFILE_STATUS_ERROR)
        {
            RwFclose(fobj->open.fhandle);
            fobj->open.fhandle = CRI_NULL;

            fobj->stat = CVFS_STAT_ERR;

            return CRI_TRUE;
        };
    };

    return CRI_FALSE;
};


static void
rwci_ExecOne(rwCiFobj* fobj)
{
    CriBool isComplete = CRI_FALSE;

    switch (fobj->reqtype)
    {
    case RWCI_REQTYPE_OPEN:
        isComplete = rwci_ExecOpen(fobj);
        break;

    case RWCI_REQTYPE_CLOSE:
        isComplete = rwci_ExecClose(fobj);
        break;

    case RWCI_REQTYPE_READ:
        isComplete = rwci_ExecRead(fobj);
        break;

    default:
        break;
    };

    if (isComplete)
    {
        fobj->reqtype = RWCI_REQTYPE_NONE;

        if (fobj->stat == CVFS_STAT_TRANS)
            fobj->stat = CVFS_STAT_COMPLETE;
    };
};


static void
rwci_ExecServer(void)
{
    RtFSManagerSyncAllFiles();

    for (CriSint32 i = 0; i < CRI_COUNTOF(rwci_fobj); ++i)
    {
        if (rwci_fobj[i].inuse)
            rwci_ExecOne(&rwci_fobj[i]);        
    };
};


static void*
rwci_Open(const CriChar8* path, void* dirbuf, CVFS_RW rw)
{
    rwCiFobj* fobj = rwci_AllocFobj();

    if (fobj == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("can not allocate handle"));
        return CRI_NULL;
    };

    memset(fobj, 0, sizeof(*fobj));

    fobj->inuse = CRI_TRUE;

    strcpy(fobj->open.path, path);
    fobj->open.rw = rw;
    fobj->open.fhandle = CRI_NULL;

    fobj->stat = CVFS_STAT_TRANS;
    fobj->reqtype = RWCI_REQTYPE_OPEN;

    return fobj;
};


static void
rwci_Close(void* hn)
{
    rwCiFobj* fobj = hn;

    if (fobj == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("invalid handle"));
        return;
    };

    if (!fobj->inuse)
    {
        ERR_CallErrFunc(CRI_STR("fobj is not in use"));
        return;
    };

    rwci_ExecClose(fobj);
    rwci_FreeFobj(fobj);
};


static CriSint32
rwci_Seek(void* hn, CriSint32 offset, CVFS_SEEK skmode)
{
    rwCiFobj* fobj = hn;

    if (fobj == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("invalid handle"));
        return 0;
    };

    if (!fobj->inuse)
    {
        ERR_CallErrFunc(CRI_STR("fobj is not in use"));
        return 0;
    };

    if (!fobj->fhandle)
    {
        ERR_CallErrFunc(CRI_STR("invalid file handle"));
        return 0;
    };

    CriSint32 skpos = 0;

    switch (skmode)
    {
    case CVFS_SEEK_SET:
        skpos = offset;
        break;

    case CVFS_SEEK_CUR:
        skpos = fobj->skpos + offset;
        break;

    case CVFS_SEEK_END:
        skpos = fobj->fnsct;
        break;

    default:
        ERR_CallErrFunc(CRI_STR("'skmode' is invalid"));
        break;
    };

    if (skpos >= fobj->fnsct)
        skpos = fobj->fnsct;

    fobj->skpos = (skpos <= 0 ? 0 : skpos);

    return fobj->skpos;
};


static CriSint32
rwci_Tell(void* hn)
{
    rwCiFobj* fobj = hn;

    if (fobj == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("invalid handle"));
        return 0;
    };

    if (!fobj->inuse)
    {
        ERR_CallErrFunc(CRI_STR("fobj is not in use"));
        return 0;
    };

    if (!fobj->fhandle)
    {
        ERR_CallErrFunc(CRI_STR("invalid file handle"));
        return 0;
    };

    return fobj->skpos;
};


static CriSint32
rwci_ReqRd(void* hn, CriSint32 nsct, void* buf)
{
    rwCiFobj* fobj = hn;

    if (fobj == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("invalid handle"));
        return 0;
    };

    if (!fobj->inuse)
    {
        ERR_CallErrFunc(CRI_STR("fobj is not in use"));
        return 0;
    };

    if (!fobj->fhandle)
    {
        ERR_CallErrFunc(CRI_STR("invalid file handle"));
        return 0;
    };

    if (fobj->reqtype != RWCI_REQTYPE_NONE)
    {
        ERR_CallErrFunc(CRI_STR("file is busy"));
        return 0;
    };

    if (nsct >= (fobj->fnsct - fobj->skpos))
        nsct = (fobj->fnsct - fobj->skpos);

    if (nsct <= 0)
    {
        fobj->ionsct = 0;
        fobj->numtr = 0;
        fobj->stat = CVFS_STAT_COMPLETE;

        return 0;
    };

    fobj->io.buf.read = buf;
    fobj->io.size = CRI_NSCT_TO_SIZE(nsct, ADXF_DEF_SCT_SIZE);
    fobj->io.offset = CRI_NSCT_TO_SIZE(fobj->skpos, ADXF_DEF_SCT_SIZE);
    fobj->io.cancelrq = CRI_FALSE;
    fobj->io.nsct = nsct;
    
    fobj->ionsct = 0;
    fobj->stat = CVFS_STAT_TRANS;
    fobj->reqtype = RWCI_REQTYPE_READ;

    return 0;
};


static void
rwci_StopTr(void* hn)
{
    rwCiFobj* fobj = hn;

    if (fobj == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("invalid handle"));
        return;
    };

    if (!fobj->inuse)
    {
        ERR_CallErrFunc(CRI_STR("fobj is not in use"));
        return;
    };

    if (!fobj->fhandle)
    {
        ERR_CallErrFunc(CRI_STR("invalid file handle"));
        return;
    };

    if (fobj->stat == CVFS_STAT_TRANS)
    {
        RwFAsyncCancel(fobj->fhandle);

        fobj->reqtype = RWCI_REQTYPE_NONE;
        fobj->stat = CVFS_STAT_IDLE;
    };
};


static CriSint32
rwci_GetStat(void* hn)
{
    rwCiFobj* fobj = hn;

    if (fobj == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("invalid handle"));
        return CVFS_STAT_IDLE;
    };

    return fobj->stat;
};


static CriSint32
rwci_GetNumTr(void* hn)
{
    rwCiFobj* fobj = hn;

    if (fobj == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("invalid handle"));
        return 0;
    };

    return fobj->numtr;
};


static CriSint32
rwci_OptFunc(void* data, CriSint32 optCode, CriSint32 param1, CriSint32 param2)
{
    switch (optCode)
    {
    case CVFS_OPTCODE_ISNEEDDEVNAME:
        return rwci_IsNeedDeviceName();

    case CVFS_OPTCODE_GETFILESIZEHANDLE:
        return rwci_GetFileSizeByHandle(data);

    case CVFS_OPTCODE_ISFILEOPENED:
        return rwci_IsFileOpened(data);

    case CVFS_OPTCODE_ISOPENREQ:
        return rwci_IsOpenReq(data);

    default:
        break;
    };

    return 0;
};


static void
rwCiExecServer(void)
{
    rwci_Lock();
    rwci_ExecServer();
    rwci_Unlock();
};


static void*
rwCiOpen(const CriChar8* path, void* dirbuf, CVFS_RW rw)
{
    rwci_Lock();
    void* result = rwci_Open(path, dirbuf, rw);
    rwci_Unlock();

    return result;
};


static void
rwCiClose(void* hn)
{
    rwci_Lock();
    rwci_Close(hn);
    rwci_Unlock();
};


static CriSint32
rwCiSeek(void* hn, CriSint32 offset, CVFS_SEEK skmode)
{
    rwci_Lock();
    CriSint32 result = rwci_Seek(hn, offset, skmode);
    rwci_Unlock();

    return result;
};


static CriSint32
rwCiTell(void* hn)
{
    rwci_Lock();
    CriSint32 result = rwci_Tell(hn);
    rwci_Unlock();

    return result;
};


static CriSint32
rwCiReqRd(void* hn, CriSint32 nsct, void* buf)
{
    rwci_Lock();
    CriSint32 result = rwci_ReqRd(hn, nsct, buf);
    rwci_Unlock();

    return result;
};


static void
rwCiStopTr(void* hn)
{
    rwci_Lock();
    rwci_StopTr(hn);
    rwci_Unlock();
};


static CriSint32
rwCiGetStat(void* hn)
{
    rwci_Lock();
    CriSint32 result = rwci_GetStat(hn);
    rwci_Unlock();

    return result;
};


static CriSint32
rwCiGetNumTr(void* hn)
{
    rwci_Lock();
    CriSint32 result = rwci_GetNumTr(hn);
    rwci_Unlock();

    return result;
};


static CriSint32
rwCiOptFunc(void* data, CriSint32 optCode, CriSint32 param1, CriSint32 param2)
{
    rwci_Lock();
    CriSint32 result = rwci_OptFunc(data, optCode, param1, param2);
    rwci_Unlock();

    return result;
};


void
rwCiInit(void)
{
    memset(rwci_fobj, 0, sizeof(rwci_fobj));
    rwci_root_dir[0] = '\0';
};


void
rwCiFinish(void)
{
    ;
};


void
rwCiSetRootDir(const CriChar8* rd)
{
    strcpy(rwci_root_dir, rd);
};


CVFS_VTBL* 
rwCiGetInterface(void)
{
    static CVFS_VTBL rwci_vtbl =
    {
        .ExecServer = rwCiExecServer,
        .EntryErrFunc = CRI_NULL,
        .GetFileSize = CRI_NULL,
        .GetFreeSize = CRI_NULL,
        .Open = rwCiOpen,
        .Close = rwCiClose,
        .Seek = rwCiSeek,
        .Tell = rwCiTell,
        .ReqRd = rwCiReqRd,
        .ReqWr = CRI_NULL,
        .StopTr = rwCiStopTr,
        .GetStat = rwCiGetStat,
        .GetSctLen = CRI_NULL,
        .SetSctLen = CRI_NULL,
        .GetNumTr = rwCiGetNumTr,
        .ChangeDir = CRI_NULL,
        .IsExistFile = CRI_NULL,
        .GetNumFiles = CRI_NULL,
        .LoadDirInfo = CRI_NULL,
        .GetMaxByteRate = CRI_NULL,
        .MakeDir = CRI_NULL,
        .RemoveDir = CRI_NULL,
        .DeleteFile = CRI_NULL,
        .GetFileSizeEx = CRI_NULL,
        .OptFn1 = rwCiOptFunc,
        .OptFn2 = CRI_NULL,
        .GetNumTr64 = CRI_NULL,
        .IsAvailableRetry = CRI_NULL,
    };

    return &rwci_vtbl;
};