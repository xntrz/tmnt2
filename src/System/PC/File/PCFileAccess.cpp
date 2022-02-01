#include "PCFileAccess.hpp"
#include "PCFileManager.hpp"
#include "PCFileStd.hpp"

#include "System/PC/PCTypedefs.hpp"


/*static*/ bool CPCPhysicalFileAccess::IsExists(const char* path)
{
    void* hFile = stdf_open(path, "rb");
    if (hFile)
    {
        stdf_close(hFile);
        hFile = nullptr;

        return true;
    };

    return false;
};


CPCPhysicalFileAccess::CPCPhysicalFileAccess(void)
: m_hFile(nullptr)
{
    ;
};


CPCPhysicalFileAccess::~CPCPhysicalFileAccess(void)
{
    ;
};


bool CPCPhysicalFileAccess::Read(const char* pszName)
{
    bool bResult = false;

    void* hFile = stdf_open(pszName, "rb");
    if (hFile)
    {
        m_status = STATUS_READING;

        stdf_seek(hFile, 0, stdf_seek_end);
        m_uBufferSize = stdf_tell(hFile);
        stdf_seek(hFile, 0, stdf_seek_begin);        

        if (m_uBufferSize)
        {
            m_pBuffer = new char[m_uBufferSize];
            ASSERT(m_pBuffer);
            if (m_pBuffer)
            {
                stdf_read(hFile, m_pBuffer, m_uBufferSize);
                
                m_status = STATUS_READEND;
                bResult = true;
            };
        }
        else
        {
            m_status = STATUS_ERROR;
        };

        stdf_close(hFile);
        hFile = nullptr;
    }
    else
    {
        m_status = STATUS_ERROR;
    };

    return bResult;
};


bool CPCPhysicalFileAccess::Read(int32 nID)
{
    ASSERT(false);
    return false;
};


void CPCPhysicalFileAccess::Clear(void)
{
    if (m_pBuffer)
    {
        delete[] m_pBuffer;
        m_pBuffer = nullptr;        
        m_uBufferSize = 0;
        
        m_status = STATUS_NOREAD;
    };
};


bool CPCPhysicalFileAccess::Write(const char* pszName, const char* data, int32 datalen)
{
    bool bResult = false;

    void* hFile = stdf_open(pszName, "wb");
    if (hFile)
    {
        m_status = STATUS_READING;

        stdf_write(hFile, data, datalen);
        m_status = STATUS_READEND;
        
        bResult = true;

        stdf_close(hFile);
        hFile = nullptr;
    }
    else
    {
        m_status = STATUS_ERROR;
    };

    return bResult;
};


CPCResFileAccess::CPCResFileAccess(void)
: m_hData(nullptr)
{
    ;
};


CPCResFileAccess::~CPCResFileAccess(void)
{
    ASSERT(!m_hData);
};


bool CPCResFileAccess::Read(const char* pszName)
{
    ASSERT(false);
    return false;
};


bool CPCResFileAccess::Read(int32 nID)
{
    bool bResult = false;

    try
    {
        m_status = STATUS_READING;
        HMODULE hModule = GetModuleHandle(NULL);
        ASSERT(hModule != NULL);

        HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(nID), TEXT("RAWFILE"));
        if (hResource == NULL)
            throw std::exception("FindResourceA");

        m_hData = LoadResource(hModule, hResource);
        if (m_hData == NULL)
            throw std::exception("LoadResource");

        m_uBufferSize = SizeofResource(hModule, hResource);
        m_pBuffer = LockResource(m_hData);
        ASSERT(m_pBuffer);

        m_status = STATUS_READEND;
        bResult = true;
    }
    catch (std::exception& e)
    {
		REF(e);

        DWORD dwError = GetLastError();
        OUTPUT("[SYS] Resource loading error: %s %u\n", e.what(), dwError);

        Clear();

        m_status = STATUS_ERROR;
    };

    return bResult;
};


void CPCResFileAccess::Clear(void)
{
    if (m_hData)
    {
        ASSERT(m_pBuffer);

        FreeResource(m_hData);
        m_hData = NULL;
        
        m_pBuffer = nullptr;
        m_uBufferSize = 0;
        
        m_status = STATUS_NOREAD;
    };
};