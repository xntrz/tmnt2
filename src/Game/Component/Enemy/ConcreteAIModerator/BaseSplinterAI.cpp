#include "BaseSplinterAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


CBaseSplinterAI::CAIBehaviorAvoid::CAIBehaviorAvoid(void)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::AVOID_ATTACK)
{
    ;
};


/*virtual*/ bool CBaseSplinterAI::CAIBehaviorAvoid::IsRunnable(void) /*override*/
{
    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
        return false;

    if (!PlayerWatcher().IsPlayerSuitRange(-1, 1.0f))
        return false;

    bool bIsGetup = (AIParam().GetStatus() == ENEMYTYPES::STATUS_GETUP);
    bool bIsOrderRefused = (EnemyChr().AIThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE);
    bool bIsSatisfyFreq = (IsSatisfyFreq(ELITEAI::FREQUENCY_ATTACK_AVOID));

    if (bIsGetup && bIsOrderRefused && bIsSatisfyFreq)
        return true;

    if (AIParam().GetAttackCount() < SPLINTERAI::ATTACK_COUNT_FOR_AVOID)
        return false;

    if (PlayerWatcher().GetLastAttackerType() != GAMEOBJECTTYPE::CHARACTER)
        return false;

    AIParam().ClearAttackCount();

    if (!IsSatisfyFreq(ELITEAI::FREQUENCY_ATTACK_AVOID))
        return false;

    return true;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorAvoid::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_ATTACKLAST);

    OrderDefence(ELITEAI::ORDERTYPE_DEFENCE_NEAR);

    if (IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_B_CHARGE))
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_B_CHARGE);
    
    if (IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_KICK))
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_KICK);

    OrderPush(ELITEAI::PRIORITY_NORMAL);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorAvoid::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


//
// *********************************************************************************
//


/*virtual*/ bool CBaseSplinterAI::CAIBehaviorEnclose::IsRunnable(void) /*override*/
{
    return CBaseEliteAI::CAIBehaviorAvoidEnclose::IsRunnable();
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorEnclose::OnActive(void) /*override*/
{
    OrderDefence(ELITEAI::ORDERTYPE_DEFENCE_NEAR);

    if (!IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_KICK))
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_KICK);
    
    if (!IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_B_CHARGE))
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_B_CHARGE);

    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseSplinterAI::CAIBehaviorHighKickAttack::OnActive(void) /*override*/
{
    OrderAttack(ELITEAI::ORDERTYPE_ATTACK_HIGH);
    OrderPush(ELITEAI::PRIORITY_NORMAL);
};


//
// *********************************************************************************
//


CBaseSplinterAI::CAIBehaviorThrowAttack::CAIBehaviorThrowAttack(void)
: CAIBehavior(SPLINTERAI::AIBEHAVIORNAME::THROW)
, m_fThrowTimeout(0.0f)
{
    ;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorThrowAttack::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fThrowTimeout, "Throw");
};


/*virtual*/ bool CBaseSplinterAI::CAIBehaviorThrowAttack::IsRunnable(void) /*override*/
{
    if (m_fThrowTimeout > 0.0f)
        return false;

    if (!AIParam().IsActionPermission())
        return false;

    if (PlayerWatcher().IsAttackWithinRange(3.0f, MATH_DEG2RAD(360.0f)))
        return false;

    if (!PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(50.0f), 1.5f))
        return false;

    if (!PlayerWatcher().IsPlayerStop(-1))
        return false;

    m_fThrowTimeout = GetFreqRatio(SPLINTERAI::FREQUENCY_WAIT) * 15.0f;

    if (!IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_THROW))
        return false;

    return true;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorThrowAttack::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_SUIT);

    OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_THROW);
    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


CBaseSplinterAI::CAIBehaviorAntiGuard::CAIBehaviorAntiGuard(void)
: CAIBehavior(SPLINTERAI::AIBEHAVIORNAME::ATTACK_KICK)
, m_fAntiGuardTimeout(0.0f)
{
    ;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorAntiGuard::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fAntiGuardTimeout, "AntiGuard");
};


/*virtual*/ bool CBaseSplinterAI::CAIBehaviorAntiGuard::IsRunnable(void) /*override*/
{
    if (m_fAntiGuardTimeout > 0.0f)
        return false;

    if (!AIParam().IsActionPermission())
        return false;

    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
        return false;

    if (!PlayerWatcher().IsPlayerSuitRange(-1, 1.0f))
        return false;

    if (!PlayerWatcher().IsPlayerGuardState())
        return false;

    m_fAntiGuardTimeout = GetFreqRatio(SPLINTERAI::FREQUENCY_WAIT) * 8.0f;

    if (!IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_KICK))
        return false;

    return true;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorAntiGuard::OnActive(void) /*override*/
{
    OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_KICK);
    OrderPush(ELITEAI::PRIORITY_LOW);

    OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, 0.5f);
    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


CBaseSplinterAI::CAIBehaviorNearAttack::CAIBehaviorNearAttack(void)
: CAIBehavior(SPLINTERAI::AIBEHAVIORNAME::ATTACK_A)
, m_fNearAttackTimeout(0.0f)
{
    ;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorNearAttack::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fNearAttackTimeout, "Near");
};


/*virtual*/ bool CBaseSplinterAI::CAIBehaviorNearAttack::IsRunnable(void) /*override*/
{
    if (m_fNearAttackTimeout > 0.0f)
        return false;

    if (!AIParam().IsActivityPermission())
        return false;

    if (!AIParam().IsActionPermission())
        return false;

    if (!PlayerWatcher().IsPlayerLookAtMe(-1, MATH_DEG2RAD(90.0f)))
    {
        if (!PlayerWatcher().IsPlayerWithinRange(-1, 2.0f))
            return false;
    };

    if (!PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(70.0f)))
        return false;

    m_fNearAttackTimeout = GetFreqRatio(SPLINTERAI::FREQUENCY_WAIT) * 2.0f;

    return true;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorNearAttack::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_SUIT);

    OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMALC);

    if (IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_B))
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_B);

    if (IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_NORMALA))
        OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMALA);

    if ((AIParam().GetStatus() == ENEMYTYPES::STATUS_WALK) && IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_RUN))
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_RUN);

    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


CBaseSplinterAI::CAIBehaviorFarAttack::CAIBehaviorFarAttack(void)
: CBaseEliteAI::CAIBehaviorAttack(SPLINTERAI::AIBEHAVIORNAME::ATTACK_B)
, m_fFarAttackTimeout(0.0f)
, m_attackType(-1)
{
    ;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorFarAttack::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fFarAttackTimeout, "Far");
};


/*virtual*/ bool CBaseSplinterAI::CAIBehaviorFarAttack::IsRunnable(void) /*override*/
{
    if (m_fFarAttackTimeout > 0.0f)
        return false;

    if (!AIParam().IsActivityPermission())
        return false;

    if (!AIParam().IsActionPermission())
        return false;

    if (!PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(70.0f), 5.0f))
        return false;

    if (!PlayerWatcher().IsPlayerStop(-1))
        return false;

    m_fFarAttackTimeout = GetFreqRatio(SPLINTERAI::FREQUENCY_WAIT) * 10.0f;
    m_attackType = -1;

    if (IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_NORMAL))
        m_attackType = ELITEAI::ORDERTYPE_ATTACK_NORMAL;

    if (IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_NORMALB))
        m_attackType = ELITEAI::ORDERTYPE_ATTACK_NORMALB;
    
    return (m_attackType != -1);
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorFarAttack::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_SUIT);

    OrderAttack(m_attackType);
    OrderPush(ELITEAI::PRIORITY_LOW);

    if (m_attackType == ELITEAI::ORDERTYPE_ATTACK_NORMALB)
    {
        float fWaitTime = GetFreqRatio(SPLINTERAI::FREQUENCY_WAIT) * 2.0f;
        OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, fWaitTime);
        OrderPush(ELITEAI::PRIORITY_LOW);
    };
};


//
// *********************************************************************************
//


CBaseSplinterAI::CAIBehaviorShot::CAIBehaviorShot(void)
: CAIBehavior(SPLINTERAI::AIBEHAVIORNAME::SHOT)
, m_fShotTimeout(10.0f)
{
    ;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorShot::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fShotTimeout, "Shot");
};


/*virtual*/ bool CBaseSplinterAI::CAIBehaviorShot::IsRunnable(void) /*override*/
{
    if (m_fShotTimeout > 0.0f)
        return false;

    if (!AIParam().IsActionPermission())
        return false;

    if (!PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(70.0f), 16.0f))
        return false;

    if (PlayerWatcher().IsPlayerWithinRange(-1, 3.0f))
        return false;

    if (!PlayerWatcher().IsPlayerStop(-1))
        return false;

    m_fShotTimeout = GetFreqRatio(SPLINTERAI::FREQUENCY_WAIT) * 6.0f;

    if (!IsSatisfyFreq(SPLINTERAI::FREQUENCY_ATTACK_SHOT))
        return false;

    return true;
};


/*virtual*/ void CBaseSplinterAI::CAIBehaviorShot::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_TEMP);

    if (PlayerWatcher().IsPlayerWithinRange(-1, 4.5f))
    {
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_SHOT);
        OrderPush(ELITEAI::PRIORITY_LOW);
    }
    else
    {
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_SHOT_JUMP);
        OrderPush(ELITEAI::PRIORITY_LOW);
        OrderAttack(SPLINTERAI::ORDERTYPE_ATTACK_SHOT_JUMP);
        OrderPush(ELITEAI::PRIORITY_NORMAL);
    };
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseSplinterAI::CAIBehaviorMove::OnActive(void) /*override*/
{
    if (PlayerWatcher().IsTogether(2, 2.0f))
        PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_TEMP);

    OrderMove(ELITEAI::ORDERTYPE_MOVE_RUN);

    RwV3d vecNearestTempPlrPos = PlayerWatcher().GetTargetPlayerData().position;
    if (AIParam().IsInsideTerritory(&vecNearestTempPlrPos))
    {
        if (!PlayerWatcher().IsPlayerWithinRange(-1, 4.0f) &&
            EnemyChr().IsAttachedStatusObserver(ENEMYTYPES::STATUS_DASH))
        {
            OrderMove(ELITEAI::ORDERTYPE_MOVE_DASH);
        };
    };

    OrderPush(ELITEAI::PRIORITY_LOW);
};

//
// *********************************************************************************
//


/*static*/ CAIModerator* CBaseSplinterAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseSplinterAI(pEnemyChr);
};


CBaseSplinterAI::CBaseSplinterAI(CEnemyCharacter* pEnemyChr)
: CBaseEliteAI(pEnemyChr)
{
    /* init behavior units (attach order is important for priority node) */
    CAIBehavior* pBehavior = nullptr;

    pBehavior = new CBaseSplinterAI::CAIBehaviorAvoid;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseEliteAI::CAIBehaviorGuard(true);
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseSplinterAI::CAIBehaviorEnclose;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseSplinterAI::CAIBehaviorHighKickAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new CBaseSplinterAI::CAIBehaviorThrowAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new CBaseSplinterAI::CAIBehaviorAntiGuard;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new CBaseSplinterAI::CAIBehaviorNearAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new CBaseSplinterAI::CAIBehaviorFarAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new CBaseSplinterAI::CAIBehaviorShot;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new CBaseSplinterAI::CAIBehaviorMove;
    Attach(pBehavior, ELITEAI::PRIORITY_LOW);
};


/*virtual*/ CBaseSplinterAI::~CBaseSplinterAI(void)
{
    ;
};


/*virtual*/ void CBaseSplinterAI::AttackCountInc(void) /*override*/
{
    AIParam().AddAttackCount();
};


/*virtual*/ void CBaseSplinterAI::CatchAttack(uint32 hAttacker) /*override*/
{
    if (!hAttacker)
        return;

    CGameObject* pAttackerObj = CGameObjectManager::GetObject(hAttacker);
    GAMEOBJECTTYPE::VALUE attackerObjType = pAttackerObj->GetType();

    int32 attackerPlayerNo = CEnemyUtils::FindPlayerNo(hAttacker);

    AIParam().PlayerWatcher().SetLastAttackerType(attackerObjType);
    AIParam().PlayerWatcher().SetPlayerData(attackerPlayerNo, CPlayerWatcher::PLAYER_DATA_ATTACK);
};