#pragma once

#include "Game/Component/Player/PlayerID.hpp"


class CHomeCharacter
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static bool CheckAppear(PLAYERID::VALUE idPlayer);
};