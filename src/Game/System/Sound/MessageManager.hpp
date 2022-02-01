#pragma once

#include "SeGroupID.hpp"

#include "Game/Component/Player/PlayerID.hpp"


class CMessageManager
{
public:
    static void Initialize(void) {};
    static void Terminate(void) {};
    static void Draw(void) {};
    static void Period(void) {};
    static void Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer){};
    static void OnlyTextRequest(SEGROUPID::VALUE idGroup){};
    static void DirectCall(int32 nVoiceCode, bool bCheckNowPlaying = false){};
};