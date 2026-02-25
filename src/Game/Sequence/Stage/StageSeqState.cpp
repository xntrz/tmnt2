#include "StageSeqState.hpp"
#include "StageBaseSequence.hpp"

#include "Game/Component/Gauge/GaugeInformation.hpp"
#include "Game/Component/Gauge/GaugeResult.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/GameLoader.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "System/Common/Controller.hpp"


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
    CDataLoader::Regist(FPATH_LANG("Language/English/Gauge_EU/Gauge_EU.lpac"));

    switch (CStageInfo::GetMode(m_idStage))
    {
    case GAMETYPES::STAGEMODE_HOME:
        CDataLoader::Regist(FPATH_LANG("Language/English/Home_EU/Home_EU.lpac"));
        break;

    case GAMETYPES::STAGEMODE_RIDE:
        CDataLoader::Regist(FPATH_LANG("Language/English/Ride_EU/Ride_EU.lpac"));
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


/*static*/ const float CIntroStageSeqState::DISPINFO_SEC = 30.0f;


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
                m_fTime += DISPINFO_SEC;

            if (m_fTime >= DISPINFO_SEC)
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

    default:
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

    default:
        break;
    };

    return (m_step >= STEP_END);
};


//
// *********************************************************************************
//


CLoadTestSeqState::CLoadTestSeqState(MAPID::VALUE idMap)
: m_idMap(idMap)
, m_step(0)
, m_idStage(STAGEID::ID_NONE)
{
    ;
};


CLoadTestSeqState::CLoadTestSeqState(STAGEID::VALUE idStage)
: m_idMap(MAPID::ID_NONE)
, m_step(0)
, m_idStage(idStage)
{
    ;
};


void CLoadTestSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    m_step = 0;
    loadSound();
};


void CLoadTestSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    pSeq->Stage().Start();
};


bool CLoadTestSeqState::OnMove(CStageBaseSequence* pSeq)
{
    switch (m_step)
    {
    case 0:
        {
            if (!CGameSound::IsLoadEnd())
                break;

            loadData();
            ++m_step;
        }
        break;

    case 1:
        {
            CDataLoader::Period();

            if (CDataLoader::IsLoadEnd())
                ++m_step;
        }
        break;

    default:
        break;
    };

    return (m_step >= 2);
};


void CLoadTestSeqState::loadData(void)
{
    if ((m_idMap == MAPID::ID_M06R) ||
        (m_idMap == MAPID::ID_M13R) ||
        (m_idMap == MAPID::ID_M22R) ||
        (m_idMap == MAPID::ID_M32R) ||
        (m_idMap == MAPID::ID_M46R))
    {
        CGameLoader::LoadStageCommonData(GAMETYPES::STAGEMODE_RIDE);
    }
    else
    {
        CGameLoader::LoadStageCommonData(GAMETYPES::STAGEMODE_NORMAL);
    };

    int32 nPlayerInfoNum = CGameData::PlayParam().GetCharaInfoNum();
    for (int32 i = 0; i < nPlayerInfoNum; ++i)
    {
        const CGamePlayParam::CHARAINFO& chrinfo = CGameData::PlayParam().CharaInfo(i);
        CGameLoader::LoadPlayer(chrinfo.m_CharacterID, chrinfo.m_Costume);
    };

    if (m_idMap != MAPID::ID_NONE)
        CGameLoader::LoadMap(m_idMap);
    else
        CGameLoader::LoadStage(m_idStage);
};


void CLoadTestSeqState::loadSound(void)
{
    CGameSound::StageBefore(CGameData::PlayParam().GetStage());
};


//
// *********************************************************************************
//


CPlayTestSeqState::CPlayTestSeqState(bool bEnableGauge)
: m_nPlayerNum(0)
, m_bEnableGauge(bEnableGauge)
{
    ;
};


void CPlayTestSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    if (m_bEnableGauge)
        pSeq->Stage().AddGauge();

    pSeq->Stage().AddStageObjects();
    pSeq->Stage().GetMapCamera()->SetCameraMode(CMapCamera::MODE_AUTOCHANGE);

    if (CGameProperty::GetPlayerNum() > 1)
        pSeq->Stage().GetMapCamera()->SetPathMode(CMapCamera::PATHMODE_MULTIPLAYER);
    else
        pSeq->Stage().GetMapCamera()->SetPathMode(CMapCamera::PATHMODE_SINGLEPLAYER);

    CScreenFade::BlackIn();
};


void CPlayTestSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    ;
};


bool CPlayTestSeqState::OnMove(CStageBaseSequence* pSeq)
{
    uint32 digital = CController::DIGITAL_SELECT | CController::DIGITAL_START;

    if ((CController::GetDigital(CController::CONTROLLER_LOCKED_ON_VIRTUAL) == digital) ||
        (CController::GetDigital(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL) == digital))
        return true;

    return false;
};


void CPlayTestSeqState::AddPlayer(int32 iPlayerNo)
{
    int32 nPlayerMax = CGameData::PlayParam().GetPlayerNum();
    if (m_nPlayerNum < nPlayerMax)
    {
        const CGamePlayParam::CHARAINFO& chrInfo = CGameData::PlayParam().CharaInfo(m_nPlayerNum);

        CGameProperty::AddPlayerCharacter(chrInfo.m_iPlayerNo, chrInfo.m_CharacterID, chrInfo.m_Costume);

        const CGamePlayParam::PLAYERCONTEXT& playerCtx = CGameData::PlayParam().PlayerContext(chrInfo.m_iPlayerNo);
        CGameProperty::Player(chrInfo.m_iPlayerNo)->LoadContext(playerCtx);

        CGimmickManager::SetPlayerStartPosition(chrInfo.m_iPlayerNo, true);

        ++m_nPlayerNum;
    };
};