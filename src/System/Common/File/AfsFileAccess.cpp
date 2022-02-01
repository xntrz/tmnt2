#include "AfsFileAccess.hpp"
#include "AfsFileSystem.hpp"
#include "AfsFilename.hpp"
#include "AfsFileID.hpp"


/*static*/ bool CAfsFileAccess::IsExists(const char* pszFilename)
{
    ASSERT(strlen(pszFilename) < CAfsFileSystem::FNAME_MAX);

    if (CAfsFilename::FileID(pszFilename) == AFSFILEID::ID_INVALID)    
        return false;

    char szFilename[CAfsFileSystem::FNAME_MAX];
    szFilename[0] = '\0';

    CAfsFilename::ConvPathPlatform(szFilename);

    int32 ptid = -1;
    int32 phyid = -1;
    CAfsFilename::MapReadInfo(pszFilename, &phyid, &ptid);

    if (ptid == -1 || phyid == -1)
    {
        ASSERT(false);
        return false;
    };

    return (CAfsFileSystem::FileSize(ptid, phyid) > 0);
};


CAfsFileAccess::CAfsFileAccess(void)
: m_uReferenceCount(0)
{
    ;
};


CAfsFileAccess::~CAfsFileAccess(void)
{
    ;
};


bool CAfsFileAccess::Read(const char* pszName)
{
    if (m_status > STATUS_PENDING)
    {
        ++m_uReferenceCount;
        return true;
    };

    ++m_uReferenceCount;

    int32 ptid = -1;
    int32 phyid = -1;

    CAfsFilename::MapReadInfo(pszName, &phyid, &ptid);

    m_status = STATUS_READING;
    return ReadCommon(ptid, phyid);
};


bool CAfsFileAccess::Read(int32 nID)
{
    if (m_status > STATUS_PENDING)
    {
        ++m_uReferenceCount;
        return true;
    };

    ++m_uReferenceCount;
    
    int32 ptid = -1;
    int32 phyid = -1;

    CAfsFilename::MapReadInfo(nID, &phyid, &ptid);

    m_status = STATUS_READING;
    return ReadCommon(ptid, phyid);
};


void CAfsFileAccess::Clear(void)
{
    ASSERT(m_uReferenceCount > 0);

    if (!--m_uReferenceCount)
    {
        if (m_pBuffer)
        {
            delete [] m_pBuffer;
            m_pBuffer = nullptr;
            m_uBufferSize = 0;

            m_status = STATUS_NOREAD;
        };
    };    
};


void CAfsFileAccess::Sync(void)
{
    ;
};


bool CAfsFileAccess::ReadCommon(int32 ptid, int32 phyid)
{
    ASSERT(ptid != -1);
    ASSERT(phyid != -1);

    bool bResult = false;

    if (ptid == -1 || phyid == -1)
        return bResult;

    m_uBufferSize = CAfsFileSystem::FileSize(ptid, phyid);
    ASSERT(m_uBufferSize);
    
    if(m_uBufferSize)
    {
        m_pBuffer = new char[m_uBufferSize];
        ASSERT(m_pBuffer);
        if (m_pBuffer)
        {
            if (CAfsFileSystem::FileRead(ptid, phyid, m_pBuffer, m_uBufferSize))
            {
                m_status = STATUS_READEND;
                bResult = true;
            }
            else
            {
                Clear();
            };
        };
    };

    return bResult;
};