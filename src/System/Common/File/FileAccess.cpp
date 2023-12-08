#include "FileAccess.hpp"


CFileAccess::CFileAccess(void)
: m_pBuffer(nullptr)
, m_uBufferSize(0)
, m_stat(STAT_NOREAD)
{
    ;
};


CFileAccess::~CFileAccess(void)
{
    ASSERT(!m_pBuffer);
    ASSERT(!m_uBufferSize);
};


bool CFileAccess::Open(const char* name)
{
    return false;
};


bool CFileAccess::Open(int32 id)
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


CFileAccess::STAT CFileAccess::Stat(void) const
{
    return m_stat;
};


uint32 CFileAccess::Size(void) const
{
    return m_uBufferSize;
};


void* CFileAccess::Data(void) const
{
    return m_pBuffer;
};