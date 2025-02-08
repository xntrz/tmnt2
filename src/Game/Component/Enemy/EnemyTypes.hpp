#pragma once

#include "EnemyID.hpp"


namespace ENEMYTYPES
{
    enum APPEARTYPE
    {
        // TODO appear type
        APPEARTYPE_WALK_SLOW    = 0,
        APPEARTYPE_WALK_FAST    = 1,
        //APPEARTYPE_UNKNOWN    = 2,
        APPEARTYPE_FALL         = 3,
        APPEARTYPE_FLY_UP       = 4,
        APPEARTYPE_FLY_DOWN     = 5,
        APPEARTYPE_MOTION       = 6, // MOTIONNAMES::TOJO
    };

    namespace MOTIONNAMES
    {
        static const char* IDLE                      = "Idle";
        static const char* WALK                      = "Walk";
        static const char* WALKR                     = "WalkR";
        static const char* WALKL                     = "WalkL";
        static const char* TURNL                     = "TurnL";
        static const char* TURNR                     = "TurnR";
        static const char* RUN                       = "Run";
        static const char* JUMP_READY                = "J1";
        static const char* JUMP                      = "J2";
        static const char* TOUCHDOWN                 = "J3";
        static const char* ATTACK_A                  = "A";
        static const char* ATTACK_AA                 = "AA";
        static const char* ATTACK_AAA                = "AAA";
        static const char* ATTACK_B                  = "B";
        static const char* ATTACK_C                  = "C";
        static const char* ATTACK_C1                 = "C1";
        static const char* ATTACK_C2                 = "C2";
        static const char* ATTACK_RUN                = "RunAttack";
        static const char* JATTACK                   = "JAttack";
        static const char* BACKAWAY                  = "BackAway";
        static const char* GUARD_1                   = "G1";
        static const char* GUARD_2                   = "G2";
        static const char* GUARD_3                   = "G3";
        static const char* DASH1                     = "Dash1";
        static const char* DASH2                     = "Dash2";
        static const char* KNOCK                     = "Yarare";
        static const char* KNOCK_BACK                = "YBack";
        static const char* KNOCK_FRONT               = "YFront";
        static const char* FLYAWAY_IDLING            = "YFrontFuttobi3";
        static const char* FLYAWAY_FRONT             = "YFrontFuttobi1";
        static const char* FLYAWAY_FRONT1            = "YFrontFuttobi1";
        static const char* FLYAWAY_FRONT2            = "YFrontFuttobi2";
        static const char* FLYAWAY_BOUND_FRONT       = "YFrontFuttobi2";
        static const char* FLYAWAY_BOUND_FRONT1      = "YFrontFuttobi3";
        static const char* FLYAWAY_BOUND_FRONT2      = "YFrontFuttobi4";
        static const char* FLYAWAY_BACK              = "YBackFuttobi1";
        static const char* FLYAWAY_BACK1             = "YBackFuttobi1";
        static const char* FLYAWAY_BACK2             = "YBackFuttobi2";
        static const char* FLYAWAY_BOUND_BACK        = "YBackFuttobi2";
        static const char* FLYAWAY_BOUND_BACK1       = "YBackFuttobi3";
        static const char* FLYAWAY_BOUND_BACK2       = "YBackFuttobi4";
        static const char* FLYAWAY_GETUP             = "YFrontFuttobi4";
        static const char* CRASHWALL_TOUCHDOWN_FRONT = "YWall2";
        static const char* CRASHWALL_BACK            = "YWall1";
        static const char* CRASHWALL_FRONT           = "YWall1";
        static const char* CRASHWALL_TOUCHDOWN_BACK  = "YWall2";
        static const char* THROW_BACK                = "NagerareBack";
        static const char* THROW_STANDBY             = "Nage1";
        static const char* THROW_SUCCESS             = "Nage2";
        static const char* LIFT_WALK                 = "NagerareWalk";
        static const char* LIFT                      = "NagerareIdle";
        static const char* LIFT_CHALLENGE            = "Nagerare1";
        static const char* LIFT_SUCCESS              = "Nagerare2";
        static const char* FIRE_C1                   = "ADown1";
        static const char* FIRE_C2                   = "ADown2";
        static const char* FIRE_C3                   = "ADown3";
        static const char* FIRE_B1                   = "A1";
        static const char* FIRE_B2                   = "A2";
        static const char* FIRE_B3                   = "A3";
        static const char* FIRE_A1                   = "AUp1";
        static const char* FIRE_A2                   = "AUp2";
        static const char* FIRE_A3                   = "AUp3";
        static const char* DINDLE                    = "BiriBiri";
        static const char* INASARE                   = "Inasare";
        static const char* STUN                      = "Piyori";
        static const char* DEATH                     = "Death";
        static const char* DEATH1                    = "Death1";
        static const char* DEATH2                    = "Death2";
        static const char* TOJO                      = "Tojo";
        static const char* BODYBLOW_B1               = "B1";
        static const char* BODYBLOW_B2               = "B2";
        static const char* A1                        = "A1";
        static const char* A2                        = "A2";
        static const char* A3                        = "A3";
    }; /* namespace MOTIONNAMES */

    struct CHARACTERISTIC
    {
        float m_fThinkingFrequency;
        float m_fRatioOfActivity;
        float m_fRatioOfMove;
        float m_fRatioOfFrontView;
        float m_fRatioOfRearView;
        float m_fRadiusOfAction;
        float m_fDistanceOfSuitable;
        float m_fRatioOfImpulsiveness;
    };

    enum STATUS
    {
        STATUS_HIDE = 0,
        STATUS_QUIT,
        STATUS_DEATH,
        STATUS_APPEAR,
        STATUS_RETREAT,
        STATUS_AERIAL,
        STATUS_TOUCHDOWN,
        STATUS_THINKING,
        STATUS_IDLE,
        STATUS_TURN,
        STATUS_WALK,
        STATUS_WALK_RIGHT,
        STATUS_WALK_LEFT,
        STATUS_WALK_BACK,
        STATUS_RUN,
        STATUS_DASH,
        STATUS_GETUP,
        STATUS_JUMP_READY,
        STATUS_JUMP,
        STATUS_JUMP_2ND,
        STATUS_ATTACK_A,
        STATUS_ATTACK_AA,
        STATUS_ATTACK_AAA,
        STATUS_ATTACK_B,
        STATUS_ATTACK_C,
        STATUS_ATTACK_RUN,
        STATUS_FIRE_A,
        STATUS_FIRE_B,
        STATUS_FIRE_C,
        STATUS_FIRE_AA,
        STATUS_FIRE_BB,
        STATUS_FIRE_CC,
        STATUS_THROW,
        STATUS_JUMP_ATTACK,
        STATUS_KNOCK,
        STATUS_KNOCK_FRONT,
        STATUS_KNOCK_BACK,
        STATUS_FLYAWAY_FRONT,
        STATUS_FLYAWAY_BACK,
        STATUS_FLYAWAY_BOUND_FRONT,
        STATUS_FLYAWAY_BOUND_BACK,
        STATUS_CRASHWALL_FRONT,
        STATUS_CRASHWALL_BACK,
        STATUS_CRASHWALL_FALL_FRONT,
        STATUS_CRASHWALL_FALL_BACK,
        STATUS_CRASHWALL_TOUCHDOWN_FRONT,
        STATUS_CRASHWALL_TOUCHDOWN_BACK,
        STATUS_DINDLE,
        STATUS_STUN,
        STATUS_COUNTERACT,
        STATUS_SLEEP,
        STATUS_FREEZE,
        STATUS_BIND,
        STATUS_CAUGHT,
        STATUS_LIFTED,
        STATUS_LIFTED_WALK,
        STATUS_THROWN_FRONT,
        STATUS_THROWN_BACK,
        STATUS_GUARD,
        
        STATUS_SPECIAL,
        STATUS_SPECIAL_MAX = 75,
        
        STATUS_MAX,
    };
    
    struct CREATEINFO
    {
        ENEMYID::VALUE  m_idEnemy;
        RwV3d           m_vPosition;
        float           m_fDirection;
        STATUS          m_status;
        int32           m_iPattern;
        float           m_fRadius;
#ifdef _DEBUG
        int32           m_iHPMax;
#endif /* _DEBUG */   
    };

    struct FEATURE
    {
        float m_fWalkMoveSpeed;
        float m_fRunMoveSpeed;
        float m_fDashMoveSpeed;
        float m_fDashMoveTime;
        float m_fRotateRate;
        float m_fJumpInitializeSpeed;
        float m_fAerialMoveSpeed;
        float m_fAerialAcceleration;
        int32 m_iHPMax;
        int32 m_iHP;
        RwV3d m_vPatrolOrigin;
        float m_fPatrolRadius;
        int32 m_iPattern;
    };

    enum FLAG : uint32
    {
        FLAG_NONE           = 0x0,
        FLAG_DEATH_STATUS   = 0x2,
        FLAG_AERIAL_STATUS  = 0x4,
        FLAG_GUARD_ALLRANGE = 0x8,
        FLAG_INVINCIBILITY  = 0x10,
        FLAG_NOREACTION     = 0x20,

        FLAG_DEFAULT        = FLAG_NONE,
    };

    DEFINE_ENUM_FLAG_OPS(FLAG);
}; /* namespace ENEMYTYPES */