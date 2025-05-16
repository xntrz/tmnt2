#include "EnemyTestStageSeqState.hpp"
#include "EnemyTestStageSequence.hpp"
#include "EnemyTestMakeFileSequence.hpp"
#include "Game/Component/Player/ConcretePlayerCharacter/Splinter.hpp"
#include "Game/Component/Player/Manipulator.hpp"

#ifdef _DEBUG

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameData/GamePlayParam.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameMain/ItemID.hpp"
#include "Game/Component/GameMain/GameLoader.hpp"
#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickFactory.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/FallRubbleGimmickManager.hpp"
#include "Game/Sequence/Stage/StageBaseSequence.hpp"
#include "Game/Sequence/Test/DebugMenuProcess.hpp"
#include "Game/Sequence/Test/DebugUtils.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/PadStream.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Map/MapClumpModelMan.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/File/FileID.hpp"

#ifdef TARGET_PC
#include "System/PC/File/PCFileManager.hpp"
#endif /* TARGET_PC */


static const char* TEST_FILES_DIRECTORY = "test/";


/*static*/ const char* CLoadEnemyTestStageSeqState::MakeFilepath(const char* pszFilename)
{
    static char s_szFilepath[256];
    s_szFilepath[0] = '\0';

    std::strcpy(s_szFilepath, TEST_FILES_DIRECTORY);
    std::strcat(s_szFilepath, pszFilename);

    return s_szFilepath;
};


/*static*/ bool CLoadEnemyTestStageSeqState::IsBoxLoadEnable(void)
{
#ifdef TARGET_PC
    uint32 fileExistMask = 0;
    int32 i = 0;
    const char* pszFilename = EnemyTestGetItemNeededFile(i);
    while (pszFilename)
    {
        /* true if all enumerated files is exist */
        if (CPCFileManager::IsExist(MakeFilepath(pszFilename)))
            fileExistMask |= (1 << i);
        
        pszFilename = EnemyTestGetItemNeededFile(++i);
    };

    uint32 fileNeedMask = ((1 << i) - 1);

    return ((fileExistMask & fileNeedMask) == fileNeedMask);
#else  /* TARGET_PC */
    return false;
#endif /* TARGET_PC */
};


CLoadEnemyTestStageSeqState::CLoadEnemyTestStageSeqState()
: m_step(STEP_INIT)
, m_testCtx()
{
    ;
};


void CLoadEnemyTestStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    m_testCtx = *reinterpret_cast<const ENEMYTESTTYPES::TEST_CTX*>(pParam);
};


void CLoadEnemyTestStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    pSeq->Stage().Start();
};


bool CLoadEnemyTestStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    switch (m_step)
    {
    case STEP_INIT:
        {
            STAGEID::VALUE stageId = CEnemy::GetNeededStage(m_testCtx.enemyId[0]);
            if (stageId != STAGEID::ID_NONE)
            {
                CGameData::PlayParam().SetStage(stageId);
                CGameSound::StageBefore(stageId);
                CGameData::PlayParam().SetStage(m_testCtx.stageId);
            }
            else
            {
                CGameSound::StageBefore(m_testCtx.stageId);
            };

            m_step = STEP_LOAD_SOUND;
        }
        break;

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


void CLoadEnemyTestStageSeqState::LoadData(void)
{
    LoadStage();
    LoadPlayers();
    if (m_testCtx.loadItemBoxFlag)
        LoadItems();
    LoadEnemy();

    if ((m_testCtx.enemyId[0] == ENEMYID::ID_HUN_A) ||
        (m_testCtx.enemyId[0] == ENEMYID::ID_HUN_B))
    {
        CDataLoader::Regist(MakeFilepath("scrap.lpac"));
    };

    Regist(FILEID::ID_SPLINTER);
};


void CLoadEnemyTestStageSeqState::LoadPlayers(void)
{
    int32 charaInfoNum = CGameData::PlayParam().GetCharaInfoNum();
    for (int32 i = 0; i < charaInfoNum; ++i)
    {
        const CGamePlayParam::CHARAINFO& charaInfo = CGameData::PlayParam().CharaInfo(i);
        CGameLoader::LoadPlayer(charaInfo.m_CharacterID, charaInfo.m_Costume);
    };
};


void CLoadEnemyTestStageSeqState::LoadStage(void)
{
    GAMETYPES::STAGEMODE stageMode = CStageInfo::GetMode(m_testCtx.stageId);
    CGameLoader::LoadStageCommonData(stageMode);
#ifdef TMNT2_BUILD_EU
    CDataLoader::Regist(FILEID::ID_LANG_GAUGE_EU);

    switch (CStageInfo::GetMode(m_testCtx.stageId))
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
    CGameLoader::LoadStage(m_testCtx.stageId);
};


void CLoadEnemyTestStageSeqState::LoadItems(void)
{
    int32 i = 0;
    const char* pszFilename = EnemyTestGetItemNeededFile(i);
    while (pszFilename)
    {
        Regist(pszFilename);
        pszFilename = EnemyTestGetItemNeededFile(++i);
    };
};


void CLoadEnemyTestStageSeqState::LoadEnemy(void)
{
    CDataLoader::Regist(FILEID::ID_STCOMMON_N); // load tournament enemy parameters

    for (int32 i = 0; i < m_testCtx.enemyCount; ++i)
    {
        ENEMYID::VALUE enemyId = m_testCtx.enemyId[i];

        ASSERT(enemyId >= ENEMYID::ID_START);
        ASSERT(enemyId <  ENEMYID::ID_MAX);

        const char* pszExtName = ENEMYID::GetExtName(enemyId);
        ASSERT(pszExtName != nullptr);

        LoadEnemyNeededEffects(enemyId);

        Regist(pszExtName);
    };
};


void CLoadEnemyTestStageSeqState::LoadEnemyNeededEffects(ENEMYID::VALUE enemyId)
{
    int32 i = 0;
    EFFECTID::VALUE neededEffectId = CEnemy::GetNeededEffect(enemyId, i);
    while (neededEffectId != EFFECTID::ID_UNKNOWN)
    {
        const char* pszName = EFFECTID::GetNameFromID(neededEffectId);
        ASSERT(pszName != nullptr);

        Regist(pszName);

        neededEffectId = CEnemy::GetNeededEffect(enemyId, ++i);
    };
};


void CLoadEnemyTestStageSeqState::Regist(int32 fileId)
{
    CDataLoader::Regist(fileId);
};


void CLoadEnemyTestStageSeqState::Regist(const char* pszFilename)
{
    CDataLoader::Regist(MakeFilepath(pszFilename));
};


//
// *********************************************************************************
//


/*static*/ CPlayEnemyTestStageSeqState* CPlayEnemyTestStageSeqState::m_pPlaySeqState = nullptr;


void CPlayEnemyTestStageSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    ASSERT(m_pPlaySeqState == nullptr);
    m_pPlaySeqState = this;

    m_testCtx = *reinterpret_cast<const ENEMYTESTTYPES::TEST_CTX*>(pParam);
    m_bAIState = false;    
    std::memset(m_apszEnemyName, 0, sizeof(m_apszEnemyName));
    m_nextEnemyHP = 100;
    m_nextEnemyPattern = 0;
    m_fNextEnemyPatrolRadius = 7.0f;

    CGameStage& stage = pSeq->Stage();

    stage.AddPlayers();
    stage.AddGauge();
    //stage.AddStageObjects();
    stage.SetCameraUpdater(CStageInfo::CAMERAUPDATE_NORMAL);

    m_step = STEP_PLAYING;

    if (CGameData::PlayParam().GetArea() == AREAID::HOME)
    {
        CMapClumpModelManager::SetDrawEnable("m03s_warphole", false);
        CMapClumpModelManager::SetDrawEnable("m03s_warphole2", false);
        CMapClumpModelManager::SetDrawEnable("m03s_warphole3", false);
    };

    CScreenFade::BlackIn();

    InitMenu();
    CreateGimmicks();    
};


void CPlayEnemyTestStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    CGameStage& stage = pSeq->Stage();

    stage.SetCameraUpdater(CStageInfo::CAMERAUPDATE_NORMAL);

    ASSERT(m_pPlaySeqState == this);
    m_pPlaySeqState = nullptr;
};


bool CPlayEnemyTestStageSeqState::OnMove(CStageBaseSequence* pSeq)
{
    CGameStage& stage = pSeq->Stage();
    float dt = CGameProperty::GetElapsedTime();

    switch (m_step)
    {
    case STEP_PLAYING:
        {
            UpdateItems(dt);

            ///*
            if (CController::GetDigitalTrigger(0, CController::DIGITAL_LUP))
            {
                static bool ok = false;
                if (!ok)
                {
                    ok = true;
                    RwV3d p = Math::VECTOR3_ZERO;
                    p.y = 3.0f;
                    auto h= CEffectManager::Play(EFFECTID::ID_SLA_SHADOW, &p);
                    CEffectManager::SetPosition(h, &p);
                };
            };
            //*/
            if (stage.CheckPauseMenu())
                stage.StartPause(CGameStage::PAUSETYPE_MENU);

            if (stage.GetResult())
            {
                stage.BeginDemo();

				CGameStage::RESULT result = stage.GetResult();
                switch (result)
                {
                case CGameStage::RESULT_RET_TITLE:
                case CGameStage::RESULT_RET_STAGESEL:
                case CGameStage::RESULT_GAMECLEAR:
                case CGameStage::RESULT_GAMEOVER:
                case CGameStage::RESULT_PLAYERDED:
                    m_step = STEP_FADEOUT;
                    break;

                default:
                    ASSERT(false);
                    break;
                };
            };
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

    return (m_step == STEP_END);
};


void CPlayEnemyTestStageSeqState::OnDraw(const CStageBaseSequence* pSeq) const
{
    ;
};


void CPlayEnemyTestStageSeqState::UpdateItems(float dt)
{
    if (!m_testCtx.loadItemBoxFlag)
        return;

    int32 numItems = CountItems();
    if (numItems <= 0)
    {
        m_fItemBoxRespawnWait += dt;
        if (m_fItemBoxRespawnWait > 3.0f)
        {
            CreateItems();
            m_fItemBoxRespawnWait = 0.0f;
        };
    };
};


void CPlayEnemyTestStageSeqState::CreateItems(void)
{
    static ITEMID::VALUE aItemBoxList[] =
    {
        ITEMID::ID_HEAL_FULL,
        ITEMID::ID_MISSILE,
        ITEMID::ID_HEAL_SMALL,
        ITEMID::ID_MISSILE,
        ITEMID::ID_COMEBACK,
        ITEMID::ID_MISSILE,
        ITEMID::ID_HEAL_SMALL,
        ITEMID::ID_MISSILE,
        ITEMID::ID_HEAL_FULL,
    };

    GIMMICKPARAM::GIMMICK_ITEMBOX param;
    std::memset(&param, 0x00, sizeof(param));

    for (int32 i = 0; i < COUNT_OF(aItemBoxList); ++i)
    {
        const float fAngleDelta = MATH_PI2 / static_cast<float>(COUNT_OF(aItemBoxList));

        param.m_vPosition.x = Math::Cos(static_cast<float>(i) * fAngleDelta) * 8.0f;
        param.m_vPosition.z = Math::Sin(static_cast<float>(i) * fAngleDelta) * 8.0f;
        param.m_vPosition.y = CWorldMap::GetMapHeight(&param.m_vPosition);
        param.m_id          = GIMMICKID::ID_N_ITEMBX;
        param.m_nItem       = aItemBoxList[i];
        CGimmickInfo::MakeName(param.m_szName, GIMMICKID::ID_N_ITEMBX, 0, i + 20);

        CGimmick* pGimmick = CGimmickManager::Create(GIMMICKID::ID_N_ITEMBX, 0, &param);
        ASSERT(pGimmick);
    };
};


int32 CPlayEnemyTestStageSeqState::CountItems(void)
{
    int32 numItemBox = 0;

    CGameObject* pObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK);
    while (pObj)
    {
        CGimmick* pGimmick = static_cast<CGimmick*>(pObj);

        if ((pGimmick->GetID() == GIMMICKID::ID_N_ITEMBX) ||
            (pGimmick->GetID() == GIMMICKID::ID_N_ITEM))
            ++numItemBox;

        pObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK, pObj);
    };

    return numItemBox;
};


void CPlayEnemyTestStageSeqState::InitMenu(void)
{
    /* init enemy name */
    for (int32 i = 0; i < m_testCtx.enemyCount; ++i)
    {
        m_apszEnemyName[i] = ENEMYID::GetName(m_testCtx.enemyId[i]);
        ASSERT(m_apszEnemyName[i] != nullptr);
    };

    /* extend debug menu for current sequence */
    CDebugMenuCtrl& menu = CDebugMenuProcess::Menu();

    /* disable stage complete */
    menu.SetItemEnable(0, false);
    menu.SetItemEnable(1, false);
    menu.SetItemEnable(2, false);
    menu.SetDispMax(20);

    /* init test menu */
    menu.AddSeparator();

    int32 baseSelect = menu.GetSelectMax();

    int32 min = 0;
    int32 max = m_testCtx.enemyCount;
    int32 step = 1;
    int32 init = 0;

    menu.AddInt("Spawn enemy at P1", min, max, step, init, m_apszEnemyName, [](int32 value, bool trigger) {
        if (trigger)
            m_pPlaySeqState->CreateEnemy(value);
    });

    min = 1;
    max = 10000;
    step = 10;
    init = m_nextEnemyHP;
    menu.AddInt("Enemy HP", min, max, step, init, [](int32 value, bool trigger) {
        m_pPlaySeqState->m_nextEnemyHP = Clamp(value, 1, 10000);
    });

    min = 0;
    max = 5;
    init = m_nextEnemyPattern;
    step = 1;
    menu.AddInt("Enemy pattern", min, max, step, init, [](int32 value, bool trigger) {
        m_pPlaySeqState->m_nextEnemyPattern = Clamp(value, 0, 5);
    });


    float fMin = 1.0f;
    float fMax = 50.0f;
    float fInit = m_fNextEnemyPatrolRadius;
    float fStep = 1.0f;
    menu.AddFloat("Enemy patrol radius", fMin, fMax, fStep, fInit, [](float fValue, bool trigger) {
        m_pPlaySeqState->m_fNextEnemyPatrolRadius = Clamp(fValue, 1.0f, 50.0f);
    });

    bool bInit = m_bAIState;
    menu.AddBool("AI Running", bInit, [](bool value, bool trigger) {
        if (!trigger)
        {
            if (m_pPlaySeqState->m_bAIState != value)
            {
                m_pPlaySeqState->m_bAIState = value;
                m_pPlaySeqState->ControlAI(m_pPlaySeqState->m_bAIState);
            };
        }
        else
        {
            m_pPlaySeqState->ControlAI(m_pPlaySeqState->m_bAIState);
        };
    });

    menu.SetSelectAtTop(baseSelect);
};


void CPlayEnemyTestStageSeqState::ControlAI(bool bEnable)
{
    CGameObject* pObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::ENEMY);
    while (pObj)
    {
        ASSERT(pObj->GetType() == GAMEOBJECTTYPE::ENEMY);
        CEnemy* pEnemy = static_cast<CEnemy*>(pObj);

        if (bEnable)
            pEnemy->StartAI();
        else
            pEnemy->StopAI();

        pObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::ENEMY, pObj);
    };
};


void CPlayEnemyTestStageSeqState::CreateEnemy(int32 index)
{
    ASSERT(index >= 0);
    ASSERT(index < m_testCtx.enemyCount);

    RwV3d vPos = Math::VECTOR3_ZERO;
    CGameProperty::Player(0)->GetPosition(&vPos);
    float fDir = CGameProperty::Player(0)->GetCurrentCharacter()->GetDirection();

    ENEMYTYPES::CREATEINFO createInfo;
    createInfo.m_vPosition  = vPos;
    createInfo.m_idEnemy    = m_testCtx.enemyId[index];
    createInfo.m_status     = ENEMYTYPES::STATUS_IDLE;
    createInfo.m_fRadius    = m_fNextEnemyPatrolRadius;
    createInfo.m_iHPMax     = m_nextEnemyHP;
    createInfo.m_iPattern   = m_nextEnemyPattern;
    createInfo.m_fDirection = fDir;

    CEnemy* pEnemy = CEnemy::New(&createInfo);
    ASSERT(pEnemy != nullptr);

    if (pEnemy)
    {
        pEnemy->Start();
        
        CGameProperty::Period(); // required for activate enemy

        if (m_bAIState)
            pEnemy->StartAI();
        else
            pEnemy->StopAI();
    };
};


void CPlayEnemyTestStageSeqState::CreateGimmicks(void)
{
    switch (m_testCtx.enemyId[0])
    {
    case ENEMYID::ID_HUN_A:
        {
            GIMMICKPARAM::GIMMICK_BASIC param;
            std::memset(&param, 0, sizeof(param));

            param.m_vPosition = Math::VECTOR3_ZERO;
            param.m_id = GIMMICKID::ID_K_M28OBRM;
            param.m_subid = CFallRubbleGimmickManager::TYPE_HUN;
            CGimmickInfo::MakeName(param.m_szName, GIMMICKID::ID_K_M28OBRM, 0, 40);

            CGimmickManager::Create(GIMMICKID::ID_K_M28OBRM, 0, &param);
        }
        break;

    case ENEMYID::ID_HUN_B:
        {
            GIMMICKPARAM::GIMMICK_BASIC param;
            std::memset(&param, 0, sizeof(param));

            param.m_vPosition = Math::VECTOR3_ZERO;
            param.m_id = GIMMICKID::ID_K_M28OBRM;
            param.m_subid = CFallRubbleGimmickManager::TYPE_HUN2;
            CGimmickInfo::MakeName(param.m_szName, GIMMICKID::ID_K_M28OBRM, 0, 40);

            CGimmickManager::Create(GIMMICKID::ID_K_M28OBRM, 0, &param);
        }
        break;

    default:
        break;
    };
};


#endif /* _DEBUG */