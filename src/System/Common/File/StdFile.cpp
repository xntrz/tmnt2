#include "StdFile.hpp"

#include "AdxFileManager.hpp"


/*static*/ bool CStdFile::exist(const char* pszFilename)
{
    return CFileAccess::IsExist(pszFilename);
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


bool CStdFile::open(const char* pszFilename)
{
    m_uPosition = 0;
    return CFile::Open(pszFilename);
};


void CStdFile::close(void)
{
    CFile::Close();
};


uint32 CStdFile::read(void* buffer, uint32 size)
{
    uint32 fsize = m_pAccessData->GetSize();
    const uint8* fdata = m_pAccessData->GetData();
    
    uint32 cbToRead = size;
    if ((m_uPosition + size) > fsize)
        cbToRead = fsize - m_uPosition;

    std::memcpy(buffer, &fdata[m_uPosition], cbToRead);
    m_uPosition += cbToRead;

    return cbToRead;
};


bool CStdFile::eof(void) const
{
    return (m_uPosition >= m_pAccessData->GetSize());
};


uint32 CStdFile::seek(uint32 offset, int32 type)
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
        m_uPosition = static_cast<uint32>(static_cast<int32>(m_pAccessData->GetSize()) + offset);
        break;

    default:
        ASSERT(false);
        break;
    };

    return m_uPosition;
};


uint32 CStdFile::tell(void) const
{
    return m_uPosition;
};