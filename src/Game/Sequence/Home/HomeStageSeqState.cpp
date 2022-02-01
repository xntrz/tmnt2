#include "HomeStageSeqState.hpp"
#include "HomeStageSequence.hpp"
#include "Home2D.hpp"
#include "HomeCharacter.hpp"
#include "HomeCamera.hpp"

#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameMain/GameLoader.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/Player/PlayerID.hpp"
#include "Game/Sequence/Stage/StageSeqState.hpp"
#include "Game/System/Map/MapClumpModelMan.hpp"
#include "Game/System/Misc/ScreenFade.hpp"


class CHomeMessage
{
public:
    static CHomeMessage& Instance(void);

private:    
    CHomeMessage(void);

public:    
    void Period(void);
    PLAYERID::VALUE GetPlayerID(void) const;
};


/*static*/ CHomeMessage& CHomeMessage::Instance(void)
{
    static CHomeMessage s_HomeMessage;
    return s_HomeMessage;
};


CHomeMessage::CHomeMessage(void)
{
    ;
};


void CHomeMessage::Period(void)
{
    ;
};


PLAYERID::VALUE CHomeMessage::GetPlayerID(void) const
{
    return PLAYERID::ID_MAX;
};


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


void CPlayHomeStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    pSeq->Stage().AddStageObjects();
    SetWarpHole();

    m_step = STEP_PLAY;

    CScreenFade::StartOut();

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

    CHomeStageSequence* pHomeStageSeq = (CHomeStageSequence*)pSeq;

    switch (m_step)
    {
    case STEP_PLAY:
        {
            if (pHomeStageSeq->GetCamera()->GetMode() == CHomeCamera::MODE_END)
            {
                m_step = STEP_PLAYEND;
            };
        }
        break;

    case STEP_PLAYEND:
        {
            CGameSound::FadeOut(CGameSound::FADESPEED_FAST);
            CScreenFade::StartIn();
            m_step = STEP_FADEIN;
        }
        break;

    case STEP_FADEIN:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_END;
        }
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