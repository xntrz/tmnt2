#pragma once

#include "EnemyID.hpp"
#include "EnemyTypes.hpp"


class CEnemyParameter
{
public:
    /* this index enum valid only for NON-bosses enemies */
    enum FREQUENCY_INDEX
    {
        /* 0 */  FREQUENCY_ATTACK_A = 0,
        /* 1 */  FREQUENCY_ATTACK_AA,
        /* 2 */  FREQUENCY_ATTACK_AAA,
        /* 3 */  FREQUENCY_ATTACK_B,
        /* 4 */  FREQUENCY_ATTACK_C,
        /* 5 */  FREQUENCY_ATTACK_RUN,
        /* 6 */  FREQUENCY_GUARD_A,
        /* 7 */  FREQUENCY_GUARD_B,
        /* 8 */  FREQUENCY_GUARD_SHOT,
        /* 9 */  FREQUENCY_GUARD_JUMP_ATTACK,
        /* 10 */ FREQUENCY_GUARD_RUN_ATTACK,
        /* 11 */ FREQUENCY_GUARD_THROW,
        /* 12 */ FREQUENCY_MOVE_APPROACH,
        /* 13 */ FREQUENCY_MOVE_FORMATION,
        /* 14 */ FREQUENCY_TRIGGER_ATTACK,
        /* 15 */ FREQUENCY_TRIGGER_MOVE,
        /* 16 */ FREQUENCY_TRIGGER_ESCAPE,
        /* 17 */ FREQUENCY_TRIGGER_REDUCTION_HP,
        /* 18 */ FREQUENCY_FIRE_CONVERGENCE,
        /* 19 */ FREQUENCY_FIRE_RANGE,
        /* 20 */ FREQUENCY_FIRE_RANGE_RATE,
        /* 21 */ FREQUENCY_FIRE_REPEATABLE,
        /* 22 */ FREQUENCY_FIRE_REPEATCOUNT,
        /* 23 */ FREQUENCY_FIRE_REPEATCHASE,
        /* 24 */ FREQUENCY_COMMON_ATTACKINTERVAL,
        /* 25 */ FREQUENCY_COMMON_1,
        /* 26 */ FREQUENCY_COMMON_2,
        
        /* 27 */ FREQUENCY_MAX,
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