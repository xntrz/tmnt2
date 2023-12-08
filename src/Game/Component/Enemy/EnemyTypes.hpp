#pragma once

#include "EnemyID.hpp"


namespace ENEMYTYPES
{
    enum MOTIONNAMES
    {

    };

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
        ENEMYID::VALUE m_idEnemy;
        RwV3d m_vPosition;
        float m_fDirection;
        STATUS m_status;
        int32 m_iPattern;
        float m_fRadius;
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
};