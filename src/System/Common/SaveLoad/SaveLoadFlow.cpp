#include "SaveLoadFlow.hpp"

#if defined(TARGET_PC)
#include "System/PC/SaveLoad/PCSaveLoadFlow.hpp"
#elif defined(TARGET_WEB)
#include "System/Web/SaveLoad/WebSaveLoadFlow.hpp"
#endif


/*static*/ bool CSaveLoadFlow::m_bNewSave = false;


/*static*/ CSaveLoadFlow* CSaveLoadFlow::Create(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData)
{
    CSaveLoadFlow* pFlow = nullptr;
    
#if defined(TARGET_PC)
    pFlow = new CPCSaveLoadFlow(mode, pFrame, pData);
#elif defined(TARGET_WEB)
    pFlow = new CWebSaveLoadFlow(mode, pFrame, pData);
#endif

    return pFlow;
};


/*static*/ void CSaveLoadFlow::Destroy(CSaveLoadFlow* pFlow)
{
    delete pFlow;
};


CSaveLoadFlow::CSaveLoadFlow(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData)
: m_mode(mode)
, m_pFrame(pFrame)
, m_pData(pData)
{
    ASSERT(m_pFrame);
    ASSERT(m_pData);
};


CSaveLoadFlow::~CSaveLoadFlow(void)
{
    ;
};

