#include "EnemyTestStageSequence.hpp"
#include "EnemyTestStageSeqState.hpp"
#include "EnemyTestMakeFileSequence.hpp"
#include "EnemyTestTypes.hpp"

#ifdef _DEBUG

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Sequence/Stage/StageBaseSequence.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Misc/PadStream.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/System2D.hpp"


class CEnemyTestStageSequence : public CStageBaseSequence
{
public:
    CEnemyTestStageSequence(void);
    virtual ~CEnemyTestStageSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    virtual void OnStateDetached(STATE state) override;
    void InitPlayers(void);
    void InitItems(void);
    void PushEnemy(ENEMYID::VALUE enemyId);

private:
    ENEMYTESTTYPES::TEST_CTX m_testCtx;
    CLoadEnemyTestStageSeqState m_stateLoad;
    CPlayEnemyTestStageSeqState m_statePlay;
};


CEnemyTestStageSequence::CEnemyTestStageSequence(void)
: m_testCtx()
, m_stateLoad()
, m_statePlay()
{
    /* make it invalid by default */
    for (int32 i = 0; i < COUNT_OF(m_testCtx.enemyId); ++i)
        m_testCtx.enemyId[i] = ENEMYID::ID_MAX;
        
    m_testCtx.enemyCount      = 0;
    m_testCtx.stageId         = STAGEID::ID_NONE;
    m_testCtx.loadItemBoxFlag = false;
};


CEnemyTestStageSequence::~CEnemyTestStageSequence(void)
{
    ;
};


bool CEnemyTestStageSequence::OnAttach(const void* pParam)
{
    /* init test ctx */
    m_testCtx.stageId = STAGEID::ID_STTEST;

    ENEMYID::VALUE enemyId = static_cast<ENEMYID::VALUE>(reinterpret_cast<int32>(pParam));
    PushEnemy(enemyId);
    if (enemyId == ENEMYID::ID_ELITE_FOOT_A)
    {
        PushEnemy(ENEMYID::ID_ELITE_FOOT_B);
        PushEnemy(ENEMYID::ID_ELITE_FOOT_C);
        PushEnemy(ENEMYID::ID_ELITE_FOOT_D);
        PushEnemy(ENEMYID::ID_ELITE_FOOT_DUMMY_A);
        PushEnemy(ENEMYID::ID_ELITE_FOOT_DUMMY_B);
        PushEnemy(ENEMYID::ID_ELITE_FOOT_DUMMY_C);
        PushEnemy(ENEMYID::ID_ELITE_FOOT_DUMMY_D);
    };

    /* init attribute */
    CGameData::Attribute().SetGamemode(GAMETYPES::GAMEMODE_NORMAL);

    /* init record */
    CGameData::PushRecord();
    CGameData::Record().SetDefault();
    CGameData::Record().Item().SetItemTaken(ITEMID::ID_DON_LASER);
    CGameData::Record().Item().ForceSetCrystalNum(GAMETYPES::CRYSTAL_MAX);

    /* init options */
    CGameData::PushOption();
    CGameData::Option().Play().SetDefault();
    CGameData::Option().Play().SetDifficulty(GAMETYPES::DIFFICULTY_NORMAL);
    CGameData::Option().Display().SetEnableHelp(false);
    CGameData::Option().Display().SetDefault();

    /* init play param */
    CGameData::PlayParam().ClearPlayer();

    InitPlayers();

    CGameData::PlayParam().ClearArea();
    CGameData::PlayParam().SetAreaPlaymode(CGamePlayParam::AREAPLAYMODE_ONE_STAGE);

    CGameData::PlayParam().SetStage(m_testCtx.stageId);

    CGameData::OnBeginArea();

    /* init stage base */
    CStageBaseSequence::OnAttach(pParam);

    InitItems();

    RegisterStateObject(STATE_LOAD, &m_stateLoad, false);
    RegisterStateObject(STATE_PLAY, &m_statePlay, false);

    ChangeState(STATE_LOAD, &m_testCtx);

    CGameSound::Stop();

    return true;
};


void CEnemyTestStageSequence::OnDetach(void)
{
    /* term stage base */
    CStageBaseSequence::OnDetach();

    /* term option */
    CGameData::PopOption();

    /* term record */
    CGameData::PopRecord();

    /* term attribute */
    CGameData::Attribute().SetGamemode(GAMETYPES::GAMEMODE_NORMAL);
};


void CEnemyTestStageSequence::OnMove(bool bRet, const void* pReturnValue)
{
    CStageBaseSequence::OnMove(bRet, pReturnValue);
};


void CEnemyTestStageSequence::OnDraw(void) const
{
    CStageBaseSequence::OnDraw();
};


void CEnemyTestStageSequence::OnStateDetached(STATE state)
{
    switch (state)
    {
    case STATE_LOAD:
        ChangeState(STATE_PLAY, &m_testCtx);
        break;

    case STATE_PLAY:
        ChangeState(STATE_END, &m_testCtx);
        break;

    default:
        break;
    };
};


void CEnemyTestStageSequence::InitPlayers(void)
{
    CGameData::PlayParam().AddPlayerCharacter(0, PLAYERID::ID_LEO);
    CGameData::PlayParam().AddPlayerCharacter(0, PLAYERID::ID_RAP);
    CGameData::PlayParam().AddPlayerCharacter(0, PLAYERID::ID_MIC);
    CGameData::PlayParam().AddPlayerCharacter(0, PLAYERID::ID_DON);
};


void CEnemyTestStageSequence::InitItems(void)
{
    m_testCtx.loadItemBoxFlag = CLoadEnemyTestStageSeqState::IsBoxLoadEnable();
};


void CEnemyTestStageSequence::PushEnemy(ENEMYID::VALUE enemyId)
{
    ASSERT(m_testCtx.enemyCount < COUNT_OF(m_testCtx.enemyId));
    
    m_testCtx.enemyId[m_testCtx.enemyCount++] = enemyId;
};


class CProcess* EnemyTestStageSequenceInstance(void)
{
    return new CEnemyTestStageSequence;
};

#endif /* _DEBUG */