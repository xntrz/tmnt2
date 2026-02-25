#include "FileData.hpp"
#include "Filename.hpp"


/*static*/ bool CFileAccess::IsExist(const char* pszFilename, bool bCheckID /*= true*/)
{
    if (bCheckID)
    {
        if (CFilename::ID(pszFilename) != -1)
            return true;
    };

    return CAdxFileAccess::IsExist(pszFilename);
};


CFileAccess::CFileAccess(void)
: m_stat(FILE_STAT_NOREAD)
{
    ;
};


CFileAccess::~CFileAccess(void)
{
    ;
};


bool CFileAccess::Open(int32 id)
{
    if (CAdxFileAccess::Open(id))
        m_stat = FILE_STAT_READING;
    else
        m_stat = FILE_STAT_ERROR;

    return (m_stat == FILE_STAT_READING);
};


bool CFileAccess::Open(const char* pszFilename)
{
    if (CAdxFileAccess::Open(pszFilename))
        m_stat = FILE_STAT_READING;
    else
        m_stat = FILE_STAT_ERROR;

    return (m_stat == FILE_STAT_READING);
};


FILE_STAT CFileAccess::SyncStat(void)
{
    int32 stat = CAdxFileAccess::GetStat();
    switch (stat)
    {
    case ADXF_STAT_STOP:
    case ADXF_STAT_ERROR:
        m_stat = FILE_STAT_ERROR;
        break;

    case ADXF_STAT_READEND:
        m_stat = FILE_STAT_READEND;
        break;

    case ADXF_STAT_READING:
        m_stat = FILE_STAT_READING;
        break;

    default:
        ASSERT(false);
        break;
    };

    return m_stat;
};


void CFileAccess::ReleaseBuff(void)
{
    CAdxFileAccess::ReleaseBuff();
    m_stat = FILE_STAT_NOREAD;
};


FILE_STAT CFileAccess::GetStat(void) const
{
    return m_stat;
};


bool CFileAccess::IsReadEnd(void) const
{
    return (CAdxFileAccess::GetStat() == FILE_STAT_READEND);
};


void CFileAccess::SetAwait(void)
{
    m_stat = FILE_STAT_READING;
};


//
// *********************************************************************************
//


CFileInfo::CFileInfo(void)
: m_uRefCount(0)
{
    ;
};


CFileInfo::~CFileInfo(void)
{
    ;
};


bool CFileInfo::Open(int32 id)
{
    if (!m_uRefCount)
        return CFileAccess::Open(id);

    ++m_uRefCount;    
    return true;
};


FILE_STAT CFileInfo::SyncStat(void)
{
    FILE_STAT fstat = CFileAccess::SyncStat();
    if (fstat == FILE_STAT_READEND)
        ++m_uRefCount;

    return fstat;
};


void CFileInfo::ReleaseBuff(void)
{
    if (m_uRefCount > 0)
    {
        if (!--m_uRefCount)
            CFileAccess::ReleaseBuff();
    };
};


//
// *********************************************************************************
//


CFileAccessFname::CFileAccessFname(const char* pszFilename)
: m_szFilename()
{
    std::strcpy(m_szFilename, pszFilename);
};


CFileAccessFname::~CFileAccessFname(void)
{
    ;
};


bool CFileAccessFname::Open(int32 id /*= -1*/)
{
    if (id == -1)
        return CFileAccess::Open(m_szFilename);

    return CFileAccess::Open(id);
};