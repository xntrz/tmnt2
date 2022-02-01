#include "FileManager.hpp"
#include "FileAccess.hpp"
#include "FileTypes.hpp"
#include "RwFileManager.hpp"


/*static*/ CFileManager* CFileManager::m_pInstance = nullptr;


/*static*/ CFileManager& CFileManager::Instance(void)
{
    ASSERT(m_pInstance);
    return *m_pInstance;
};


void CFileManager::REQUEST::SetName(const char* pszName)
{
    ASSERT(m_type == TYPE_NAME);

    if (m_type == TYPE_NAME)
    {
        int32 nLen = std::strlen(pszName);
        ASSERT(nLen < FILETYPES::FILE_NAME_MAX);

        m_data.name = new char[nLen + 1];
        ASSERT(m_data.name);

        std::strcpy(m_data.name, pszName);
    };    
};


void CFileManager::REQUEST::ClearName(void)
{
    ASSERT(m_type == TYPE_NAME);

    if (m_type == TYPE_NAME)
    {
        ASSERT(m_data.name);
        
        delete[] m_data.name;
        m_data.name = nullptr;
    };
};


CFileManager::CFileManager(void)
: m_state(STATE_IDLE)
, m_RequestQueue(32)
, m_pCurrentRequest(nullptr)
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
    return CRwFileManager::Initialize();
};


void CFileManager::Stop(void)
{
    CRwFileManager::Terminate();
};


void CFileManager::Sync(void)
{
    if (m_state != STATE_LOADING)
        return;

    ASSERT(!m_RequestQueue.empty());
    
    if (!m_pCurrentRequest)
        m_pCurrentRequest = m_RequestQueue.peek();

    ASSERT(m_pCurrentRequest);

    if (m_pCurrentRequest)
    {
        CFileAccess* pAccess = m_pCurrentRequest->m_pAccessData;
        ASSERT(pAccess);

        switch (pAccess->Status())
        {
        case CFileAccess::STATUS_PENDING:
            {
                switch (m_pCurrentRequest->m_type)
                {
                case REQUEST::TYPE_ID:
                    pAccess->Read(m_pCurrentRequest->m_data.id);
                    break;

                case REQUEST::TYPE_NAME:
                    pAccess->Read(m_pCurrentRequest->m_data.name);
                    break;

                default:
                    ASSERT(false);
                    break;
                };
            }
            break;

        case CFileAccess::STATUS_READING:
            {
                pAccess->Sync();
            }
            break;

        case CFileAccess::STATUS_READEND:
        case CFileAccess::STATUS_NOREAD:
            {
                if (m_pCurrentRequest->m_type == REQUEST::TYPE_NAME)
                    m_pCurrentRequest->ClearName();
                
	            m_pCurrentRequest = nullptr;
                
                m_RequestQueue.pop();
                if (m_RequestQueue.empty())
                    m_state = STATE_IDLE;
            }
            break;

        case CFileAccess::STATUS_ERROR:
            {
                Error("Reading file error");

                if (m_pCurrentRequest->m_type == REQUEST::TYPE_NAME)
                    m_pCurrentRequest->ClearName();

                m_pCurrentRequest = nullptr;

                m_RequestQueue.pop();
                if (m_RequestQueue.empty())
                    m_state = STATE_IDLE;
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CFileManager::Error(const char* pszDescription)
{
    ;
};


void CFileManager::RegistRequest(const REQUEST& rRequest)
{
    CFileAccess* pAccess = rRequest.m_pAccessData;
    ASSERT(pAccess);
    pAccess->m_status = CFileAccess::STATUS_PENDING;
    
    m_RequestQueue.push(&rRequest);
    m_state = STATE_LOADING;
};


CFileAccess* CFileManager::AllocRequest(int32 nID, int32 nLabel)
{
    return nullptr;
};


CFileAccess* CFileManager::AllocRequest(const char* pszName, int32 nLabel)
{
    return nullptr;
};