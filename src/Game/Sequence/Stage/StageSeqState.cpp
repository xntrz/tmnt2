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
#include "System/Common/File/AfsFileID.hpp"


/*static*/ CDummyStageState* CDummyStageState::Instance(void)
{
    static CDummyStageState s_DummyStageStateInstance;
    return &s_DummyStageStateInstance;
};


void CDummyStageState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    ;
};


void CDummyStageState::OnDetach(CStageBaseSequence* pSeq)
{
    ;
};


bool CDummyStageState::OnMove(CStageBaseSequence* pSeq)
{
    return true;
};


/*static*/ CLoadStageSeqState* CLoadStageSeqState::Create(STAGEID::VALUE idStage)
{
	return new CLoadStageSeqState(idStage);
};


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
    pSeq->Stage().AddStageObjects();
};


bool CLoadStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    switch (m_step)
    {
    case STEP_LOAD_SOUND:
        if (CGameSound::IsLoadEnd())
        {
            LoadStageCommon();
            LoadPlayers();
            LoadStage();
            m_step = STEP_LOAD_DATA;
        };
        break;
        
    case STEP_LOAD_DATA:
        if (CDataLoader::IsLoadEnd())
        {
            m_step = STEP_END;
        }
        else
        {
            CDataLoader::Period();
        };
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

        CGameLoader::LoadPlayer(
            CharaInfo.m_CharacterID,
            CharaInfo.m_Costume
        );
    };
};


void CLoadStageSeqState::LoadStageCommon(void)
{
    CGameLoader::LoadStageCommonData(
        CStageInfo::GetMode(m_idStage)
    );
};


void CLoadStageSeqState::LoadStage(void)
{
    CGameLoader::LoadStage(m_idStage);
};


void CIntroStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    pSeq->Stage().GetMapCamera()->SetCameraMode(CMapCamera::MODE_INTRODUCTION);
    CScreenFade::StartOut();
    m_step = STEP_INTRO;
    m_fTime = 0.0f;
};


void CIntroStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    pSeq->Stage().GetMapCamera()->SetCameraMode(CMapCamera::MODE_MANUAL);
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
            CGaugeInformation::MissionInfoSet(
                CGameData::PlayParam().GetStage()
            );
            
            m_fTime += CGameProperty::GetElapsedTime();

            int32 iController = CGameData::Attribute().GetVirtualPad();
            if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
                m_fTime += 30.0f;

            if (m_fTime >= 30.0f)
            {
                CScreenFade::StartIn();
                m_step = STEP_FADEIN;
            };
        }
        break;
        
    case STEP_FADEIN:
        {
            CGaugeInformation::MissionInfoSet(
                CGameData::PlayParam().GetStage()
            );
            
            if (!CScreenFade::IsFading())
                m_step = STEP_END;
        }
        break;
    };
    
    return (m_step >= STEP_END);
};


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
    

    CScreenFade::StartOut();
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

            if (stage.CheckPauseMenu())
                stage.StartPause(CGameStage::PAUSETYPE_MENU, nullptr);

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
                            m_step = STEP_FADEIN;
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
                        m_step = STEP_FADEIN;
                    }
                    break;
                    
                case CGameStage::RESULT_RET_STAGESEL:
                    {
                        m_step = STEP_FADEIN;
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
                m_step = STEP_FADEIN;
        }
        break;
        
    case STEP_FADEIN:
        {
            CGameSound::FadeOut(CGameSound::FADESPEED_FAST);
            CScreenFade::StartIn(0.5f);
            m_step = STEP_FADEWAIT;
        }
        break;

    case STEP_FADEWAIT:
        {
            if (!CScreenFade::IsFading())
                m_step = STEP_END;
        }
        break;
    };

    return (m_step >= STEP_END);
};