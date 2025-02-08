#include "NexusStageSeqState.hpp"
#include "NexusStageSequence.hpp"
#include "NexusCamera.hpp"

#include "Game/Component/Player/Player.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/Component/Gauge/GaugeInformation.hpp"
#include "Game/Component/Gauge/GaugeResult.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


void CPlayNexusStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    m_pCameraUpdater = nullptr;
    m_bPause = false;
    m_step = STEP_INTRO;
    m_fTime = 0.0f;
    
    CGaugeInformation::SetBattleNexusInfo();

    CGameStage& stage = pSeq->Stage();

    stage.AddStageObjects();
    stage.AddPlayers();

    RecoverHP();
    SleepPlayers();

    CMapCamera* pMapCamera = pSeq->Stage().GetMapCamera();
    ASSERT(pMapCamera);

    pMapCamera->SetCameraMode(CMapCamera::MODE_AUTOCHANGE);
    pMapCamera->SetPathMode((CGameProperty::GetPlayerNum() > 1 ? CMapCamera::PATHMODE_MULTIPLAYER : CMapCamera::PATHMODE_SINGLEPLAYER));    

    CScreenFade::BlackIn();

    CNexusEnemyIntroductionCamera* pCameraUpdater = new CNexusEnemyIntroductionCamera;
    SetCameraUpdater(pSeq, pCameraUpdater);

    CGameData::Attribute().SetInteractive(false);

    OUTPUT(" %s starting nexus round %d\n", __FUNCTION__, CGameData::PlayParam().GetStageIndex());
};


void CPlayNexusStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    SetCameraUpdater(pSeq, nullptr);
};


bool CPlayNexusStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    CGameStage& stage = pSeq->Stage();

    m_fTime += CGameProperty::GetElapsedTime();

    switch (m_step)
    {
    case STEP_INTRO:
        {
            if (CGaugeInformation::IsBattleNexusInfoEnd())
            {
                StartEnemyGenerator();
                AwakePlayers();
                SetCameraUpdater(pSeq, nullptr);
                stage.AddGauge();
                CGameData::Attribute().SetInteractive(true);

                m_step = STEP_PLAY;
                m_fTime = 0.0f;
            };
        }
        break;
        
    case STEP_PLAY:
        {
            if (!stage.IsPaused())
            {
                if (stage.CheckPauseMenu())
                    stage.StartPause(CGameStage::PAUSETYPE_MENU, nullptr);                
            };

            CGameStage::RESULT result = stage.GetResult();
            switch (result)
            {
			case CGameStage::RESULT_NONE:
				break;

            case CGameStage::RESULT_GAMECLEAR:
                {
                    CGameSound::PlaySE(SDCODE_SE(8207));
                    CGaugeResult::SetMissionResult(CGaugeResult::RESULTREQ_SUCCESS);
                    stage.BeginDemo();

                    if (CGameData::PlayParam().IsLastStage())
                    {
                        m_step = STEP_FADEIN;
                    }
                    else
                    {
                        m_step = STEP_RESULT;
                        stage.EndDemo();
                    };
                }
                break;
                
            case CGameStage::RESULT_GAMEOVER:
            case CGameStage::RESULT_PLAYERDED:
                {
                    CGameSound::PlaySE(SDCODE_SE(8207));
                    CGameSound::PlaySE(SDCODE_SE(8206));
                    stage.BeginDemo();

                    m_step = STEP_FADEIN;
                    m_fTime = 0.0f;

                    if (result == CGameStage::RESULT_PLAYERDED)
                    {
                        CGaugeResult::SetMissionResult(CGaugeResult::RESULTREQ_FAIL, stage.GetDeadPlayer());
                    }
                    else
                    {
                        CGaugeResult::SetMissionResult(CGaugeResult::RESULTREQ_FAIL);
                    };
                }
                break;
                
            case CGameStage::RESULT_RET_TITLE:
			case CGameStage::RESULT_RET_STAGESEL:
                {
                    CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
                    stage.BeginDemo();

                    m_step = STEP_FADEIN;
                    m_fTime = 0.0f;
                }
                break;            
                
            default:
                ASSERT(false);
                break;
            };
        }
        break;
        
    case STEP_RESULT:
        {
            if (CGaugeResult::IsMissionResultEnd())
            {
                m_step = STEP_BONUS;
                m_fTime = 0.0f;
                SetItemBox();
            };
        }
        break;
        
    case STEP_BONUS:
        {
            if (m_fTime >= 5.0f)
            {
                m_step = STEP_FADEWAIT;
                m_fTime = 0.0f;
                CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
                CScreenFade::BlackOut();
            };
        }
        break;
        
    case STEP_FADEIN:
        {
            if (CGaugeResult::IsMissionResultEnd())
            {
                m_step = STEP_FADEWAIT;
                m_fTime = 0.0f;
                CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
                CScreenFade::BlackOut();
            };
        }
        break;
        
    case STEP_FADEWAIT:
        {
            if (!CScreenFade::IsFading())
            {
                m_step = STEP_END;
                m_fTime = 0.0f;
            };
        }
        break;

    default:
        break;
    };

    return (m_step >= STEP_END);
};


void CPlayNexusStageSeqState::RecoverHP(void)
{
    IGamePlayer* pGamePlayer = CGameProperty::Player(0);

    int32 hp = static_cast<int32>(static_cast<float>(pGamePlayer->GetHPMax()) * 0.1f);
    pGamePlayer->AddHP(hp);
};


void CPlayNexusStageSeqState::AwakePlayers(void)
{
    int32 nPlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < nPlayerNum; ++i)
    {
        IGamePlayer* pGamePlayer = CGameProperty::Player(i);
        CPlayer* pPlayer = pGamePlayer->GetPlayer();
        pPlayer->Awake();
    };
};


void CPlayNexusStageSeqState::SleepPlayers(void)
{
    int32 nPlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < nPlayerNum; ++i)
    {
        IGamePlayer* pGamePlayer = CGameProperty::Player(i);
        CPlayer* pPlayer = pGamePlayer->GetPlayer();
        pPlayer->Sleep();
    };
};


void CPlayNexusStageSeqState::StartEnemyGenerator(void)
{
    CGameObject* pGameObject = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK);
    while (pGameObject)
    {
        ASSERT(pGameObject->GetType() == GAMEOBJECTTYPE::GIMMICK);
        CGimmick* pGimmick = static_cast<CGimmick*>(pGameObject);
        
        if (pGimmick->GetID() == GIMMICKID::ID_G_SET)
            CGimmickManager::PostEvent(pGimmick->GetName(), "nexus_stage", GIMMICKTYPES::EVENTTYPE_ACTIVATE);

        pGameObject = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK, pGameObject);
    };
};


void CPlayNexusStageSeqState::SetItemBox(void)
{
    static const ITEMID::VALUE s_aItemList[] =
    {
        ITEMID::ID_HEAL_FULL,
        ITEMID::ID_HEAL_SMALL,
        ITEMID::ID_HEAL_SMALL,
        ITEMID::ID_MISSILE,
        ITEMID::ID_MISSILE,
        ITEMID::ID_MISSILE,
    };

    ITEMID::VALUE aItemBoxList[8] = { ITEMID::ID_NONE };

    for (int32 i = 0; i < COUNT_OF(s_aItemList); ++i)
    {
        int32 nListIndex = 0;
        
        do
        {
            nListIndex = Math::RandRange(0, COUNT_OF(aItemBoxList));
        } while (aItemBoxList[nListIndex]);

        aItemBoxList[nListIndex] = s_aItemList[i];
    };

    GIMMICKPARAM::GIMMICK_ITEMBOX param;
    std::memset(&param, 0x00, sizeof(param));

    for (int32 i = 0; i < COUNT_OF(aItemBoxList); ++i)
    {                
        const float fAngleDelta = Math::PI2 / static_cast<float>(COUNT_OF(aItemBoxList));
        
        param.m_vPosition.x = Math::Cos(i * fAngleDelta) * 8.0f;
        param.m_vPosition.z = Math::Sin(i * fAngleDelta) * 8.0f;
        param.m_vPosition.y = CWorldMap::GetMapHeight(&param.m_vPosition);
        param.m_id          = GIMMICKID::ID_N_ITEMBX;
        param.m_nItem       = aItemBoxList[i];
        CGimmickInfo::MakeName(param.m_szName, GIMMICKID::ID_N_ITEMBX, 0, i + 20);

        CGimmick* pGimmick = CGimmickManager::Create(GIMMICKID::ID_N_ITEMBX, 0, &param);
        ASSERT(pGimmick);
    };
};


void CPlayNexusStageSeqState::SetFailedMessage(void)
{
    if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST60X_D04)
        CMessageManager::Request(SEGROUPID::VALUE(146));
};


void CPlayNexusStageSeqState::SetCameraUpdater(CStageBaseSequence* pSeq, IGameStageCameraUpdater* pCameraUpdater)
{
    if (m_pCameraUpdater)
    {
        delete m_pCameraUpdater;
        m_pCameraUpdater = nullptr;
    };

    if (pCameraUpdater)
    {
        m_pCameraUpdater = pCameraUpdater;
        pSeq->Stage().SetCameraUpdater(m_pCameraUpdater);
    }
    else
    {
        STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();
        CStageInfo::CAMERAUPDATE cameraUpdateType = CStageInfo::GetCameraUpdateType(stageId);

        pSeq->Stage().SetCameraUpdater(cameraUpdateType);
    };
};