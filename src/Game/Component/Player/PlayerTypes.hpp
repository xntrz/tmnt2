#pragma once


namespace PLAYERTYPES
{
    static const int32 WALLJUMP_MAX = 5;
    
    enum STATUS
    {
        STATUS_INVALID = -1,

        NORMALSTART = 0,
        STATUS_IDLE = NORMALSTART,
        STATUS_WALK,
        STATUS_RUN,
        STATUS_DASH_CANCEL,
        STATUS_DASH,
        STATUS_DASH_FINISH,
        STATUS_JUMP_READY,
        STATUS_JUMP,
        STATUS_JUMP_2ND,
        STATUS_JUMP_WALL,
        STATUS_JUMP_WALL_FAILURE,
        STATUS_AERIAL,
        STATUS_AERIAL_MOVE,
        STATUS_TOUCHDOWN,
        STATUS_GUARD_READY,
        STATUS_GUARD,
        STATUS_GUARD_BACK,
        STATUS_GUARD_IMPACT,
        STATUS_GUARD_FINISH,
        STATUS_ATTACK_A,
        STATUS_ATTACK_AA,
        STATUS_ATTACK_AAB,
        STATUS_ATTACK_AAC,
        STATUS_ATTACK_AABB,
        STATUS_ATTACK_AABC,
        STATUS_ATTACK_AABBB,
        STATUS_ATTACK_AABBC,
        STATUS_ATTACK_B_CHARGE,
        STATUS_ATTACK_B,
        STATUS_ATTACK_RUN,
        STATUS_ATTACK_JUMP,
        STATUS_ATTACK_KNIFE,
        STATUS_ATTACK_KNIFE_JUMP,
        STATUS_LIFT_CHALLENGE,
        STATUS_LIFT_SUCCESS,
        STATUS_LIFT,
        STATUS_LIFT_WALK,
        STATUS_THROW,
        STATUS_THROW_BACK,
        STATUS_THROW_COMBINATION,
        STATUS_CAUGHT,
        STATUS_LIFTED,
        STATUS_LIFTED_WALK,
        STATUS_THROWN,
        STATUS_THROWN_BACK,
        STATUS_THROWN_COMBINATION,
        STATUS_THROWN_COMBINATION_END,
        STATUS_THROWN_MISS,
        STATUS_KNOCK_FRONT,
        STATUS_KNOCK_BACK,
        STATUS_FLYAWAY_FRONT,
        STATUS_FLYAWAY_BACK,
        STATUS_FLYAWAY_BOUND_FRONT,
        STATUS_FLYAWAY_BOUND_BACK,
        STATUS_CRASH_WALL_FRONT,
        STATUS_CRASH_WALL_BACK,
        STATUS_CRASH_WALL_FALL_FRONT,
        STATUS_CRASH_WALL_FALL_BACK,
        STATUS_CRASH_WALL_TOUCHDOWN_FRONT,
        STATUS_CRASH_WALL_TOUCHDOWN_BACK,
        STATUS_DINDLE,
        STATUS_STUN,
        STATUS_SLEEP,
        STATUS_BIND,
        STATUS_FREEZE,
        STATUS_DOWN_FRONT,
        STATUS_DOWN_BACK,
        STATUS_TUMBLER_FRONT,
        STATUS_TUMBLER_BACK,
        STATUS_PUSH,
        STATUS_CONSOLE,
        STATUS_DEAD,
        STATUS_DEAD_FLYAWAY,
        NORMALMAX,

        RIDESTART = 0,
        RIDESTATUS_RUN = RIDESTART,
        RIDESTATUS_RIGHT,
        RIDESTATUS_LEFT,
        RIDESTATUS_SPEED_UP,
        RIDESTATUS_SPEED_DOWN,
        RIDESTATUS_CRASH_WALL,
        RIDESTATUS_REVIVE,
        RIDESTATUS_STAGGER,
        RIDESTATUS_JUMP,
        RIDESTATUS_TOUCHDOWN,
        RIDESTATUS_AERIAL,
        RIDESTATUS_AERIAL_STAGGER,
        RIDESTATUS_SHIP_CRASH,
        RIDESTATUS_SHIP_ROLL_RIGHT,
        RIDESTATUS_SHIP_ROLL_LEFT,
        RIDEMAX,
    };

    enum CHARGEPHASE
    {
        CHARGEPHASE_ZERO,
        CHARGEPHASE_1ST,
        CHARGEPHASE_2ND,
        CHARGEPHASE_3RD,
    };

    enum FLAG
    {
        FLAG_NONE                   = 0x0,
        FLAG_JUMP_2ND               = 0x1,
        FLAG_ENABLE_ATTACK_CONNECT  = 0x2,
        FLAG_DISABLE_THROW_KNIFE    = 0x4,
        FLAG_AERIAL_STATUS          = 0x8,
        FLAG_REQUEST_ATTACK_A       = 0x10,
        FLAG_REQUEST_ATTACK_B       = 0x20,
        FLAG_REQUEST_ATTACK_C       = 0x40,
        FLAG_REQUEST_ATTACK_MASK    = FLAG_REQUEST_ATTACK_A | FLAG_REQUEST_ATTACK_B | FLAG_REQUEST_ATTACK_C,
        FLAG_REQUEST_PASSIVE        = 0x80,
        FLAG_REQUEST_RECOVER        = 0x100,
        FLAG_PUSH                   = 0x200,
        FLAG_CONSOLE                = 0x400,
        
        FLAG_DEFAULT = FLAG_NONE,
    };

    struct FEATURE
    {
        float m_fWalkMoveSpeed;
        float m_fLiftWalkMoveSpeed;
        float m_fRunMoveSpeed;
        float m_fDashMoveSpeed;
        float m_fDashTime;
        float m_fJumpInitializeSpeed;
        float m_fAerialMoveSpeed;
        float m_fAerialAcceleration;
        int32 m_nKnifeAttachBoneID;
    };

    struct ATTRIBUTETIME
    {
        float m_fAttackPowerup;
        float m_fDefencePowerup;
        float m_fChargePowerup;
        float m_fKnifePowerup;
        float m_fInvincibility;
        float m_fBlink;
        bool m_bItemEffectExists;
    };

    enum ATTRIBUTE
    {
        ATTRIBUTE_NONE              = 0x0,
        ATTRIBUTE_ATTACK_POWER_UP   = 0x1,
        ATTRIBUTE_DEFENCE_POWER_UP  = 0x2,
        ATTRIBUTE_CHARGE_TIME_CUT   = 0x4,
        ATTRIBUTE_INVINCIBILITY     = 0x8,
        ATTRIBUTE_INNUMERABLE_KNIFE = 0x10,
        ATTRIBUTE_CONFUSION         = 0x20,
    };
};