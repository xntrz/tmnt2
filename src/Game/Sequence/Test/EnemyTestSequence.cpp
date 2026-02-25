#include "EnemyTestSequence.hpp"

#ifdef _DEBUG

#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Sequence/Stage/StageSeqState.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/Gamepad.hpp"


CLoadEnemyTestSeqState::CLoadEnemyTestSeqState(MAPID::VALUE idMap, ENEMYID::VALUE idEnemy)
: CLoadTestSeqState(idMap)
, m_idEnemy(idEnemy)
{
    ;
};


CLoadEnemyTestSeqState::CLoadEnemyTestSeqState(STAGEID::VALUE idStage, ENEMYID::VALUE idEnemy)
: CLoadTestSeqState(idStage)
, m_idEnemy(idEnemy)
{
    ;
};


void CLoadEnemyTestSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    CLoadTestSeqState::OnAttach(pSeq, pParam);

    //
    // CGameLoader::LoadEnemy(m_idEnemy);
    // we are dont have non compiled assets so we load by custom way
    //

    const char* pszDirPath = "data/enemy/";

    char szFName[256];
    std::sprintf(szFName, "%s%s.list", pszDirPath, ENEMYID::GetExtName(m_idEnemy));

    CDataLoader::Regist(szFName, true);
};


//
// *********************************************************************************
//


CPlayEnemyTestSeqState::CPlayEnemyTestSeqState(ENEMYID::VALUE idEnemy)
: CPlayTestSeqState(false)
, m_idEnemy(idEnemy)
{
    ;
};


void CPlayEnemyTestSeqState::OnAttach(CStageBaseSequence* pSeq, const void* pParam)
{
    CPlayTestSeqState::OnAttach(pSeq, pParam);
    UnlockAllControllers();
};


bool CPlayEnemyTestSeqState::OnMove(CStageBaseSequence* pSeq)
{
    if (CPlayTestSeqState::OnMove(pSeq))
        return true;
    
    if (IGamepad::GetDigitalTrigger(IGamepad::CONTROLLER_LOCKED_ON_VIRTUAL, IGamepad::DIGITAL_START) && (m_nPlayerNum > 0))
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        CGameProperty::Player(0)->GetPosition(&vPosition);
        vPosition.y += 10.0f;

        ENEMYTYPES::CREATEINFO createInfo;
        createInfo.m_idEnemy = m_idEnemy;
        createInfo.m_vPosition = vPosition;
        createInfo.m_fDirection = 0.0f;
        createInfo.m_status = ENEMYTYPES::STATUS_IDLE;
        createInfo.m_iPattern = 0;
        createInfo.m_fRadius = 100.0f;
        createInfo.m_iHPMax = 1000;

        CEnemy* pEnemy = CEnemy::New(&createInfo);
        if (pEnemy)
            pEnemy->Start();
    };

    int32 iPlayerNo = LockTriggeredController(IGamepad::DIGITAL_SELECT);
    if (iPlayerNo != -1)
    {
        AddPlayer();

        if ((m_nPlayerNum - 1) == 0)
            pSeq->Stage().AddGauge();

        RwV3d vPosition = { 0.0f, 10.0f, 0.0f };
        CGameProperty::Player(iPlayerNo)->Relocation(&vPosition, 0.0f, true);

        CGameProperty::Player(iPlayerNo)->AddHP(500);
        CGameProperty::Player(iPlayerNo)->AddShurikenNum(30);
    };

    return false;
};


//
// *********************************************************************************
//


/*static*/ CProcess* CEnemyTestSequence::Instance(void)
{
    return new CEnemyTestSequence;
};


CEnemyTestSequence::CEnemyTestSequence(void)
{
    ;
};


CEnemyTestSequence::~CEnemyTestSequence(void)
{
    ;
};


bool CEnemyTestSequence::OnAttach(const void* pParam)
{
    ENEMYID::VALUE eID = static_cast<ENEMYID::VALUE>(reinterpret_cast<int32>(pParam));
    STAGEID::VALUE stageID = STAGEID::ID_STTEST;

    CGameData::PlayParam().Clear();

    CGameData::PlayParam().AddPlayerCharacter(0, PLAYERID::ID_LEO);
    CGameData::PlayParam().AddPlayerCharacter(1, PLAYERID::ID_RAP);
    CGameData::PlayParam().AddPlayerCharacter(2, PLAYERID::ID_MIC);
    CGameData::PlayParam().AddPlayerCharacter(3, PLAYERID::ID_DON);

    CGameData::PlayParam().SetStage(stageID);

    CGameData::Record().Item().SetItemTaken(ITEMID::ID_DON_LASER);
    CGameData::Record().Item().ForceSetCrystalNum(GAMETYPES::CRYSTAL_MAX);

    CGameData::PlayParam().InitForArea();

    CStageBaseSequence::OnAttach(pParam);

    RegisterStateObject(STATE_LOAD, new CLoadEnemyTestSeqState(stageID, eID), true);
    RegisterStateObject(STATE_PLAY, new CPlayEnemyTestSeqState(eID), true);
    ChangeState(STATE_LOAD);

    return true;
};


void CEnemyTestSequence::OnStateDetached(STATE state)
{
    switch (state)
    {
    case STATE_LOAD:
        ChangeState(STATE_PLAY);
        break;

    case STATE_PLAY:
        ChangeState(STATE_END);
        break;

    default:
        break;
    };
};

#endif /* _DEBUG */