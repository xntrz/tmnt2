#include "File.hpp"
#include "FileAccess.hpp"
#include "FileManager.hpp"


CFile::CFile(int32 nLabel)
: m_nLabel(nLabel)
, m_status(STATUS_NOREAD)
, m_pAccessData(nullptr)
{
    ;
};


CFile::~CFile(void)
{
    ASSERT(!m_pAccessData);    
};


bool CFile::Open(int32 id)
{
    m_pAccessData = CFileManager::Instance().AllocRequest(id, m_nLabel);
    ASSERT(m_pAccessData);
    return (m_pAccessData != nullptr);
};


bool CFile::Open(const char* pszName)
{
    m_pAccessData = CFileManager::Instance().AllocRequest(pszName, m_nLabel);
    ASSERT(m_pAccessData);
    return (m_pAccessData != nullptr);
};


void CFile::Close(void)
{
    if (m_pAccessData)
    {
        m_pAccessData->Clear();
        m_pAccessData = nullptr;

        m_status = STATUS_NOREAD;
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


CFile::STATUS CFile::Status(void)
{
    ASSERT(m_pAccessData);
    
    switch (m_pAccessData->Status())
    {
    case CFileAccess::STATUS_READING:
    case CFileAccess::STATUS_PENDING:
        m_status = STATUS_READING;
        break;
        
    case CFileAccess::STATUS_READEND:
        m_status = STATUS_READEND;
        break;
        
    case CFileAccess::STATUS_ERROR:
        m_status = STATUS_ERROR;
        break;

    default:
        ASSERT(false);
        m_status = STATUS_ERROR;
        break;
    };

    return m_status;
};