#include "PCFileAccess.hpp"


/*static*/ bool CPCPhyFileAccess::IsExists(const char* pszFilename)
{
    void* hFile = RwFopen(pszFilename, "rb");
    if (hFile)
    {
        RwFclose(hFile);
        return true;
    };

    return false;
};


bool CPCPhyFileAccess::Open(const char* pszFilename)
{
    bool bResult = false;

    void* hFile = RwFopen(pszFilename, "rb");
    if (hFile)
    {
        m_stat = STAT_READING;

        RwFseek(hFile, 0, SEEK_END);
        m_uBufferSize = uint32(RwFtell(hFile));
        RwFseek(hFile, 0, SEEK_SET);

        if (m_uBufferSize)
        {
            m_pBuffer = new char[m_uBufferSize];
            ASSERT(m_pBuffer);

            if (m_pBuffer)
            {
                uint32 readed = RwFread(m_pBuffer, sizeof(uint8), m_uBufferSize, hFile);
                bResult = (readed == m_uBufferSize);
                m_stat = STAT_READEND;
            };
        };

        RwFclose(hFile);
        hFile = nullptr;
    };

    if (!bResult)
        m_stat = STAT_ERROR;

    return bResult;
};


void CPCPhyFileAccess::Clear(void)
{
    if (m_pBuffer)
    {
        delete[] m_pBuffer;
        m_pBuffer = nullptr;        
        m_uBufferSize = 0;
        
        m_stat = STAT_NOREAD;
    };
};