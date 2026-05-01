#include "FileManager.hpp"
#include "File.hpp"
#include "Filename.hpp"


/*static*/ CFileManager* CFileManager::m_pInstance = nullptr;


/*static*/ CFileManager& CFileManager::Instance(void)
{
    ASSERT(m_pInstance);
    return *m_pInstance;
};


CFileManager::CFileManager(void)
: m_stat(STAT_READY)
, m_id(-1)
, m_pAccessData(nullptr)
, m_readQueue(32)
, m_pInfoTable(nullptr)
, m_rwFileSystem()
{
    m_pInfoTable = new CFileInfo[FILEID::FILEID_MAX];

    m_pInstance = this;
    CBaseFile::Initialize(this);
};


CFileManager::~CFileManager(void)
{
    CBaseFile::Terminate();
    m_pInstance = nullptr;

    if (m_pInfoTable)
    {
        delete[] m_pInfoTable;
        m_pInfoTable = nullptr;
    };
};


bool CFileManager::Start(void)
{
    return m_rwFileSystem.Initialize();
};


void CFileManager::Stop(void)
{
    m_rwFileSystem.Terminate();
};


void CFileManager::Sync(void)
{
    if (m_stat != STAT_BUSY)
        return;

    if (m_pAccessData)
    {
        FILE_STAT fstat = m_pAccessData->SyncStat();
        switch (fstat)
        {
        case FILE_STAT_ERROR:
            {
                char szErrBuf[256];
                szErrBuf[0] = '\0';

                std::sprintf(szErrBuf, "Failed to read file: \"%s\" (%d)", CFileAccess::GetLastFilename(), m_id);
                Error(szErrBuf);

                m_pAccessData->Close();
                m_pAccessData->ReleaseBuff();

                OpenFile(m_pAccessData, m_id);
            }
            break;

        case FILE_STAT_READEND:
            {
                ReadEnd(m_pAccessData);

                m_pAccessData->Close();
                m_pAccessData = nullptr;

                if (!m_readQueue.is_empty())
                {
                    READ_REQUEST request = m_readQueue.front();
                    m_readQueue.pop();

                    OpenFile(request.pData, request.id);
                };
            }
            break;

        default:
            break;
        };
    }
    else
    {
        if (!m_readQueue.is_empty())
        {
            READ_REQUEST request = m_readQueue.front();
            m_readQueue.pop();

            OpenFile(request.pData, request.id);
        }
        else
        {
            m_stat = STAT_READY;
        };
    };
};


void CFileManager::ReadEnd(CFileAccess* pData)
{
    ;
};


void CFileManager::ReadDataRequest(CFileAccess* pData, int32 id /*= -1*/)
{
    ASSERT(pData);

    READ_REQUEST request;
    request.pData = pData;
    request.id = id;

    ASSERT(!m_readQueue.is_full());
    m_readQueue.push(request);

    pData->SetAwait();

    m_stat = STAT_BUSY;
};


void CFileManager::OpenFile(CFileAccess* pData, int32 id)
{
    m_id = id;
    m_pAccessData = pData;

    m_pAccessData->Open(m_id);

    //OUTPUT("Reading \"%s\"\n", m_pAccessData->GetLastFilename());
};


CFileAccess* CFileManager::GetFileInfo(int32 id)
{
    ASSERT(id >= 0);
    ASSERT(id < FILEID::FILEID_MAX);
    
    return &m_pInfoTable[id];
};