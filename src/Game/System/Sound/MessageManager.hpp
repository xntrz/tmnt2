#pragma once

#include "SeGroupID.hpp"

#include "Game/Component/Player/PlayerID.hpp"


class CMessageManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Draw(void);
    static void Period(void);
    static bool Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer);
    static bool OnlyTextRequest(SEGROUPID::VALUE idGroup);
    static bool DirectCall(int32 nVoiceCode, bool bBusyCheck = false);
};