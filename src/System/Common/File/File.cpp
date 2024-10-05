#include "File.hpp"
#include "FileAccess.hpp"
#include "FileManager.hpp"


CFile::CFile(void)
: m_stat(STAT_NOREAD)
, m_pAccessData(nullptr)
{
    ;
};


CFile::~CFile(void)
{
    ASSERT(!m_pAccessData);    
};


bool CFile::Open(int32 nType, void* pTypeData)
{
    m_pAccessData = CFileManager::Instance().AllocRequest(nType, pTypeData);
    ASSERT(m_pAccessData);
    
    return (m_pAccessData != nullptr);
};


void CFile::Close(void)
{
    if (m_pAccessData)
    {
        m_pAccessData->Clear();
        m_pAccessData = nullptr;

        m_stat = STAT_NOREAD;
    };
};


void* CFile::Data(void) const
{
    if (m_pAccessData)
        return m_pAccessData->Data();
    else
        return nullptr;
};


uint32 CFile::Size(void) const
{
    if (m_pAccessData)
        return m_pAccessData->Size();
    else
        return 0;
};


CFile::STAT CFile::Stat(void)
{
    ASSERT(m_pAccessData);
    
    switch (m_pAccessData->Stat())
    {
    case CFileAccess::STAT_READING:
    case CFileAccess::STAT_PENDING:
        m_stat = STAT_READING;
        break;
        
    case CFileAccess::STAT_READEND:
        m_stat = STAT_READEND;
        break;
        
    case CFileAccess::STAT_ERROR:
        m_stat = STAT_ERROR;
        break;

    default:
        ASSERT(false);
        m_stat = STAT_ERROR;
        break;
    };

    return m_stat;
};