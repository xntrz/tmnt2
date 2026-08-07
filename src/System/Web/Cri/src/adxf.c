#include "adxf.h"
#include "cvfs.h"
#include "ut.h"
#include "err.h"
#include "adxcrs.h"
#include "adxt.h"

#include <string.h>
#include <memory.h>
#include <limits.h>


#define ADXF_MAX_FSIZE          (0x7FFFFFFF) // 2GB
#define ADXF_MAX_FNSCT          (CRI_SIZE_TO_NSCT(ADXF_MAX_FSIZE, ADXF_DEF_SCT_SIZE))


//
//  Since this is simple emulation we skip STM & SJ subsystems
//  so rename some of ADX_FS struct vars names for our tasks
//
#define fhandle stm


typedef struct
{
    CriChar8  magic[4];
    CriSint32 nfile;
} AFS_HEADER;


typedef struct
{
    CriSint32 ofst;
    CriSint32 size;
} AFS_FINFO;


typedef struct
{
    AFS_HEADER header;
    AFS_FINFO finfo[];
} AFS_INFO;


static ADX_FS adxf_obj[ADXF_OBJ_MAX];
static ADXF_PTINFO* adxf_ptinfo[ADXF_PART_MAX];
static ADXF adxf_ldptnw_hn = CRI_NULL;
static CriSint32 adxf_ldptnw_ptid = -1;
static CriSint32 adxf_ldptnw_last_stat = ADXF_STAT_STOP;
static CriSint32 adxf_flno = 0;
static CriChar8* adxf_ldpt_buf = CRI_NULL;
static CriSint32 adxf_ldpt_rdsct = 0;
static CriSint32 adxf_init_cnt = 0;
static CriChar8 adxf_ldpt_work[ADXF_DEF_SCT_SIZE];


static void
adxf_Enter(void)
{
    ADXCRS_Enter();
};


static void
adxf_Leave(void)
{
    ADXCRS_Leave();
};


static CriBool
adxf_IsOpened(ADX_FS * adxf)
{
    if (adxf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'adxf' is NULL"));
        return CRI_FALSE;
    };

    return cvFsIsFileOpened(adxf->fhandle);
};


static CriBool
adxf_WaitUntilFileOpen(ADX_FS* adxf)
{
    while (!cvFsIsFileOpened(adxf->fhandle))
    {
        if (!cvFsIsOpenReq(adxf->fhandle))
            break;

        ADXT_ExecFsSvr();
        UT_OsIdle();
    };
    
    return adxf_IsOpened(adxf);
};


static CriChar8*
adxf_GetFnameFromPt(CriSint32 ptid)
{
    const ADXF_PTINFO* ptinfo = adxf_ptinfo[ptid];

    if (ptinfo == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("Partition info is NULL"));
        return CRI_NULL;
    };

    return (CriChar8*)(ptinfo->fname);
};


static CriSint32
adxf_ChkPrmPt(CriSint32 ptid, void* ptinfo)
{
    if ((ptid < 0) || (ptid >= ADXF_PART_MAX))
    {
        ERR_CallErrFunc(CRI_STR("Partition ID is out of range"));
        return ADXF_ERR_PRM;
    };

    if (ptinfo == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'ptinfo' is NULL"));
        return ADXF_ERR_PRM;
    };

    return ADXF_ERR_OK;
};


static CriSint32
adxf_ChkPrmGfr(CriSint32 ptid, CriSint32 flid)
{
    if ((ptid < 0) || (ptid >= ADXF_PART_MAX))
    {
        ERR_CallErrFunc(CRI_STR("Partition ID is out of range"));
        return ADXF_ERR_PRM;
    };

    const ADXF_PTINFO* ptinfo = adxf_ptinfo[ptid];

    if (ptinfo == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("Partition info is NULL"));
        return ADXF_ERR_PRM;
    };

    if (ptinfo->nfile == 0)
    {
        ERR_CallErrFunc(CRI_STR("Load of partition is not completed"));
        return ADXF_ERR_PRM;
    };

    if ((flid < 0) && (flid >= ptinfo->nfile))
    {
        ERR_CallErrFunc(CRI_STR("File ID is out of range: %d/%d"), flid, ptinfo->nfile);
        return ADXF_ERR_PRM;
    };

    if (!((ptinfo->fmg_type == ADXF_FMG_TYPE_LONG) ||
          (ptinfo->fmg_type == ADXF_FMG_TYPE_SHORT)))
    {
        ERR_CallErrFunc(CRI_STR("FMG type is invalid: %d"), ptinfo->fmg_type);
        return ADXF_ERR_PRM;
    };

    return ADXF_ERR_OK;
};


static CriSint32
adxf_GetFnameRangeEx(CriSint32 ptid,
                     CriSint32 flid,
                     CriChar8* fname,
                     void** dir,
                     CriSint32* ofst,
                     CriSint32* fnsct,
                     CriSint32* fsize)
{
    CriSint32 err = adxf_ChkPrmGfr(ptid, flid);

    if (err == ADXF_ERR_OK)
    {
        const ADXF_PTINFO* ptinfo = adxf_ptinfo[ptid];
        CriSint32 sctofs = 0;

        if (ptinfo->fmg_type == ADXF_FMG_TYPE_LONG)
        {
            ERR_NOIMPL();
        }
        else if (ptinfo->fmg_type == ADXF_FMG_TYPE_SHORT)
        {
            CriUint16* tbl = ((CriUint16*)&ptinfo->top) + 0;

            sctofs = (CriSint32)((CriUint16)(ptinfo->top));

            for (CriSint32 i = 0; i < flid; ++i)
                sctofs += tbl[i + 1];

            *fnsct = tbl[flid + 1];
            *fsize = CRI_NSCT_TO_SIZE(tbl[flid + 1], ADXF_DEF_SCT_SIZE);
        };

        if (fname != CRI_NULL)
            strncpy(fname, (const CriChar8*)ptinfo->fname, sizeof(ptinfo->fname));

        *dir = ptinfo->curdir;
        *ofst = ptinfo->ofst + sctofs;
    }
    else
    {
        if (fname != CRI_NULL)
            fname[0] = '\0';

        *dir = CRI_NULL;
        *ofst = -1;
        *fnsct = -1;
        *fsize = -1;
    };

    return err;
};


static void
adxf_FreeAdxFs(ADX_FS* obj)
{
    memset(obj, 0, sizeof(*obj));
};


static ADX_FS*
adxf_AllocAdxFs(void)
{
    for (CriSint32 i = 0; i < CRI_COUNTOF(adxf_obj); ++i)
    {
        if (!adxf_obj[i].used)
            return &adxf_obj[i];
    };

    return CRI_NULL;
};


static void
adxf_DestroyAdxFs(ADX_FS* obj)
{
    adxf_FreeAdxFs(obj);
};


static ADX_FS*
adxf_CreateAdxFs(void)
{
    ADX_FS* adxf = adxf_AllocAdxFs();

    if (adxf != CRI_NULL)
    {
        adxf->rdstpos = 0;
        adxf->rqsct = 0;
        adxf->rdsct = 0;
        adxf->sjflag = CRI_FALSE;
        adxf->sj = 0;
        adxf->stopnw_flg = CRI_FALSE;
        adxf->stat = ADXF_STAT_STOP;
        adxf->rqrdsct = ADXF_DEF_REQ_RD_SCT;
        adxf->used = CRI_TRUE;
    }
    else
    {
        ERR_CallErrFunc(CRI_STR("not enough ADXF handle"));        
    };

    return adxf;
};


static CriSint32
adxf_GetStat(ADX_FS* adxf)
{
    if (adxf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'adxf' is NULL"));
        return ADXF_ERR_PRM;
    };

    return adxf->stat;
};


static CriSint32
adxf_GetFsizeSct(ADX_FS* adxf)
{
    if (adxf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'adxf' is NULL"));
        return ADXF_ERR_PRM;
    };

    if (adxf->fnsct == ADXF_MAX_FSIZE)
    {
        adxf_WaitUntilFileOpen(adxf);

        CriSint32 fsize = cvFsGetFileSizeByHn(adxf->fhandle);

        if (fsize > 0)
            adxf->fnsct = CRI_SIZE_TO_NSCT(fsize, ADXF_DEF_SCT_SIZE);

        if (adxf->fnsct == ADXF_MAX_FSIZE)
            return ADXF_ERR_FATAL;
    };

    return adxf->fnsct;
};


static CriSint32
adxf_GetFsizeByte(ADX_FS* adxf)
{
    if (adxf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'adxf' is NULL"));
        return ADXF_ERR_PRM;
    };

    CriSint32 nsct = adxf_GetFsizeSct(adxf);

    return CRI_NSCT_TO_SIZE(nsct, ADXF_DEF_SCT_SIZE);
};


static CriSint32
adxf_SetFileInfoRangeNw(ADX_FS* adxf, const CriChar8* fname,
                        void* dir, CriSint32 ofst, CriSint32 fnsct)
{
    adxf->fnsct = fnsct;
    adxf->ofst = ofst;
    adxf->dir = dir;
    adxf->skpos = 0;

    adxf->fhandle = cvFsOpen(fname, dir, CVFS_RW_RD);

    if (adxf->fhandle == CRI_NULL)
        return ADXF_ERR_FATAL;

    return ADXF_ERR_OK;
};


static CriSint32
adxf_SetAfsFileInfoNw(ADX_FS* adxf, CriSint32 ptid, CriSint32 flid)
{
    const CriChar8* fname = adxf_GetFnameFromPt(ptid);
    void* dir = CRI_NULL;
    CriSint32 ofst = -1;
    CriSint32 fnsct = -1;
    CriSint32 fsize = -1;

    if (adxf_GetFnameRangeEx(ptid, flid, CRI_NULL, &dir, &ofst, &fnsct, &fsize) < 0)
        return ADXF_ERR_PRM;

    adxf->ofst = ofst;
    adxf->dir = dir;
    adxf->skpos = 0;
    adxf->fnsct = fnsct;

    adxf->fhandle = cvFsOpen(fname, dir, CVFS_RW_RD);

    if (adxf->fhandle == CRI_NULL)
        return ADXF_ERR_FATAL;

    return ADXF_ERR_OK;
};


static CriSint32
adxf_SetFileInfoNw(ADX_FS* adxf, const CriChar8* fname, void* atr)
{
    if (fname == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'fname' is NULL"));
        return ADXF_ERR_FATAL;
    };

    adxf->dir = atr;
    adxf->skpos = 0;
    adxf->fnsct = ADXF_MAX_FSIZE;

    adxf->fhandle = cvFsOpen(fname, atr, CVFS_RW_RD);

    if (adxf->fhandle == CRI_NULL)
        return ADXF_ERR_FATAL;

    return ADXF_ERR_OK;
};


static CriSint32
adxf_ReadNw32(ADX_FS* adxf, CriSint32 nsct, void* buf)
{
    if (nsct <= 0)
    {
        ERR_CallErrFunc(CRI_STR("'nsct' is negative or zero"));
        return ADXF_ERR_PRM;
    };

    if (nsct >= ADXF_MAX_FNSCT)
    {
        ERR_CallErrFunc(CRI_STR("'nsct' is greater or equal than 2GB"));
        return ADXF_ERR_PRM;
    };

    if (buf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'buf' is NULL"));
        return ADXF_ERR_PRM;
    };

    if (adxf->stat == ADXF_STAT_READING)
    {
        return ADXF_ERR_OK;
    };

    if (!adxf_IsOpened(adxf))
    {
        ERR_CallErrFunc(CRI_STR("file is not opened yet"));
        return ADXF_ERR_FATAL;
    };

    CriSint32 rqsct = (adxf->fnsct - adxf->skpos);

    if (nsct < rqsct)
        rqsct = nsct;
    
    adxf->rdstpos = (adxf->skpos + adxf->ofst);
    adxf->rqsct = rqsct;
    adxf->rdsct = 0;

    if (adxf->rqsct)
    {
        adxf->buf = buf;
        adxf->bsize = CRI_NSCT_TO_SIZE(nsct, ADXF_DEF_SCT_SIZE);
        adxf->stat = ADXF_STAT_READING;
        adxf->stopnw_flg = CRI_FALSE;

        cvFsSeek(adxf->fhandle, adxf->rdstpos, CVFS_SEEK_SET);
        cvFsReqRd(adxf->fhandle, rqsct, adxf->buf);
    }
    else
    {
        adxf->stat = ADXF_STAT_READEND;
    };

    return rqsct;
};


static CriSint32
adxf_ReadNw(ADX_FS* adxf, CriSint32 nsct, void* buf)
{
    return adxf_ReadNw32(adxf, nsct, buf);
};


static CriSint32
adxf_StopNw(ADX_FS* adxf)
{
    if (adxf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'adxf' is NULL"));
        return ADXF_ERR_PRM;
    };

    if (adxf->stat == ADXF_STAT_READEND)
    {
        adxf->stat = ADXF_STAT_STOP;
        return adxf->skpos;
    };

    if (adxf->stat != ADXF_STAT_STOP)
    {
        if (adxf->fhandle == CRI_NULL)
        {
            ERR_CallErrFunc(CRI_STR("'fhandle' is NULL"));
            return ADXF_ERR_FATAL;
        };

        adxf->stopnw_flg = CRI_TRUE;
    };

    return adxf->skpos;
};


static CriSint32
adxf_Stop(ADX_FS* adxf)
{
    if (adxf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'adxf' is NULL"));
        return ADXF_ERR_PRM;
    };

    if (adxf->stat == ADXF_STAT_READEND)
    {
        adxf->stat = ADXF_STAT_STOP;
        return adxf->skpos;
    };

    if (adxf->stat != ADXF_STAT_STOP)
    {
        if (adxf->fhandle == CRI_NULL)
        {
            ERR_CallErrFunc(CRI_STR("'fhandle' is NULL"));
            return ADXF_ERR_FATAL;
        };

        cvFsStopTr(adxf->fhandle);

        adxf->rdsct = (cvFsTell(adxf->fhandle) - adxf->skpos);
        adxf->stat = ADXF_STAT_STOP;
    };

    return adxf->skpos;
};


static CriSint32
adxf_Tell(ADX_FS* adxf)
{
    if (adxf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'adxf' is NULL"));
        return ADXF_ERR_PRM;
    };

    return adxf->skpos;
};


static CriSint32
adxf_Seek(ADX_FS* adxf, CriSint32 pos, CriSint32 type)
{
    if (adxf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'adxf' is NULL"));
        return ADXF_ERR_PRM;
    };

    if (adxf->fnsct == ADXF_MAX_FSIZE)
        adxf->fnsct = adxf_GetFsizeSct(adxf);

    if (adxf->stat == ADXF_STAT_READING)
        adxf_Stop(adxf);

    if (type == ADXF_SEEK_SET)
    {
        adxf->skpos = (pos);
    }
    else if (type == ADXF_SEEK_CUR)
    {
        adxf->skpos = (adxf->skpos + pos);
    }
    else if (type == ADXF_SEEK_END)
    {
        adxf->skpos = (adxf->fnsct + pos);
    }
    else
    {
        ERR_CallErrFunc(CRI_STR("'type' is illigal"));
        return ADXF_ERR_PRM;
    };

    if (adxf->skpos >= 0)
    {
        if (adxf->skpos > adxf->fnsct)
            adxf->skpos = adxf->fnsct;
    }
    else
    {
        adxf->skpos = 0;
    };

    return adxf->skpos;
};


static void
adxf_Close(ADX_FS* adxf)
{
    if (adxf != CRI_NULL)
    {
        adxf_WaitUntilFileOpen(adxf);

        if (adxf->stat == ADXF_STAT_READING)
            adxf_Stop(adxf);

        if (adxf->fhandle)
        {
            cvFsClose(adxf->fhandle);
            adxf->fhandle = CRI_NULL;
        };

        adxf_DestroyAdxFs(adxf);
    };
};


static void
adxf_CloseAll(void)
{
    for (CriSint32 i = 0; i < CRI_COUNTOF(adxf_obj); ++i)
    {
        if (adxf_obj[i].used == CRI_TRUE)
            adxf_Close(&adxf_obj[i]);
    };
};


static ADX_FS*
adxf_OpenRangeNw(const CriChar8* fname, void* dir, CriSint32 ofst, CriSint32 fnsct)
{
    if (fname == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'fname' is NULL"));
        return CRI_NULL;
    };

    ADX_FS* adxf = adxf_CreateAdxFs();

    if (adxf != CRI_NULL)
        adxf_SetFileInfoRangeNw(adxf, fname, dir, ofst, fnsct);

    return adxf;
};


static ADX_FS*
adxf_OpenAfsNw(CriSint32 ptid, CriSint32 flid)
{
    ADX_FS* adxf = adxf_CreateAdxFs();

    if (adxf != CRI_NULL)
        adxf_SetAfsFileInfoNw(adxf, ptid, flid);

    return adxf;
};


static ADX_FS*
adxf_OpenNw(const CriChar8* fname, void* atr)
{
    if (fname == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'fname' is NULL"));
        return CRI_NULL;
    };

    ADX_FS* adxf = adxf_CreateAdxFs();

    if (adxf != CRI_NULL)
        adxf_SetFileInfoNw(adxf, fname, atr);

    return adxf;
};


static CriSint32
adxf_GetNumFilesFromAfs(void* buf)
{
    const AFS_INFO* afsinfo = buf;

    CriBool isAFS = ((afsinfo->header.magic[0] == 'A') &&
                     (afsinfo->header.magic[1] == 'F') &&
                     (afsinfo->header.magic[2] == 'S') &&
                     (afsinfo->header.magic[3] == '\0'));

    if (isAFS)
        return afsinfo->header.nfile;

    return ADXF_ERR_AFS_FILE;
};


static CriSint32
adxf_GetFileSizeFromPt(CriSint32 ptid, CriSint32 flid)
{
    void* dir = CRI_NULL;
    CriSint32 ofst = -1;
    CriSint32 fnsct = -1;
    CriSint32 fsize = -1;

    adxf_GetFnameRangeEx(ptid, flid, CRI_NULL, &dir, &ofst, &fnsct, &fsize);

    return fsize;
};


static CriSint32
adxf_GetNumFilesFromPt(CriSint32 ptid)
{
    return adxf_ptinfo[ptid]->nfile;
};


static CriSint32
adxf_GetPtInfoSize(CriSint32 ptid)
{
    return adxf_ptinfo[ptid]->size;
};


static CriSint32
adxf_CloseLdptnwHn(void)
{
    if (adxf_ldptnw_hn != CRI_NULL)
    {
        adxf_Close(adxf_ldptnw_hn);
        adxf_ldptnw_hn = CRI_NULL;
    };

    adxf_flno = 0;
    adxf_ldpt_rdsct = 0;

    return ADXF_ERR_OK;
};


static void
adxf_StopPtLd(void)
{
    if (adxf_ldptnw_hn && (adxf_ldptnw_ptid >= 0))
    {
        if (adxf_GetStat(adxf_ldptnw_hn) != ADXF_STAT_STOP)
            adxf_Stop(adxf_ldptnw_hn);

        adxf_ldptnw_last_stat = ADXF_STAT_STOP;
        
        adxf_CloseLdptnwHn();
    };
};


static CriSint32
adxf_GetPtStat(CriSint32 ptid)
{
    if (adxf_ldptnw_ptid != ptid)
    {
        ERR_CallErrFunc(CRI_STR("illigal parameter 'ptid'"));
        return ADXF_ERR_PRM;
    };

    if (adxf_ldptnw_ptid != -1)
    {
        if (adxf_ldptnw_hn == CRI_NULL)
            return adxf_ldptnw_last_stat;
    };

    CriSint32 stat = adxf_GetStat(adxf_ldptnw_hn);

    if (stat == ADXF_STAT_ERROR)
    {
        adxf_ldptnw_last_stat = stat;
        return adxf_ldptnw_last_stat;
    };

    if (!ADXF_IsOpened(adxf_ldptnw_hn))
        return adxf_ldptnw_last_stat;

    if (stat == ADXF_STAT_STOP)
    {
        if (adxf_ReadNw(adxf_ldptnw_hn, adxf_ldpt_rdsct, adxf_ldpt_buf) < 0)
        {
            adxf_ldptnw_last_stat = ADXF_STAT_ERROR;
            adxf_CloseLdptnwHn();
        };

        return adxf_ldptnw_last_stat;
    };

    adxf_ldptnw_last_stat = stat;

    if (adxf_ldptnw_last_stat != ADXF_STAT_READEND)
        return adxf_ldptnw_last_stat;

    ADXF_PTINFO* ptinfo = adxf_ptinfo[ptid];
    const AFS_FINFO* finfo = CRI_NULL;

    if (ptinfo->nfile > 0)
    {
        finfo = (const AFS_FINFO*)adxf_ldpt_buf;
    }
    else
    {
        const AFS_INFO* afsinfo = (const AFS_INFO*)adxf_ldpt_buf;

        CriBool isAFS = ((afsinfo->header.magic[0] == 'A') &&
                         (afsinfo->header.magic[1] == 'F') &&
                         (afsinfo->header.magic[2] == 'S') &&
                         (afsinfo->header.magic[3] == '\0'));

        if (!isAFS)
        {
            ERR_CallErrFunc(CRI_STR("Unacceptable format(not AFS)"));

            adxf_ldptnw_last_stat = ADXF_STAT_ERROR;
            adxf_CloseLdptnwHn();

            return adxf_ldptnw_last_stat;
        };

        if (afsinfo->header.nfile > ADXF_FILE_MAX)
        {
            ERR_CallErrFunc(CRI_STR("Unacceptable number of file"));

            adxf_ldptnw_last_stat = ADXF_STAT_ERROR;
            adxf_CloseLdptnwHn();

            return adxf_ldptnw_last_stat;
        };

        ptinfo->nentry = (CriUint16)afsinfo->header.nfile;
        ptinfo->nfile = afsinfo->header.nfile;

        if (ptinfo->fmg_type == ADXF_FMG_TYPE_LONG)
        {
            ptinfo->size = ADXF_CALC_PTINFO_FMGL_REAL_SIZE(ptinfo->nfile);

            *((CriUint32*)(&ptinfo->top)) =
                (CriUint32)CRI_SIZE_TO_NSCT(afsinfo->finfo[0].ofst, ADXF_DEF_SCT_SIZE);
        }
        else if (ptinfo->fmg_type == ADXF_FMG_TYPE_SHORT)
        {
            ptinfo->size = ADXF_CALC_PTINFO_REAL_SIZE(ptinfo->nfile);

            *((CriUint16*)(&ptinfo->top)) =
                (CriUint16)CRI_SIZE_TO_NSCT(afsinfo->finfo[0].ofst, ADXF_DEF_SCT_SIZE);
        };

        finfo = (const AFS_FINFO*)afsinfo->finfo;
    };

    CriSint32 i = 0;

    while (CRI_TRUE)
    {
        if (ptinfo->fmg_type == ADXF_FMG_TYPE_LONG)
        {
            ERR_NOIMPL();
        }
        else if (ptinfo->fmg_type == ADXF_FMG_TYPE_SHORT)
        {
            CriSint32 nsct = CRI_SIZE_TO_NSCT(finfo[i++].size, ADXF_DEF_SCT_SIZE);

            if ((nsct & 0xFFFF0000) != 0)
            {
                ERR_CallErrFunc(CRI_STR("AFS file has 128MB or more of inside file"));

                adxf_ldptnw_last_stat = ADXF_STAT_ERROR;
                adxf_CloseLdptnwHn();

                return adxf_ldptnw_last_stat;
            };

            CriUint16* tbl = ((CriUint16*)&ptinfo->top) + 1;

            tbl[adxf_flno++] = (CriUint16)nsct;
        };

        if (adxf_flno >= ptinfo->nfile)
        {
            adxf_ldptnw_last_stat = ADXF_STAT_READEND;
            adxf_CloseLdptnwHn();

            break;
        };

        const CriSint32 nfilePerSector =
            CRI_NSCT_TO_SIZE(adxf_ldpt_rdsct, ADXF_DEF_SCT_SIZE) / sizeof(AFS_FINFO);

        if (i >= nfilePerSector)
        {
            if (adxf_ReadNw(adxf_ldptnw_hn, adxf_ldpt_rdsct, adxf_ldpt_buf) < 0)
            {
                adxf_ldptnw_last_stat = ADXF_STAT_ERROR;
                adxf_CloseLdptnwHn();

                return adxf_ldptnw_last_stat;
            };

            adxf_ldptnw_last_stat = adxf_GetStat(adxf_ldptnw_hn);

            break;
        };
    };

    return adxf_ldptnw_last_stat;
};


static CriSint32
adxf_LoadPtBothNw(CriSint32       set_ptid,
                  CriSint32       rd_ptid,
                  CriSint32       rd_flid,
                  const CriChar8* fname,
                  void*           dir,
                  CriSint32       ofst,
                  CriSint32       fnsct,
                  void*           ptinfo,
                  void*           tmpbuf,
                  CriSint32       tbsize,
                  CriSint32       fmgtype)
{
    if (adxf_ldptnw_last_stat == ADXF_STAT_READING)
    {
        ERR_CallErrFunc(CRI_STR("It is impossible to load AFS files in parallel"));
        return ADXF_ERR_FATAL;
    };

    if (adxf_ldptnw_hn != CRI_NULL)
    {
        adxf_Close(adxf_ldptnw_hn);
        adxf_ldptnw_hn = CRI_NULL;
    };

    adxf_ldptnw_ptid = -1;
    adxf_ldptnw_last_stat = ADXF_STAT_STOP;

    if (tmpbuf == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("'tmpbuf' is null"));
        adxf_ldptnw_last_stat = ADXF_STAT_ERROR;

        return ADXF_ERR_PRM;
    };

    if (tbsize <= 0)
    {
        ERR_CallErrFunc(CRI_STR("'tbsize' <= 0"));
        adxf_ldptnw_last_stat = ADXF_STAT_ERROR;

        return ADXF_ERR_PRM;
    };

    CriSint32 crierr = adxf_ChkPrmPt(set_ptid, ptinfo);

    if (crierr < 0)
    {
        adxf_ldptnw_last_stat = ADXF_STAT_ERROR;
        return crierr;
    };

    if (adxf_ldptnw_ptid >= 0)
    {
        ERR_CallErrFunc(CRI_STR("multi-load partition"));
        adxf_ldptnw_last_stat = ADXF_STAT_ERROR;

        return ADXF_ERR_FATAL;
    };

    ADXF_PTINFO* adxfptinfo = ptinfo;

    memset(adxfptinfo, 0, sizeof(*adxfptinfo));

    adxf_ptinfo[set_ptid] = adxfptinfo;
    adxfptinfo->next = CRI_NULL;

    adxf_ldptnw_last_stat = ADXF_STAT_READING;
    adxf_ldptnw_ptid = set_ptid;
    adxf_flno = 0;

    if (fname != CRI_NULL)
    {
        strcpy((CriChar8*)adxfptinfo->fname, fname);

        adxf_ldptnw_hn = adxf_OpenRangeNw((const CriChar8*)adxfptinfo->fname, dir, ofst, fnsct);

        if (adxf_ldptnw_hn != CRI_NULL)
            adxfptinfo->ofst = ofst;
    }
    else
    {
        adxf_ldptnw_hn = adxf_OpenAfsNw(rd_ptid, rd_flid);

        if (adxf_ldptnw_hn != CRI_NULL)
        {
            CriSint32 fsize = -1;

            crierr = adxf_GetFnameRangeEx(rd_ptid, rd_flid, (CriChar8*)adxfptinfo->fname, &dir, &ofst, &fnsct, &fsize);

            if (crierr < 0)
            {
                adxf_ldptnw_last_stat = ADXF_STAT_ERROR;
                return crierr;
            };

            adxfptinfo->ofst = ofst;
        };
    };

    if (adxf_ldptnw_hn == CRI_NULL)
    {
        ERR_CallErrFunc(CRI_STR("can't open file"));
        adxf_ldptnw_last_stat = ADXF_STAT_ERROR;

        return ADXF_ERR_FATAL;
    };

    adxfptinfo->fmg_type = (CriSint8)fmgtype;
    adxfptinfo->curdir = dir;
    adxfptinfo->type = 0;
    adxfptinfo->nfile = 0;

    adxf_ldpt_buf = tmpbuf;
    adxf_ldpt_rdsct = CRI_SIZE_TO_NSCT(tbsize, ADXF_DEF_SCT_SIZE);

    return ADXF_ERR_OK;
};


static CriSint32
adxf_LoadPtNwEx(CriSint32       set_ptid,
                void*           ptinfo,
                const CriChar8* fname,
                void*           dir,
                void*           tmpbuf,
                CriSint32       tbsize)
{
    return adxf_LoadPtBothNw(set_ptid, 0, 0, fname, dir, 0, ADXF_MAX_FSIZE, ptinfo, tmpbuf, tbsize, ADXF_FMG_TYPE_SHORT);
};


static CriSint32
adxf_LoadPartitionNw(CriSint32 ptid, const CriChar8* fname, void* dir, void* ptinfo)
{
    return adxf_LoadPtNwEx(ptid, ptinfo, fname, dir, adxf_ldpt_work, sizeof(adxf_ldpt_work));
};


static void
adxf_ExecOne(ADX_FS* adxf)
{
    CriSint32 fstat = cvFsGetStat(adxf->fhandle);

    if (fstat == CVFS_STAT_ERR)
    {
        adxf->stat = ADXF_STAT_ERROR;
        return;
    };

    if (adxf->stat == ADXF_STAT_READING)
    {
        adxf->rdsct = cvFsTell(adxf->fhandle) - adxf->skpos;

        if (fstat == CVFS_STAT_ERR)
        {
            adxf->skpos += adxf->rdsct;
            adxf->stat = ADXF_STAT_ERROR;
        };

        if (fstat == CVFS_STAT_COMPLETE)
        {
            adxf->skpos += adxf->rdsct;
            adxf->stat = ADXF_STAT_READEND;
        };
    };

    if (adxf->stopnw_flg == CRI_TRUE)
    {
        if (fstat == CVFS_STAT_TRANS)
        {
            cvFsStopTr(adxf->fhandle);

            adxf->rdsct = (cvFsTell(adxf->fhandle) - adxf->skpos);
            adxf->stat = ADXF_STAT_STOP;
            adxf->stopnw_flg = CRI_FALSE;
        };
    };
};


static void
adxf_ExecServer(void)
{
    ADXCRS_Lock();

    for (CriSint32 i = 0; i < CRI_COUNTOF(adxf_obj); ++i)
    {
        if (adxf_obj[i].used == CRI_TRUE)
            adxf_ExecOne(&adxf_obj[i]);
    };

    ADXCRS_Unlock();
};


void CRIAPI
ADXF_Init(void)
{
    if (!adxf_init_cnt++)
    {
        memset(adxf_obj, 0, sizeof(adxf_obj));
        memset(adxf_ptinfo, 0, sizeof(adxf_ptinfo));

        adxf_ldptnw_hn = CRI_NULL;
        adxf_ldptnw_ptid = -1;
        adxf_ldptnw_last_stat = ADXF_STAT_STOP;
    };
};


void CRIAPI
ADXF_Finish(void)
{
    if (adxf_init_cnt == 0)
        return;

    if (!--adxf_init_cnt)
    {
        ADXF_CloseAll();

        adxf_ldptnw_last_stat = ADXF_STAT_STOP;
        adxf_ldptnw_ptid = -1;
        adxf_ldptnw_hn = CRI_NULL;

        memset(adxf_ptinfo, 0, sizeof(adxf_ptinfo));
        memset(adxf_obj, 0, sizeof(adxf_obj));
    };
};


void CRIAPI
ADXF_ExecServer(void)
{
    adxf_Enter();
    adxf_ExecServer();
    adxf_Leave();
};


ADXF CRIAPI
ADXF_Open(const CriChar8* fname, void* atr)
{
    ADX_FS* adxf = ADXF_OpenNw(fname, atr);

    if (adxf == CRI_NULL)
        return CRI_NULL;

    while (!ADXF_IsOpened(adxf))
    {
        ADXM_ExecMain();

        if (ADXF_GetStat(adxf) == ADXF_STAT_ERROR)
        {
            ADXF_Close(adxf);
            return CRI_NULL;
        };
    };

    return adxf;
};


ADXF CRIAPI
ADXF_OpenNw(const CriChar8* fname, void* atr)
{
    adxf_Enter();
    ADXF adxf = adxf_OpenNw(fname, atr);
    adxf_Leave();

    return adxf;
};


ADXF CRIAPI
ADXF_OpenAfs(CriSint32 ptid, CriSint32 flid)
{
    ADX_FS* adxf = ADXF_OpenAfsNw(ptid, flid);

    if (adxf == CRI_NULL)
        return CRI_NULL;

    while (!ADXF_IsOpened(adxf))
    {
        ADXM_ExecMain();

        if (ADXF_GetStat(adxf) == ADXF_STAT_ERROR)
        {
            ADXF_Close(adxf);
            return CRI_NULL;
        };
    };

    return adxf;
};


ADXF CRIAPI
ADXF_OpenAfsNw(CriSint32 ptid, CriSint32 flid)
{
    adxf_Enter();
    ADXF adxf = adxf_OpenAfsNw(ptid, flid);
    adxf_Leave();

    return adxf;
};


CriBool CRIAPI
ADXF_IsOpened(ADXF adxf)
{
    adxf_Enter();
    CriBool result = cvFsIsFileOpened(adxf->fhandle);
    adxf_Leave();

    return result;
};


void CRIAPI
ADXF_CloseAll(void)
{
    adxf_Enter();
    adxf_CloseAll();
    adxf_Leave();
};


void CRIAPI
ADXF_Close(ADXF adxf)
{
    adxf_Enter();
    adxf_Close(adxf);
    adxf_Leave();
};


CriSint32 CRIAPI
ADXF_ReadNw(ADXF adxf, CriSint32 nsct, void* buf)
{
    adxf_Enter();
    CriSint32 result = adxf_ReadNw(adxf, nsct, buf);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_Stop(ADXF adxf)
{
    adxf_Enter();
    CriSint32 result = adxf_Stop(adxf);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_StopNw(ADXF adxf)
{
    adxf_Enter();
    CriSint32 result = adxf_StopNw(adxf);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_Seek(ADXF adxf, CriSint32 pos, CriSint32 type)
{
    adxf_Enter();
    CriSint32 result = adxf_Seek(adxf, pos, type);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_Tell(ADXF adxf)
{
    adxf_Enter();
    CriSint32 result = adxf_Tell(adxf);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_GetFsizeSct(ADXF adxf)
{
    adxf_Enter();
    CriSint32 result = adxf_GetFsizeSct(adxf);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_GetFsizeByte(ADXF adxf)
{
    adxf_Enter();
    CriSint32 result = adxf_GetFsizeByte(adxf);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_GetStat(ADXF adxf)
{
    ADXT_ExecFsSvr();

    adxf_Enter();
    CriSint32 result = adxf_GetStat(adxf);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_LoadPartitionNw(CriSint32 ptid, const CriChar8* fname, void* dir, void* ptinfo)
{
    adxf_Enter();
    CriSint32 result = adxf_LoadPartitionNw(ptid, fname, dir, ptinfo);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_GetPtStat(CriSint32 ptid)
{
    adxf_Enter();
    CriSint32 result = adxf_GetPtStat(ptid);
    adxf_Leave();

    return result;
};


void CRIAPI
ADXF_StopPtLd(void)
{
    adxf_Enter();
    adxf_StopPtLd();
    adxf_Leave();
};


CriSint32 CRIAPI
ADXF_GetPtinfoSize(CriSint32 ptid)
{
    adxf_Enter();
    CriSint32 result = adxf_GetPtInfoSize(ptid);
    adxf_Leave();

    return result;
};


CriChar8* CRIAPI
ADXF_GetFnameFromPt(CriSint32 ptid)
{
    adxf_Enter();
    CriChar8* result = adxf_GetFnameFromPt(ptid);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_GetNumFilesFromPt(CriSint32 ptid)
{
    adxf_Enter();
    CriSint32 result = adxf_GetNumFilesFromPt(ptid);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_GetFileSizeFromPt(CriSint32 ptid, CriSint32 fid)
{
    adxf_Enter();
    CriSint32 result = adxf_GetFileSizeFromPt(ptid, fid);
    adxf_Leave();

    return result;
};


CriSint32 CRIAPI
ADXF_GetNumFilesFromAfs(void* buf)
{
    adxf_Enter();
    CriSint32 result = adxf_GetNumFilesFromAfs(buf);
    adxf_Leave();

    return result;
};