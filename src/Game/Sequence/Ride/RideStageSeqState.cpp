#include "RideStageSeqState.hpp"
#include "RideStageSequence.hpp"
#include "RideCamera.hpp"
#include "RideStage.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameLoader.hpp"
#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/ScreenFade.hpp"


CLoadRideStageSeqState::CLoadRideStageSeqState(STAGEID::VALUE idStage)
: CLoadStageSeqState(idStage)
{
    ;
};


CLoadRideStageSeqState::~CLoadRideStageSeqState(void)
{
    ;
};


void CLoadRideStageSeqState::LoadPlayers(void)
{
    int32 nPlayerInfoNum = CGameData::PlayParam().GetCharaInfoNum();
    for (int32 i = 0; i < nPlayerInfoNum; ++i)
    {
        const CGamePlayParam::CHARAINFO& CharaInfo = CGameData::PlayParam().CharaInfo(i);
        CGameLoader::LoadRidePlayer(CharaInfo.m_CharacterID, CharaInfo.m_Costume);
    };
};


void CPlayRideStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    m_pRideCamera = new CRideCamera;
    ASSERT(m_pRideCamera);

    pSeq->Stage().SetCameraUpdater(m_pRideCamera);

    pSeq->Stage().AddGauge();
    pSeq->Stage().AddStageObjects();
    pSeq->Stage().AddPlayers();

    CScreenFade::StartOut();
    
    m_step = STEP_PLAY;

    CRideStage::OnLoaded();
};


void CPlayRideStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    pSeq->Stage().SetCameraUpdater(nullptr);

    if (m_pRideCamera)
    {
        delete m_pRideCamera;
        m_pRideCamera = nullptr;
    };
};


bool CPlayRideStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
	CGameStage& stage = pSeq->Stage();
	if (!stage.IsPaused())
		CRideStage::Period();

    switch (m_step)
    {
    case STEP_PLAY:
        {            
            if (stage.CheckPauseMenu())
                stage.StartPause(CGameStage::PAUSETYPE_MENU, nullptr);

            if (stage.GetResult())
            {
                CRideStage::UpdateResultData();
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