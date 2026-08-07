#include "WebRwFS.hpp"


static void _rwFileSystemClose(RtFileSystem* fs)
{
    static_cast<CWebRwFS::RtFileSystemEx*>(fs)->cppobj->rwFileSystemClose(fs);
};


static RwChar* _rwFileSystemGetDeviceName(RtFileSystem* fs)
{
    return  static_cast<CWebRwFS::RtFileSystemEx*>(fs)->cppobj->rwFileSystemGetDeviceName(fs);
};


static RtFile* _rwFileSystemGetFile(RtFileSystem* fs, RwUInt32 index)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(fs)->cppobj->rwFileSystemGetFile(fs, index);
};


static RtFileSystemError _rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(fs)->cppobj->rwOpen(fs, file, filename, flags);
};


static void _rwClose(RtFile* file)
{
    static_cast<CWebRwFS::RtFileSystemEx*>(file->fileSystem)->cppobj->rwClose(file);
};


static RwUInt32 _rwRead(RtFile* file, void* pBuffer, RwUInt32 nBytes)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(file->fileSystem)->cppobj->rwRead(file, pBuffer, nBytes);
};


static RwUInt32 _rwWrite(RtFile* file, const void* pBuffer, RwUInt32 nBytes)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(file->fileSystem)->cppobj->rwWrite(file, pBuffer, nBytes);
};


static RtInt64 _rwSetPosition(RtFile* file, RwInt32 nOffset, RtFileSeekMethod fPosition)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(file->fileSystem)->cppobj->rwSetPosition(file, nOffset, fPosition);
};


static RtFileStatus _rwSync(RtFile* file, RwBool block)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(file->fileSystem)->cppobj->rwSync(file, block);
};


static RwBool _rwAbort(RtFile* file)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(file->fileSystem)->cppobj->rwAbort(file);
};


static RwBool _rwEof(RtFile* file)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(file->fileSystem)->cppobj->rwEof(file);
};


static RtFileStatus _rwGetStatus(RtFile* file)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(file->fileSystem)->cppobj->rwGetStatus(file);
};


static RwBool _rwExists(RtFileSystem* fs, const RwChar* filename)
{
    return static_cast<CWebRwFS::RtFileSystemEx*>(fs)->cppobj->rwExists(fs, filename);
};


CWebRwFS::CWebRwFS(void)
: m_szFSName()
, m_szDeviceName()
, m_pFileSystem(nullptr)
, m_bRegist(false)
{
    m_szFSName[0] = '\0';
    m_szDeviceName[0] = '\0';
};


CWebRwFS::~CWebRwFS(void)
{
    ;
};


bool CWebRwFS::Initialize(const char* pszFSName, const char* pszDeviceName, int32 fileNum)
{
    ASSERT(fileNum > 0);

    ASSERT(pszFSName != nullptr);
    ASSERT(std::strlen(pszFSName) < sizeof(m_szFSName));

    std::strcpy(m_szFSName, pszFSName);

    ASSERT(pszDeviceName != nullptr);
    ASSERT(std::strlen(pszDeviceName) < sizeof(m_szDeviceName));

    std::strcpy(m_szDeviceName, pszDeviceName);

    m_pFileSystem = static_cast<RtFileSystemEx*>(RwMalloc(sizeof(*m_pFileSystem), rwMEMHINTDUR_GLOBAL));

    if (m_pFileSystem == nullptr)
        return false;

    m_pFileSystem->cppobj = this;

    m_pFileSystem->fsClose = _rwFileSystemClose;
    m_pFileSystem->fsGetDeviceName = _rwFileSystemGetDeviceName;
    m_pFileSystem->fsGetObject = _rwFileSystemGetFile;
    m_pFileSystem->fsFileFunc.open = _rwOpen;
    m_pFileSystem->fsFileFunc.close = _rwClose;
    m_pFileSystem->fsFileFunc.read = _rwRead;
    m_pFileSystem->fsFileFunc.write = _rwWrite;
    m_pFileSystem->fsFileFunc.setPosition = _rwSetPosition;
    m_pFileSystem->fsFileFunc.sync = _rwSync;
    m_pFileSystem->fsFileFunc.abort = _rwAbort;
    m_pFileSystem->fsFileFunc.isEOF = _rwEof;
    m_pFileSystem->fsFileFunc.getStatus = _rwGetStatus;
    m_pFileSystem->fsFileFunc.fExists = _rwExists;

    RtFileSystemError err = _rtFSInit(m_pFileSystem, fileNum, m_szFSName, m_szDeviceName);

    if (err != RTFS_ERROR_NOERROR)
        return false;

    if (!RtFSManagerRegister(m_pFileSystem))
        return false;

    m_bRegist = true;

    for (int32 i = 0; i < fileNum; ++i)
    {
        RtFile* file = m_pFileSystem->fsGetObject(m_pFileSystem, i);
        file->fileSystem = m_pFileSystem;
    };

    return true;
};


void CWebRwFS::Terminate(void)
{
    if (m_pFileSystem)
    {
        if (m_bRegist)
            RtFSManagerUnregister(m_pFileSystem);
        else
            RwFree(m_pFileSystem);
       
        m_pFileSystem = nullptr;
        m_bRegist = false;
    };
};


bool CWebRwFS::Sync(void)
{
    return true;
};


RtFileSystem* CWebRwFS::GetRtFS(void)
{
    return m_pFileSystem;
};