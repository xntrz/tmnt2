#include "BaseMouserAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


CBaseMouserAI::CDecisionUnitAttackCommon::CDecisionUnitAttackCommon(const char* pszUnitName,
                                                                    BASEMOUSER_AI::ORDERTYPE_ATTACK orderType)
: CBaseAI6045::CDecisionUnitAttackNormal(pszUnitName)    
{
    m_orderType = orderType;
};


/*virtual*/ bool CBaseMouserAI::CDecisionUnitAttackCommon::CheckTerm(void) /*override*/
{
    if (CBaseAI6045::CDecisionUnitAttackNormal::CheckTerm())
        return !CheckObstacleFront(0.5f, 0.3f);

    return false;
};


/*virtual*/ void CBaseMouserAI::CDecisionUnitAttackCommon::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();

    DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_MOVE_HITANDAWAY);
};


bool CBaseMouserAI::CDecisionUnitAttackCommon::CheckObstacleFront(float fDistance, float fSafeHeight)
{
    float fObstacleHeight = 0.0f;
    if (CAIUtils::CheckStep(&EnemyChr().Compositor(), fDistance, &fObstacleHeight))
    {
        RwV3d vecFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecFootPos);

        if (Math::FEqual(vecFootPos.y, fObstacleHeight))
            return true;
        
        if ((vecFootPos.y + fSafeHeight) < fObstacleHeight)
            return true;
    };

    return false;
};


//
// *********************************************************************************
//


CBaseMouserAI::CDecisionUnitAttackA::CDecisionUnitAttackA(void)
: CDecisionUnitAttackCommon(BASEMOUSER_AI::AIDECISIONUNITNAME::ATTACK_A, BASEMOUSER_AI::ORDERTYPE_ATTACK_A)
{
    ;
};


//
// *********************************************************************************
//


CBaseMouserAI::CDecisionUnitAttackB::CDecisionUnitAttackB(void)
: CDecisionUnitAttackCommon(BASEMOUSER_AI::AIDECISIONUNITNAME::ATTACK_B, BASEMOUSER_AI::ORDERTYPE_ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseMouserAI::CDecisionUnitAttackC::CDecisionUnitAttackC(void)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEMOUSER_AI::AIDECISIONUNITNAME::ATTACK_C)
{
    m_orderType = BASEMOUSER_AI::ORDERTYPE_ATTACK_C;
};


/*virtual*/ bool CBaseMouserAI::CDecisionUnitAttackC::CheckTerm(void) /*override*/
{
    if (CBaseAI6045::CDecisionUnitAttackNormal::CheckTerm())
    {
        if (DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_ENABLE) &&
            !DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_PERMIT))
        {
            if (EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_REDUCTION_HP) == 4) // TODO freq naming
                return true;
        };
    };

    return false;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* CBaseMouserAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseMouserAI(pEnemyChr);
};


CBaseMouserAI::CBaseMouserAI(CEnemyCharacter* pEnemyChr)
: CBaseAI6045(pEnemyChr)
{
    /* init common param */
    uint8 freq = AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_COMMON_ATTACKINTERVAL);
    float fAttackInterval = static_cast<float>(freq) * 0.01f;
    
    DecisionUnitCommonParameter().SetAttackInterval(fAttackInterval);
    DecisionUnitCommonParameter().SetMoveInterval(0.5f);

    /* init decision units (attach order is important!) */
    CEnemyAIDecisionUnit* pUnit = nullptr;

    pUnit = new CBaseMouserAI::CDecisionUnitAttackA;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_A);
    Attach(pUnit);

    pUnit = new CBaseMouserAI::CDecisionUnitAttackB;
    pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_B);
    Attach(pUnit);

    if (EnemyCharacter().GetID() != ENEMYID::ID_RAPTOR)
    {
        pUnit = new CBaseMouserAI::CDecisionUnitAttackC;
        pUnit->SetFreqTable(CEnemyParameter::FREQUENCY_ATTACK_C);
        Attach(pUnit);
    };

    pUnit = new CBaseAI6045::CDecisionUnitMove2;
    static_cast<CBaseAI6045::CDecisionUnitMove2*>(pUnit)->SetMoveTime(0.5f);
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


/*virtual*/ CBaseMouserAI::~CBaseMouserAI(void)
{
    ;
};