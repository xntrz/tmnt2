#include "SaveLoadFlow.hpp"

#include "System/PC/SaveLoad/PCSaveLoadFlow.hpp"


/*static*/ bool CSaveLoadFlow::m_bNewSave = false;


/*static*/ CSaveLoadFlow* CSaveLoadFlow::Create(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData)
{
    CSaveLoadFlow* pFlow = nullptr;
    
#ifdef TARGET_PC
    pFlow = new CPCSaveLoadFlow(mode, pFrame, pData);
#endif /* TARGET_PC */

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

