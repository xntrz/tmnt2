#include "BaseStoneBiterAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


CBaseStoneBiterAI::CDecisionUnitBaseBiter::CDecisionUnitBaseBiter(CEnemyCharacter* pEnemyChr,
                                                                  const char* pszUnitName,
                                                                  CEnemyAIDecisionUnit::TYPE type)
: CEnemyAIDecisionUnit(pszUnitName, type)
, m_enemyWatcher()
{
    m_enemyWatcher.Initialize(pEnemyChr, true);
};


/*virtual*/ bool CBaseStoneBiterAI::CDecisionUnitBaseBiter::CheckTerm(void) /*override*/
{
    return true;
};


/*virtual*/ CBaseStoneBiterAI::CDecisionUnitBaseBiter::RESULT
CBaseStoneBiterAI::CDecisionUnitBaseBiter::Update(void) /*override*/
{
    return RESULT_STOP;
};


/*virtual*/ void CBaseStoneBiterAI::CDecisionUnitBaseBiter::OnStart(void) /*override*/
{
    ;
};


/*virtual*/ void CBaseStoneBiterAI::CDecisionUnitBaseBiter::OnEnd(void) /*override*/
{
    ;
};


void CBaseStoneBiterAI::CDecisionUnitBaseBiter::SetAttackOrder(int32 iOrderTypeAttack)
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
    EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = iOrderTypeAttack;
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = DecisionUnitCommonParameter().PlayerWatcher().GetTargetPlayerData().no;
    EnemyChr().AIThinkOrder().OrderAttack().m_vAt = DecisionUnitCommonParameter().PlayerWatcher().GetTargetPlayerData().position;
};


void CBaseStoneBiterAI::CDecisionUnitBaseBiter::SetMoveOrder(int32 iOrderTypeMove)
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
    EnemyChr().AIThinkOrder().OrderMove().m_iMoveType = iOrderTypeMove;
    EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo = DecisionUnitCommonParameter().PlayerWatcher().GetTargetPlayerData().no;
    EnemyChr().AIThinkOrder().OrderMove().m_vAt = DecisionUnitCommonParameter().PlayerWatcher().GetTargetPlayerData().position;
};


bool CBaseStoneBiterAI::CDecisionUnitBaseBiter::IsSatisfyFrequency(int32 iTableNo) const
{
    uint8 freq = EnemyChr().AIModerator().AIFreqParam().GetFrequency(iTableNo);
    uint8 rand = static_cast<uint8>(Math::RandRange(0, 100));

    return (freq > rand);
};


//
// *********************************************************************************
//


CBaseStoneBiterAI::CDecisionUnitAttackFar::CDecisionUnitAttackFar(CEnemyCharacter* pEnemyChr)
: CDecisionUnitBaseBiter(pEnemyChr, BASESTONEBITER_AI::AIDECISIONUNITNAME::ATTACK_FAR, TYPE_ATTACK)
{
    ;
};


/*virtual*/ bool CBaseStoneBiterAI::CDecisionUnitAttackFar::CheckTerm(void) /*override*/
{
    if (!DecisionUnitCommonParameter().IsAttackPermission())
        return false;

    if (!DecisionUnitCommonParameter().PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(60.0f)))
        return false;

    int32 iNo = DecisionUnitCommonParameter().PlayerWatcher().GetTargetPlayerData().no;
    if (CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(), iNo, false))
        return false;

    float fAttackInterval = DecisionUnitCommonParameter().GetAttackInterval();
    float fAttackTimer = Math::RandFloatRange(0.0f, (fAttackInterval * 2.0f));
    DecisionUnitCommonParameter().SetAttackTimer(fAttackTimer);

    return IsSatisfyFrequency(CEnemyParameter::FREQUENCY_ATTACK_RUN);
};


/*virtual*/ void CBaseStoneBiterAI::CDecisionUnitAttackFar::OnStart(void) /*override*/
{
    DecisionUnitCommonParameter().PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_SUIT);

    SetAttackOrder(BASESTONEBITER_AI::ORDERTYPE_ATTACK_RUN);
};


//
// *********************************************************************************
//


CBaseStoneBiterAI::CDecisionUnitAttackNear::CDecisionUnitAttackNear(CEnemyCharacter* pEnemyChr)
: CDecisionUnitBaseBiter(pEnemyChr, BASESTONEBITER_AI::AIDECISIONUNITNAME::ATTACK_NEAR, TYPE_ATTACK)
{
    ;
};


/*virtual*/ bool CBaseStoneBiterAI::CDecisionUnitAttackNear::CheckTerm(void) /*override*/
{
    if (!DecisionUnitCommonParameter().IsAttackPermission())
        return false;

    if (!DecisionUnitCommonParameter().PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(70.0f)))
        return false;

    int32 iNo = DecisionUnitCommonParameter().PlayerWatcher().GetTargetPlayerData().no;
    if (CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(), iNo, false))
        return false;

    if (m_enemyWatcher.IsFriendStayFront(3.0f))
        return false;

    if (m_enemyWatcher.IsFriendAttack())
        return false;

    float fAttackInterval = DecisionUnitCommonParameter().GetAttackInterval();
    float fAttackTimer = Math::RandFloatRange(0.0f, (fAttackInterval * 2.0f));
    DecisionUnitCommonParameter().SetAttackTimer(fAttackTimer);

    return true;
};


/*virtual*/ void CBaseStoneBiterAI::CDecisionUnitAttackNear::OnStart(void) /*override*/
{
    DecisionUnitCommonParameter().PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_SUIT);

    SetAttackOrder(BASESTONEBITER_AI::ORDERTYPE_ATTACK_A);

    if (IsSatisfyFrequency(CEnemyParameter::FREQUENCY_ATTACK_B))
        SetAttackOrder(BASESTONEBITER_AI::ORDERTYPE_ATTACK_B);

    if (IsSatisfyFrequency(CEnemyParameter::FREQUENCY_ATTACK_C))
        SetAttackOrder(BASESTONEBITER_AI::ORDERTYPE_ATTACK_C);
};


//
// *********************************************************************************
//


CBaseStoneBiterAI::CDecisionUnitMoveSide::CDecisionUnitMoveSide(CEnemyCharacter* pEnemyChr)
: CDecisionUnitBaseBiter(pEnemyChr, BASESTONEBITER_AI::AIDECISIONUNITNAME::MOVE_SIDE, TYPE_MOVE)
, m_iPrevMoveType(0)
{
    ;
};


/*virtual*/ bool CBaseStoneBiterAI::CDecisionUnitMoveSide::CheckTerm(void) /*override*/
{
    if (!DecisionUnitCommonParameter().PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
        return true;

    if (m_enemyWatcher.IsFriendStayFront(3.0f))
        return m_enemyWatcher.IsFriendAttack();

    return false;
};


/*virtual*/ void CBaseStoneBiterAI::CDecisionUnitMoveSide::OnStart(void) /*override*/
{
    DecisionUnitCommonParameter().PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_SUIT);

    int32 iOrderMoveType = Math::RandRange(BASESTONEBITER_AI::ORDERTYPE_MOVE_SIDE_RIGHT,
                                           BASESTONEBITER_AI::ORDERTYPE_MOVE_SIDE_LEFT + 1);

    if (iOrderMoveType == m_iPrevMoveType)
    {
        iOrderMoveType = (m_iPrevMoveType == BASESTONEBITER_AI::ORDERTYPE_MOVE_SIDE_RIGHT ?
                                             BASESTONEBITER_AI::ORDERTYPE_MOVE_SIDE_LEFT :
                                             BASESTONEBITER_AI::ORDERTYPE_MOVE_SIDE_RIGHT);
    };

    SetMoveOrder(iOrderMoveType);

    EnemyChr().AIThinkOrder().OrderMove().m_fMoveTimer = Math::RandFloatRange(1.0f, 5.0f);

    m_iPrevMoveType = iOrderMoveType;
};


//
// *********************************************************************************
//


CBaseStoneBiterAI::CDecisionUnitMove::CDecisionUnitMove(CEnemyCharacter* pEnemyChr)
: m_enemyWatcher()
{
    m_enemyWatcher.Initialize(pEnemyChr, true);
};


/*virtual*/ bool CBaseStoneBiterAI::CDecisionUnitMove::CheckTerm(void) /*override*/
{
    return CBaseAI6045::CDecisionUnitMove2::CheckTerm();
};


/*virtual*/ CBaseStoneBiterAI::CDecisionUnitMove::RESULT
CBaseStoneBiterAI::CDecisionUnitMove::Update(void) /*override*/
{
    return RESULT_STOP;
};


/*virtual*/ void CBaseStoneBiterAI::CDecisionUnitMove::OnStart(void) /*override*/
{
    m_fMoveTime = Math::RandFloatRange(0.5f, 1.5f);

    CBaseAI6045::CDecisionUnitMove2::OnStart();

    int32 iNo = m_enemyWatcher.GetFreePlayerNo(2);
    if (iNo == -1)
        iNo = m_enemyWatcher.GetFreePlayerNo();

    EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo = iNo;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* CBaseStoneBiterAI::Intsance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseStoneBiterAI(pEnemyChr);
};


CBaseStoneBiterAI::CBaseStoneBiterAI(CEnemyCharacter* pEnemyChr)
: CBaseAI6045(pEnemyChr)
{
    /* init common param */
    uint8 freq = AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_COMMON_ATTACKINTERVAL);
    float fAttackInterval = static_cast<float>(freq) * 0.01f;
    
    DecisionUnitCommonParameter().SetAttackInterval(fAttackInterval);
    DecisionUnitCommonParameter().SetMoveInterval(0.5f);

    /* init decision units (attach order is important!) */
    CEnemyAIDecisionUnit* pUnit = nullptr;

    pUnit = new CBaseAI6045::CDecisionUnitGuard2;
    Attach(pUnit);

    pUnit = new CBaseStoneBiterAI::CDecisionUnitAttackNear(pEnemyChr);
    Attach(pUnit);

    pUnit = new CBaseStoneBiterAI::CDecisionUnitAttackFar(pEnemyChr);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitMovePatrolOrigin;
    static_cast<CBaseAI6045::CDecisionUnitMovePatrolOrigin*>(pUnit)->SetMoveTime(2.0f);
    static_cast<CBaseAI6045::CDecisionUnitMovePatrolOrigin*>(pUnit)->SetWaitTimer(0.2f);
    Attach(pUnit);

    pUnit = new CBaseStoneBiterAI::CDecisionUnitMoveSide(pEnemyChr);
    pUnit->SetConditionDistance(5.0f);
    Attach(pUnit);

    pUnit = new CBaseStoneBiterAI::CDecisionUnitMove(pEnemyChr);
    static_cast<CBaseStoneBiterAI::CDecisionUnitMove*>(pUnit)->SetConditionDistance(6.0f);
    static_cast<CBaseStoneBiterAI::CDecisionUnitMove*>(pUnit)->SetCheckObstacleDistance(3.0f);
    static_cast<CBaseStoneBiterAI::CDecisionUnitMove*>(pUnit)->SetCheckObstacleDistanceDivNum(3);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitMoveObstacleJump;
    static_cast<CBaseAI6045::CDecisionUnitMoveObstacleJump*>(pUnit)->SetCheckObstacleDistance(3.0f);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitIdle;
    Attach(pUnit);

    pUnit = new CEnemyAIDecisionUnitTrigger;
    Attach(pUnit);
};


/*virtual*/ CBaseStoneBiterAI::~CBaseStoneBiterAI(void)
{
    ;
};