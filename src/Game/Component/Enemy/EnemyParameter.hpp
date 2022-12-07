#pragma once

#include "EnemyID.hpp"
#include "EnemyTypes.hpp"


class CEnemyParameter
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(const void* Buffer, uint32 BufferSize);
    static int32 Search(ENEMYID::VALUE id, int32 pattern);
    static int32 GetHP(int32 index);
    static uint8 GetFrequency(int32 index, int32 num);
    static int32 GetFrequencyMax(int32 index);
    static const ENEMYTYPES::CHARACTERISTIC& GetAICharacteristic(int32 index);
};