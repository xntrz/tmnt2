#pragma once

#include "BaseHunAI.hpp"


namespace HUN_AI_083
{
    enum FREQUENCY_INDEX
    {
        /*  0 */ FREQUENCY_ATTACK_A = 0,
        /*  1 */ FREQUENCY_ATTACK_B,
        /*  2 */ FREQUENCY_ATTACK_C,
        /*  3 */ FREQUENCY_ATTACK_D,
        /*  4 */ FREQUENCY_ATTACK_D_FOR_ESCAPE,
        /*  5 */ FREQUENCY_ATTACK_THROW,
        /*  6 */ FREQUENCY_ATTACK_F,
        /*  7 */ FREQUENCY_MOVE,
        /*  8 */ FREQUENCY_ATTACK_F_ON_TALL_OBJ,
        /*  9 */ FREQUENCY_REVENGE_NEAR,
        /* 10 */ FREQUENCY_REVENGE_MIDDLE,
        /* 11 */ FREQUENCY_GUARD_N, // normal mode guard
        /* 12 */ FREQUENCY_GUARD_B, // burst mode guard

        /* 13 */ FREQUENCY_MAX,
    }; /* enum FREQUENCY_INDEX */

    static float ATTACK_INTERVAL[] = { 1.5f, 1.0f, 0.5f };
    static_assert(COUNT_OF(ATTACK_INTERVAL) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_INTERVAL_FOR_BURST_MODE[] = { 2.5f, 2.0f, 1.0f };
    static_assert(COUNT_OF(ATTACK_INTERVAL_FOR_BURST_MODE) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_WAITTIME_ALLPLAYERDOWN[] = { 1.5f, 1.0f, 0.75f };
    static_assert(COUNT_OF(ATTACK_WAITTIME_ALLPLAYERDOWN) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_B_CONDITION_DISTANCE = 8.0f;
    static float ATTACK_C_CONDITION_DISTANCE = 5.0f;
    static float ATTACK_F_CONDITION_DISTANCE = 5.0f;
    static float ATTACK_F_ON_TALL_OBJ_CONDITION_DISTANCE = 8.0f;
    static float MOVE_INTERVAL_BASE = 0.5f;
    static float MOVE_CONDITION_DISTANCE = 2.85f;
    static float MOVE_TIME = 0.125f;
    static float MOVE_STOP_DINSTANCE = 2.85f;
}; /* namespace HUN_AI_083 */


class C083HunAI : public CBaseHunAI
{
public:
    class CDecisionUnitAttackRevenge : public CBaseHunAI::CDecisionUnitAttackRevenge
    {
    public:
        CDecisionUnitAttackRevenge(CEnemyCharacter* pEnemyChr);
        virtual void SetAttackTypeNearRange(void) override;
        virtual void SetAttackTypeMiddleRange(void) override;
    };
    
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C083HunAI(CEnemyCharacter* pEnemyChr);
    virtual ~C083HunAI(void);
    virtual void CatchTriggerActionOrder(void) override;
};