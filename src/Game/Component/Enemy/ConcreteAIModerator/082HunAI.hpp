#pragma once

#include "BaseHunAI.hpp"


namespace HUN_AI_082
{
    enum FREQUENCY_INDEX
    {
        /* 0 */ FREQUENCY_ATTACK_A = 0,
        /* 1 */ FREQUENCY_ATTACK_B,
        /* 2 */ FREQUENCY_ATTACK_C,        
        /* 3 */ FREQUENCY_ATTACK_D,
        /* 4 */ FREQUENCY_ATTACK_D_FOR_ESCAPE,

        /* 5 */ FREQUENCY_MAX,
    };

    static float ATTACK_INTERVAL[] = { 3.0f, 2.0f, 1.0f };
    static_assert(COUNT_OF(ATTACK_INTERVAL) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_WAITTIME_ALLPLAYERDOWN[] = { 3.0f, 2.0f, 1.0f };
    static_assert(COUNT_OF(ATTACK_WAITTIME_ALLPLAYERDOWN) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_C_CONDITION_DISTANCE = 5.0f;
    static float ATTACK_B_CONDITION_DISTANCE = 8.0f;
    static float MOVE_TIME = 0.5f;
    static float MOVE_CONDITION_DISTANCE = 7.0f;
    static float MOVE_INTERVAL_BASE = 1.0f;
};


class C082HunAI : public CBaseHunAI
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C082HunAI(CEnemyCharacter* pEnemyChr);
    virtual ~C082HunAI(void);
};