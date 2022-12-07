#include "AdxRwFile.hpp"
#include "AdxFileManager.hpp"
#include "AdxFileAccess.hpp"


/*static*/ bool CAdxRwFile::IsExists(const char* pszFilename)
{
    return CAdxFileAccess::IsExists(pszFilename);
};


CAdxRwFile::CAdxRwFile(void)
: m_uPosition(0)
{
    ;
};


CAdxRwFile::~CAdxRwFile(void)
{
    ;
};


bool CAdxRwFile::Open(int32 id)
{
    ASSERT(false);
    return false;
};


bool CAdxRwFile::Open(const char* pszFilename)
{
    if (CAdxFileISO::Open(pszFilename))
    {
        while (Status() == STATUS_READING)
            CAdxFileManager::Instance().Sync();

        return (Status() == STATUS_READEND);
    };

    return false;
};


void CAdxRwFile::Close(void)
{
	m_uPosition = 0;
    CAdxFileISO::Close();
};


uint32 CAdxRwFile::Read(void* buffer, uint32 size)
{
    uint32 uReaded = 0;
    
    const void* pBuff = Data();
    uint32 uSize = Size();

    ASSERT(pBuff);
    ASSERT(uSize);

    uint32 uBytesToRead = size;
    if (m_uPosition + size > uSize)
        uBytesToRead = uSize - m_uPosition;

    std::memcpy(buffer, &((const char*)pBuff)[m_uPosition], uBytesToRead);

    m_uPosition += uBytesToRead;
    uReaded = uBytesToRead;

    return uReaded;
};


bool CAdxRwFile::IsEof(void)
{
    return (m_uPosition >= Size());
};


uint32 CAdxRwFile::Seek(int32 pos, SEEKTYPE type)
{
    switch (type)
    {
    case SEEKTYPE_BEGIN:
        m_uPosition = pos;
        break;

    case SEEKTYPE_CURRENT:
        m_uPosition += pos;
        break;

    case SEEKTYPE_END:
        m_uPosition = Size() + pos;
        break;

    default:
        ASSERT(false);
        break;
    };

    return m_uPosition;
};


bool CAdxRwFile::IsOpen(void)
{
    return (Status() == STATUS_READEND);
};