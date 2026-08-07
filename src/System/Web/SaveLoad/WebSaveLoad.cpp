#include "WebSaveLoad.hpp"
#include "WebSaveLoadManager.hpp"

#include "System/Common/SaveLoad/SaveLoadData.hpp"
#include "System/Common/SaveLoad/SaveLoadFrame.hpp"


/*static*/ CWebSaveManager* CWebSaveLoadBaseFlow::m_pSave = nullptr;
/*static*/ CWebLoadManager* CWebSaveLoadBaseFlow::m_pLoad = nullptr;
/*static*/ CWebQuestManager* CWebSaveLoadBaseFlow::m_pQuest = nullptr;
/*static*/ CWebWarningManager* CWebSaveLoadBaseFlow::m_pWarning = nullptr;


CWebSaveLoadBaseFlow::CWebSaveLoadBaseFlow(void)
: m_step(-1)
{
    ASSERT(!m_pSave);
    ASSERT(!m_pLoad);
    ASSERT(!m_pQuest);
    ASSERT(!m_pWarning);
};


CWebSaveLoadBaseFlow::~CWebSaveLoadBaseFlow(void)
{
    ASSERT(!m_pSave);
    ASSERT(!m_pLoad);
    ASSERT(!m_pQuest);
    ASSERT(!m_pWarning);
};


CWebStartCheckFlow::CWebStartCheckFlow(void)
{
    m_step = STEP_INIT;

    CWebSaveLoadManager::InitializeFrame(CWebSaveLoadManager::MESSAGEID_LOAD_CHECK,
                                        CWebSaveLoadManager::TITLEID_CHECK);
};


CWebStartCheckFlow::~CWebStartCheckFlow(void)
{
    CWebSaveLoadManager::TerminateFrame();
};


bool CWebStartCheckFlow::Proc(void)
{
    bool bResult = false;
    
    switch (m_step)
    {
    case STEP_INIT:
        {
            m_pLoad = new CWebLoadManager;
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


CWebMenuSaveFlow::CWebMenuSaveFlow(void)
{
    m_step = STEP_QUEST_INIT;

    CWebSaveLoadManager::InitializeFrame(CWebSaveLoadManager::MESSAGEID_SAVE_SURE,
                                        CWebSaveLoadManager::TITLEID_SAVE);
};


CWebMenuSaveFlow::~CWebMenuSaveFlow(void)
{
    CWebSaveLoadManager::TerminateFrame();
};


bool CWebMenuSaveFlow::Proc(void)
{
    bool bResult = false;

    switch (m_step)
    {
    case STEP_QUEST_INIT:
        {
            m_pQuest = new CWebQuestManager(CWebSaveLoadManager::MESSAGEID_SAVE_SURE);
            m_step = STEP_QUEST;
        }
        break;

    case STEP_QUEST:
        {
            CWebQuestManager::RESULT result = m_pQuest->Proc();
            switch (result)
            {
            case CWebQuestManager::RESULT_NO:
                m_step = STEP_EOL;
                break;
                
            case CWebQuestManager::RESULT_YES:
                m_step = STEP_RUN_INIT;
                break;

            default:
                break;
            };

            if (result != CWebQuestManager::RESULT_NONE)
            {
                delete m_pQuest;
                m_pQuest = nullptr;
            };
        }
        break;

    case STEP_RUN_INIT:
        {
            m_pSave = new CWebSaveManager;
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


CWebMenuLoadFlow::CWebMenuLoadFlow(void)
{
    m_step = STEP_QUEST_INIT;

    CWebSaveLoadManager::InitializeFrame(CWebSaveLoadManager::MESSAGEID_LOAD_SURE,
                                        CWebSaveLoadManager::TITLEID_LOAD);
};


CWebMenuLoadFlow::~CWebMenuLoadFlow(void)
{
    CWebSaveLoadManager::TerminateFrame();
};


bool CWebMenuLoadFlow::Proc(void)
{
    bool bResult = false;

    switch (m_step)
    {
    case STEP_QUEST_INIT:
        {
            m_pQuest = new CWebQuestManager(CWebSaveLoadManager::MESSAGEID_LOAD_SURE);
            m_step = STEP_QUEST;
        }
        break;

    case STEP_QUEST:
        {
            CWebQuestManager::RESULT result = m_pQuest->Proc();
            switch (result)
            {
            case CWebQuestManager::RESULT_NO:
                m_step = STEP_EOL;
                break;

            case CWebQuestManager::RESULT_YES:
                m_step = STEP_RUN_INIT;
                break;

            default:
                break;
            };

            if (result != CWebQuestManager::RESULT_NONE)
            {
                delete m_pQuest;
                m_pQuest = nullptr;
            };
        }
        break;

    case STEP_RUN_INIT:
        {
            m_pLoad = new CWebLoadManager;
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


CWebAutosaveFlow::CWebAutosaveFlow(void)
{
    m_step = STEP_INIT;

    CWebSaveLoadManager::InitializeFrame(CWebSaveLoadManager::MESSAGEID_SAVE_NOW,
                                        CWebSaveLoadManager::TITLEID_SAVE);
};


CWebAutosaveFlow::~CWebAutosaveFlow(void)
{
    CWebSaveLoadManager::TerminateFrame();
};


bool CWebAutosaveFlow::Proc(void)
{
    bool bResult = false;

    switch (m_step)
    {
    case STEP_INIT:
        {
            m_pSave = new CWebSaveManager;
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