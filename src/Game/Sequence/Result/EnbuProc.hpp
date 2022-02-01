#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CEnbuProc
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Settings(void);
    static void Period(void);
    static void Draw2D(void);
    static void Draw3D(void);
    static PLAYERID::VALUE GetEnbuMvp(void);
    static GAMETYPES::CLEARRANK GetEnbuRank(void);
    static GAMETYPES::COSTUME GetEnbuCostume(void);
    static float GetEnbuTime(void);
    static float GetEnbuEndTime(void);
    static bool IsEnbuIdle(void);
};