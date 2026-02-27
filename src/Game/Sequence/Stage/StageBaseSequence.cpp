#include "StageBaseSequence.hpp"
#include "StageSeqState.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Sound/GameSound.hpp"


CStageBaseSequence::CStageBaseSequence(void)
: m_state(STATE_NONE)
, m_stateRequest(STATE_NONE)
, m_pAttachStateParam(nullptr)
, m_pCurrentStateObj(nullptr)
{
    for (int32 i = 0; i < COUNT_OF(m_aStateNode); ++i)
    {
        m_aStateNode[i].m_bAutoDelete = false;
        m_aStateNode[i].m_pStateObj = nullptr;
    };
};


CStageBaseSequence::~CStageBaseSequence(void)
{
    ;
};


bool CStageBaseSequence::OnAttach(const void* pParam)
{
    m_GameStage.Initialize();
    setupState();

    CScreenFade::BlackOut(0.0f);
    
    STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();
    CGameData::OnBeginStage(stageId);

    CStageInfo::CAMERAUPDATE cameraUpdateType = CStageInfo::GetCameraUpdateType(stageId);
    m_GameStage.SetCameraUpdater(cameraUpdateType);
    
	EnableStickToDirButton(false);

    return true;
};


void CStageBaseSequence::OnDetach(void)
{
	EnableStickToDirButton(true);

    CLoadingDisplay::Stop(this);

    CGameData::Attribute().SetInteractive(false);

    m_GameStage.Stop();
    cleanupState();
    m_GameStage.Terminate();

    STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();

    if (m_GameStage.GetResult() == CGameStage::RESULT_GAMECLEAR)
		CGameData::OnEndStage(stageId);

    CGameSound::StageAfter(stageId);
	CGameSound::Stop();
};


void CStageBaseSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (runState())
        OnStateDetached(m_state);

    m_GameStage.Period();
    
    if (GetState() >= STATE_END)
        Ret();
};


void CStageBaseSequence::OnDraw(void) const
{
    m_GameStage.Draw();
    
    if (m_pCurrentStateObj)
        m_pCurrentStateObj->OnDraw(this);
};


void CStageBaseSequence::OnStateDetached(STATE state)
{
    ChangeState(STATE_END);
};


void CStageBaseSequence::RegisterStateObject(STATE state, IStageSeqState* pStateObj, bool bAutoDelete)
{
    ASSERT(pStateObj);
    ASSERT(state > STATE_NONE);
    ASSERT(state < STATE_END);

    m_aStateNode[state].m_bAutoDelete = bAutoDelete;
    m_aStateNode[state].m_pStateObj   = pStateObj;
};


void CStageBaseSequence::ChangeState(STATE state, const void* pParam /*= nullptr*/)
{
    if (m_state != state)
    {
        if (state == STATE_LOAD)
            CLoadingDisplay::Start(this);
        else if (m_state == STATE_LOAD)
            CLoadingDisplay::Stop(this);

        if (state == STATE_PLAY)
        {
            CGameData::Attribute().SetInteractive(true);
            m_GameStage.StartPlay();
        }
        else if (m_state == STATE_PLAY)
        {
            CGameData::Attribute().SetInteractive(false);
        };
    };
    
    m_pAttachStateParam = pParam;
    m_stateRequest      = state;
};


CStageBaseSequence::STATE CStageBaseSequence::GetState(void) const
{
    return m_state;
};


CGameStage& CStageBaseSequence::Stage(void)
{
    return m_GameStage;
};


const CGameStage& CStageBaseSequence::Stage(void) const
{
    return m_GameStage;
};


void CStageBaseSequence::setupState(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aStateNode); ++i)
    {
        m_aStateNode[i].m_bAutoDelete = false;
        m_aStateNode[i].m_pStateObj   = CDummyStageState::Instance();
    };

    m_state             = STATE_NONE;
    m_stateRequest      = STATE_NONE;
    m_pAttachStateParam = nullptr;
    m_pCurrentStateObj  = getStateObj(STATE_NONE);
};


void CStageBaseSequence::cleanupState(void)
{
    if (m_pCurrentStateObj)
    {
        m_pCurrentStateObj->OnDetach(this);
        m_pCurrentStateObj = nullptr;
    };
    
    for (int32 i = 0; i < COUNT_OF(m_aStateNode); ++i)
    {
        if (m_aStateNode[i].m_bAutoDelete)
        {
            ASSERT(m_aStateNode[i].m_pStateObj);
            
            delete m_aStateNode[i].m_pStateObj;
            m_aStateNode[i].m_pStateObj = nullptr;
        };
    };
};


IStageSeqState* CStageBaseSequence::getStateObj(STATE state)
{
    ASSERT(state >= STATE_NONE);
    ASSERT(state <= STATE_END);

    return m_aStateNode[state].m_pStateObj;
};


bool CStageBaseSequence::runState(void)
{
    if (m_state != m_stateRequest)
    {
        m_state = m_stateRequest;
        m_pCurrentStateObj = getStateObj(m_state);
        m_pCurrentStateObj->OnAttach(this, m_pAttachStateParam);
    };

    if (!m_pCurrentStateObj->OnMove(this))
        return false;

    m_pCurrentStateObj->OnDetach(this);
    return true;
};

