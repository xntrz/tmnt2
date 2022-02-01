#include "AfsRwFile.hpp"
#include "AfsFileManager.hpp"
#include "AfsFileAccess.hpp"


/*static*/ bool CAfsRwFile::IsExists(const char* pszFilename)
{
    return CAfsFileAccess::IsExists(pszFilename);
};


CAfsRwFile::CAfsRwFile(void)
: m_uPosition(0)
{
    ;
};


CAfsRwFile::~CAfsRwFile(void)
{
    ;
};


bool CAfsRwFile::Open(int32 id)
{
    ASSERT(false);
    return false;
};


bool CAfsRwFile::Open(const char* pszFilename)
{
    if (CAfsFileISO::Open(pszFilename))
    {
        while (Status() == STATUS_READING)
            CAfsFileManager::Instance().Sync();

        return (Status() == STATUS_READEND);
    };

    return false;
};


void CAfsRwFile::Close(void)
{
	m_uPosition = 0;
    CAfsFileISO::Close();
};


uint32 CAfsRwFile::Read(void* buffer, uint32 size)
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


bool CAfsRwFile::IsEof(void)
{
    return (m_uPosition >= Size());
};


uint32 CAfsRwFile::Seek(int32 pos, SEEKTYPE type)
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


bool CAfsRwFile::IsOpen(void)
{
    return (Status() == STATUS_READEND);
};