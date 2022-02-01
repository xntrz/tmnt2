#include "PCSaveLoadFlow.hpp"
#include "PCSaveLoad.hpp"
#include "PCSaveLoadManager.hpp"


CPCSaveLoadFlow::CPCSaveLoadFlow(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData)
: CSaveLoadFlow(mode, pFrame, pData)
, m_pFlow(nullptr)
{
    ;
};


CPCSaveLoadFlow::~CPCSaveLoadFlow(void)
{
    ASSERT(!m_pFlow);
};


void CPCSaveLoadFlow::Initialize(void)
{
    CPCSaveLoadManager::Initialize(m_pFrame, m_pData);
    
    switch (m_mode)
    {
    case MODE_CHECK:
        m_pFlow = new CPCStartCheckFlow;
        break;
        
    case MODE_SAVE:
        m_pFlow = new CPCMenuSaveFlow;
        break;
        
    case MODE_LOAD:
        m_pFlow = new CPCMenuLoadFlow;
        break;
        
    case MODE_AUTOSAVE:
        m_pFlow = new CPCAutosaveFlow;
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CPCSaveLoadFlow::Terminate(void)
{    
    if (m_pFlow)
    {
        delete m_pFlow;
        m_pFlow = nullptr;
    };
    
    CPCSaveLoadManager::Terminate();
};


bool CPCSaveLoadFlow::Run(void)
{
    ASSERT(m_pFlow);

    if (!m_pFlow)
        return true;

    return m_pFlow->Proc();
};

