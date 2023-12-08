#include "GameStage.hpp"
#include "GameStageCameraUpdater.hpp"
#include "GameStageGauge.hpp"
#include "GameStagePause.hpp"
#include "GameProperty.hpp"
#include "GamePlayer.hpp"
#include "GameRadar.hpp"
#include "GameToon.hpp"
#include "AutoHpCtrlObj.hpp"

#include "Game/Component/Gauge/GaugeInformation.hpp"
#include "Game/Component/Gauge/GaugeManager.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Model/ToonManager.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Character/CharacterManager.hpp"
#include "Game/System/MotionParameter/MotionParameterManager.hpp"
#include "Game/System/Sound/VoiceManager.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Controller.hpp"

#ifdef TARGET_PC
#include "System/PC/PCSpecific.hpp"
#include "System/PC/PCPhysicalControllerKey.hpp"
#endif


/*static*/ CGameStage* CGameStage::m_pCurrent = nullptr;


/*static*/ CGameStage* CGameStage::GetCurrent(void)
{
    return m_pCurrent;
};


CGameStage::CGameStage(void)
: m_idStage(STAGEID::ID_NONE)
, m_nPauseLevel(0)
, m_systemstate(SYSTEMSTATE_NONE)
, m_bCreatedGauge(false)
, m_bCreatedGimmick(false)
, m_bCreatedRadar(false)
, m_bPlayStarted(false)
, m_fTimer(0.0f)
, m_result(RESULT_NONE)
, m_resultParam(-1)
, m_bMultipleBoss(false)
, m_pWorld(nullptr)
, m_pMapCamera(nullptr)
, m_pRecoveryObj(nullptr)
, m_pCameraUpdater(nullptr)
, m_pPauseHandler(nullptr)
{
    std::memset(m_apExGauge, 0x00, sizeof(m_apExGauge));
};


CGameStage::~CGameStage(void)
{
    ;
};


void CGameStage::Initialize(void)
{
    m_idStage = CGameData::PlayParam().GetStage();
    m_nPauseLevel = 0;
    m_systemstate = SYSTEMSTATE_NONE;
    m_bCreatedGauge = false;
    m_bCreatedGimmick = false;
    m_bCreatedRadar = false;
    m_bPlayStarted = false;
    m_fTimer = 0.0f;
    m_result = RESULT_NONE;
    m_resultParam = -1;
    m_bMultipleBoss = false;
    m_pWorld = nullptr;
    m_pMapCamera = nullptr;
    m_pRecoveryObj = nullptr;
    m_pCameraUpdater = CDefaultCameraUpdater::Instance();
    m_pPauseHandler = CDummyStagePause::Instance();
    std::memset(m_apExGauge, 0x00, sizeof(m_apExGauge));

    CTextureManager::GenerationInc();
    CModelManager::Initialize();
    CMotionManager::Initialize();
    CToonManager::Initialize();
    CWorldMap::Initialize();
    CBodyHitManager::Initialize();
    CHitAttackManager::Initialize();
    CGameObjectManager::Initialize();
    CEffectManager::Initialize();
    CGimmickManager::Initialize();
    CShotManager::Initialize();
    CMotionParameterManager::Initialize();
    CCharacterManager::Initialize();
    CEnemyParameter::Initialize();
    CGameProperty::Initialize();
    CMessageManager::Initialize();
    CVoiceManager::Initialize();

    m_pCurrent = this;

#ifdef _DEBUG
    CDebugShape::Initialize();
#endif
};


void CGameStage::Terminate(void)
{
#ifdef _DEBUG
    CDebugShape::Terminate();
#endif
    
    m_pCurrent = nullptr;

    m_pPauseHandler->Stop();
    CGameProperty::Terminate();
    CGameObjectManager::Terminate();
    CEnemyParameter::Terminate();
    CCharacterManager::Terminate();
    CMotionParameterManager::Terminate();
    CShotManager::Terminate();
    CGimmickManager::Terminate();
    CEffectManager::Terminate();
    CBodyHitManager::Terminate();
    CHitAttackManager::Terminate();
    CWorldMap::Terminate();
    CToonManager::Terminate();
    CMotionManager::Terminate();
    CModelManager::Terminate();
    CMessageManager::Terminate();
    CVoiceManager::Terminate();
    CTextureManager::GenerationDec();
};


void CGameStage::Start(void)
{
    CWorldMap::OnLoaded();
    RpWorld* pWorld = CWorldMap::GetWorld();
    createCamera(pWorld);
    GameToon::SetTextureSetOfStage(m_idStage);
    CGimmickManager::CreateStageSpecialGimmick(m_idStage);
    CGaugeInformation::Initialize();
    
    changeSystemState(SYSTEMSTATE_NORMAL);
};


void CGameStage::Stop(void)
{
    changeSystemState(SYSTEMSTATE_NONE);
    m_bPlayStarted = false;
    stopExGauge();
    destroyExGauge();
    updatePlayData();
    updateResultData();    
    CGaugeInformation::Terminate();
    
    if (m_bCreatedGauge)
        CGaugeManager::Terminate();
    
    if (m_bCreatedRadar)
        CGameRadar::Terminate();
    
    destroyCamera();
};


void CGameStage::Period(void)
{
    if (m_systemstate == SYSTEMSTATE_NONE)
        return;

    float dt = CGameProperty::GetElapsedTime();
    
    if (IsPaused())
	{
		ASSERT(m_pPauseHandler);
		if (m_pPauseHandler->Update())
		{
			m_pPauseHandler->Stop();
			resume();
			m_pPauseHandler = CDummyStagePause::Instance();
		};
	}
	else
	{
		m_pCameraUpdater->Update(m_pMapCamera);

		if (m_bPlayStarted && m_systemstate == SYSTEMSTATE_NORMAL)
            m_fTimer += dt;

        CGameProperty::Period();
        if (m_bCreatedRadar)
            CGameRadar::Update();
        CWorldMap::Period();
		CGameObjectManager::Period();
		CHitAttackManager::Period();
		CShotManager::Period();
		CGimmickManager::DispatchEvent();
#ifdef _DEBUG
        CDebugShape::Period(dt);  
#endif
		CEffectManager::Period();
		CGaugeInformation::MissionInfoPeriod();
        CMessageManager::Period();
        CVoiceManager::Period();

		if (m_bCreatedGauge)
		{
			CGaugeManager::Period();
			updateExGauge();
		};
	};
};


void CGameStage::Draw(void) const
{
    if (m_systemstate == SYSTEMSTATE_NONE)
        return;

    ASSERT(m_pMapCamera);

#ifdef _DEBUG
    CDebugShape::FrameBegin();
#endif

    if (m_pMapCamera->BeginScene())
    {
        CRenderStateManager::SetDefault();
        CWorldMap::Draw(CWorldMap::DRAWTYPE_NORMAL);
		CGimmickManager::Draw(CGimmickInfo::DRAWPRI_PRECHARA);
        CCharacterManager::Draw();
        CGimmickManager::Draw(CGimmickInfo::DRAWPRI_POSTCHARA);
        CWorldMap::Draw(CWorldMap::DRAWTYPE_AFTER);
        CShotManager::Draw();
        CGimmickManager::Draw(CGimmickInfo::DRAWPRI_POSTALPHAMAP);

#ifdef _DEBUG
        CDebugShape::Draw3D();
#endif
        
        CEffectManager::Draw(CCamera::CameraCurrent());        
        
        if (m_bCreatedRadar)
            CGameRadar::Draw();

        if (m_bCreatedGauge)
            CGaugeManager::Draw();

        m_pMapCamera->EndScene();
    };

#ifdef _DEBUG
	CDebugShape::Draw2D();
    CDebugShape::FrameEnd();
#endif

    CGaugeInformation::BossGaugeDraw(0);
    if (m_bMultipleBoss)
        CGaugeInformation::BossGaugeDraw(1);

    CMessageManager::Draw();

    if(IsPaused())
    {
        ASSERT(m_pPauseHandler);
        m_pPauseHandler->Draw();        
    };

    CGaugeInformation::MissionInfoDraw();
    CGaugeInformation::DispBattleNexusInfo();
};


void CGameStage::AddGauge(void)
{
    if (m_bCreatedGauge)
        return;

    CGaugeManager::Initialize();
    CGameRadar::Initialize(20.0f);
    createExGauge();
    startExGauge();

    m_bCreatedGauge = true;
    m_bCreatedRadar = true;
};


void CGameStage::AddStageObjects(void)
{
    if (m_bCreatedGimmick)
        return;

    CGimmickManager::CreateSetGimmick();
    m_bCreatedGimmick = true;    
};


void CGameStage::AddPlayers(bool bProtect)
{
    for (int32 i = 0; i < CGameData::PlayParam().GetCharaInfoNum(); ++i)
    {
        const CGamePlayParam::CHARAINFO& rCharaInfo = CGameData::PlayParam().CharaInfo(i);

        CGameProperty::AddPlayerCharacter(
            rCharaInfo.m_iPlayerNo,
            rCharaInfo.m_CharacterID,
            rCharaInfo.m_Costume
        );

		if (CGameData::PlayParam().GetStageMode() != GAMETYPES::STAGEMODE_RIDE)
			CGimmickManager::SetPlayerStartPosition(rCharaInfo.m_iPlayerNo, bProtect);
    };

    for (int32 i = 0; i < CGameData::PlayParam().GetPlayerNum(); ++i)
        CGameProperty::Player(i)->LoadContext(CGameData::PlayParam().PlayerContext(i));

    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_SELFRECOVERY))
    {
        m_pRecoveryObj = new CAutoHpCtrlObj(CAutoHpCtrlObj::MODE_RECOVER);
    }
    else if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHALLENGE_POISON))
    {
        m_pRecoveryObj = new CAutoHpCtrlObj(CAutoHpCtrlObj::MODE_DAMAGE);
    };
};


void CGameStage::StartPlay(void)
{
    CGimmickManager::NotifyPlayStarted();
    m_bPlayStarted = true;
};


bool CGameStage::CheckPauseMenu(void) const
{
#ifdef TARGET_PC    
    return CPCSpecific::IsKeyDown(DIK_ESCAPE);
#else
    return CController::GetDigitalTrigger(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::DIGITAL_START);
#endif
};


void CGameStage::StartPause(PAUSETYPE pausetype, void* param)
{
    if (CScreenFade::IsDrawing())
        return;
    
    IGameStagePause* pPauseHandler = nullptr;
    
    switch (pausetype)
    {
    case PAUSETYPE_MENU:
        pPauseHandler = CMenuStagePause::Instance();
        break;

    case PAUSETYPE_TUTORIAL:
        pPauseHandler = CTutorialStagePause::Instance();
        break;

    default:
        ASSERT(false);
        break;
    };

    if (pPauseHandler && (m_pPauseHandler != pPauseHandler))
    {
        m_pPauseHandler->Stop();
        m_pPauseHandler = pPauseHandler;
        m_pPauseHandler->Start(param);
        
        pause();
    };
};


bool CGameStage::IsPaused(void) const
{
    return (m_nPauseLevel > 0);
};


void CGameStage::BeginDemo(void)
{
    changeSystemState(SYSTEMSTATE_DEMO);
};


void CGameStage::EndDemo(void)
{
    changeSystemState(SYSTEMSTATE_NORMAL);
};


void CGameStage::BeginPlayerNegateDamage(void)
{
    CGamePlayer::SetDamageNegation(true);
};


void CGameStage::EndPlayerNegateDamager(void)
{
    CGamePlayer::SetDamageNegation(false);
};


bool CGameStage::SetResult(RESULT result)
{
    if (m_result)
    {
        return false;
    }
    else
    {
        if (result)
            changeSystemState(SYSTEMSTATE_DEMO);

        m_result = result;
        
        return true;
    };
};


void CGameStage::NotifyGameClear(CGamePlayResult::CLEARSUB clearsub)
{
    if (SetResult(RESULT_GAMECLEAR))
    {
        CGameData::PlayResult().SetClearSub(clearsub);
    };
};


void CGameStage::NotifyGameOver(void)
{
    if (SetResult(RESULT_GAMEOVER))
    {
        CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_GAMEOVER);
    };
};


void CGameStage::NotifyPlayerDead(int32 nPlayerNo)
{
    if (SetResult(RESULT_PLAYERDED))
    {
        m_resultParam = nPlayerNo;
        CGameData::PlayResult().SetAreaResult(CGamePlayResult::AREARESULT_GAMEOVER);
    };
};


int32 CGameStage::GetDeadPlayer(void) const
{
    return m_resultParam;
};


void CGameStage::NotifyEnemyDead(CEnemy* pEnemy)
{
    if (m_apExGauge[EXGAUGE_THREERACE])
        ((CThreeRaceExGauge*)m_apExGauge[EXGAUGE_THREERACE])->HandleEnemyDestroyed(pEnemy);
};


void CGameStage::NotifyEnemyDamaged(CEnemy* pEnemy, int32 nRemainHP)
{
    if (m_apExGauge[EXGAUGE_BOSS])
        ((CBossExGauge*)m_apExGauge[EXGAUGE_BOSS])->HandleEnemyDamaged(pEnemy, nRemainHP);
};


CGameStage::RESULT CGameStage::GetResult(void) const
{
    return m_result;
};


CMapCamera* CGameStage::GetMapCamera(void) const
{
    return m_pMapCamera;
};


void CGameStage::SetCameraUpdater(CStageInfo::CAMERAUPDATE cameraupdtype)
{
    switch (cameraupdtype)
    {
    case CStageInfo::CAMERAUPDATE_NORMAL:
	case CStageInfo::CAMERAUPDATE_RIDE:
        m_pCameraUpdater = CDefaultCameraUpdater::Instance();
        break;

    case CStageInfo::CAMERAUPDATE_BOSSBATTLE:
        m_pCameraUpdater = CBossBattleCameraUpdater::Instance();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CGameStage::SetCameraUpdater(IGameStageCameraUpdater* pCameraUpdater)
{
	m_pCameraUpdater = (pCameraUpdater ? pCameraUpdater : CDefaultCameraUpdater::Instance());
};


void CGameStage::pause(void)
{
    ++m_nPauseLevel;
};


void CGameStage::resume(void)
{
    ASSERT(m_nPauseLevel >= 0);
    
    if (--m_nPauseLevel < 0)
        m_nPauseLevel = 0;
};


void CGameStage::updatePlayData(void)
{
	int32 nPlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < nPlayerNum; ++i)
        CGameProperty::Player(i)->SaveContext(CGameData::PlayParam().PlayerContext(i));
};


void CGameStage::updateResultData(void)
{
    if (CStageInfo::GetMode(m_idStage) == GAMETYPES::STAGEMODE_RIDE)
        return;

	if (CGameProperty::GetPlayerNum() > 0)
	{
		CGameData::PlayResult().SetStageClearSecond(m_fTimer);
		CGameData::PlayResult().SetRemainedHP(CGameProperty::Player(0)->GetHP());
	};
};


void CGameStage::createCamera(RpWorld* pWorld)
{
    ASSERT(!m_pMapCamera);
    
    m_pMapCamera = new CMapCamera;
    ASSERT(m_pMapCamera);

    m_pWorld = pWorld;
    m_pMapCamera->WorldAddCamera(pWorld);
    m_pMapCamera->SetCameraMode(CMapCamera::MODE_AUTOCHANGE);
    CGameProperty::SetMapCamera(m_pMapCamera);
    CGameProperty::SetCurrentRwCamera(m_pMapCamera->GetRwCamera());
    CGameSound::AttachCamera(m_pMapCamera->GetRwCamera());
};


void CGameStage::destroyCamera(void)
{
    ASSERT(m_pMapCamera);
    
    CGameSound::DetachCamera();
    m_pMapCamera->WorldRemoveCamera(m_pWorld);
    m_pWorld;

    delete m_pMapCamera;
    m_pMapCamera = nullptr;
};


void CGameStage::createExGauge(void)
{
    STAGEID::VALUE IdStage = CGameData::PlayParam().GetStage();
    if (CStageInfo::IsBossGaugeNecessary(IdStage))
    {
        if (CStageInfo::GetGaugeEnemyID(IdStage, 1))
            m_bMultipleBoss = true;

        m_apExGauge[EXGAUGE_BOSS] = new CBossExGauge();
    };

    if ((IdStage == STAGEID::ID_ST11J)
        || (IdStage == STAGEID::ID_ST15N)
        || (IdStage == STAGEID::ID_ST20F)
        || (IdStage == STAGEID::ID_ST39N)
        || (IdStage == STAGEID::ID_ST40OB)
        || (IdStage == STAGEID::ID_ST45N))
    {
        m_apExGauge[EXGAUGE_TIMER] = new CTimerExGauge();
    };

    switch (IdStage)
    {
    case STAGEID::ID_ST40OB:
        m_apExGauge[EXGAUGE_COUNTER] = new CCounterExGauge();
        break;
        
    case STAGEID::ID_ST15N:
        m_apExGauge[EXGAUGE_LIFE] = new CLifeExGauge();
        break;
        
    case STAGEID::ID_ST45N:
        m_apExGauge[EXGAUGE_THREERACE] = new CThreeRaceExGauge();
        break;
    };
};


void CGameStage::destroyExGauge(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apExGauge); ++i)
    {
        if (m_apExGauge[i])
        {
            delete m_apExGauge[i];
            m_apExGauge[i] = nullptr;
        };
    };
};


void CGameStage::startExGauge(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apExGauge); ++i)
    {
        if (m_apExGauge[i])
            m_apExGauge[i]->Start();
    };
};


void CGameStage::stopExGauge(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apExGauge); ++i)
    {
        if (m_apExGauge[i])
            m_apExGauge[i]->Stop();        
    };
};


void CGameStage::updateExGauge(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apExGauge); ++i)
    {
        if (m_apExGauge[i])
            m_apExGauge[i]->Update();
    };
};


void CGameStage::changeSystemState(SYSTEMSTATE sysstate)
{
    SYSTEMSTATE SystemstatePrev = m_systemstate;
    m_systemstate = sysstate;
    if (m_systemstate != SystemstatePrev)
    {
        if (m_systemstate == SYSTEMSTATE_DEMO)
        {
            CHitAttackManager::Pause();
            BeginPlayerNegateDamage();
        }
        else if (SystemstatePrev == SYSTEMSTATE_DEMO)
        {
            EndPlayerNegateDamager();
            CHitAttackManager::Resume();
        };
    };
};

