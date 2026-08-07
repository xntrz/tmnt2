#include "WebRwWFS.hpp"

#include <emscripten.h>


#define workerFSWaitFor(flag)   \
    while (!(flag)) {           \
        emscripten_sleep(0);    \
    }


typedef struct
{
    int32 opid;
} IO_CANCEL;


typedef struct
{
    bool  bReady;
    bool  bMounted;
    char  szMountPath[256];
}  WORKERFS_STATE;


static WORKERFS_STATE s_workerFSState;
static IO_CANCEL s_aIOCancelData[32];
static int32 s_iIOCancelOPCounter = 0;
static int32 s_iIOInProgress = 0;


static int32
workerFSGenerateIOCancelOpid(void)
{
    if (s_iIOCancelOPCounter == 0x7FFFFFFF)
        s_iIOCancelOPCounter = 0;

    return s_iIOCancelOPCounter++;
};


static void
workerFSInitIOCancel(void)
{
    for (int32 i = 0; i < COUNT_OF(s_aIOCancelData); ++i)
    {
        s_aIOCancelData[i].opid = -1;
    };
};


static IO_CANCEL*
workerFSGetIOCancel(int32 opid)
{
    for (int32 i = 0; i < COUNT_OF(s_aIOCancelData); ++i)
    {
        if (s_aIOCancelData[i].opid == opid)
                return &s_aIOCancelData[i];
    };

    return nullptr;
};


static void
workerFSUnregistIOCancel(IO_CANCEL* iocancel)
{
    iocancel->opid = -1;
};


static bool
workerFSRegistIOCancel(int32 opid)
{
    if (!workerFSGetIOCancel(opid))
    {
        for (int32 i = 0; i < COUNT_OF(s_aIOCancelData); ++i)
        {
            if (s_aIOCancelData[i].opid == -1)
            {
                s_aIOCancelData[i].opid = opid;

                return true;
            };
        };
    };

    return false;
};


EMSCRIPTEN_KEEPALIVE
extern "C" void
onWorkerFSReady(void)
{
    s_workerFSState.bReady = true;
};



EMSCRIPTEN_KEEPALIVE
extern "C" void
onWorkerFSMountReady(void)
{
    s_workerFSState.bMounted = true;
};


EMSCRIPTEN_KEEPALIVE
extern "C" void
onWorkerFSDataReady(const uint8* data, size_t size, int32 opid, void* param)
{
    CWebRwWFS::RtFileX* file =
        reinterpret_cast<CWebRwWFS::RtFileX*>(param);

    if (file->opid != opid)
    {
        ASSERT(s_iIOInProgress > 0);
        --s_iIOInProgress;
        
        return;
    };

    IO_CANCEL* iocancel = workerFSGetIOCancel(opid);

    if (!iocancel)
    {
        switch (file->outstandingFileOp)
        {
        case RTFS_FILEOP_OPEN:
            {
                file->iosize = size;
            }
            break;

        case RTFS_FILEOP_READ:
            {
                if (data && size)
                {
                    std::memcpy(file->ioaddr, data, size);
                    file->iosize = size;
                };
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    }
    else
    {
        ;
    };

    MEMORY_BARRIER();
    file->iorun = false;

    ASSERT(s_iIOInProgress > 0);
    --s_iIOInProgress;
};


static bool
workerFSMakeIORequest(CWebRwWFS::RtFileX* file)
{
    ++s_iIOInProgress;

    switch (file->outstandingFileOp)
    {
    case RTFS_FILEOP_OPEN:
        {
            EM_ASM({
               postWorkerFSGetSize(UTF8ToString($0), $1, $2);
            }, file->path, file->opid, file);
        }
        return true;

    case RTFS_FILEOP_READ:
        {
            EM_ASM({
                postWorkerFSRead(UTF8ToString($0), $1, $2, $3, $4);
            }, file->path, file->ioofst, file->iosize, file->opid, file);
        }
        return true;

    default:
        ASSERT(false);
        break;
    };

    return false;
};


static void
workerFSCancelIORequest(CWebRwWFS::RtFileX* file)
{
    IO_CANCEL* iocancel = workerFSGetIOCancel(file->opid);

    if (!iocancel)
    {
        if (!workerFSRegistIOCancel(file->opid))
        {
            OUTPUT("can not regist io cancelation record\n");
            return;
        };
    };    
};


static void
workerFSShutdown(void)
{
    EM_ASM({
        terminateWorkerFS();
    });

    s_workerFSState.szMountPath[0] = '\0';
    s_workerFSState.bMounted = false;
    s_workerFSState.bReady = false;
};


static void
workerFSSartup(const char* pszMountPath)
{
    EM_ASM({
        initializeWorkerFS();
    });

    workerFSWaitFor(s_workerFSState.bReady);
    
    if (pszMountPath != nullptr)
    {
        EM_ASM({
            postWorkerFSMount(UTF8ToString($0));
        }, pszMountPath);

        workerFSWaitFor(s_workerFSState.bMounted);

        std::strcpy(s_workerFSState.szMountPath, pszMountPath);
    };
};


CWebRwWFS::CWebRwWFS(void)
: m_szMountPath()
, m_aFile()
, m_numOpen(0)
{
    m_szMountPath[0] = '\0';

    for (int32 i = 0; i < COUNT_OF(m_aFile); ++i)
    {
        RtFileX* xFile = &m_aFile[i];

        xFile->id = -1;
        xFile->opid = 0;
        xFile->path[0] = '\0';
        xFile->isopen = false;
        xFile->ioaddr = nullptr;
        xFile->iosize = 0;
        xFile->ioofst = 0;
        xFile->iorun = false;
    };
};


CWebRwWFS::~CWebRwWFS(void)
{
    ;
};


bool CWebRwWFS::Initialize(const char* pszFSName, const char* pszDeviceName, const char* pszMountPath)
{
    if (pszMountPath)
    {
        ASSERT(std::strlen(pszMountPath) < sizeof(m_szMountPath));
        std::strcpy(m_szMountPath, pszMountPath);
    };

    if (CWebRwFS::Initialize(pszFSName, pszDeviceName, COUNT_OF(m_aFile)))
    {
        workerFSSartup(pszMountPath);
        workerFSInitIOCancel();

        m_pFileSystem->defaultToAsync = TRUE;

        for (int32 i = 0; i < COUNT_OF(m_aFile); ++i)
            m_aFile[i].isAsync = TRUE;

        return true;
    };

    return false;
};


void CWebRwWFS::Terminate(void)
{
    ASSERT(m_numOpen == 0);

    while (s_iIOInProgress > 0)
        emscripten_sleep(0);

    workerFSShutdown();

    CWebRwFS::Terminate();
};


bool CWebRwWFS::Sync(void)
{
    RtFSManagerSyncAllFilesOnFileSystem(m_pFileSystem);
    return true;
};


void CWebRwWFS::rwFileSystemClose(RtFileSystem* fs)
{
    ;
};


RwChar* CWebRwWFS::rwFileSystemGetDeviceName(RtFileSystem* fs)
{
    static char s_szDeviceName[] = "WFS";
    return s_szDeviceName;
};


RtFile* CWebRwWFS::rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index)
{
    return &m_aFile[index];
};


RtFileSystemError CWebRwWFS::rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    xFile->isAsync = TRUE;
    xFile->error = RTFILE_ERROR_NOERROR;
    xFile->length.supportValue = 0;
    xFile->position.supportValue = 0;

    xFile->id = static_cast<int32>(xFile - m_aFile);
    xFile->opid = workerFSGenerateIOCancelOpid();
    std::strcpy(xFile->path, PathCorrection(filename));
    xFile->isopen = false;
    
    xFile->status = RTFILE_STATUS_BUSY;
    xFile->outstandingFileOp = RTFS_FILEOP_OPEN;
    xFile->iorun = true;

    if (!workerFSMakeIORequest(xFile))
        return RTFS_ERROR_FILENOTFOUND;

    ++m_numOpen;

    return RTFS_ERROR_NOERROR;
};


void CWebRwWFS::rwClose(RtFile* file)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    if (xFile->iorun)
    {
        xFile->iorun = false;
        workerFSCancelIORequest(xFile);
    };

    xFile->id = -1;
    xFile->path[0] = '\0';

    ASSERT(m_numOpen > 0);
    --m_numOpen;
};


RwUInt32 CWebRwWFS::rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    if (!xFile->isopen)
    {
        ASSERT(false);
        return 0;
    };

    if (xFile->status != RTFILE_STATUS_READY)
    {
        ASSERT(false);
        return 0;
    };

    size_t fsize = static_cast<size_t>(TkFSManagerGetValue(file->length));
    size_t fpos = static_cast<size_t>(TkFSManagerGetValue(file->position));

    if (fpos >= fsize)
        return 0;

    size_t bytesRead = static_cast<size_t>(nBytes);
    size_t bytesLeft = (fsize - fpos);

    bytesRead = Min(bytesRead, bytesLeft);

    if (bytesRead > 0)
    {
        xFile->opid = workerFSGenerateIOCancelOpid();

        xFile->ioaddr = pBuffer;
        xFile->iosize = bytesRead;
        xFile->ioofst = fpos;
        xFile->iorun = true;
        xFile->status = RTFILE_STATUS_BUSY;
        xFile->outstandingFileOp = RTFS_FILEOP_READ;

        workerFSMakeIORequest(xFile);
    };

    return 0;
};


RwUInt32 CWebRwWFS::rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes)
{
    ASSERT(false);
    return 0;
};


RtInt64 CWebRwWFS::rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    if (!xFile->isopen)
    {
        ASSERT(false);
        return { -1 };
    };

    int64 position = 0;
    int64 offset = static_cast<int64>(nOffset);
    int64 fsize = static_cast<int64>(TkFSManagerGetValue(xFile->length));
    int64 fpos = static_cast<int64>(TkFSManagerGetValue(xFile->position));

    if (fPosition == RTFILE_POS_BEGIN)
        position = 0;
    else if (fPosition == RTFILE_POS_CURRENT)
        position = fpos;
    else
        position = fsize;

    position += offset;
    position = Clamp(position, 0ll, fsize);

    xFile->position.supportValue = position;

    if (xFile->outstandingFileOp == RTFS_FILEOP_SEEK)
    {
        xFile->iosize = static_cast<size_t>(position);
        xFile->status = rwSync(xFile, TRUE);
    };

    return xFile->position;
};


RtFileStatus CWebRwWFS::rwSync(RtFile* file, RwBool block)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    if (xFile->status == RTFILE_STATUS_BUSY)
    {
        if (!xFile->iorun)
        {
            MEMORY_BARRIER();

            IO_CANCEL* iocancel = workerFSGetIOCancel(xFile->opid);

            if (iocancel)
            {
                xFile->status = RTFILE_STATUS_READY;
                workerFSUnregistIOCancel(iocancel);
            }
            else
            {
                switch (xFile->outstandingFileOp)
                {
                case RTFS_FILEOP_OPEN:
                    {
                        xFile->length.supportValue = static_cast<RwInt64>(xFile->iosize);

                        if (TkFSManagerGetValue(xFile->length) > 0)
                            xFile->isopen = true;
                    }
                    break;

                case RTFS_FILEOP_READ:
                    {
                        xFile->fileSystem->fsFileFunc.setPosition(xFile, static_cast<RwInt32>(xFile->iosize), RTFILE_POS_CURRENT);
                    }                    
                    break;

                case RTFS_FILEOP_SEEK:
                    {
                        xFile->iosize = static_cast<size_t>(TkFSManagerGetValue(xFile->position));
                    }
                    break;

                default:
                    break;
                };
                
                xFile->outstandingSize = static_cast<RwUInt32>(xFile->iosize);
                xFile->status = RTFILE_STATUS_READY;

                rtFSTriggerFileOutstandingOpCallback(xFile);
            };
        };
    };

    return xFile->status;
};


RwBool CWebRwWFS::rwAbort(RtFile* file)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    workerFSCancelIORequest(xFile);

    return TRUE;
};


RwBool CWebRwWFS::rwEof(RtFile* file)
{
    return (file->position.supportValue == file->length.supportValue);
};


RtFileStatus CWebRwWFS::rwGetStatus(RtFile* file)
{
    if (file->status != RTFILE_STATUS_CLOSED)
        rwSync(file, FALSE);

    return file->status;
};


RwBool CWebRwWFS::rwExists(RtFileSystem* fs, const RwChar* filename)
{
    RtFileX xFile = {};

    xFile.isAsync = FALSE;
    xFile.error = RTFILE_ERROR_NOERROR;
    xFile.length.supportValue = 0;
    xFile.position.supportValue = 0;

    xFile.id = -1;
    xFile.opid = 0;
    std::strcpy(xFile.path, PathCorrection(filename));
    xFile.isopen = false;

    xFile.status = RTFILE_STATUS_BUSY;
    xFile.outstandingFileOp = RTFS_FILEOP_OPEN;
    xFile.iorun = true;

    if (workerFSMakeIORequest(&xFile))
        return (xFile.iosize != 0);

    return FALSE;
};


const RwChar* CWebRwWFS::PathCorrection(const RwChar* filename) const
{
    static RwChar s_szCorrectPath[256];
    s_szCorrectPath[0] = '\0';

    const RwChar* filenameStart = std::strchr(filename, ':');
    if (filenameStart)
        filename = filenameStart + 1;

    /* worker fs may open file in MEMFS too, so check it out */
    if (CheckFileExistInMEMFS(filename))
    {
        /* file exist in MEMFS, return pure path (without device name) */
        return filename;
    };

    std::strcpy(s_szCorrectPath, m_szMountPath);

    if (filename[0] != '/')
        std::strcat(s_szCorrectPath, "/");

    std::strcat(s_szCorrectPath, filename);

    return s_szCorrectPath;
};


bool CWebRwWFS::CheckFileExistInMEMFS(const RwChar* filename) const
{
    FILE* fptr = std::fopen(filename, "rb");
    if (fptr)
    {
        std::fclose(fptr);
        return true;
    };

    return false;
};