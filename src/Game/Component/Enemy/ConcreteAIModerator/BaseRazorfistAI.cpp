#include "BaseRazorfistAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


CBaseRazorfistAI::CDecisionUnitAttackCommon::CDecisionUnitAttackCommon(const char* pszUnitName,
                                                                       BASERAZORFIST_AI::ORDERTYPE_ATTACK orderType)
: CBaseAI6045::CDecisionUnitAttackNormal(pszUnitName)    
{
    m_orderType = orderType;
};


/*virtual*/ void CBaseRazorfistAI::CDecisionUnitAttackCommon::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();

    DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_MOVE_HITANDAWAY);
};


//
// *********************************************************************************
//


CBaseRazorfistAI::CDecisionUnitAttackA::CDecisionUnitAttackA(void)
: CDecisionUnitAttackCommon(BASERAZORFIST_AI::AIDECISIONUNITNAME::ATTACK_A, BASERAZORFIST_AI::ORDERTYPE_ATTACK_A)
{
    ;
};


//
// *********************************************************************************
//


CBaseRazorfistAI::CDecisionUnitAttackB::CDecisionUnitAttackB(void)
: CDecisionUnitAttackCommon(BASERAZORFIST_AI::AIDECISIONUNITNAME::ATTACK_B, BASERAZORFIST_AI::ORDERTYPE_ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseRazorfistAI::CDecisionUnitAttackC::CDecisionUnitAttackC(void)
: CDecisionUnitAttackCommon(BASERAZORFIST_AI::AIDECISIONUNITNAME::ATTACK_C, BASERAZORFIST_AI::ORDERTYPE_ATTACK_C)
{
    ;
};


//
// *********************************************************************************
//


CBaseRazorfistAI::CDecisionUnitAttackD::CDecisionUnitAttackD(void)
: CDecisionUnitAttackCommon(BASERAZORFIST_AI::AIDECISIONUNITNAME::ATTACK_D, BASERAZORFIST_AI::ORDERTYPE_ATTACK_D)
{
    ;
};


//
// *********************************************************************************
//


CBaseRazorfistAI::CDecisionUnitAttackCounter::CDecisionUnitAttackCounter(void)
: CBaseAI6045::CDecisionUnitAttackCounter(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER)
{
    ;
};


/*virtual*/ bool CBaseRazorfistAI::CDecisionUnitAttackCounter::CheckTerm(void) /*override*/
{
    return false;
};


void CBaseRazorfistAI::CDecisionUnitAttackCounter::ResetTarget(void)
{
    m_orderTargetNo = -1;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* CBaseRazorfistAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseRazorfistAI(pEnemyChr);
};


CBaseRazorfistAI::CBaseRazorfistAI(CEnemyCharacter* pEnemyChr)
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

    pUnit = new CBaseRazorfistAI::CDecisionUnitAttackA;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_A);
    pUnit->SetConditionDistance(3.0f);
    Attach(pUnit);

    pUnit = new CBaseRazorfistAI::CDecisionUnitAttackB;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_B);
    Attach(pUnit);

    pUnit = new CBaseRazorfistAI::CDecisionUnitAttackC;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_C);
    Attach(pUnit);

    pUnit = new CBaseRazorfistAI::CDecisionUnitAttackD;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_AA);
    pUnit->SetConditionDistance(static_cast<float>(AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE)));
    Attach(pUnit);

    pUnit = new CBaseRazorfistAI::CDecisionUnitAttackCounter;
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


/*virtual*/ CBaseRazorfistAI::~CBaseRazorfistAI(void)
{
    ;
};


/*virtual*/ void CBaseRazorfistAI::CatchTriggerActionOrder(void) /*override*/
{
    if (m_catchTriggerAction & BASERAZORFIST_AI::CATCHTRIGGER_COUNTER)
    {
        DecisionUnitCommonParameter().ChangeDicisionUnit(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);

        CEnemyAIDecisionUnit* pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);
        if (pUnit)
            static_cast<CBaseRazorfistAI::CDecisionUnitAttackCounter*>(pUnit)->ResetTarget();
    };
};