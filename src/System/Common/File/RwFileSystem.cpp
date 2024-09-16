#include "RwFileSystem.hpp"


/*static*/ void CRwFileSystem::rwFileSystemClose(RtFileSystem* fs)
{
    ;
};


/*static*/ RwChar* CRwFileSystem::rwFileSystemGetDeviceName(RtFileSystem* fs)
{
    static char s_deviceName[] = "";
    return s_deviceName;
};


/*static*/ RtFile* CRwFileSystem::rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index)
{
    return &((CRwFileSystem*)fs)->m_pFile[index];
};


/*static*/ RtFileSystemError CRwFileSystem::rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags)
{
    if (!((CRwFile*)file)->m_extFile.Open(filename))
        return RTFS_ERROR_FILENOTFOUND;

    file->fileSystem = fs;
    
    return RTFS_ERROR_NOERROR;
};


/*static*/ void CRwFileSystem::rwClose(RtFile* file)
{
    ((CRwFile*)file)->m_extFile.Close();
};


/*static*/ RwUInt32 CRwFileSystem::rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes)
{
    return ((CRwFile*)file)->m_extFile.Read(pBuffer, nBytes);
};


/*static*/ RwUInt32 CRwFileSystem::rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes)
{
    ASSERT(false);
    return 0;
};


/*static*/ RtInt64 CRwFileSystem::rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition)
{
    static_assert((RTFILE_POS_BEGIN - 1)    == SEEK_SET, "update me");
    static_assert((RTFILE_POS_CURRENT - 1)  == SEEK_CUR, "update me");
    static_assert((RTFILE_POS_END - 1)      == SEEK_END, "update me");

    ASSERT(
        ((fPosition - 1) >= SEEK_SET) &&
        ((fPosition - 1) <= SEEK_END)
    );

    RtInt64 Result;
    Result.supportValue = ((CRwFile*)file)->m_extFile.Seek(nOffset, fPosition - 1);

    return Result;
};


/*static*/ RtFileStatus CRwFileSystem::rwSync(RtFile* fie, RwBool block)
{
    return RTFILE_STATUS_READY;
};


/*static*/ RwBool CRwFileSystem::rwAbort(RtFile* file)
{
    return 0;
};


/*static*/ RwBool CRwFileSystem::rwEof(RtFile* file)
{
    return ((CRwFile*)file)->m_extFile.IsEof();
};


/*static*/ RtFileStatus CRwFileSystem::rwGetStatus(RtFile* file)
{
    return (((CRwFile*)file)->m_extFile.IsOpen() ? RTFILE_STATUS_READY : RTFILE_STATUS_CLOSED);
};


/*static*/ RwBool CRwFileSystem::rwExists(RtFileSystem* fs, const RwChar* filename)
{
    return CStdFile::IsExists(filename);
};


CRwFileSystem::CRwFileSystem(void)
: m_pFile(nullptr)
{
    ;
};


CRwFileSystem::~CRwFileSystem(void)
{
    ;
};


bool CRwFileSystem::Initialize(void)
{
    fsClose                 = rwFileSystemClose;
    fsGetDeviceName         = rwFileSystemGetDeviceName;
    fsGetObject             = rwFileSystemGetFile;
    fsFileFunc.open         = rwOpen;
    fsFileFunc.close        = rwClose;
    fsFileFunc.read         = rwRead;
    fsFileFunc.write        = rwWrite;
    fsFileFunc.setPosition  = rwSetPosition;
    fsFileFunc.sync         = rwSync;
    fsFileFunc.abort        = rwAbort;
    fsFileFunc.isEOF        = rwEof;
    fsFileFunc.getStatus    = rwGetStatus;
    fsFileFunc.fExists      = rwExists;

    m_pFile = new CRwFile[MAX_FILES];
    if (!m_pFile)
        return false;

    static char s_fsName[] = "afs";
    static char s_deviceName[] = "afs:";
    RtFileSystemError err = _rtFSInit(this, MAX_FILES, s_fsName, s_deviceName);
    if (err != RTFS_ERROR_NOERROR)
        return false;

    return true;
};


void CRwFileSystem::Terminate(void)
{
    if (m_pFile)
    {
        delete[] m_pFile;
        m_pFile = nullptr;
    };
};