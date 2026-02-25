#include "File.hpp"
#include "FileManager.hpp"
#include "AdxFileManager.hpp"


/*static*/ CFileManager* CBaseFile::m_pFileManager = nullptr;


/*static*/ void CBaseFile::Initialize(CFileManager* pFileManager)
{
    m_pFileManager = pFileManager;
};


/*static*/ void CBaseFile::Terminate(void)
{
    m_pFileManager = nullptr;
};


/*static*/ bool CBaseFile::LoadPartition(int32 ptid, const char* fname, void* dir, void* ptinfo)
{
    return CAdxFileManager::LoadPt(ptid, fname, dir, ptinfo);
};


CBaseFile::CBaseFile(void)
: m_pAccessData(nullptr)
{
    ;
};


CBaseFile::~CBaseFile(void)
{
    ;
};


uint8* CBaseFile::GetData(void)
{
    if (m_pAccessData)
        return m_pAccessData->GetData();

    return nullptr;
};


const uint8* CBaseFile::GetData(void) const
{
    if (m_pAccessData)
        return m_pAccessData->GetData();

    return nullptr;
};


uint32 CBaseFile::GetSize(void) const
{
    if (m_pAccessData)
        return m_pAccessData->GetSize();

    return 0;
};


FILE_STAT CBaseFile::GetStat(void) const
{
    if (m_pAccessData)
        return m_pAccessData->GetStat();

    return FILE_STAT_NOREAD;
};


//
// *********************************************************************************
//


CIDFile::CIDFile(void)
{
    ;
};


CIDFile::~CIDFile(void)
{
    ;
};


void CIDFile::OpenRequest(int32 id)
{
    ASSERT(m_pFileManager);
    ASSERT(id >= 0);
    ASSERT(id < FILEID::FILEID_MAX);

    m_pAccessData = m_pFileManager->GetFileInfo(id);
    m_pFileManager->ReadDataRequest(m_pAccessData, id);
};


void CIDFile::Close(void)
{
    ASSERT(m_pAccessData);

    m_pAccessData->ReleaseBuff();
    m_pAccessData = nullptr;
};


//
// *********************************************************************************
//


CISOFile::CISOFile(void)
{
    ;
};


CISOFile::~CISOFile(void)
{
    ;
};


void CISOFile::OpenRequest(const char* pszFilename)
{
    ASSERT(m_pFileManager);

    m_pAccessData = new CFileAccessFname(pszFilename);
    m_pFileManager->ReadDataRequest(m_pAccessData);
};


void CISOFile::Close(void)
{
    ASSERT(m_pAccessData);

    m_pAccessData->ReleaseBuff();

    delete m_pAccessData;
    m_pAccessData = nullptr;
};


//
// *********************************************************************************
//


CFile::CFile(void)
: m_type(TYPE_UNKNOWN)
{
    ;
};


CFile::CFile(int32 id)
: m_type(TYPE_UNKNOWN)
{
    OpenRequest(id);
};


CFile::~CFile(void)
{
    if (m_pAccessData)
        Close();
};


void CFile::OpenRequest(int32 id)
{
    CIDFile::OpenRequest(id);
    m_type = TYPE_ID;
};


void CFile::OpenRequest(const char* pszFilename, bool bSearchID /*= true*/)
{
    if (bSearchID)
    {
        int32 id = CFilename::ID(pszFilename);
        if (id != -1)
        {
            CIDFile::OpenRequest(id);
            m_type = TYPE_ID;

            return;
        };
    };

    CISOFile::OpenRequest(pszFilename);
    m_type = TYPE_ISO;
};


bool CFile::Open(const char* pszFilename)
{
    OpenRequest(pszFilename);

    while (m_pAccessData->GetStat() == FILE_STAT_READING)
        m_pFileManager->Sync();

    return (m_pAccessData->GetStat() == FILE_STAT_READEND);
};


void CFile::Close(void)
{
    switch (m_type)
    {
    case TYPE_ID:
        CIDFile::Close();
        break;

    case TYPE_ISO:
        CISOFile::Close();
        break;

    default:
        ASSERT(false);
        break;
    };
};


CFile::TYPE CFile::GetType(void) const
{
    return m_type;
};