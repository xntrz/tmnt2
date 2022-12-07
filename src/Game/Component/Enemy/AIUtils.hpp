#pragma once

#include "Game/Component/Player/PlayerTypes.hpp"


class CPlayerCharacter;

class CAIUtils
{
public:
    enum PLAYER_STATE_FLAG
    {
        PLAYER_STATE_NULL       = 0x0,
        PLAYER_STATE_ENABLE     = (1 << 0),
        PLAYER_STATE_IMPOSSIBLE = (1 << 1),
        PLAYER_STATE_ATTACK     = (1 << 2),
        PLAYER_STATE_AERIAL     = (1 << 3),
        PLAYER_STATE_DOWN       = (1 << 4),
        PLAYER_STATE_UNUSUAL    = (1 << 5),
        PLAYER_STATE_KNIFE      = (1 << 6),
        PLAYER_STATE_GUARD      = (1 << 7),
        PLAYER_STATE_ALL        =   PLAYER_STATE_ENABLE |
                                    PLAYER_STATE_IMPOSSIBLE |
                                    PLAYER_STATE_ATTACK |
                                    PLAYER_STATE_AERIAL |
                                    PLAYER_STATE_DOWN |
                                    PLAYER_STATE_UNUSUAL |
                                    PLAYER_STATE_KNIFE |
                                    PLAYER_STATE_GUARD
    };
        
public:
    static int32 GetPlayerNum(void) { return 0; };
    static CPlayerCharacter* GetActivePlayer(int32 iPlayerIndex) { return nullptr; };
    static PLAYER_STATE_FLAG GetPlayerStateFlag(PLAYERTYPES::STATUS eStatus) { return PLAYER_STATE_NULL; };
};