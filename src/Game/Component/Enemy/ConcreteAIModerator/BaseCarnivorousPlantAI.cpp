#include "BaseCarnivorousPlantAI.hpp"

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


CBaseCarnivorousPlantAI::CDecisionUnitAttackA::CDecisionUnitAttackA(void)
: CBaseAI6045::CDecisionUnitAttackNormal(BASECARNIVOROUSPLANT_AI::AIDECISIONUNITNAME::ATTACK_A)
{
    m_orderType = BASECARNIVOROUSPLANT_AI::ORDERTYPE_ATTACK_A;
};


//
// *********************************************************************************
//


CBaseCarnivorousPlantAI::CDecisionUnitAttackB::CDecisionUnitAttackB(void)
: CBaseAI6045::CDecisionUnitAttackNormal(BASECARNIVOROUSPLANT_AI::AIDECISIONUNITNAME::ATTACK_B)
{
    m_orderType = BASECARNIVOROUSPLANT_AI::ORDERTYPE_ATTACK_B;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* CBaseCarnivorousPlantAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseCarnivorousPlantAI(pEnemyChr);
};


CBaseCarnivorousPlantAI::CBaseCarnivorousPlantAI(CEnemyCharacter* pEnemyChr)
: CBaseAI6045(pEnemyChr)
{
    /* init common param */
    uint8 freq = AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_COMMON_ATTACKINTERVAL);
    float fAttackInterval = static_cast<float>(freq) * 0.01f;

    DecisionUnitCommonParameter().SetAttackInterval(fAttackInterval);

    /* init decision units (attach order is important!) */
    CEnemyAIDecisionUnit* pUnit = nullptr;

    pUnit = new CBaseCarnivorousPlantAI::CDecisionUnitAttackA;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_A);
    if (pEnemyChr->GetID() == ENEMYID::ID_MURAL_CARNIVOROUS_PLANT)
        pUnit->SetConditionAngle(MATH_DEG2RAD(72.0f));
    Attach(pUnit);

    pUnit = new CBaseCarnivorousPlantAI::CDecisionUnitAttackB;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_B);
    pUnit->SetConditionDistance(static_cast<float>(AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE)));
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitIdle;
    Attach(pUnit);
};


/*virtual*/ CBaseCarnivorousPlantAI::~CBaseCarnivorousPlantAI(void)
{
    ;
};