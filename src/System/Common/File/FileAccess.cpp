#include "FileAccess.hpp"


CFileAccess::CFileAccess(void)
: m_pBuffer(nullptr)
, m_uBufferSize(0)
, m_status(STATUS_NOREAD)
{
    ;
};


CFileAccess::~CFileAccess(void)
{
    ASSERT(!m_pBuffer);
    ASSERT(!m_uBufferSize);
};


bool CFileAccess::Read(const char* pszName)
{
    return false;
};


bool CFileAccess::Read(int32 nID)
{
    return false;
};


void CFileAccess::Clear(void)
{
    ;
};


void CFileAccess::Sync(void)
{
    ;
};


CFileAccess::STATUS CFileAccess::Status(void) const
{
    return m_status;
};


uint32 CFileAccess::Size(void) const
{
    return m_uBufferSize;
};


void* CFileAccess::Data(void) const
{
    return m_pBuffer;
};