#include "WebRwStdFS.hpp"


CWebRwStdFS::CWebRwStdFS(void)
: m_aFile()
, m_numOpen(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aFile); ++i)
        m_aFile[i].fptr = nullptr;
};


CWebRwStdFS::~CWebRwStdFS(void)
{
    ;
};


bool CWebRwStdFS::Initialize(const char* pszFSName, const char* pszDeviceName)
{
    return CWebRwFS::Initialize(pszFSName, pszDeviceName, COUNT_OF(m_aFile));
};


void CWebRwStdFS::Terminate(void)
{
    ASSERT(m_numOpen == 0);

    CWebRwFS::Terminate();
};


void CWebRwStdFS::rwFileSystemClose(RtFileSystem* fs)
{
    ;
};


RwChar* CWebRwStdFS::rwFileSystemGetDeviceName(RtFileSystem* fs)
{
    static char s_szDeviceName[] = "STDFS";
    return s_szDeviceName;
};


RtFile* CWebRwStdFS::rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index)
{
    ASSERT(index >= 0);
    ASSERT(index < COUNT_OF(m_aFile));

    return &m_aFile[index];
};


RtFileSystemError CWebRwStdFS::rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    const char* mode = GetFOpenMode(flags);

    if (!mode)
        return RTFS_ERROR_FILENOTFOUND;

    xFile->fptr = std::fopen(filename, mode);

    if (!xFile->fptr)
        return RTFS_ERROR_FILENOTFOUND;

    xFile->fileSystem = fs;
    ++m_numOpen;

    return RTFS_ERROR_NOERROR;
};


void CWebRwStdFS::rwClose(RtFile* file)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    if (xFile->fptr)
    {
        ASSERT(m_numOpen > 0);
        --m_numOpen;

        std::fclose(xFile->fptr);
        xFile->fptr = nullptr;
    };
};


RwUInt32 CWebRwStdFS::rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    return static_cast<RwUInt32>(std::fread(pBuffer, sizeof(uint8), static_cast<size_t>(nBytes), xFile->fptr));
};


RwUInt32 CWebRwStdFS::rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    return static_cast<RwUInt32>(std::fwrite(pBuffer, sizeof(uint8), static_cast<size_t>(nBytes), xFile->fptr));
};


RtInt64 CWebRwStdFS::rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);
    int seekMethod;

    if (fPosition == RTFILE_POS_BEGIN)
    {
        seekMethod = SEEK_SET;
    }
    else if (fPosition == RTFILE_POS_CURRENT)
    {
        seekMethod = SEEK_CUR;
    }
    else
    {
        seekMethod = SEEK_END;
    };

    std::fseek(xFile->fptr, static_cast<long>(nOffset), seekMethod);
    file->position.supportValue = static_cast<RwInt64>(std::ftell(xFile->fptr));

    return file->position;
};


RtFileStatus CWebRwStdFS::rwSync(RtFile* file, RwBool block)
{
    return RTFILE_STATUS_READY;
};


RwBool CWebRwStdFS::rwAbort(RtFile* file)
{
    return TRUE;
};


RwBool CWebRwStdFS::rwEof(RtFile* file)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    return (std::feof(xFile->fptr) != 0);
};


RtFileStatus CWebRwStdFS::rwGetStatus(RtFile* file)
{
    RtFileX* xFile = static_cast<RtFileX*>(file);

    if (xFile->fptr == nullptr)
        return RTFILE_STATUS_CLOSED;

    return RTFILE_STATUS_READY;
};


RwBool CWebRwStdFS::rwExists(RtFileSystem* fs, const RwChar* filename)
{
    FILE* fptr = std::fopen(filename, "rb");
    if (fptr)
    {
        std::fclose(fptr);
        return TRUE;
    };

    return FALSE;
};


const char* CWebRwStdFS::GetFOpenMode(RwUInt32 flags) const
{
    if (flags & RTFILE_ACCESS_OPEN_ASYNC)
        return nullptr;

    const RwUInt32 mode_r = (RTFILE_ACCESS_OPEN_READ);
    const RwUInt32 mode_r_plus = (mode_r | (RTFILE_ACCESS_OPEN_WRITE | RTFILE_ACCESS_OPEN_APPEND));

    const RwUInt32 mode_w = (RTFILE_ACCESS_OPEN_WRITE | RTFILE_ACCESS_OPEN_CREATE);
    const RwUInt32 mode_w_plus = (mode_w | (RTFILE_ACCESS_OPEN_READ));

    const RwUInt32 mode_a = (RTFILE_ACCESS_OPEN_APPEND | RTFILE_ACCESS_OPEN_WRITE);
    const RwUInt32 mode_a_plus = (mode_a | (RTFILE_ACCESS_OPEN_READ));

    if (flags == mode_r)
    {
        return "rb";
    }
    else if (flags == mode_r_plus)
    {
        return "r+b";
    }
    else if (flags == mode_w)
    {
        return "wb";
    }
    else if (flags == mode_w_plus)
    {
        return "w+b";
    }
    else if (flags == mode_a)
    {
        return "ab";
    }
    else if (flags == mode_a_plus)
    {
        return "a+b";
    };

    return nullptr;
};


void CWebRwStdFS::SetRedirectFromAfs(void)
{
    /* rw AFS FS is used by rw just for open font .met files inside rt 2d subsystems
       so we just place our replacement on top of org rw AFS FS that just maps rw
       file ops to the std c file api to read emscripten preload files */

    RtFSManagerSetDefaultFileSystem(m_pFileSystem);
};


void CWebRwStdFS::ClrRedirectFromAfs(void)
{
    static char fsName[] = "afs";

    RtFileSystem* fs = RtFSManagerGetFileSystemFromName(fsName);

    if (fs != nullptr)
        RtFSManagerSetDefaultFileSystem(fs);
};