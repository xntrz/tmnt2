#ifndef CVFS_H
#define CVFS_H

#include "cri_xpt.h"


#define CVFS_PATH_LEN                       (300)


#define CVFS_OPTCODE_SETCURVOL              (1)
#define CVFS_OPTCODE_ADDVOL                 (2)
#define CVFS_OPTCODE_DELVOL                 (3)
#define CVFS_OPTCODE_GETNUMFILES            (4)
#define CVFS_OPTCODE_GETVOLINFO             (5)
#define CVFS_OPTCODE_SETDEFVOL              (6)

#define CVFS_OPTCODE_ISNEEDDEVNAME          (100)

#define CVFS_OPTCODE_GETNUMTR_U             (200)
#define CVFS_OPTCODE_GETNUMTR_L             (201)
#define CVFS_OPTCODE_GETFILESIZE_U          (202)
#define CVFS_OPTCODE_GETFILESIZE_L          (203)
#define CVFS_OPTCODE_GETFILESIZEEX_U        (204)
#define CVFS_OPTCODE_GETFILESIZEEX_L        (205)
#define CVFS_OPTCODE_GETFSYSINFO            (299)

#define CVFS_OPTCODE_GETFILESIZEHANDLE      (300)
#define CVFS_OPTCODE_GETFILESIZEHANDLE_U    (301)
#define CVFS_OPTCODE_GETFILESIZEHANDLE_L    (302)

#define CVFS_OPTCODE_ISAVAILABLERETRY       (400)

#define CVFS_OPTCODE_EXECIDLE               (500)
#define CVFS_OPTCODE_CANCELIO               (501)

#define CVFS_OPTCODE_ISFILEOPENED           (600)
#define CVFS_OPTCODE_REQUESTCLOSE           (601)

#define CVFS_OPTCODE_ISOPENREQ              (700)


typedef enum
{
    CVFS_STAT_IDLE = 0,
	CVFS_STAT_COMPLETE,
	CVFS_STAT_TRANS,
	CVFS_STAT_ERR,
} CVFS_STAT;


typedef enum
{
    CVFS_RW_RD = 0,
    CVFS_RW_WR,
} CVFS_RW;


typedef enum
{
	CVFS_SEEK_SET = 0,
	CVFS_SEEK_CUR,
	CVFS_SEEK_END,
} CVFS_SEEK;


typedef void (*cvFsErrFunc)(void* obj, const CriChar8* msg, CriSint32 errcode);


typedef struct CVFS_VTBL
{
    void        (*ExecServer)(void);
    void        (*EntryErrFunc)(cvFsErrFunc fn, void* obj);
    CriSint32   (*GetFileSize)(const CriChar8* path);
    CriSint32   (*GetFreeSize)(void);
    void*       (*Open)(const CriChar8* path, void* dirbuf, CVFS_RW rw);
    void        (*Close)(void* hn);
    CriSint32   (*Seek)(void* hn, CriSint32 offset, CVFS_SEEK skmode); // NOTE: sets offset in sectors
    CriSint32   (*Tell)(void* hn); // NOTE: returns offset in sectors
    CriSint32   (*ReqRd)(void* hn, CriSint32 nsct, void* buf);
    CriSint32   (*ReqWr)(void* hn, CriSint32 nsct, const void* buf);
    void        (*StopTr)(void* hn);
    CriSint32   (*GetStat)(void* hn);
    CriSint32   (*GetSctLen)(void* hn);
    void        (*SetSctLen)(void* hn, CriSint32 len);
    CriSint32   (*GetNumTr)(void* hn);
    CriSint32   (*ChangeDir)(const CriChar8* path);
    CriSint32   (*IsExistFile)(const CriChar8* path);
    CriSint32   (*GetNumFiles)(void);
    CriSint32   (*LoadDirInfo)(const CriChar8* path, void* dirbuf, CriSint32 param2);
    CriSint32   (*GetMaxByteRate)(void* hn);
    CriSint32   (*MakeDir)(const CriChar8* path);
    CriSint32   (*RemoveDir)(const CriChar8* path);
    CriSint32   (*DeleteFile)(const CriChar8* path);
    CriSint32   (*GetFileSizeEx)(const CriChar8* path, void* param1);
    CriSint32   (*OptFn1)(void* data, CriSint32 optCode, CriSint32 param1, CriSint32 param2);
    CriSint32   (*OptFn2)(void* data, CriSint32 optCode, CriSint32 param1, CriSint32 param2);
	CriSint64	(*GetNumTr64)(void* hn);
	CriBool		(*IsAvailableRetry)(void* hn);
} CVFS_VTBL;


typedef CVFS_VTBL*(*cvFsGetInterface)(void);


void            cvFsInit(void);
void            cvFsFinish(void);
void            cvFsEntryErrFunc(cvFsErrFunc fn, void* obj);
void            cvFsAddDev(const CriChar8* deviceName, cvFsGetInterface fn);
void            cvFsDelDev(const CriChar8* deviceName);
void            cvFsSetDefDev(const CriChar8* deviceName);
const CriChar8* cvFsGetDefDev(void);
void            cvFsExecServer(void);
void            cvFsExecIdle(void);
void*           cvFsOpen(const CriChar8* path, void* param, CVFS_RW mode);
void            cvFsClose(void* hn);
CriSint32       cvFsSeek(void* hn, CriSint32 ofst, CVFS_SEEK mode);
CriSint32       cvFsTell(void* hn);
CriSint32       cvFsReqRd(void* hn, CriSint32 nsct, void* buf);
void            cvFsStopTr(void* hn);
CriSint32       cvFsGetStat(void* hn);
CriSint32       cvFsGetNumTr(void* hn);
CriSint32       cvFsGetFileSizeByHn(void* hn);
CriBool         cvFsIsFileOpened(void* hn);
CriBool         cvFsIsOpenReq(void* hn);

#endif /* CVFS_H */
