#pragma once

#include "EnemyID.hpp"
#include "EnemyTypes.hpp"


class CEnemyParameter
{
public:
    enum FREQUENCY_INDEX
    {
        FREQUENCY_ATTACK_A = 0,
        FREQUENCY_ATTACK_AA,
        FREQUENCY_ATTACK_AAA,
        FREQUENCY_ATTACK_B,
        FREQUENCY_ATTACK_C,
        FREQUENCY_ATTACK_RUN,
        FREQUENCY_GUARD_A,
        FREQUENCY_GUARD_B,
        FREQUENCY_GUARD_SHOT,
        FREQUENCY_GUARD_JUMP_ATTACK,
        FREQUENCY_GUARD_RUN_ATTACK,
        FREQUENCY_GUARD_THROW,
        FREQUENCY_MOVE_APPROACH,
        FREQUENCY_MOVE_FORMATION,
        FREQUENCY_TRIGGER_ATTACK,
        FREQUENCY_TRIGGER_MOVE,
        FREQUENCY_TRIGGER_ESCAPE,
        FREQUENCY_TRIGGER_REDUCTION_HP,
        FREQUENCY_FIRE_CONVERGENCE,
        FREQUENCY_FIRE_RANGE,
        FREQUENCY_FIRE_RANGE_RATE,
        FREQUENCY_FIRE_REPEATABLE,
        FREQUENCY_FIRE_REPEATCOUNT,
        FREQUENCY_FIRE_REPEATCHASE,
        FREQUENCY_COMMON_ATTACKINTERVAL,
        FREQUENCY_COMMON_1, // appear type
        FREQUENCY_COMMON_2,
        
        FREQUENCY_MAX,
    };

public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(const void* Buffer, uint32 BufferSize);
    static int32 Search(ENEMYID::VALUE id, int32 pattern);
    static int32 GetHP(int32 index);
    static uint8 GetFrequency(int32 index, int32 num);
    static int32 GetFrequencyMax(int32 index);
    static const ENEMYTYPES::CHARACTERISTIC& GetAICharacteristic(int32 index);
    static bool Exist(ENEMYID::VALUE id);
};