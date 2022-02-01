#pragma once


namespace MAGICTYPES
{
    struct COLLISIONATTACK
    {
        int32 m_nStatus;
        float m_fStatusTime;
        float m_fFlyawayX;
        float m_fFlyawayY;
        int32 m_nAntiguard;
        int32 m_nDamage;
        RwV3d m_vPosition;
        float m_fRadius;
        bool m_bConfusion;
    };
    
    struct COLLISIONBODY
    {
        RwV3d m_vPosition;
        float m_fRadius;
    };

    struct MOVEMENT
    {
        RwV3d m_vAcceleration;
        RwV3d m_vVelocity;
    };

    struct LOCUSINFO
    {
        RwRGBA m_Color;
        float m_fRadius;
        int32 m_nNumPoint;
        uint32 m_uAlphaBasis;
    };

    enum FEATURE
    {
        FEATURE_NONE                    = 0x0,
        FEATURE_BODY                    = 0x1,
        FEATURE_ATTACK                  = 0x2,
        FEATURE_COLLISION_HIT           = 0x4,
        FEATURE_COLLISION_HIT_NUM       = 0x8,
        FEATURE_LIVETIME                = 0x10,
        FEATURE_STRAIGHT                = 0x20,
        FEATURE_ACCELERATE              = 0x40,
        FEATURE_GRAVITY                 = 0x80,
        FEATURE_PARENT                  = 0x100,
        FEATURE_HOMING                  = 0x200,
        FEATURE_COLLISION_MAP           = 0x400,
        FEATURE_COLLISION_CHARA         = 0x800,
        FEATURE_COLLISION_MAPOBJECT     = 0x1000,
        FEATURE_COLLISION_ATTACKED      = 0x2000,
        FEATURE_COLLISION_MAP_HIT       = 0x4000,
        FEATURE_COLLISION_CHARA_HIT     = 0x8000,
        FEATURE_COLLISION_MAPOBJECT_HIT = 0x10000,
        FEATURE_COLLISION_ATTACKED_HIT  = 0x20000,
        FEATURE_ATTACK_HIT              = 0x40000,
        FEATURE_ATTACK_TO_PLAYER        = 0x80000,
        FEATURE_ATTACK_TO_ENEMY         = 0x100000,
        FEATURE_ATTACK_TO_MAPOBJ        = 0x200000,
        FEATURE_COLLISION_CHANGE_SIZE   = 0x400000,
        FEATURE_COLLISION_REFLECTION    = 0x800000,
        FEATURE_LOCUS_VER               = 0x1000000,
        FEATURE_LOCUS_HOR               = 0x2000000,
        FEATURE_APPEAR_HORIZON          = 0x4000000,
        FEATURE_LOST_IMMEDIATE          = 0x8000000,

        FEATURE_HITOBJECT               = FEATURE_COLLISION_MAP | FEATURE_COLLISION_CHARA | FEATURE_COLLISION_MAPOBJECT,
        FEATURE_HITOBJECT_VANISH        = FEATURE_COLLISION_MAP_HIT | FEATURE_COLLISION_CHARA_HIT | FEATURE_COLLISION_MAPOBJECT_HIT,
        FEATURE_ENABLE_ATTACK           = FEATURE_COLLISION_ATTACKED | FEATURE_COLLISION_ATTACKED_HIT,
        FEATURE_PLAYER_ATTACK           = FEATURE_ATTACK | FEATURE_PARENT | FEATURE_ATTACK_TO_ENEMY | FEATURE_ATTACK_TO_MAPOBJ | FEATURE_COLLISION_CHANGE_SIZE,
    };
};