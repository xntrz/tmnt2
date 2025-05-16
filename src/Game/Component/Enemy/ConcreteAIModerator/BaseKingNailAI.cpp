#include "BaseKingNailAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


CBaseKingNailAI::CDecisionUnitAttackCommon::CDecisionUnitAttackCommon(const char* pszUnitName,
                                                                      BASEKINGNAIL_AI::ORDERTYPE_ATTACK orderType)
: CBaseAI6045::CDecisionUnitAttackNormal(pszUnitName)    
{
    m_orderType = orderType;
};


/*virtual*/ void CBaseKingNailAI::CDecisionUnitAttackCommon::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();

    DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_MOVE_HITANDAWAY);
};


//
// *********************************************************************************
//


CBaseKingNailAI::CDecisionUnitAttackA::CDecisionUnitAttackA(void)
: CDecisionUnitAttackCommon(BASEKINGNAIL_AI::AIDECISIONUNITNAME::ATTACK_A, BASEKINGNAIL_AI::ORDERTYPE_ATTACK_A)
{
    ;
};


//
// *********************************************************************************
//


CBaseKingNailAI::CDecisionUnitAttackB::CDecisionUnitAttackB(void)
: CDecisionUnitAttackCommon(BASEKINGNAIL_AI::AIDECISIONUNITNAME::ATTACK_B, BASEKINGNAIL_AI::ORDERTYPE_ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseKingNailAI::CDecisionUnitAttackC::CDecisionUnitAttackC(void)
: CDecisionUnitAttackCommon(BASEKINGNAIL_AI::AIDECISIONUNITNAME::ATTACK_C, BASEKINGNAIL_AI::ORDERTYPE_ATTACK_C)
{
    ;
};


//
// *********************************************************************************
//


CBaseKingNailAI::CDecisionUnitAttackD::CDecisionUnitAttackD(void)
: CDecisionUnitAttackCommon(BASEKINGNAIL_AI::AIDECISIONUNITNAME::ATTACK_D, BASEKINGNAIL_AI::ORDERTYPE_ATTACK_D)
{
    ;
};


//
// *********************************************************************************
//


CBaseKingNailAI::CDecisionUnitAttackCounter::CDecisionUnitAttackCounter(void)
: CBaseAI6045::CDecisionUnitAttackCounter(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER)
{
    ;
};


/*virtual*/ bool CBaseKingNailAI::CDecisionUnitAttackCounter::CheckTerm(void) /*override*/
{
    return false;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* CBaseKingNailAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseKingNailAI(pEnemyChr);
};


CBaseKingNailAI::CBaseKingNailAI(CEnemyCharacter* pEnemyChr)
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

    pUnit = new CBaseKingNailAI::CDecisionUnitAttackA;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_A);
    pUnit->SetConditionDistance(static_cast<float>(AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE)));
    Attach(pUnit);

    pUnit = new CBaseKingNailAI::CDecisionUnitAttackB;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_B);
    Attach(pUnit);

    pUnit = new CBaseKingNailAI::CDecisionUnitAttackC;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_C);
    Attach(pUnit);

    pUnit = new CBaseKingNailAI::CDecisionUnitAttackD;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_AA);
    Attach(pUnit);

    pUnit = new CBaseKingNailAI::CDecisionUnitAttackCounter;
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


/*virtual*/ CBaseKingNailAI::~CBaseKingNailAI(void)
{
    ;
};


/*virtual*/ void CBaseKingNailAI::CatchTriggerActionOrder(void) /*override*/
{
    if (m_catchTriggerAction & BASEKINGNAIL_AI::CATCHTRIGGER_COUNTER)
    {
        DecisionUnitCommonParameter().ChangeDicisionUnit(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);

        CEnemyAIDecisionUnit* pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);
        if (pUnit)
            static_cast<CBaseKingNailAI::CDecisionUnitAttackCounter*>(pUnit)->ResetTarget();
    };
};