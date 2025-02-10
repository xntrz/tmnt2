#include "PCSaveLoad.hpp"
#include "PCSaveLoadManager.hpp"

#include "System/Common/SaveLoad/SaveLoadData.hpp"
#include "System/Common/SaveLoad/SaveLoadFrame.hpp"


/*static*/ CPCSaveManager* CPCSaveLoadBaseFlow::m_pSave = nullptr;
/*static*/ CPCLoadManager* CPCSaveLoadBaseFlow::m_pLoad = nullptr;
/*static*/ CPCQuestManager* CPCSaveLoadBaseFlow::m_pQuest = nullptr;
/*static*/ CPCWarningManager* CPCSaveLoadBaseFlow::m_pWarning = nullptr;


CPCSaveLoadBaseFlow::CPCSaveLoadBaseFlow(void)
: m_step(-1)
{
    ASSERT(!m_pSave);
    ASSERT(!m_pLoad);
    ASSERT(!m_pQuest);
    ASSERT(!m_pWarning);
};


CPCSaveLoadBaseFlow::~CPCSaveLoadBaseFlow(void)
{
    ASSERT(!m_pSave);
    ASSERT(!m_pLoad);
    ASSERT(!m_pQuest);
    ASSERT(!m_pWarning);
};


CPCStartCheckFlow::CPCStartCheckFlow(void)
{
    m_step = STEP_INIT;

    CPCSaveLoadManager::InitializeFrame(CPCSaveLoadManager::MESSAGEID_LOAD_CHECK,
                                        CPCSaveLoadManager::TITLEID_CHECK);
};


CPCStartCheckFlow::~CPCStartCheckFlow(void)
{
    CPCSaveLoadManager::TerminateFrame();
};


bool CPCStartCheckFlow::Proc(void)
{
    bool bResult = false;
    
    switch (m_step)
    {
    case STEP_INIT:
        {
            m_pLoad = new CPCLoadManager;
            m_step = STEP_RUN;
        }
        break;

    case STEP_RUN:
        {
            if (m_pLoad->Proc())
            {
                delete m_pLoad;
                m_pLoad = nullptr;

                m_step = STEP_EOL;
            };
        }
        break;

    case STEP_EOL:
        {
            bResult = true;
        }        
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


//
// *********************************************************************************
//


CPCMenuSaveFlow::CPCMenuSaveFlow(void)
{
    m_step = STEP_QUEST_INIT;

    CPCSaveLoadManager::InitializeFrame(CPCSaveLoadManager::MESSAGEID_SAVE_SURE,
                                        CPCSaveLoadManager::TITLEID_SAVE);
};


CPCMenuSaveFlow::~CPCMenuSaveFlow(void)
{
    CPCSaveLoadManager::TerminateFrame();
};


bool CPCMenuSaveFlow::Proc(void)
{
    bool bResult = false;

    switch (m_step)
    {
    case STEP_QUEST_INIT:
        {
            m_pQuest = new CPCQuestManager(CPCSaveLoadManager::MESSAGEID_SAVE_SURE);
            m_step = STEP_QUEST;
        }
        break;

    case STEP_QUEST:
        {
            CPCQuestManager::RESULT result = m_pQuest->Proc();
            switch (result)
            {
            case CPCQuestManager::RESULT_NO:
                m_step = STEP_EOL;
                break;
                
            case CPCQuestManager::RESULT_YES:
                m_step = STEP_RUN_INIT;
                break;

            default:
                break;
            };

            if (result != CPCQuestManager::RESULT_NONE)
            {
                delete m_pQuest;
                m_pQuest = nullptr;
            };
        }
        break;

    case STEP_RUN_INIT:
        {
            m_pSave = new CPCSaveManager;
            m_step = STEP_RUN;
        }
        break;

    case STEP_RUN:
        {
            if (m_pSave->Proc())
            {
                delete m_pSave;
                m_pSave = nullptr;

                m_step = STEP_EOL;
            };
        }
        break;

    case STEP_EOL:
        {
            bResult = true;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


//
// *********************************************************************************
//


CPCMenuLoadFlow::CPCMenuLoadFlow(void)
{
    m_step = STEP_QUEST_INIT;

    CPCSaveLoadManager::InitializeFrame(CPCSaveLoadManager::MESSAGEID_LOAD_SURE,
                                        CPCSaveLoadManager::TITLEID_LOAD);
};


CPCMenuLoadFlow::~CPCMenuLoadFlow(void)
{
    CPCSaveLoadManager::TerminateFrame();
};


bool CPCMenuLoadFlow::Proc(void)
{
    bool bResult = false;

    switch (m_step)
    {
    case STEP_QUEST_INIT:
        {
            m_pQuest = new CPCQuestManager(CPCSaveLoadManager::MESSAGEID_LOAD_SURE);
            m_step = STEP_QUEST;
        }
        break;

    case STEP_QUEST:
        {
            CPCQuestManager::RESULT result = m_pQuest->Proc();
            switch (result)
            {
            case CPCQuestManager::RESULT_NO:
                m_step = STEP_EOL;
                break;

            case CPCQuestManager::RESULT_YES:
                m_step = STEP_RUN_INIT;
                break;

            default:
                break;
            };

            if (result != CPCQuestManager::RESULT_NONE)
            {
                delete m_pQuest;
                m_pQuest = nullptr;
            };
        }
        break;

    case STEP_RUN_INIT:
        {
            m_pLoad = new CPCLoadManager;
            m_step = STEP_RUN;
        }
        break;

    case STEP_RUN:
        {
            if (m_pLoad->Proc())
            {
                delete m_pLoad;
                m_pLoad = nullptr;

                m_step = STEP_EOL;
            };
        }
        break;

    case STEP_EOL:
        {
            bResult = true;
        }        
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


//
// *********************************************************************************
//


CPCAutosaveFlow::CPCAutosaveFlow(void)
{
    m_step = STEP_INIT;

    CPCSaveLoadManager::InitializeFrame(CPCSaveLoadManager::MESSAGEID_SAVE_NOW,
                                        CPCSaveLoadManager::TITLEID_SAVE);
};


CPCAutosaveFlow::~CPCAutosaveFlow(void)
{
    CPCSaveLoadManager::TerminateFrame();
};


bool CPCAutosaveFlow::Proc(void)
{
    bool bResult = false;

    switch (m_step)
    {
    case STEP_INIT:
        {
            m_pSave = new CPCSaveManager;
            m_step = STEP_RUN;
        }
        break;

    case STEP_RUN:
        {
            if (m_pSave->Proc())
            {
                delete m_pSave;
                m_pSave = nullptr;

                m_step = STEP_EOL;
            };
        }
        break;

    case STEP_EOL:
        {
            bResult = true;
        }        
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};