#include "AdxFileAccess.hpp"
#include "Filename.hpp"
#include "FileID.hpp"
#include "FileTypes.hpp"

#include "cri_adxf.h"


/*static*/ ADXF CAdxFileAccess::m_adxf = 0;


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


bool CAdxFileAccess::Open(const char* name)
{
    bool bResult = true;

    if (!m_uReferenceCount++)
    {
        m_adxf = ADXF_Open(name, nullptr);
        bResult = (m_adxf ? ReadNoWait() : false);
    };
    
    return bResult;    
};


bool CAdxFileAccess::Open(int32 id)
{
    ASSERT(id >= 0);
    ASSERT(id < FILEID::ID_MAX);

	if (!m_uReferenceCount++)
	{
		int32 ptId = -1;
		int32 fileNo = -1;

		if (id < FILEID::COMMONMAX)
		{
			ptId = 0;
			fileNo = id;
		}
		else
		{
			ptId = 1;
			fileNo = (id - FILEID::LANGBEGIN);
		};

		m_adxf = ADXF_OpenAfs(ptId, fileNo);
		ASSERT(m_adxf != NULL);

		if (m_adxf)
			return ReadNoWait();

		m_stat = STAT_ERROR;
		return false;
	};

	return true;
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

        if (m_adxf)
        {
            ADXF_Close(m_adxf);
            m_adxf = 0;
        };

        m_stat = STAT_NOREAD;
    };
};


void CAdxFileAccess::Sync(void)
{
    if (m_adxf)
    {
        int32 AdxfStat = ADXF_GetStat(m_adxf);
        switch (AdxfStat)
        {
        case ADXF_STAT_STOP:
        case ADXF_STAT_ERROR:
            m_stat = STAT_ERROR;
            break;

        case ADXF_STAT_READING:
            m_stat = STAT_READING;
            break;

        case ADXF_STAT_READEND:
            m_stat = STAT_READEND;
            break;

        default:
            ASSERT(false);
            break;
        };
    }
    else
    {
        m_stat = STAT_ERROR;
    };    
};


bool CAdxFileAccess::ReadNoWait(void)
{
    int32 FSizeSct = ADXF_GetFsizeSct(m_adxf);
    int32 FSizeByte = ADXF_GetFsizeByte(m_adxf);

    m_pBuffer = new char[FSizeByte];
    m_uBufferSize = static_cast<uint32>(FSizeByte);
    
    if (ADXF_ReadNw(m_adxf, FSizeSct, m_pBuffer))
        m_stat = STAT_READING;
    else
        m_stat = STAT_ERROR;
    
    return (m_stat == STAT_READING);
};