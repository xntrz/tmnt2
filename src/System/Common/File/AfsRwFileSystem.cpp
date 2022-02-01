#include "AfsRwFileSystem.hpp"
#include "AfsRwFile.hpp"


class CAfsRwFileSystem::CFile final : public RtFile
{
public:
    CAfsRwFile m_extFile;
};


CAfsRwFileSystem::CAfsRwFileSystem(void)
: m_paFile(nullptr)
{
    ;
};


CAfsRwFileSystem::~CAfsRwFileSystem(void)
{
    ASSERT(!m_paFile);
};


bool CAfsRwFileSystem::Initialize(void)
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

    m_paFile = new CFile[MAX_FILES];
    ASSERT(m_paFile);
    if (m_paFile)
    {
        RtFileSystemError err = _rtFSInit(this, MAX_FILES, "afs", "afs:");
        ASSERT(err == RTFS_ERROR_NOERROR);
        return true;
    }
    else
    {
        return false;
    };
};


void CAfsRwFileSystem::Terminate(void)
{
    if (m_paFile)
    {
        delete[] m_paFile;
        m_paFile = nullptr;
    };
};


/*static*/ void CAfsRwFileSystem::FsClose(RtFileSystem* fs)
{
    ;
};


/*static*/ RwChar * CAfsRwFileSystem::FsGetDeviceName(RtFileSystem* fs)
{
    return "afs:";
};


/*static*/ RtFile * CAfsRwFileSystem::FsGetFile(RtFileSystem* fs, RwUInt32 index)
{
    return &((CAfsRwFileSystem*)fs)->m_paFile[index];
};


/*static*/ RtFileSystemError CAfsRwFileSystem::FOpen(RtFileSystem* fs, RtFile* file, const RwChar * filename, RwUInt32 flags)
{
    if (!((CFile*)file)->m_extFile.Open(filename))
        return RTFS_ERROR_FILENOTFOUND;

    file->fileSystem = fs;
    return RTFS_ERROR_NOERROR;
};


/*static*/ void CAfsRwFileSystem::FClose(RtFile* file)
{
    ((CFile*)file)->m_extFile.Close();
};


/*static*/ RwUInt32 CAfsRwFileSystem::FRead(RtFile* file, void* pBuffer, RwUInt32 nBytes)
{
    return ((CFile*)file)->m_extFile.Read(pBuffer, nBytes);
};


/*static*/ RwUInt32 CAfsRwFileSystem::FWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes)
{
    ASSERT(false);
    return 0;
};


/*static*/ RtInt64 CAfsRwFileSystem::FSetPos(RtFile * file, RwInt32 nOffset, RtFileSeekMethod fPosition)
{
    uint32 uSeek = 0;
    RtInt64 Result;

    switch (fPosition)
    {
    case RTFILE_POS_BEGIN:
        uSeek = ((CFile*)file)->m_extFile.Seek(nOffset, CAfsRwFile::SEEKTYPE_BEGIN);
        break;

    case RTFILE_POS_CURRENT:
        uSeek = ((CFile*)file)->m_extFile.Seek(nOffset, CAfsRwFile::SEEKTYPE_CURRENT);
        break;

    case RTFILE_POS_END:
        uSeek = ((CFile*)file)->m_extFile.Seek(nOffset, CAfsRwFile::SEEKTYPE_END);
        break;

    default:
        ASSERT(false);
        break;
    };

    Result = { uSeek };

    return Result;
};


/*static*/ RtFileStatus CAfsRwFileSystem::FSync(RtFile* file, RwBool block)
{
    return RTFILE_STATUS_READY;
};


/*static*/ RwBool CAfsRwFileSystem::FAbort(RtFile* file)
{
    return false;
};


/*static*/ RwBool CAfsRwFileSystem::FEOF(RtFile* file)
{
    return ((CFile*)file)->m_extFile.IsEof();
};


/*static*/ RtFileStatus CAfsRwFileSystem::FStatus(RtFile* file)
{
    return (((CFile*)file)->m_extFile.IsOpen() ? RTFILE_STATUS_READY : RTFILE_STATUS_CLOSED);
};


/*static*/ RwBool CAfsRwFileSystem::FExists(RtFileSystem* fs, const RwChar* filename)
{
    return CAfsRwFile::IsExists(filename);
};