#include "AdxFileData.hpp"
#include "Filename.hpp"


static inline int32 GetPtFileID(int32 id)
{
    if (id < FILEID::COMMON_MAX)
        return id;

    return (id - FILEID::COMMON_MAX);
};


static inline int32 GetPtID(int32 id)
{
    return (id < FILEID::COMMON_MAX ? 0 : 1);
};


/*static*/ char CAdxFileAccess::m_szLastFilename[ADXF_FNAME_MAX];
/*static*/ ADXF CAdxFileAccess::m_adxf = NULL;


/*static*/ bool CAdxFileAccess::IsExist(const char* pszFilename)
{
    ASSERT(std::strlen(pszFilename) < ADXF_FNAME_MAX);

    char szFilename[ADXF_FNAME_MAX];
    std::strcpy(szFilename, pszFilename);
    CFilename::ConvPathPlatform(szFilename);

    ADXF adxf = ADXF_Open(szFilename, NULL);
    if (adxf != NULL)
    {
        ADXF_Stop(adxf);
        ADXF_Close(adxf);
        return true;
    };

    return false;
};


/*static*/ const char* CAdxFileAccess::GetLastFilename(void)
{
    return m_szLastFilename;
};


CAdxFileAccess::CAdxFileAccess(void)
: m_pBuff(nullptr)
, m_uSize(0)
{
    ;
};


CAdxFileAccess::~CAdxFileAccess(void)
{
    ASSERT(m_pBuff == nullptr);
};


bool CAdxFileAccess::Open(int32 id)
{
    int32 ptid = GetPtID(id);
    int32 fid = GetPtFileID(id);

#ifdef _DEBUG
    std::strcpy(m_szLastFilename, CFilename::FullName(id));
#endif /* _DEBUG */    

    m_adxf = ADXF_OpenAfs(ptid, fid);
    if (m_adxf != NULL)
    {
        Read();
        return true;
    };

    return false;
};


bool CAdxFileAccess::Open(const char* pszFilename)
{
    std::strcpy(m_szLastFilename, pszFilename);

    m_adxf = ADXF_Open(m_szLastFilename, NULL);
    if (m_adxf != NULL)
    {
        Read();
        return true;
    };

    return false;
};


void CAdxFileAccess::Read(void)
{
    ASSERT(m_adxf != NULL);

    int32 fsizeSectors = ADXF_GetFsizeSct(m_adxf);
    int32 fsizeBytes = ADXF_GetFsizeByte(m_adxf);

    m_pBuff = new uint8[fsizeSectors * ADXF_DEF_SCT_SIZE];
    m_uSize = static_cast<uint32>(fsizeBytes);

    ADXF_ReadNw(m_adxf, fsizeSectors, m_pBuff);
};


void CAdxFileAccess::Close(void)
{
    ASSERT(m_adxf != NULL);

    if (m_adxf != NULL)
    {
        ADXF_Close(m_adxf);
        m_adxf = NULL;
    };
};


void CAdxFileAccess::ReleaseBuff(void)
{
    if (m_pBuff != nullptr)
    {
        delete[] m_pBuff;
        m_pBuff = nullptr;
        
        m_uSize = 0;
    };
};


int32 CAdxFileAccess::GetStat(void) const
{
    if (m_adxf != NULL)
        return ADXF_GetStat(m_adxf);

    return ADXF_STAT_ERROR;
};


uint8* CAdxFileAccess::GetData(void)
{
    return m_pBuff;
};


const uint8* CAdxFileAccess::GetData(void) const
{
    return m_pBuff;
};


uint32 CAdxFileAccess::GetSize(void) const
{
    return m_uSize;
};