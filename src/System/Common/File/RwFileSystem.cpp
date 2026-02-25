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
    return &static_cast<CRwFileSystem*>(fs)->m_pFile[index];
};


/*static*/ RtFileSystemError CRwFileSystem::rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags)
{
    if (!static_cast<CRwFile*>(file)->m_extFile.open(filename))
        return RTFS_ERROR_FILENOTFOUND;

    file->fileSystem = fs;
    
    return RTFS_ERROR_NOERROR;
};


/*static*/ void CRwFileSystem::rwClose(RtFile* file)
{
    static_cast<CRwFile*>(file)->m_extFile.close();
};


/*static*/ RwUInt32 CRwFileSystem::rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes)
{
    return static_cast<CRwFile*>(file)->m_extFile.read(pBuffer, nBytes);
};


/*static*/ RwUInt32 CRwFileSystem::rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes)
{
    ASSERT(false);
    return 0;
};


/*static*/ RtInt64 CRwFileSystem::rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition)
{
    RtInt64 ret;
    ret.supportValue = 0;

    CStdFile* pFile = &static_cast<CRwFile*>(file)->m_extFile;

    switch (fPosition)
    {
    case RTFILE_POS_BEGIN:
        pFile->seek(static_cast<uint32>(nOffset), SEEK_SET);
        break;

    case RTFILE_POS_CURRENT:
        pFile->seek(static_cast<uint32>(nOffset), SEEK_CUR);
        break;

    case RTFILE_POS_END:
        pFile->seek(static_cast<uint32>(nOffset), SEEK_END);
        break;

    default:
        ASSERT(false);
        break;
    };

    ret.supportValue = pFile->tell();
    return ret;
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
    return static_cast<CRwFile*>(file)->m_extFile.eof();
};


/*static*/ RtFileStatus CRwFileSystem::rwGetStatus(RtFile* file)
{
    return RTFILE_STATUS_ERROR;
};


/*static*/ RwBool CRwFileSystem::rwExists(RtFileSystem* fs, const RwChar* filename)
{
    return CStdFile::exist(filename);
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