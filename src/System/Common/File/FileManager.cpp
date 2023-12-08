#include "FileManager.hpp"
#include "FileAccess.hpp"
#include "FileTypes.hpp"


CFileManager::CRequest::CRequest(const char* pszName, CFileAccess* pAccess)
: m_pAccess(pAccess)
, m_type(TYPE_NAME)
{
    m_data.name = new char[std::strlen(pszName) + 1];
    std::strcpy(m_data.name, pszName);
};


CFileManager::CRequest::CRequest(int32 nID, CFileAccess* pAccess)
: m_pAccess(pAccess)
, m_type(TYPE_ID)
{
    m_data.id = nID;
};


void CFileManager::CRequest::Cleanup(void)
{
    switch (m_type)
    {
    case TYPE_NAME:
        {
            delete[] m_data.name;
            m_data.name = nullptr;
        }
        break;

    case TYPE_ID:
        {
            m_data.id = -1;
        }
        break;

    default:
        break;
    };
};


/*static*/ CFileManager* CFileManager::m_pInstance = nullptr;


/*static*/ CFileManager& CFileManager::Instance(void)
{
    ASSERT(m_pInstance);
    return *m_pInstance;
};


CFileManager::CFileManager(void)
: m_state(STATE_IDLE)
, m_pReqCurrent(nullptr)
{
    ASSERT(!m_pInstance);
    m_pInstance = this;
};


CFileManager::~CFileManager(void)
{
    ASSERT(m_pInstance);
    ASSERT(m_pInstance == this);
    m_pInstance = nullptr;
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
    if (m_state != STATE_LOADING)
        return;

    ASSERT(!m_ReqQueue.empty());

    if (!m_pReqCurrent)
        m_pReqCurrent = &m_ReqQueue.front();

    CFileAccess* pAccess = m_pReqCurrent->access();
    switch (pAccess->Stat())
    {
    case CFileAccess::STAT_PENDING:
        {
            switch (m_pReqCurrent->type())
            {
            case CRequest::TYPE_ID:
                pAccess->Open(m_pReqCurrent->id());
                break;

            case CRequest::TYPE_NAME:
                pAccess->Open(m_pReqCurrent->name());
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CFileAccess::STAT_READING:
        {
            pAccess->Sync();
        }
        break;

    case CFileAccess::STAT_READEND:
    case CFileAccess::STAT_NOREAD:
    case CFileAccess::STAT_ERROR:
        {
            if (pAccess->Stat() == CFileAccess::STAT_ERROR)
            {
                ASSERT(false);
                
                char szBuffer[256];
                
                if (m_pReqCurrent->type() == CRequest::TYPE_ID)
                    std::sprintf(szBuffer, "file id %d read error!", m_pReqCurrent->id());
                else if (m_pReqCurrent->type() == CRequest::TYPE_NAME)
                    std::sprintf(szBuffer, "file \"%s\" read error!", m_pReqCurrent->name());
                else
                    std::sprintf(szBuffer, "file read error!");
                
                Error(szBuffer);
            };

            m_pReqCurrent->Cleanup();
            m_pReqCurrent = nullptr;

            m_ReqQueue.pop();
            if (m_ReqQueue.empty())
                m_state = STATE_IDLE;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CFileManager::RegistRequest(CRequest& Request)
{
    Request.access()->m_stat = CFileAccess::STAT_PENDING;
    
    m_ReqQueue.push(Request);
    m_state = STATE_LOADING;
};