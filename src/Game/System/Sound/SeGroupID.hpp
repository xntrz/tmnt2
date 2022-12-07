#pragma once

#include "Game/System/2d/GameText.hpp"


namespace SEGROUPID
{
    enum VALUE
    {
        ID_ONLYVOX_START = 0,
        ID_ONLYVOX_MAX = 227,
        ID_ONLYTEXT_START = ID_ONLYVOX_MAX,
        ID_ONLYTEXT_MAX = 232,
        ID_MAX = ID_ONLYTEXT_MAX,
        ID_INVALID = -1,
    };

    bool IsHelpID(VALUE id);
    bool IsCallFree(VALUE id);
    GAMETEXT::VALUE GetGameText(VALUE id);
};