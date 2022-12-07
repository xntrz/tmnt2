#pragma once

#include "SeGroupID.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CVoiceManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static bool SetVoice(SEGROUPID::VALUE idSeGroup, bool bIsHelpID);
    static bool SetVoice(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer, bool bIsHelpID);
    static bool PlayVoice(int32 nVoiceCode, bool bIsHelpID);
};