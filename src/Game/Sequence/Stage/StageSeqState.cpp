#include "StageSeqState.hpp"
#include "StageBaseSequence.hpp"

#include "Game/Component/Gauge/GaugeInformation.hpp"
#include "Game/Component/Gauge/GaugeResult.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/GameLoader.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/File/FileID.hpp"


/*static*/ CDummyStageState* CDummyStageState::Instance(void)
{
    static CDummyStageState s_DummyStageStateInstance;
    return &s_DummyStageStateInstance;
};


//
// *********************************************************************************
//


CLoadStageSeqState::CLoadStageSeqState(STAGEID::VALUE idStage)
: m_idStage(idStage)
, m_step(STEP_LOAD_SOUND)
{
    ;
};


void CLoadStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    CGameSound::StageBefore(m_idStage);
    m_step = STEP_LOAD_SOUND;
};


void CLoadStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    pSeq->Stage().Start();
};


bool CLoadStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    switch (m_step)
    {
    case STEP_LOAD_SOUND:
        {
            if (!CGameSound::IsLoadEnd())
                break;

            LoadData();
            m_step = STEP_LOAD_DATA;
        }
        break;
        
    case STEP_LOAD_DATA:
        {
            CDataLoader::Period();
            if (CDataLoader::IsLoadEnd())
                m_step = STEP_END;
        }
        break;

    default:
        break;
    };
    
    return (m_step >= STEP_END);
};


void CLoadStageSeqState::LoadPlayers(void)
{
    int32 nPlayerInfoNum = CGameData::PlayParam().GetCharaInfoNum();
    for (int32 i = 0; i < nPlayerInfoNum; ++i)
    {
        const CGamePlayParam::CHARAINFO& CharaInfo = CGameData::PlayParam().CharaInfo(i);
        CGameLoader::LoadPlayer(CharaInfo.m_CharacterID, CharaInfo.m_Costume);
    };
};


void CLoadStageSeqState::LoadData(void)
{
    LoadStageCommon();

#ifdef TMNT2_BUILD_EU
    CDataLoader::Regist(FILEID::ID_LANG_GAUGE_EU);

    switch (CStageInfo::GetMode(m_idStage))
    {
    case GAMETYPES::STAGEMODE_HOME:
        CDataLoader::Regist(FILEID::ID_LANG_HOME_EU);
        break;

    case GAMETYPES::STAGEMODE_RIDE:
        CDataLoader::Regist(FILEID::ID_LANG_RIDE_EU);
        break;

    default:
        break;
    };
#endif /* TMNT2_BUILD_EU */

    LoadPlayers();
    LoadStage();
};


void CLoadStageSeqState::LoadStageCommon(void)
{
    CGameLoader::LoadStageCommonData(CStageInfo::GetMode(m_idStage));
};


void CLoadStageSeqState::LoadStage(void)
{
    CGameLoader::LoadStage(m_idStage);
};


//
// *********************************************************************************
//


void CIntroStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    pSeq->Stage().GetMapCamera()->SetCameraMode(CMapCamera::MODE_INTRODUCTION);

    CScreenFade::BlackIn();

    m_step = STEP_INTRO;
    m_fTime = 0.0f;
};


void CIntroStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    pSeq->Stage().GetMapCamera()->SetCameraMode(CMapCamera::MODE_AUTOCHANGE);
};


bool CIntroStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    CMapCamera* pMapCamera = pSeq->Stage().GetMapCamera();
    ASSERT(pMapCamera);
    
    switch (m_step)
    {
    case STEP_INTRO:
        {
            int32 iController = CGameData::Attribute().GetVirtualPad();
            if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
                pMapCamera->Skip();

            if (pMapCamera->IsIntroductionEnd())
                m_step = STEP_DISPINFO;
        }
        break;
        
    case STEP_DISPINFO:
        {
            CGaugeInformation::MissionInfoSet(CGameData::PlayParam().GetStage());
            
            m_fTime += CGameProperty::GetElapsedTime();

            int32 iController = CGameData::Attribute().GetVirtualPad();
            if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
                m_fTime += 30.0f;

            if (m_fTime >= 30.0f)
            {
                CScreenFade::BlackOut();
                m_step = STEP_FADEOUT;
            };
        }
        break;
        
    case STEP_FADEOUT:
        {
            CGaugeInformation::MissionInfoSet(CGameData::PlayParam().GetStage());
            
            if (!CScreenFade::IsFading())
                m_step = STEP_END;
        }
        break;
    };
    
    return (m_step >= STEP_END);
};


//
// *********************************************************************************
//


void CPlayStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    pSeq->Stage().AddGauge();
    pSeq->Stage().AddPlayers();

    CMapCamera* pMapCamera = pSeq->Stage().GetMapCamera();
    ASSERT(pMapCamera);
    
    pMapCamera->SetCameraMode(CMapCamera::MODE_AUTOCHANGE);
    if (CGameProperty::GetPlayerNum() > 1)
        pMapCamera->SetPathMode(CMapCamera::PATHMODE_MULTIPLAYER);
    else
        pMapCamera->SetPathMode(CMapCamera::PATHMODE_SINGLEPLAYER);

    CScreenFade::BlackIn();
    m_fTime = 0.0f;
    m_step = STEP_PLAYING;
};


void CPlayStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    ;
};


bool CPlayStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    switch (m_step)
    {
    case STEP_PLAYING:
        {
            CGameStage& stage = pSeq->Stage();

			if (!stage.IsPaused())
			{
				if (stage.CheckPauseMenu())
					stage.StartPause(CGameStage::PAUSETYPE_MENU, nullptr);
			};

            if (stage.GetResult())
            {
                m_fTime = 5.0f;
                m_step = STEP_RESULT;
                stage.BeginDemo();

                switch (stage.GetResult())
                {
                case CGameStage::RESULT_GAMECLEAR:
                    {
                        if (!CGameData::PlayParam().IsLastStage())
                            m_step = STEP_FADEOUT;
                        else
                            CGaugeResult::SetMissionResult(CGaugeResult::RESULTREQ_SUCCESS);
                    }
                    break;
                    
                case CGameStage::RESULT_GAMEOVER:
                    {
                        CGaugeResult::SetMissionResult(CGaugeResult::RESULTREQ_FAIL);
                    }
                    break;
                    
                case CGameStage::RESULT_PLAYERDED:
                    {
                        CGaugeResult::SetMissionResult(CGaugeResult::RESULTREQ_FAIL, stage.GetDeadPlayer());
                    }
                    break;
                    
                case CGameStage::RESULT_RET_TITLE:
                    {
                        m_step = STEP_FADEOUT;
                    }
                    break;
                    
                case CGameStage::RESULT_RET_STAGESEL:
                    {
                        m_step = STEP_FADEOUT;
                    }
                    break;

                default:
                    ASSERT(false);
                    break;
                };
            };
        }
        break;

    case STEP_RESULT:
        {
            if (CGaugeResult::IsMissionResultEnd())
                m_step = STEP_PLAYEND;
        }
        break;
        
    case STEP_PLAYEND:
        {
            m_fTime -= CGameProperty::GetElapsedTime();
            if (!CGameSound::IsVoicePlaying() || m_fTime <= 0.0f)
                m_step = STEP_FADEOUT;
        }
        break;
        
    case STEP_FADEOUT:
        {
            CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
            CScreenFade::BlackOut(0.5f);
            m_step = STEP_FADEOUT_WAIT;
        }
        break;

    case STEP_FADEOUT_WAIT:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_END;
        }
        break;
    };

    return (m_step >= STEP_END);
};