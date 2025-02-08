#pragma once


namespace GAMETYPES
{
    enum
    {
        ATTACKPOWER_MIN = 5,
        ATTACKPOWER_MAX = 20,
        GIMMICK_MAX = 256,
        ENEMY_MAX = 64,
        PLAYERS_MAX = 4,
        CHARACTER_MAX = 4,
        PRIZE_MAX = 4,
        STAGE_MAX = 10, // per area
        CRYSTAL_MAX = 30,
    };

    enum STAGEMODE
    {
        STAGEMODE_NONE = 0,
        STAGEMODE_NORMAL,
        STAGEMODE_HOME,
        STAGEMODE_RIDE,
        STAGEMODE_NEXUS,
        STAGEMODE_SPARRING, // not exists in game
        STAGEMODE_PLAYDEMO,

        STAGEMODE_NUM,
    };

    enum GAMEMODE
    {
        GAMEMODE_NORMAL = 0,
        GAMEMODE_DEMO,
        GAMEMODE_NEXUS,

        GAMEMODE_NUM,
    };

    enum DIFFICULTY
    {
        DIFFICULTY_EASY = 0,
        DIFFICULTY_NORMAL,
        DIFFICULTY_HARD,

        DIFFICULTY_NUM,
    };

    enum CRYSTALTYPE
    {
        CRYSTALTYPE_NONE = 0,
        CRYSTALTYPE_RED,
        CRYSTALTYPE_GREEN,
        CRYSTALTYPE_WHITE,
        CRYSTALTYPE_ORANGE,

        CRYSTALTYPE_NUM,
    };

    enum PRIZE
    {
        PRIZE_NONE = 0x0,
        PRIZE_CRYSTAL,
        PRIZE_ANTIQUE,
        PRIZE_COMEBACK,
        PRIZE_BONUS,
        PRIZE_SECRET,

        PRIZE_NUM,
    };

    enum COSTUME
    {
        COSTUME_NONE = 0,
        COSTUME_SAMURAI,
        COSTUME_NEXUS,

        COSTUME_NUM,
    };

    enum TECACT
    {
        TECACT_NONE         = 0,
        
        TECACT_AABBC        = 0x1,
        TECACT_B_PHASE3     = 0x2,
        TECACT_AABB         = 0x3,
        TECACT_THROW_BACK   = 0x4,
        TECACT_AAC          = 0x5,
        TECACT_B_PHASE2     = 0x6,
        TECACT_AAB          = 0x7,
        TECACT_B_PHASE1     = 0x8,
        TECACT_RUN          = 0x9,
        TECACT_THROW        = 0xA,
        TECACT_AA           = 0xB,
        TECACT_B_PHASENONE  = 0xC,
        TECACT_KNIFE        = 0xD,
        TECACT_A            = 0xE,
        TECACT_DASH         = 0xF,
        TECACT_GUARD_IMPACT = 0x10,
        TECACT_AABC         = 0x11,
        TECACT_AABBB        = 0x12,
        TECACT_COMBINATION  = 0x13,
        TECACT_KNIFE_JUMP   = 0x14,        
        TECACT_JUMP         = 0x15,
        TECACT_JUMP_WALL    = 0x16,
        TECACT_JUMP_DOUBLE  = 0x17,        
        TECACT_DAMAGED      = 0x18,
        TECACT_FALLEN       = 0x19,
        TECACT_MAX          = 0x1A,
    };

    enum RIDEACT
    {
        RIDEACT_NONE        = 0,
        RIDEACT_SCORE_SILVER= 0x1,
        RIDEACT_SCORE_GOLD  = 0x2,
        RIDEACT_JUMP        = 0x3,
        RIDEACT_TRICK       = 0x4,    // turn (normal ride) or roll (space ride)
        RIDEACT_WALL_CRASH  = 0x5,
        RIDEACT_KILL        = 0x6,
        //
        //  TODO: 0x7
        //
        RIDEACT_MAX         = 0x8,        
    };

    enum CLEARRANK : uint8
    {
        CLEARRANK_NONE = 0,
        CLEARRANK_E,
        CLEARRANK_D,
        CLEARRANK_C,
        CLEARRANK_B,
        CLEARRANK_A,
        CLEARRANK_S,
        CLEARRANK_SS,

        CLEARRANK_NUM,
    };

    enum RESULTTYPE
    {
        RESULTTYPE_NORMAL = 0,
        RESULTTYPE_RIDE,
        RESULTTYPE_NEXUS,
        RESULTTYPE_ENDING,
    };

    enum NEXUSID
    {
        NEXUSID_KITTY_OPEN = 0,
        NEXUSID_MONSTER_OPEN,
        NEXUSID_FOOT_COMBAT,
        NEXUSID_BATTLE_NEXUS,

        NEXUSID_NUM,
    };

    enum ENDINGTYPE
    {
        ENDINGTYPE_STORY = 0,
        ENDINGTYPE_NEXUS,
        ENDINGTYPE_SHOP,
    };
};