#include "083HunAI.hpp"

#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseHunChr.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


C083HunAI::CDecisionUnitAttackRevenge::CDecisionUnitAttackRevenge(CEnemyCharacter* pEnemyChr)
: CBaseHunAI::CDecisionUnitAttackRevenge(pEnemyChr)
{
    ;
};


/*virtual*/ void C083HunAI::CDecisionUnitAttackRevenge::SetAttackTypeNearRange(void) /*override*/
{
    uint8 freq = EnemyChr().FrequencyParameter(HUN_AI_083::FREQUENCY_REVENGE_NEAR);
    uint8 rand = static_cast<uint8>(Math::RandRange(0, 100));

    if (rand >= freq)
        m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_D;
    else
        m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_B;
};


/*virtual*/ void C083HunAI::CDecisionUnitAttackRevenge::SetAttackTypeMiddleRange(void) /*override*/
{
    uint8 freq = EnemyChr().FrequencyParameter(HUN_AI_083::FREQUENCY_REVENGE_MIDDLE);
    uint8 rand = static_cast<uint8>(Math::RandRange(0, 100));

    if (rand >= freq)
        m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_C;
    else
        m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_F;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* C083HunAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C083HunAI(pEnemyChr);
};


C083HunAI::C083HunAI(CEnemyCharacter* pEnemyChr)
: CBaseHunAI(pEnemyChr)
{
    CBaseHunChr* pBaseHunChr = static_cast<CBaseHunChr*>(pEnemyChr);

     /* init common param */
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();

    DecisionUnitCommonParameter().SetAttackInterval(HUN_AI_083::ATTACK_INTERVAL[difficulty]);
    DecisionUnitCommonParameter().SetMoveInterval(HUN_AI_083::MOVE_INTERVAL_BASE);
    DecisionUnitCommonParameter().SetWaitTimerAllPlayerDown(HUN_AI_083::ATTACK_WAITTIME_ALLPLAYERDOWN[difficulty]);

    /* init decision units (attach order is important!) */
    CEnemyAIDecisionUnit* pUnit = nullptr;

    pUnit = new C083HunAI::CDecisionUnitAttackRevenge(pEnemyChr);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackDForEscape;
    pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_ATTACK_D_FOR_ESCAPE);
    Attach(pUnit);

    bool bIsNexusMap = CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                       CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X);
#ifdef _DEBUG
    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_TEST))
        bIsNexusMap |= true;
#endif /* _DEBUG */
    if (bIsNexusMap)
    {
        pUnit = new CBaseHunAI::CDecisionUnitAttackFOnTallObject;
        pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_ATTACK_F_ON_TALL_OBJ);
        pUnit->SetConditionDistance(HUN_AI_083::ATTACK_F_ON_TALL_OBJ_CONDITION_DISTANCE);
        Attach(pUnit);
    };

    pUnit = new CBaseAI6045::CDecisionUnitGuard;
    pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_GUARD_N);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackA;
    pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_ATTACK_A);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitAttackThrow;
    pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_ATTACK_THROW);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackC;
    pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_ATTACK_C);
    pUnit->SetConditionDistance(HUN_AI_083::ATTACK_C_CONDITION_DISTANCE);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackF(pBaseHunChr);
    pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_ATTACK_F);
    pUnit->SetConditionDistance(HUN_AI_083::ATTACK_F_CONDITION_DISTANCE);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackB(pEnemyChr);
    pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_ATTACK_B);
    pUnit->SetConditionDistance(HUN_AI_083::ATTACK_B_CONDITION_DISTANCE);
    Attach(pUnit);
    
    pUnit = new CBaseHunAI::CDecisionUnitAttackD;
    pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_ATTACK_D);
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackFConnectFromRun;
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitAttackCounter;
    Attach(pUnit);

    pUnit = new CBaseHunAI::CDecisionUnitMove(pBaseHunChr);
    static_cast<CBaseHunAI::CDecisionUnitMove*>(pUnit)->SetMoveTime(HUN_AI_083::MOVE_TIME);
    static_cast<CBaseHunAI::CDecisionUnitMove*>(pUnit)->SetMoveStopDistance(HUN_AI_083::MOVE_STOP_DINSTANCE);
    static_cast<CBaseHunAI::CDecisionUnitMove*>(pUnit)->SetConditionDistance(HUN_AI_083::MOVE_CONDITION_DISTANCE);
    static_cast<CBaseHunAI::CDecisionUnitMove*>(pUnit)->SetFreqTable(HUN_AI_083::FREQUENCY_MOVE);
    static_cast<CBaseHunAI::CDecisionUnitMove*>(pUnit)->SetCheckJump(false);
    Attach(pUnit);

#ifdef _DEBUG
    pUnit = new CBaseAI6045::CDecisionUnitMoveObstacleJump;
    static_cast<CBaseAI6045::CDecisionUnitMoveObstacleJump*>(pUnit)->SetCheckObstacleDistance(2.0f);
    Attach(pUnit);
#endif /* _DEBUG */    

    pUnit = new CBaseAI6045::CDecisionUnitIdle;
    Attach(pUnit);
};


/*virtual*/ C083HunAI::~C083HunAI(void)
{
    ;
};


/*virtual*/ void C083HunAI::CatchTriggerActionOrder(void) /*override*/
{
    CBaseHunAI::CatchTriggerActionOrder();

    if (m_catchTriggerAction & BASEHUN_AI::CATCHTRIGGER_BURST)
    {
        GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
        DecisionUnitCommonParameter().SetAttackInterval(HUN_AI_083::ATTACK_INTERVAL_FOR_BURST_MODE[difficulty]);

        CEnemyAIDecisionUnit* pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::GUARD);
        if (pUnit)
            pUnit->SetFreqTable(HUN_AI_083::FREQUENCY_GUARD_B);
    };
};