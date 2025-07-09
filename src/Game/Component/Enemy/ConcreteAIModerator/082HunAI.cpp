#include "082HunAI.hpp"

#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseHunChr.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


/*static*/ CAIModerator* C082HunAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C082HunAI(pEnemyChr);
};


C082HunAI::C082HunAI(CEnemyCharacter* pEnemyChr)
: CBaseHunAI(pEnemyChr)
{
    /* init common param */
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();

    DecisionUnitCommonParameter().SetAttackInterval(HUN_AI_082::ATTACK_INTERVAL[difficulty]);
    DecisionUnitCommonParameter().SetMoveInterval(HUN_AI_082::MOVE_INTERVAL_BASE);
    DecisionUnitCommonParameter().SetWaitTimerAllPlayerDown(HUN_AI_082::ATTACK_WAITTIME_ALLPLAYERDOWN[difficulty]);

    /* init decision units (attach order is important!) */
    CEnemyAIDecisionUnit* pUnit = nullptr;

    pUnit = new CBaseHunAI::CDecisionUnitAttackRevenge(pEnemyChr);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackDForEscape;
    pUnit->SetFreqTable(HUN_AI_082::FREQUENCY_ATTACK_D_FOR_ESCAPE);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackA;
    pUnit->SetFreqTable(HUN_AI_082::FREQUENCY_ATTACK_A);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackC;
    pUnit->SetFreqTable(HUN_AI_082::FREQUENCY_ATTACK_C);
    pUnit->SetConditionDistance(HUN_AI_082::ATTACK_C_CONDITION_DISTANCE);    
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackB(pEnemyChr);
    pUnit->SetFreqTable(HUN_AI_082::FREQUENCY_ATTACK_B);
    pUnit->SetConditionDistance(HUN_AI_082::ATTACK_B_CONDITION_DISTANCE);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackD;
    pUnit->SetFreqTable(HUN_AI_082::FREQUENCY_ATTACK_D);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackCounter;
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitMoveBoss;
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetMoveTime(HUN_AI_082::MOVE_TIME);
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetConditionDistance(HUN_AI_082::MOVE_CONDITION_DISTANCE);
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


/*virtual*/ C082HunAI::~C082HunAI(void)
{
    ;
};