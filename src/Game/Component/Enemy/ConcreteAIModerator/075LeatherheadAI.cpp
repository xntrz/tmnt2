#include "075LeatherheadAI.hpp"

#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseLeatherheadChr.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"

#ifdef _DEBUG
#include "Game/Sequence/Test/DebugMenuProcess.hpp"
#endif /* _DEBUG */


C075LeatherheadAI::CDecisionUnitSwimAttack::CDecisionUnitSwimAttack(CBaseLeatherheadChr* pBaseLeatherheadChr)
: CBaseAI6045::CDecisionUnitAttackNormal(LEATHERHEAD_AI::AIDECISIONUNITNAME::SWIM_ATTACK)
, m_pBaseLeatherheadChr(pBaseLeatherheadChr)
{
    ;
};


/*virtual*/ bool C075LeatherheadAI::CDecisionUnitSwimAttack::CheckTerm(void) /*override*/
{
    if (!m_pAIDecisionUnitCommonParameter->IsAttackPermission())
        return false;

    if (!m_pBaseLeatherheadChr->IsSwimming())
        return false;

    if (m_pBaseLeatherheadChr->GetSwimTimer() > 0.0f)
        return false;

    if (!m_pAIDecisionUnitCommonParameter->IsEnableDataValid())
        return false;

    int32 numEnableData = m_pAIDecisionUnitCommonParameter->GetEnableDataNum();
    int32 rand = Math::Rand();
    int32 targetPlayerNo = (rand % numEnableData);

    m_orderTargetNo = m_pAIDecisionUnitCommonParameter->GetEnableData(targetPlayerNo).no;

    return true;
};


/*virtual*/ void C075LeatherheadAI::CDecisionUnitSwimAttack::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    float fAttackTime = LEATHERHEAD_AI::SWIMMODE_ATTACK_TIME[difficulty];
    float fAttackTimeRand = LEATHERHEAD_AI::SWIMMODE_ATTACK_TIME_RANDOMSIZE[difficulty];

    float fRandNorm = Math::RandFloat();
    float fSwimTimer = ((fAttackTimeRand * 0.5f) - (fRandNorm * fAttackTimeRand)) + fAttackTime;

    m_pBaseLeatherheadChr->SetSwimTimer(fSwimTimer);
};


//
// *********************************************************************************
//


C075LeatherheadAI::CDecisionUnitSwimMove::CDecisionUnitSwimMove(CBaseLeatherheadChr* pBaseLeatherheadChr)
: CEnemyAIDecisionUnit(LEATHERHEAD_AI::AIDECISIONUNITNAME::SWIM_MOVE, TYPE_MOVE)
, m_pBaseLeatherheadChr(pBaseLeatherheadChr)
{
    ;
};


/*virtual*/ bool C075LeatherheadAI::CDecisionUnitSwimMove::CheckTerm(void) /*override*/
{
    if (m_pBaseLeatherheadChr->IsSwimming())
        return true;

    return false;
};


/*virtual*/ void C075LeatherheadAI::CDecisionUnitSwimMove::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
    EnemyChr().AIThinkOrder().OrderMove().m_iMoveType = LEATHERHEAD_AI::ORDERTYPE_MOVE_SWIM;
};


//
// *********************************************************************************
//


C075LeatherheadAI::CDecisionUnitSwimIdle::CDecisionUnitSwimIdle(void)
: CEnemyAIDecisionUnit(LEATHERHEAD_AI::AIDECISIONUNITNAME::SWIM_IDLE, TYPE_WAIT)
{
    ;
};


/*virtual*/ bool C075LeatherheadAI::CDecisionUnitSwimIdle::CheckTerm(void) /*override*/
{
    /* make it enable only by manual decision unit change (see C075LeatherheadAI::CatchTriggerActionOrder) */
    return false;
};


/*virtual*/ void C075LeatherheadAI::CDecisionUnitSwimIdle::OnStart(void) /*override*/
{
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();

    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
    EnemyChr().AIThinkOrder().OrderWait().m_iWaitType = BASEAI6045::ORDERTYPE_WAIT_IDLE;
    EnemyChr().AIThinkOrder().OrderWait().m_fWaitTimer = LEATHERHEAD_AI::SWIMMODE_AFTERSWIMATTACK_IDLETIME[difficulty];
};


//
// *********************************************************************************
//


/*virtual*/ bool C075LeatherheadAI::CDecisionUnitAttackToAir::CheckTerm(void) /*override*/
{
    if (CBaseAI6045::CDecisionUnitAttackToAir::CheckTerm())
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
        if (pPlrChr)
        {
            RwV3d vecPlrVel = Math::VECTOR3_ZERO;
            pPlrChr->GetVelocity(&vecPlrVel);

            if (vecPlrVel.y > 1.0f)
                return true;
        };
    };

    return false;
};


//
// *********************************************************************************
//


C075LeatherheadAI::CDecisionUnitAttackTackle::CDecisionUnitAttackTackle(void)
: CBaseAI6045::CDecisionUnitAttackNormal(LEATHERHEAD_AI::AIDECISIONUNITNAME::ATTACK_TACKLE)
{
    m_orderType = LEATHERHEAD_AI::ORDERTYPE_ATTACK_E;
};


/*virtual*/ bool C075LeatherheadAI::CDecisionUnitAttackTackle::CheckTerm(void) /*override*/
{
    if (!m_pAIDecisionUnitCommonParameter->IsAttackPermission())
        return false;

    int32 numViewData = m_pAIDecisionUnitCommonParameter->GetViewDataNum();
    for (int32 i = 0; i < numViewData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = m_pAIDecisionUnitCommonParameter->GetViewData(i);

        bool bIsSatisfyDistCondition = (nearerPlayerData.distance <= m_fDistanceCondition);
        bool bIsSatisfyDistSuitable = (nearerPlayerData.distance >= EnemyChr().AICharacteristic().m_fDistanceOfSuitable);
        bool bIsSatisfyAngle = IsSatisfyAngleCondition(&nearerPlayerData);

        if (bIsSatisfyDistCondition &&
            bIsSatisfyDistSuitable &&
            bIsSatisfyAngle)
        {
            bool bIsObstacleBetweenUs = CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(),
                                                                                        nearerPlayerData.no,
                                                                                        false);
            if (!bIsObstacleBetweenUs)
            {
                if (!IsSatisfyFrequency(m_frequencyTableNo))
                    return false;

                m_orderTargetNo = nearerPlayerData.no;
                return true;
            };        
        };
    };

    return false;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* C075LeatherheadAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C075LeatherheadAI(pEnemyChr);
};


C075LeatherheadAI::C075LeatherheadAI(CEnemyCharacter* pEnemyChr)
: CBaseAI6045(pEnemyChr)
{
    CBaseLeatherheadChr* pBaseLeatherheadChr = static_cast<CBaseLeatherheadChr*>(pEnemyChr);

    /* init common param */
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();

    DecisionUnitCommonParameter().SetAttackInterval(LEATHERHEAD_AI::ATTACK_INTERVAL[difficulty]);
    DecisionUnitCommonParameter().SetMoveInterval(LEATHERHEAD_AI::MOVE_INTERVAL_BASE);
    DecisionUnitCommonParameter().SetWaitTimerAllPlayerDown(LEATHERHEAD_AI::ATTACK_WAITTIME_ALLPLAYERDOWN[difficulty]);

    /* init decision units (attach order is important!) */
    CEnemyAIDecisionUnit* pUnit = nullptr;

    pUnit = new C075LeatherheadAI::CDecisionUnitSwimAttack(pBaseLeatherheadChr);
    Attach(pUnit);

    pUnit = new C075LeatherheadAI::CDecisionUnitSwimMove(pBaseLeatherheadChr);
    Attach(pUnit);

    pUnit = new C075LeatherheadAI::CDecisionUnitSwimIdle;
    Attach(pUnit);

    /* NOTE: it will spam counter attack when permission player count (2 by default) is near like in retail game
       TODO: maybe it should be overwritten and do counter attack after some guard time ? */
    pUnit = new CBaseAI6045::CDecisionUnitAttackCounter(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitGuard;
    pUnit->SetFreqTable(LEATHERHEAD_AI::FREQUENCY_GUARD_N);
    Attach(pUnit);

    pUnit = new C075LeatherheadAI::CDecisionUnitAttackToAir;
    static_cast<C075LeatherheadAI::CDecisionUnitAttackToAir*>(pUnit)->SetHeightLow(LEATHERHEAD_AI::ATTACK_TO_AIR_HEIGHT_LOW);
    static_cast<C075LeatherheadAI::CDecisionUnitAttackToAir*>(pUnit)->SetFreqTable(LEATHERHEAD_AI::FREQUENCY_ATTACK_TO_AIR);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitAttackThrow(BASEAI6045::AIDECISIONUNITNAME::ATTACK_THROW);
    pUnit->SetFreqTable(LEATHERHEAD_AI::FREQUENCY_ATTACK_THROW);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitAttackNormal(BASEAI6045::AIDECISIONUNITNAME::ATTACK_NORMAL);
    pUnit->SetFreqTable(LEATHERHEAD_AI::FREQUENCY_ATTACK_AAA);
    Attach(pUnit);

    pUnit = new C075LeatherheadAI::CDecisionUnitAttackTackle;
    pUnit->SetConditionDistance(LEATHERHEAD_AI::ATTACK_TACKLE_CONDITION_DISTANCE);
    pUnit->SetFreqTable(LEATHERHEAD_AI::FREQUENCY_ATTACK_TACKLE);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitMoveBoss;
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetMoveStopDistance(LEATHERHEAD_AI::MOVE_STOP_DINSTANCE);
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetMoveTime(LEATHERHEAD_AI::MOVE_TIME);
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetConditionDistance(LEATHERHEAD_AI::MOVE_CONDITION_DISTANCE);
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetCheckJump(false);    
    Attach(pUnit);

#ifdef _DEBUG
    pUnit = new CBaseAI6045::CDecisionUnitMoveObstacleJump;
    static_cast<CBaseAI6045::CDecisionUnitMoveObstacleJump*>(pUnit)->SetCheckObstacleDistance(2.0f);
    Attach(pUnit);
#endif /* _DEBUG */    

    pUnit = new CBaseAI6045::CDecisionUnitIdle;
    Attach(pUnit);
};


/*virtual*/ C075LeatherheadAI::~C075LeatherheadAI(void)
{
    ;
};


/*virtual*/ void C075LeatherheadAI::CatchTriggerActionOrder(void) /*override*/
{
    if (m_catchTriggerAction & LEATHERHEAD_AI::CATCHTRIGGER_COUNTER)
    {
        DecisionUnitCommonParameter().ChangeDicisionUnit(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);

        CEnemyAIDecisionUnit* pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);
        if (pUnit)
            static_cast<CBaseAI6045::CDecisionUnitAttackCounter*>(pUnit)->ResetTarget();
    };

    if (m_catchTriggerAction & LEATHERHEAD_AI::CATCHTRIGGER_SWIM_IDLE)
    {
        DecisionUnitCommonParameter().ChangeDicisionUnit(LEATHERHEAD_AI::AIDECISIONUNITNAME::SWIM_IDLE);
        EnemyCharacter().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    };

    if (m_catchTriggerAction & LEATHERHEAD_AI::CATCHTRIGGER_SWIM_MOVE)
    {
        DecisionUnitCommonParameter().ChangeDicisionUnit(LEATHERHEAD_AI::AIDECISIONUNITNAME::SWIM_MOVE);
        EnemyCharacter().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    };

    if (m_catchTriggerAction & LEATHERHEAD_AI::CATCHTRIGGER_SWIM_MODE)
    {
        GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
        DecisionUnitCommonParameter().SetAttackInterval(LEATHERHEAD_AI::ATTACK_INTERVAL_FORSWIMMODE[difficulty]);

        CEnemyAIDecisionUnit* pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::GUARD);
        if (pUnit)
            pUnit->SetFreqTable(LEATHERHEAD_AI::FREQUENCY_GUARD_S);
    };
};