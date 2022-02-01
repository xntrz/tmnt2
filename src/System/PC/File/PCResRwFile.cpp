#include "PCResRwFile.hpp"
#include "PCFileManager.hpp"
#include "PCFilename.hpp"


/*static*/ bool CPCResRwFile::IsExists(const char* pszFilename)
{
    int32 nRCFID = -1;
    
    if (CPCFilename::RCFileID(pszFilename, nRCFID))
        return true;
    else
        return false;
};


CPCResRwFile::CPCResRwFile(void)
: CPCFile(CPCFileManager::LABELPC_RESID)
, m_uPosition(0)
{
    ;
};


CPCResRwFile::~CPCResRwFile(void)
{
    ;
};


bool CPCResRwFile::Open(int32 id)
{
    ASSERT(false);
    return false;
};


bool CPCResRwFile::Open(const char* pszFilename)
{
    int32 nRCFID = 0;
    if (CPCFilename::RCFileID(pszFilename, nRCFID))
    {
        if (CPCFile::Open(nRCFID))
        {
            while (Status() == STATUS_READING)
                CPCFileManager::Instance().Sync();

            return (Status() == STATUS_READEND);
        };
    };

    return false;
};


void CPCResRwFile::Close(void)
{
    m_uPosition = 0;
    CPCFile::Close();
};


uint32 CPCResRwFile::Read(void* buffer, uint32 size)
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


bool CPCResRwFile::IsEof(void)
{
    return (m_uPosition >= Size());
};


uint32 CPCResRwFile::Seek(int32 pos, SEEKTYPE type)
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


bool CPCResRwFile::IsOpen(void)
{
    return (Status() == STATUS_READEND);
};