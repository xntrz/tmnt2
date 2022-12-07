#include "AdxFileAccess.hpp"
#include "Filename.hpp"
#include "FileID.hpp"
#include "FileTypes.hpp"

#include "cri_adxf.h"


static ADXF s_adxf = 0;


/*static*/ bool CAdxFileAccess::IsExists(const char* pszFilename)
{
    ASSERT(std::strlen(pszFilename) < ADXF_FNAME_MAX);
    
    char szFilenameBuff[ADXF_FNAME_MAX];
    szFilenameBuff[0] = '\0';

    std::strcpy(szFilenameBuff, pszFilename);
    CFilename::ConvPathPlatform(szFilenameBuff);
    
    ADXF adxf = ADXF_Open(szFilenameBuff, nullptr);
    if (!adxf)
        return false;

    ADXF_Stop(adxf);
    ADXF_Close(adxf);
    return true;
};


CAdxFileAccess::CAdxFileAccess(void)
: m_uReferenceCount(0)
{
    ;
};


CAdxFileAccess::~CAdxFileAccess(void)
{
    ;
};


bool CAdxFileAccess::Read(const char* pszName)
{
    s_adxf = ADXF_Open(pszName, nullptr);
    if (!s_adxf)
        return false;
    
    return ReadNw();
};


bool CAdxFileAccess::Read(int32 Id)
{
    ASSERT(Id >= 0 && Id < FILEID::ID_MAX);

    int32 PtId = 0;
    int32 FileId = 0;
    
    if (Id >= 237)
    {
        PtId = 1;
        FileId = Id - 237;
    }
    else
    {
        PtId = 0;
		FileId = Id;
    };

    s_adxf = ADXF_OpenAfs(PtId, FileId);
    if (!s_adxf)
        return false;

    return ReadNw();
};


void CAdxFileAccess::Clear(void)
{
    ASSERT(m_uReferenceCount > 0);
    if (!--m_uReferenceCount)
    {
        if (m_pBuffer)
        {
            delete[] m_pBuffer;
            m_pBuffer = nullptr;
            m_uBufferSize = 0;
        };

        if (s_adxf)
        {
            ADXF_Close(s_adxf);
            s_adxf = 0;
        };

        m_status = STATUS_NOREAD;
    };
};


void CAdxFileAccess::Sync(void)
{
    if (s_adxf)
    {
        int32 AdxfStat = ADXF_GetStat(s_adxf);
        switch (AdxfStat)
        {
        case ADXF_STAT_STOP:
        case ADXF_STAT_ERROR:
            m_status = STATUS_ERROR;
            break;

        case ADXF_STAT_READING:
            m_status = STATUS_READING;
            break;

        case ADXF_STAT_READEND:
            m_status = STATUS_READEND;
            break;

        default:
            ASSERT(false);
            break;
        };
    }
    else
    {
        m_status = STATUS_ERROR;
    };    
};


bool CAdxFileAccess::ReadNw(void)
{
    if (m_status > STATUS_PENDING)
    {
        ++m_uReferenceCount;
        return true;
    };

    ++m_uReferenceCount;

    int32 FSizeSct = ADXF_GetFsizeSct(s_adxf);
    int32 FSizeByte = ADXF_GetFsizeByte(s_adxf);

    m_pBuffer = new char[FSizeByte];
    m_uBufferSize = FSizeByte;
    
    ADXF_ReadNw(s_adxf, FSizeSct, m_pBuffer);
    m_status = STATUS_READING;
    
    return true;
};