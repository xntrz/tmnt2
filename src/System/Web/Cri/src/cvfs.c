#include "cvfs.h"
#include "ut.h"
#include "err.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>


#define CVFS_DEV_NAME_LEN (12)


typedef struct CVFS
{
    CVFS_VTBL*  vtbl;
    char        devName[CVFS_DEV_NAME_LEN];
} CVFS;


typedef struct CVFS_OBJ
{
    CVFS_VTBL*  vtbl;
    void*       hn;
} CVFS_OBJ;


typedef union CVFS_SIZE_64
{
    struct
    {
        CriSint32 lower;
        CriSint32 upper;
    } parts;
    CriSint64 value;
} CVFS_SIZE_64;


static CriSint32 cvfs_init_cnt = 0;
static char cvfs_defdev[CVFS_PATH_LEN];
static CVFS_OBJ cvfs_obj[80];
static CVFS cvfs_tbl[32];
static cvFsErrFunc cvfs_errfn = CRI_NULL;
static void* cvfs_errobj = CRI_NULL;


static void
toUpperStr(CriChar8* str)
{
    while (*str != '\0')
    {
        if ((*str >= 'a') && (*str <= 'z'))
            *str = (*str - ' ');

        ++str;
    };
};


static void
freeCvFsHn(CVFS_OBJ* obj)
{
    obj->vtbl = NULL;
    obj->hn = NULL;
};


static CVFS_OBJ*
allocCvFsHn(void)
{
    for (CriSint32 i = 0; i < CRI_COUNTOF(cvfs_obj); ++i)
    {
        if (cvfs_obj[i].hn == NULL)
            return &cvfs_obj[i];
    };

    return NULL;
};


static const CriChar8*
correctDeviceName(const CriChar8* devName)
{
    static CriChar8 deviceName[CVFS_DEV_NAME_LEN];
    deviceName[0] = '\0';

    if (strlen(devName) < sizeof(deviceName))
    {
        strcpy(deviceName, devName);
        toUpperStr(deviceName);
    };
    
    return deviceName;
};


static void
getDefDevice(CriChar8* devName)
{
    if (cvfs_defdev[0] == '\0')
    {
        devName[0] = '\0';
        return;
    };

    strncpy(devName, cvfs_defdev, strlen(cvfs_defdev) + 1);
};


static void
getDeviceName(CriChar8* devName, CriChar8* fileName, const CriChar8* path)
{
    if (path != CRI_NULL)
    {
        const CriChar8* devEnd = strchr(path, ':');

        if (devEnd != NULL)
        {
            CriSint32 devNameLen = (CriSint32)(devEnd - path);

            strncpy(devName, path, devNameLen);
            devName[devNameLen] = '\0';

            strcpy(fileName, devEnd + 1);
        }
        else
        {
            devName[0] = '\0';

            strcpy(fileName, path);
        };
    };    
};


static CVFS_VTBL*
getDevice(const CriChar8* devName)
{
    size_t len = strlen(devName);

    for (CriSint32 i = 0; i < CRI_COUNTOF(cvfs_tbl); ++i)
    {
        if (!strncmp(cvfs_tbl[i].devName, devName, len))
            return cvfs_tbl[i].vtbl;            
    };

    return CRI_NULL;
};


static void
delDevice(const CriChar8* devName)
{
    devName = correctDeviceName(devName);

    for (CriSint32 i = 0; i < CRI_COUNTOF(cvfs_tbl); ++i)
    {
        CVFS* cvfs = &cvfs_tbl[i];

        if (!strcmp(cvfs->devName, devName))
        {
            cvfs->vtbl = CRI_NULL;
            cvfs->devName[0] = '\0';

            break;
        };
    };
};


static CVFS_VTBL*
addDevice(const CriChar8* devName, cvFsGetInterface fn)
{
    devName = correctDeviceName(devName);

    if (!getDevice(devName))
    {
        for (CriSint32 i = 0; i < CRI_COUNTOF(cvfs_tbl); ++i)
        {
            CVFS* cvfs = &cvfs_tbl[i];

            if (cvfs->vtbl == CRI_NULL)
            {
                cvfs->vtbl = fn();
                strcpy(cvfs->devName, devName);

                return cvfs->vtbl;
            };
        };
    };

    return CRI_NULL;
};


static CriBool
isNeedDevName(const CriChar8* devName)
{
    CVFS_VTBL* device = getDevice(devName);

    if (device == CRI_NULL)
        return CRI_FALSE;

    if (device->OptFn1 == CRI_NULL)
        return CRI_FALSE;

    return device->OptFn1(CRI_NULL, CVFS_OPTCODE_ISNEEDDEVNAME, 0, 0);
};


static void
addDevName(const CriChar8* devName, CriChar8* fileName)
{
    if (devName == CRI_NULL)
        devName = cvFsGetDefDev();

    if (isNeedDevName(devName))
    {
        CriChar8 tmpbuf[CVFS_PATH_LEN];
        tmpbuf[0] = '\0';

        strcpy(tmpbuf, fileName);
        sprintf(fileName, "%s:%s", devName, tmpbuf);
    };
};


static CVFS_VTBL*
variousProc(CriChar8* devName, CriChar8* fileName, const CriChar8* path)
{
    if (devName[0] == '\0')
    {
        getDefDevice(devName);

        if (devName[0] == '\0')
            return CRI_NULL;
    };

    addDevName(devName, fileName);

    CVFS_VTBL* device = getDevice(devName);
    
    if (device == CRI_NULL)
    {
        getDefDevice(devName);        
        device = getDevice(devName);

        if (device == CRI_NULL)
            return CRI_NULL;

        strcpy(fileName, path);
    };

    return device;
};


static void
cvFsCallUsrErrFn(void* obj, const CriChar8* msg, CriSint32 errcode)
{
    if (cvfs_errfn != CRI_NULL)
        cvfs_errfn(obj, msg, errcode);
};


static void
cvFsError(const CriChar8* msg, ...)
{
    va_list vl;
    va_start(vl, msg);

    CriChar8 msgbuf[2049];
    msgbuf[0] = '\0';

    vsprintf(msgbuf, msg, vl);

    cvFsCallUsrErrFn(cvfs_errobj, msgbuf, 0);

    va_end(vl);
};


void
cvFsInit(void)
{
    if (!cvfs_init_cnt++)
    {
        memset(cvfs_obj, 0, sizeof(cvfs_obj));
        memset(cvfs_tbl, 0, sizeof(cvfs_tbl));

        cvfs_defdev[0] = '\0';        
    };
};


void
cvFsFinish(void)
{
    if (cvfs_init_cnt == 0)
        return;

    if (!--cvfs_init_cnt)
    {
        for (CriSint32 i = 0; i < CRI_COUNTOF(cvfs_obj); ++i)
        {
            CVFS_OBJ* obj = &cvfs_obj[i];

            if (obj->vtbl != CRI_NULL)
            {
                if (obj->vtbl->Close != CRI_NULL)
                {
                    obj->vtbl->Close(obj->hn);
                }
                else
                {
                    cvFsError(CRI_STR("vtbl error"));
                };
            };
        };

        memset(cvfs_obj, 0, sizeof(cvfs_obj));
        memset(cvfs_tbl, 0, sizeof(cvfs_tbl));

        cvfs_defdev[0] = '\0';
    };
};


void
cvFsEntryErrFunc(cvFsErrFunc fn, void* obj)
{
    if (fn != CRI_NULL)
    {
        cvfs_errfn = fn;
        cvfs_errobj = obj;
    }
    else
    {
        cvfs_errfn = CRI_NULL;
        cvfs_errobj = CRI_NULL;
    };
};


void
cvFsAddDev(const CriChar8* deviceName, cvFsGetInterface fn)
{
    if (deviceName == CRI_NULL)
    {
        cvFsError(CRI_STR("'deviceName' is NULL"));
        return;
    };

    if (fn == CRI_NULL)
    {
        cvFsError(CRI_STR("'fn' is NULL"));
        return;
    };

    CVFS_VTBL* device = addDevice(deviceName, fn);

    if (device == CRI_NULL)
    {
        cvFsError(CRI_STR("can not add device: %s"), deviceName);
        return;
    };

    if (device->EntryErrFunc)
        device->EntryErrFunc(cvFsCallUsrErrFn, CRI_NULL);
};


void
cvFsDelDev(const CriChar8* deviceName)
{
    delDevice(deviceName);
};


void
cvFsSetDefDev(const CriChar8* deviceName)
{
    if (deviceName == CRI_NULL)
    {
        cvFsError(CRI_STR("'deviceName' is NULL"));
        return;
    };

    CriSint32 len = (CriSint32)strlen(deviceName);

    if (len > 0)
    {
        deviceName = correctDeviceName(deviceName);

        if (getDevice(deviceName))
            strcpy(cvfs_defdev, deviceName);
        else
            cvFsError(CRI_STR("unknown device: %d"), deviceName);
    }
    else
    {
        cvfs_defdev[0] = '\0';
    };
};


const CriChar8*
cvFsGetDefDev(void)
{
    return cvfs_defdev;
};


void
cvFsExecServer(void)
{
    for (CriSint32 i = 0; i < CRI_COUNTOF(cvfs_tbl); ++i)
    {
        CVFS* cvfs = &cvfs_tbl[i];

        if (cvfs->vtbl != CRI_NULL)
        {
            if (cvfs->vtbl->ExecServer != CRI_NULL)
                cvfs->vtbl->ExecServer();
        };
    };
};


void
cvFsExecIdle(void)
{
    for (CriSint32 i = 0; i < CRI_COUNTOF(cvfs_tbl); ++i)
    {
        CVFS* cvfs = &cvfs_tbl[i];

        if (cvfs->vtbl != CRI_NULL)
        {
            if (cvfs->vtbl->OptFn1 != CRI_NULL)
                cvfs->vtbl->OptFn1(CRI_NULL, CVFS_OPTCODE_EXECIDLE, 0, 0);
        };
    };
};


void*
cvFsOpen(const CriChar8* path, void* param, CVFS_RW mode)
{
    if (path == CRI_NULL)
    {
        cvFsError(CRI_STR("'path' is NULL"));
        return CRI_NULL;
    };

    CriChar8 devName[CVFS_PATH_LEN];
    devName[0] = '\0';

    CriChar8 fileName[CVFS_PATH_LEN];
    fileName[0] = '\0';

    getDeviceName(devName, fileName, path);

    if (fileName[0] == '\0')
    {
        cvFsError(CRI_STR("filename is invalid"));
        return CRI_NULL;
    };

    CVFS_OBJ* obj = allocCvFsHn();

    if (obj == CRI_NULL)
    {
        cvFsError(CRI_STR("can not allocate handle"));
        return CRI_NULL;
    };

    CVFS_VTBL* device = variousProc(devName, fileName, path);

    if (device == CRI_NULL)
    {
        freeCvFsHn(obj);
        cvFsError(CRI_STR("device not found"));
        return CRI_NULL;
    };

    obj->vtbl = device;

    if (device->Open == CRI_NULL)
    {
        freeCvFsHn(obj);
        cvFsError(CRI_STR("vtbl error"));
        return CRI_NULL;
    };

    obj->hn = device->Open(fileName, param, mode);

    if (obj->hn == CRI_NULL)
    {
        freeCvFsHn(obj);
        cvFsError(CRI_STR("can not open file"));
        return CRI_NULL;
    };

    return obj;
};


void
cvFsClose(void* hn)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->Close == CRI_NULL)
    {
        cvFsError(CRI_STR("vtbl error"));
        return;
    };

    obj->vtbl->Close(obj->hn);

    freeCvFsHn(obj);
};


CriSint32
cvFsSeek(void* hn, CriSint32 ofst, CVFS_SEEK mode)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return 0;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->Seek == CRI_NULL)
    {
        cvFsError(CRI_STR("vtbl error"));
        return 0;
    };

    return obj->vtbl->Seek(obj->hn, ofst, mode);
};


CriSint32
cvFsTell(void* hn)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return 0;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->Tell == CRI_NULL)
    {
        cvFsError(CRI_STR("vtbl error"));
        return 0;
    };

    return obj->vtbl->Tell(obj->hn);
};


CriSint32
cvFsReqRd(void* hn, CriSint32 nsct, void* buf)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return 0;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->ReqRd == CRI_NULL)
    {
        cvFsError(CRI_STR("vtbl error"));
        return 0;
    };

    return obj->vtbl->ReqRd(obj->hn, nsct, buf);
};


void
cvFsStopTr(void* hn)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->StopTr == CRI_NULL)
    {
        cvFsError(CRI_STR("vtbl error"));
        return;
    };

    obj->vtbl->StopTr(obj->hn);
};


CriSint32
cvFsGetStat(void* hn)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return CVFS_STAT_ERR;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->GetStat == CRI_NULL)
    {
        cvFsError(CRI_STR("vtbl error"));
        return CVFS_STAT_ERR;
    };

    return obj->vtbl->GetStat(obj->hn);
};


CriSint32
cvFsGetNumTr(void* hn)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return 0;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->GetNumTr == CRI_NULL)
    {
        cvFsError(CRI_STR("vtbl error"));
        return 0;
    };

    return obj->vtbl->GetNumTr(obj->hn);
};


CriSint32
cvFsGetFileSizeByHn(void* hn)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return 0;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->OptFn1 != CRI_NULL)
        return obj->vtbl->OptFn1(obj->hn, CVFS_OPTCODE_GETFILESIZEHANDLE, 0, 0);

    return CRI_MAX_FSIZE_32;
};


CriBool
cvFsIsFileOpened(void* hn)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return CRI_FALSE;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->OptFn1 != CRI_NULL)
        return obj->vtbl->OptFn1(obj->hn, CVFS_OPTCODE_ISFILEOPENED, 0, 0);

    return CRI_TRUE;
};


CriBool
cvFsIsOpenReq(void* hn)
{
    if (hn == CRI_NULL)
    {
        cvFsError(CRI_STR("'hn' is NULL"));
        return CRI_FALSE;
    };

    CVFS_OBJ* obj = hn;

    if (obj->vtbl->OptFn1 != CRI_NULL)
        return obj->vtbl->OptFn1(obj->hn, CVFS_OPTCODE_ISOPENREQ, 0, 0);

    return CRI_FALSE;
};