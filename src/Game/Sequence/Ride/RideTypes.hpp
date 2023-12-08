#pragma once


namespace RIDETYPES
{
    enum SCOREKIND
    {
        SCOREKIND_SILVER = 0,
        SCOREKIND_GOLD,
        
        SCOREKINDNUM,
    };

    enum
    {
        MESSAGEID_SCORE_INC = 301,
        MESSAGEID_SHOT_EOL,
        MESSAGEID_SHOT_HIT,
    };
};