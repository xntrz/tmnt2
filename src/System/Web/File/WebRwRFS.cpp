#include "WebRwRFS.hpp"

#include <emscripten.h>


EMSCRIPTEN_KEEPALIVE
extern "C" void
remoteFSOnIOComplete(const uint8* data, size_t size, int32 iscancel, void* param)
{
    CWebRwRFS::RtFileX* file =
        reinterpret_cast<CWebRwRFS::RtFileX*>(param);

    if (!iscancel)
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

                file->fetch = nullptr;
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    }
    else
    {
        file->iscancel = true;
    };

    MEMORY_BARRIER();
    file->iorun = false;
};


static void
remoteFSOnFetchSuccess(emscripten_fetch_t* fetch)
{
    remoteFSOnIOComplete(reinterpret_cast<const uint8*>(fetch->data),
                         static_cast<size_t>(fetch->numBytes),
                         0,
                         fetch->userData);

    emscripten_fetch_close(fetch);
};


static void
remoteFSOnFetchError(emscripten_fetch_t* fetch)
{
    remoteFSOnIOComplete(nullptr, 0, 0, fetch->userData);

    emscripten_fetch_close(fetch);
};


static bool
remoteFSMakeIORequest(CWebRwRFS::RtFileX* file)
{
    switch (file->outstandingFileOp)
    {
    case RTFS_FILEOP_OPEN:
        {
            if (!file->isAsync)
            {
                size_t fsize = EM_ASM_INT({
                    var xhr = new XMLHttpRequest();
                    xhr.open("HEAD", UTF8ToString($0), false);
                    
                    try {
                        xhr.send();
                        if ((xhr.status >= 200) && (xhr.status < 300)) {
                            var sizeHeader = xhr.getResponseHeader("Content-Length");
                            var size = parseInt(sizeHeader || "0", 10);
                            return size;
                        }
                    } catch (e) {
                        console.error("XHR HEAD err: " + e);
                    }

                    return 0;
                }, file->url);

                file->iosize = fsize;

                return true;
            };

            EM_ASM({
                var requestId = $0;
                var url = UTF8ToString($1);
                var userData = $2;

                var xhr = new XMLHttpRequest();
                xhr.open("HEAD", url, true);

                if (!Module.activeXHRs)
                    Module.activeXHRs = {};

                Module.activeXHRs[requestId] = xhr;

                xhr.onload = function() {
                    var size = 0;
                    if ((xhr.status >= 200) && (xhr.status < 300)) {
                        var sizeHeader = xhr.getResponseHeader("Content-Length");
                        size = parseInt(sizeHeader || "0", 10);
                    }
                    Module['_remoteFSOnIOComplete'](0, size, 0, userData);
                    delete Module.activeXHRs[requestId];
                };

                xhr.onerror = function() {
                    console.error("XHR HEAD err: " + requestId);
                    Module['_remoteFSOnIOComplete'](0, 0, 1, userData);
                    delete Module.activeXHRs[requestId];
                };

                xhr.onabort = function() {
                    console.warn("XHR HEAD abort " + requestId);
                    Module['_remoteFSOnIOComplete'](0, 0, 1, userData);
                    delete Module.activeXHRs[requestId];
                };

                xhr.send();
            }, file->id, file->url, file);
        }
        return true;

    case RTFS_FILEOP_READ:
        {
            if (file->iosize == 0)
                return false;

            emscripten_fetch_attr_t attr;
            emscripten_fetch_attr_init(&attr);

            std::strcpy(attr.requestMethod, "GET");
            attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
            attr.onsuccess = &remoteFSOnFetchSuccess;
            attr.onerror = &remoteFSOnFetchError;
            attr.userData = file;

            char szRangeHdr[64];
            std::snprintf(szRangeHdr, sizeof(szRangeHdr),
                          "bytes=%zu-%zu", file->ioofst, file->ioofst + file->iosize - 1);

            const char* pszHeaders[] = { "Range", szRangeHdr, NULL };
            attr.requestHeaders = pszHeaders;

            file->fetch = emscripten_fetch(&attr, file->url);
        }
        return (file->fetch != nullptr);

    default:
        break;
    };

    return false;
};


static void
remoteFSCancelIORequest(CWebRwRFS::RtFileX* file)
{
    switch (file->outstandingFileOp)
    {
    case RTFS_FILEOP_OPEN:
        {
            ASSERT(file->isAsync == TRUE);

            EM_ASM({
                var requestId = $0;
                if (Module.activeXHRs && Module.activeXHRs[requestId]) {
                    Module.activeXHRs[requestId].abort();
                }
            }, file->id);
        }
        break;

    case RTFS_FILEOP_READ:
        {
            if (file->fetch)
            {
                emscripten_fetch_close(file->fetch);
                file->fetch = nullptr;
            };
        }
        break;

    default:
        break;
    };
};


CWebRwRFS::CWebRwRFS(void)
: m_aFile()
, m_numOpen(0)
{
    m_szMountPath[0] = '\0';

    for (int32 i = 0; i < COUNT_OF(m_aFile); ++i)
    {
        RtFileX* xFile = &m_aFile[i];
        
        xFile->id = -1;
        xFile->url[0] = '\0';
        xFile->isopen = false;
        xFile->iscancel = false;
        xFile->fetch = nullptr;
        xFile->ioaddr = nullptr;
        xFile->iosize = 0;
        xFile->ioofst = 0;
        xFile->iorun = false;
    };
};


CWebRwRFS::~CWebRwRFS(void)
{
    ;
};


bool CWebRwRFS::Initialize(const char* pszFSName, const char* pszDeviceName, const char* pszMountPath)
{
    ASSERT(std::strlen(pszMountPath) < sizeof(m_szMountPath));
    std::strcpy(m_szMountPath, pszMountPath);

    if (CWebRwFS::Initialize(pszFSName, pszDeviceName, COUNT_OF(m_aFile)))
    {
        m_pFileSystem->defaultToAsync = TRUE;

        for (int32 i = 0; i < COUNT_OF(m_aFile); ++i)
            m_aFile[i].isAsync = TRUE;
        
        return true;
    };

    return false;
};


void CWebRwRFS::Terminate(void)
{
    ASSERT(m_numOpen == 0);

    CWebRwFS::Terminate();
};


bool CWebRwRFS::Sync(void)
{
    RtFSManagerSyncAllFilesOnFileSystem(m_pFileSystem);
    return true;
};


void CWebRwRFS::rwFileSystemClose(RtFileSystem* fs)
{
    ;
};


RwChar* CWebRwRFS::rwFileSystemGetDeviceName(RtFileSystem* fs)
{
    static char s_szDeviceName[] = "RFS";
    return s_szDeviceName;
};


RtFile* CWebRwRFS::rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index)
{
    return &m_aFile[index];
};


RtFileSystemError CWebRwRFS::rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    xFile->isAsync = TRUE;
    xFile->error = RTFILE_ERROR_NOERROR;
    xFile->length.supportValue = 0;
    xFile->position.supportValue = 0;

    xFile->id = static_cast<int32>(xFile - m_aFile);
    std::strcpy(xFile->url, PathCorrection(filename));
    xFile->isopen = false;
    xFile->iscancel = false;
    
    xFile->iorun = true;
    xFile->status = RTFILE_STATUS_BUSY;
    xFile->outstandingFileOp = RTFS_FILEOP_OPEN;

    if (!remoteFSMakeIORequest(xFile))
        return RTFS_ERROR_FILENOTFOUND;

    ++m_numOpen;

    return RTFS_ERROR_NOERROR;
};


void CWebRwRFS::rwClose(RtFile* file)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    remoteFSCancelIORequest(xFile);

    xFile->id = -1;
    xFile->url[0] = '\0';

    ASSERT(m_numOpen > 0);
    --m_numOpen;
};


RwUInt32 CWebRwRFS::rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes)
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
        xFile->iscancel = false;

        xFile->ioaddr = pBuffer;
        xFile->iosize = bytesRead;
        xFile->ioofst = fpos;
        xFile->iorun = true;
        xFile->status = RTFILE_STATUS_BUSY;
        xFile->outstandingFileOp = RTFS_FILEOP_READ;

        remoteFSMakeIORequest(xFile);
    };

    return 0;
};


RwUInt32 CWebRwRFS::rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes)
{
    ASSERT(false);
    return 0;
};


RtInt64 CWebRwRFS::rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition)
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


RtFileStatus CWebRwRFS::rwSync(RtFile* file, RwBool block)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    if (xFile->status == RTFILE_STATUS_BUSY)
    {        
        if (!xFile->iorun)
        {
            MEMORY_BARRIER();

            if (xFile->iscancel)
            {
                xFile->status = RTFILE_STATUS_READY;
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


RwBool CWebRwRFS::rwAbort(RtFile* file)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    remoteFSCancelIORequest(xFile);

    return TRUE;
};


RwBool CWebRwRFS::rwEof(RtFile* file)
{
    return (TkFSManagerGetValue(file->position) == TkFSManagerGetValue(file->length));
};


RtFileStatus CWebRwRFS::rwGetStatus(RtFile* file)
{
    if (file->status != RTFILE_STATUS_CLOSED)
        rwSync(file, FALSE);

    return file->status;
};


RwBool CWebRwRFS::rwExists(RtFileSystem* fs, const RwChar* filename)
{
    RtFileX xFile = {};

    xFile.isAsync = FALSE;
    xFile.error = RTFILE_ERROR_NOERROR;

    xFile.id = -1;
    std::strcpy(xFile.url, PathCorrection(filename));
    xFile.isopen = false;
    xFile.iscancel = false;

    xFile.iorun = true;
    xFile.status = RTFILE_STATUS_BUSY;
    xFile.outstandingFileOp = RTFS_FILEOP_OPEN;

    if (remoteFSMakeIORequest(&xFile))
        return (xFile.iosize != 0);

    return FALSE;
};


const RwChar* CWebRwRFS::PathCorrection(const RwChar* filename) const
{
    static RwChar s_szCorrectPath[256];
    s_szCorrectPath[0] = '\0';

    const RwChar* filenameStart = std::strchr(filename, ':');
    if (filenameStart)
        filename = filenameStart + 1;

    std::strcpy(s_szCorrectPath, m_szMountPath);

    if (filename[0] != '/')
        std::strcat(s_szCorrectPath, "/");

    std::strcat(s_szCorrectPath, filename);

    return s_szCorrectPath;
};