#include "StdFile.hpp"

#include "AdxFileManager.hpp"


/*static*/ bool CStdFile::IsExists(const char* pszFilename)
{
    return CAdxFileAccess::IsExists(pszFilename);
};


CStdFile::CStdFile(void)
: m_uPosition(0)
{
    ;
};


CStdFile::~CStdFile(void)
{
    ;
};


bool CStdFile::Open(const char* pszFilename)
{
    if (CAdxFileISO::Open(pszFilename))
    {
        while (Stat() == STAT_READING)
            CAdxFileManager::Instance().Sync();

        return (Stat() == STAT_READEND);
    };

    return false;
};


void CStdFile::Close(void)
{
    m_uPosition = 0;
    CAdxFileISO::Close();
};


uint32 CStdFile::Read(void* buffer, uint32 size)
{
    uint32 uReaded = 0;

    const void* pBuff = Data();
    uint32 uSize = Size();

    ASSERT(pBuff);
    ASSERT(uSize);

    uint32 uBytesToRead = size;
    if (m_uPosition + size > uSize)
        uBytesToRead = uSize - m_uPosition;

    std::memcpy(buffer, &static_cast<const char*>(pBuff)[m_uPosition], uBytesToRead);

    m_uPosition += uBytesToRead;
    uReaded = uBytesToRead;

    return uReaded;
};


bool CStdFile::IsEof(void)
{
    return (m_uPosition >= Size());
};


uint32 CStdFile::Seek(uint32 offset, int32 type)
{
    switch (type)
    {
    case SEEK_SET:
        m_uPosition = offset;
        break;

    case SEEK_CUR:
        m_uPosition += offset;
        break;

    case SEEK_END:
        m_uPosition = Size() + offset;
        break;

    default:
        ASSERT(false);
        break;
    };

    return m_uPosition;
};


bool CStdFile::IsOpen(void)
{
    return (Stat() == STAT_READEND);
};