#include "HomeStageSeqState.hpp"
#include "HomeStageSequence.hpp"
#include "Home2D.hpp"
#include "HomeCharacter.hpp"
#include "HomeCamera.hpp"

#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameMain/GameLoader.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/Player/PlayerID.hpp"
#include "Game/Sequence/Stage/StageSeqState.hpp"
#include "Game/System/Map/MapClumpModelMan.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Sound/MessageManager.hpp"


class CHomeMessage
{
public:
    static CHomeMessage& Instance(void);

private:    
    CHomeMessage(void);

public:    
    void Period(void);
    PLAYERID::VALUE GetPlayerID(void);

private:
    float m_fTimeNow;    
    float m_fTimeActivate;
    PLAYERID::VALUE m_lastPlayerIdVoice;
};


//
// *********************************************************************************
//


/*static*/ CHomeMessage& CHomeMessage::Instance(void)
{
    static CHomeMessage s_HomeMessage;
    return s_HomeMessage;
};


CHomeMessage::CHomeMessage(void)
: m_fTimeNow(0.0f)
, m_fTimeActivate(Math::RandFloatRange(20.0f, 40.0f))
, m_lastPlayerIdVoice(PLAYERID::ID_MAX)
{
    ;
};


void CHomeMessage::Period(void)
{
    m_fTimeNow += CGameProperty::GetElapsedTime();
    if (m_fTimeNow > m_fTimeActivate)
    {
        PLAYERID::VALUE playerIdVoice = GetPlayerID();
        CMessageManager::Request(SEGROUPID::VALUE(45), playerIdVoice);

        m_fTimeActivate += Math::RandFloatRange(20.0f, 40.0f);
    };
};


PLAYERID::VALUE CHomeMessage::GetPlayerID(void)
{
    PLAYERID::VALUE playerIdVoice = m_lastPlayerIdVoice;

    while (playerIdVoice == m_lastPlayerIdVoice)
    {
        do
        {
            uint32 playerIdMin = 0;
            uint32 playerIdMax = static_cast<uint32>(PLAYERID::ID_MAX) - 1;

            playerIdVoice = static_cast<PLAYERID::VALUE>(Math::RandRange(playerIdMin, playerIdMax));

        } while (!CHomeCharacter::CheckAppear(playerIdVoice));
    };

    m_lastPlayerIdVoice = playerIdVoice;
    return playerIdVoice;
};


//
// *********************************************************************************
//


CLoadHomeStageSeqState::CLoadHomeStageSeqState(void)
: CLoadStageSeqState(STAGEID::ID_ST03S)
{
    ;
};


CLoadHomeStageSeqState::~CLoadHomeStageSeqState(void)
{
    ;
};


void CLoadHomeStageSeqState::LoadPlayers(void)
{
    for (int32 i = 0; i < PLAYERID::ID_MAX; ++i)
    {
        if (CHomeCharacter::CheckAppear(PLAYERID::VALUE(i)))
            CGameLoader::LoadPlayer(PLAYERID::VALUE(i), GAMETYPES::COSTUME_NONE);
    };
};


//
// *********************************************************************************
//


void CPlayHomeStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    pSeq->Stage().AddStageObjects();
    SetWarpHole();

    m_step = STEP_PLAY;

    CScreenFade::BlackIn();

    CHomeCharacter::Initialize();
};


void CPlayHomeStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    CHomeCharacter::Terminate();
};


bool CPlayHomeStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    CHome2D::Period();
    CHomeMessage::Instance().Period();

    CHomeStageSequence* pHomeStageSeq = static_cast<CHomeStageSequence*>(pSeq);

    switch (m_step)
    {
    case STEP_PLAY:
        {
            if (pHomeStageSeq->GetCamera()->GetMode() == CHomeCamera::MODE_END)
                m_step = STEP_PLAYEND;
        }
        break;

    case STEP_PLAYEND:
        {
            CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
            CScreenFade::BlackOut();
            m_step = STEP_FADEOUT;
        }
        break;

    case STEP_FADEOUT:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_END;
        }
        break;

    default:
        break;
    };

    return (m_step >= STEP_END);
};


void CPlayHomeStageSeqState::SetWarpHole(void)
{    
    bool bEnable = CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS) > CNexusRecord::STATE_NONE;

    CMapClumpModelManager::SetDrawEnable("m03s_warphole", bEnable);
    CMapClumpModelManager::SetDrawEnable("m03s_warphole2", bEnable);
    CMapClumpModelManager::SetDrawEnable("m03s_warphole3", bEnable);
};