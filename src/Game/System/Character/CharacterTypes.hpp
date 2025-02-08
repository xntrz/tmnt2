#pragma once


class CHitAttackData;


namespace CHARACTERTYPES
{
    enum
    {
        MESSAGEID_AERIAL = 0x12,
        MESSAGEID_TOUCHDOWN,
        MESSAGEID_RECVDMG,
        MESSAGEID_DEATHFLOOR,
        MESSAGEID_CATCH,
        MESSAGEID_LIFT,
        MESSAGEID_THROW,
        MESSAGEID_MISSTHROW,
        MESSAGEID_COMBINATION,
    };

    enum BONEID
    {
        // TODO naming and move this to PLAYERTYPES because they are are different for enemy and player
        BONEID_POSITION     = 0,
        BONEID_GROUND       = 1,
        BONEID_BODY         = 2,
        BONEID_RIGHT_WRIST  = 3,

        BONEID_RIGHT_HAND   = 7,
        BONEID_LEFT_HAND    = 8,

        BONEID_HEAD         = 10,

        BONEID_RIGHT_HAND_S = 12,
        BONEID_LEFT_HAND_S  = 13,

        BONEID_RIDE_WING    = 20,

        BONEIDNUM,
    };

    enum ATTACKDIRECTIONTYPE
    {
        ATTACKDIRECTIONTYPE_FRONT = 0,
        ATTACKDIRECTIONTYPE_BACK,
    };

    struct ATTACKPARAMETER
    {
        RwV3d               m_vPosition;
        RwV3d               m_vDirection;
        RwV3d               m_vVelocity;
        ATTACKDIRECTIONTYPE m_direction;
        float               m_fTroubleTime;
        CHitAttackData*     m_pHitAttackData;
    };

    enum ATTACKRESULTTYPE
    {
        ATTACKRESULTTYPE_INEFFECTIVE = 0,
        ATTACKRESULTTYPE_DAMAGE_KNOCK,
        ATTACKRESULTTYPE_DAMAGE_FLYAWAY,
        ATTACKRESULTTYPE_DAMAGE_NOREACTION,
        ATTACKRESULTTYPE_THROW,
        ATTACKRESULTTYPE_DINDLE,
        ATTACKRESULTTYPE_STUN,
        ATTACKRESULTTYPE_SLEEP,
        ATTACKRESULTTYPE_FREEZE,
        ATTACKRESULTTYPE_BIND,
        ATTACKRESULTTYPE_PARTY_ATTACK,
        ATTACKRESULTTYPE_GUARD,
        ATTACKRESULTTYPE_GUARD_IMPACT,
        ATTACKRESULTTYPE_GUARD_BREAK,
    };

    enum ATTACKTYPE
    {
        ATTACKTYPE_UNKNOWN = 0,
        ATTACKTYPE_DAMAGE_KNOCK,
        ATTACKTYPE_DAMAGE_FLYAWAY,
        ATTACKTYPE_DAMAGE_NOREACTION,
        ATTACKTYPE_THROW,
        ATTACKTYPE_DINDLE,
        ATTACKTYPE_STUN,
        ATTACKTYPE_SLEEP,
        ATTACKTYPE_FREEZE,
        ATTACKTYPE_BIND,
        ATTACKTYPE_PARTY_ATTACK,
    };

    enum FLAG : uint32
    {
        FLAG_NONE                           = 0x0,
        FLAG_FIXED_DIRECTION                = 0x1,
        FLAG_FIXED_MODEL_ROTATION           = 0x2,
        FLAG_CLAMP_VELOCITY_XZ              = 0x4,
        FLAG_CANCEL_GRAVITY                 = 0x8,
        FLAG_MOTION_SPEED_CTRL              = 0x10,
        FLAG_INVALID_HIT_BODY               = 0x20,
        FLAG_INVALID_HIT_CATCH              = 0x40,
        FLAG_OCCURED_TIMING                 = 0x80,
        FLAG_OCCURED_INVINCIBILITY_TIMING   = 0x100,
        FLAG_CANCEL_COLLISION_WALL          = 0x200,

        FLAG_DEFAULT                        = FLAG_FIXED_DIRECTION
                                            | FLAG_FIXED_MODEL_ROTATION
                                            | FLAG_INVALID_HIT_CATCH,
    };

    DEFINE_ENUM_FLAG_OPS(FLAG);

    enum DEFENCERSTATUSFLAG : uint32
    {
        DEFENCERSTATUSFLAG_NONE                 = 0x0,
        DEFENCERSTATUSFLAG_AERIAL               = 0x1,
        DEFENCERSTATUSFLAG_GUARD                = 0x2,
        DEFENCERSTATUSFLAG_GUARD_ALLRANGE       = 0x4,
        DEFENCERSTATUSFLAG_GUARD_IMPACT         = 0x8,
        DEFENCERSTATUSFLAG_DEFENCEPOWER_ITEMUP  = 0x10,
        DEFENCERSTATUSFLAG_DEFENCEPOWER_GROWUP  = 0x20,
        DEFENCERSTATUSFLAG_DISABLE_THROW        = 0x40,
        DEFENCERSTATUSFLAG_STAGGER              = 0x80,
        DEFENCERSTATUSFLAG_CHEATCODE_HEALTH     = 0x100,    // NOTE: 'health' cheatcode also morph all trouble attacks to knock attacks

        DEFENCERSTATUSFLAG_STUN                 = 0x200,
        DEFENCERSTATUSFLAG_BIND                 = 0x400,
        DEFENCERSTATUSFLAG_SLEEP                = DEFENCERSTATUSFLAG_STUN | DEFENCERSTATUSFLAG_BIND,
        DEFENCERSTATUSFLAG_DINDLE               = 0x800,
        DEFENCERSTATUSFLAG_UNKN0                = 0x1000,   // TODO unknown character defence flag
        
        DEFENCERSTATUSFLAG_FREEZE               = DEFENCERSTATUSFLAG_DINDLE | DEFENCERSTATUSFLAG_STUN,
        
        DEFENCERSTATUSFLAG_STATUS_SHIFT         = DEFENCERSTATUSFLAG_GUARD_IMPACT,
        
        DEFENCERSTATUSFLAG_STATUS_MASK          = DEFENCERSTATUSFLAG_STUN
                                                | DEFENCERSTATUSFLAG_BIND
                                                | DEFENCERSTATUSFLAG_SLEEP
                                                | DEFENCERSTATUSFLAG_DINDLE
                                                | DEFENCERSTATUSFLAG_FREEZE
                                                | DEFENCERSTATUSFLAG_UNKN0,
    };

    DEFINE_ENUM_FLAG_OPS(DEFENCERSTATUSFLAG);
};