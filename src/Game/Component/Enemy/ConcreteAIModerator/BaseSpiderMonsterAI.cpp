#include "BaseSpiderMonsterAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


CBaseSpiderMonsterAI::CDecisionUnitAttackCommon::CDecisionUnitAttackCommon(const char* pszUnitName,
                                                                           BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK orderType)
: CBaseAI6045::CDecisionUnitAttackNormal(pszUnitName)    
{
    m_orderType = orderType;
};


/*virtual*/ void CBaseSpiderMonsterAI::CDecisionUnitAttackCommon::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();

    DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_MOVE_HITANDAWAY);
};


//
// *********************************************************************************
//


CBaseSpiderMonsterAI::CDecisionUnitAttackA::CDecisionUnitAttackA(void)
: CDecisionUnitAttackCommon(BASESPIDERMONSTER_AI::AIDECISIONUNITNAME::ATTACK_A, BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK_WEB_SHOT)
{
    ;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterAI::CDecisionUnitAttackB::CDecisionUnitAttackB(void)
: CDecisionUnitAttackCommon(BASESPIDERMONSTER_AI::AIDECISIONUNITNAME::ATTACK_B, BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterAI::CDecisionUnitAttackC::CDecisionUnitAttackC(void)
: CDecisionUnitAttackCommon(BASESPIDERMONSTER_AI::AIDECISIONUNITNAME::ATTACK_C, BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK_C)
{
    ;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterAI::CDecisionUnitAttackD::CDecisionUnitAttackD(void)
: CDecisionUnitAttackCommon(BASESPIDERMONSTER_AI::AIDECISIONUNITNAME::ATTACK_D, BASESPIDERMONSTER_AI::ORDERTYPE_ATTACK_D)
{
    ;
};


//
// *********************************************************************************
//


CBaseSpiderMonsterAI::CDecisionUnitAttackCounter::CDecisionUnitAttackCounter(void)
: CBaseAI6045::CDecisionUnitAttackCounter(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER)
{
    ;
};


/*virtual*/ bool CBaseSpiderMonsterAI::CDecisionUnitAttackCounter::CheckTerm(void) /*override*/
{
    return false;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* CBaseSpiderMonsterAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseSpiderMonsterAI(pEnemyChr);
};


CBaseSpiderMonsterAI::CBaseSpiderMonsterAI(CEnemyCharacter* pEnemyChr)
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

    pUnit = new CBaseSpiderMonsterAI::CDecisionUnitAttackA;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_A);
    pUnit->SetConditionDistance(static_cast<float>(AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE)));
    Attach(pUnit);

    pUnit = new CBaseSpiderMonsterAI::CDecisionUnitAttackB;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_B);
    Attach(pUnit);

    pUnit = new CBaseSpiderMonsterAI::CDecisionUnitAttackC;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_C);
    Attach(pUnit);

    pUnit = new CBaseSpiderMonsterAI::CDecisionUnitAttackD;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_AA);
    Attach(pUnit);

    pUnit = new CBaseSpiderMonsterAI::CDecisionUnitAttackCounter;
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitMove2;
    static_cast<CBaseAI6045::CDecisionUnitMove2*>(pUnit)->SetMoveTime(0.5f);
    static_cast<CBaseAI6045::CDecisionUnitMove2*>(pUnit)->SetConditionDistance(6.0f);
    static_cast<CBaseAI6045::CDecisionUnitMove2*>(pUnit)->SetPatrolAreaEnable(true);
    static_cast<CBaseAI6045::CDecisionUnitMove2*>(pUnit)->SetCheckObstacleDistance(3.0f);
    static_cast<CBaseAI6045::CDecisionUnitMove2*>(pUnit)->SetCheckObstacleDistanceDivNum(3);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitMoveObstacleJump;
    static_cast<CBaseAI6045::CDecisionUnitMoveObstacleJump*>(pUnit)->SetCheckObstacleDistance(3.0f);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitMovePatrolOrigin;
    static_cast<CBaseAI6045::CDecisionUnitMovePatrolOrigin*>(pUnit)->SetMoveTime(0.5f);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitIdle;
    Attach(pUnit);

    pUnit = new CEnemyAIDecisionUnitTrigger;
    Attach(pUnit);
};


/*virtual*/ CBaseSpiderMonsterAI::~CBaseSpiderMonsterAI(void)
{
    ;
};


/*virtual*/ void CBaseSpiderMonsterAI::CatchTriggerActionOrder(void) /*override*/
{
    /* TODO: unused in retail game */
    if (m_catchTriggerAction & BASESPIDERMONSTER_AI::CATCHTRIGGER_COUNTER)
    {
        DecisionUnitCommonParameter().ChangeDicisionUnit(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);
        EnemyCharacter().AIThinkOrder().OrderAttack().m_iPlayerNo = -1;
    };
};