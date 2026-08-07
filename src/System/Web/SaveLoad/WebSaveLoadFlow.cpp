#include "WebSaveLoadFlow.hpp"
#include "WebSaveLoad.hpp"
#include "WebSaveLoadManager.hpp"


CWebSaveLoadFlow::CWebSaveLoadFlow(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData)
: CSaveLoadFlow(mode, pFrame, pData)
, m_pFlow(nullptr)
{
    ;
};


CWebSaveLoadFlow::~CWebSaveLoadFlow(void)
{
    ASSERT(!m_pFlow);
};


void CWebSaveLoadFlow::Initialize(void)
{
    CWebSaveLoadManager::Initialize(m_pFrame, m_pData);
    
    switch (m_mode)
    {
    case MODE_CHECK:
        m_pFlow = new CWebStartCheckFlow;
        break;
        
    case MODE_SAVE:
        m_pFlow = new CWebMenuSaveFlow;
        break;
        
    case MODE_LOAD:
        m_pFlow = new CWebMenuLoadFlow;
        break;
        
    case MODE_AUTOSAVE:
        m_pFlow = new CWebAutosaveFlow;
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CWebSaveLoadFlow::Terminate(void)
{    
    if (m_pFlow)
    {
        delete m_pFlow;
        m_pFlow = nullptr;
    };
    
    CWebSaveLoadManager::Terminate();
};


bool CWebSaveLoadFlow::Run(void)
{
    ASSERT(m_pFlow);

    if (!m_pFlow)
        return true;

    return m_pFlow->Proc();
};

