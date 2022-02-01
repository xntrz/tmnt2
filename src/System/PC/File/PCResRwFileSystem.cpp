#include "PCResRwFileSystem.hpp"
#include "PCResRwFile.hpp"

#include "System/Common/File/FileTypes.hpp"


class CPCResRwFileSystem::CFile final : public RtFile
{
public:
    CPCResRwFile m_extFile;
};


CPCResRwFileSystem::CPCResRwFileSystem(void)
: m_paFile(nullptr)
{
    ;
};


CPCResRwFileSystem::~CPCResRwFileSystem(void)
{
    ASSERT(!m_paFile);
};


bool CPCResRwFileSystem::Initialize(void)
{
    fsClose                 = FsClose;
    fsGetDeviceName         = FsGetDeviceName;
    fsGetObject             = FsGetFile;
    fsFileFunc.open         = FOpen;
    fsFileFunc.close        = FClose;
    fsFileFunc.read         = FRead;
    fsFileFunc.write        = FWrite;
    fsFileFunc.setPosition  = FSetPos;
    fsFileFunc.sync         = FSync;
    fsFileFunc.abort        = FAbort;
    fsFileFunc.isEOF        = FEOF;
    fsFileFunc.getStatus    = FStatus;
    fsFileFunc.fExists      = FExists;

	m_paFile = new CFile[MAX_FILES]();
    ASSERT(m_paFile);
    if (m_paFile)
    {
        RtFileSystemError err = _rtFSInit(this, MAX_FILES, "rc", "rc:");
        ASSERT(err == RTFS_ERROR_NOERROR);
        return true;
    }
    else
    {
        return false;
    };
};


void CPCResRwFileSystem::Terminate(void)
{
    if (m_paFile)
    {
        delete[] m_paFile;
        m_paFile = nullptr;
    };
};


/*static*/ void CPCResRwFileSystem::FsClose(RtFileSystem* fs)
{
    ;
};


/*static*/ RwChar* CPCResRwFileSystem::FsGetDeviceName(RtFileSystem* fs)
{
    return "rc:";
};


/*static*/ RtFile* CPCResRwFileSystem::FsGetFile(RtFileSystem* fs, RwUInt32 index)
{
    return &((CPCResRwFileSystem*)fs)->m_paFile[ index ];
};


/*static*/ RtFileSystemError CPCResRwFileSystem::FOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags)
{
    char szRcPath[FILETYPES::FILE_NAME_MAX];
    szRcPath[0] = '\0';

    if (!ConvertPath(szRcPath, sizeof(szRcPath), filename))
        return RTFS_ERROR_FILESYSTEMNAMEINUSE;

    if (!((CFile*)file)->m_extFile.Open(szRcPath))
        return RTFS_ERROR_FILENOTFOUND;

    file->fileSystem = fs;
    return RTFS_ERROR_NOERROR;
};


/*static*/ void CPCResRwFileSystem::FClose(RtFile* file)
{
    ((CFile*)file)->m_extFile.Close();
};


/*static*/ RwUInt32 CPCResRwFileSystem::FRead(RtFile* file, void* pBuffer, RwUInt32 nBytes)
{
    return ((CFile*)file)->m_extFile.Read(pBuffer, nBytes);
};


/*static*/ RwUInt32 CPCResRwFileSystem::FWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes)
{
    ASSERT(false);
    return 0;
};


/*static*/ RtInt64 CPCResRwFileSystem::FSetPos(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition)
{
    uint32 uSeek = 0;
    RtInt64 Result;

    switch (fPosition)
    {
    case RTFILE_POS_BEGIN:
        uSeek = ((CFile*)file)->m_extFile.Seek(nOffset, CPCResRwFile::SEEKTYPE_BEGIN);
        break;

    case RTFILE_POS_CURRENT:
        uSeek = ((CFile*)file)->m_extFile.Seek(nOffset, CPCResRwFile::SEEKTYPE_CURRENT);
        break;

    case RTFILE_POS_END:
        uSeek = ((CFile*)file)->m_extFile.Seek(nOffset, CPCResRwFile::SEEKTYPE_END);
        break;

    default:
        ASSERT(false);
        break;
    };

    Result = { uSeek };

    return Result;
};


/*static*/ RtFileStatus CPCResRwFileSystem::FSync(RtFile* file, RwBool block)
{
    return RTFILE_STATUS_READY;
};


/*static*/ RwBool CPCResRwFileSystem::FAbort(RtFile* file)
{
    return false;
};


/*static*/ RwBool CPCResRwFileSystem::FEOF(RtFile* file)
{
    return ((CFile*)file)->m_extFile.IsEof();
};


/*static*/ RtFileStatus CPCResRwFileSystem::FStatus(RtFile* file)
{
    return (((CFile*)file)->m_extFile.IsOpen() ? RTFILE_STATUS_READY : RTFILE_STATUS_CLOSED);
};


/*static*/ RwBool CPCResRwFileSystem::FExists(RtFileSystem* fs, const RwChar* filename)
{
    char szRcPath[FILETYPES::FILE_NAME_MAX];
    szRcPath[0] = '\0';

    if (ConvertPath(szRcPath, sizeof(szRcPath), filename))
        return CPCResRwFile::IsExists(filename);
    else
        return false;
};


/*static*/ bool CPCResRwFileSystem::ConvertPath(char* pszPathBuffer, int32 nPathBufferSize, const char* pszPath)
{
    const char szDrive[] = "rc:";

    const char* pszDriveResult = std::strstr(pszPath, szDrive);
    if (!pszDriveResult)
        return false;

    const char* pszFilename = (pszDriveResult + (sizeof(szDrive) - 1));
    int32 nFilenameLen = std::strlen(pszFilename);
    
    if (nPathBufferSize < (nFilenameLen + 1))
        return false;

    std::strncpy(pszPathBuffer, pszFilename, (nFilenameLen + 1));
    return true;
};