#include "PlayerStatusDamage.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Player/PlayerUtil.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace PlayerStatus
{
    DEFINE_ENABLED_STATUS_FOR(CKnockCommon, { PLAYERTYPES::STATUS_ATTACK_B,
                                              PLAYERTYPES::STATUS_ATTACK_B,
                                              PLAYERTYPES::STATUS_IDLE,
                                              PLAYERTYPES::STATUS_WALK,
                                              PLAYERTYPES::STATUS_RUN,
                                              PLAYERTYPES::STATUS_DASH,
                                              PLAYERTYPES::STATUS_DASH_FINISH,
                                              PLAYERTYPES::STATUS_JUMP_READY,
                                              PLAYERTYPES::STATUS_TOUCHDOWN,
                                              PLAYERTYPES::STATUS_GUARD_READY,
                                              PLAYERTYPES::STATUS_GUARD,
                                              PLAYERTYPES::STATUS_GUARD_FINISH,
                                              PLAYERTYPES::STATUS_ATTACK_A,
                                              PLAYERTYPES::STATUS_ATTACK_AA,
                                              PLAYERTYPES::STATUS_ATTACK_AAB,
                                              PLAYERTYPES::STATUS_ATTACK_AAC,
                                              PLAYERTYPES::STATUS_ATTACK_AABB,
                                              PLAYERTYPES::STATUS_ATTACK_AABC,
                                              PLAYERTYPES::STATUS_ATTACK_AABBB,
                                              PLAYERTYPES::STATUS_ATTACK_AABBC,
                                              PLAYERTYPES::STATUS_ATTACK_B_CHARGE,
                                              PLAYERTYPES::STATUS_ATTACK_RUN,
                                              PLAYERTYPES::STATUS_ATTACK_KNIFE,
                                              PLAYERTYPES::STATUS_KNOCK_BACK,
                                              PLAYERTYPES::STATUS_KNOCK_FRONT,
                                              PLAYERTYPES::STATUS_STUN,
                                              PLAYERTYPES::STATUS_LIFT_CHALLENGE,
                                              PLAYERTYPES::STATUS_LIFT_SUCCESS,
                                              PLAYERTYPES::STATUS_LIFT,
                                              PLAYERTYPES::STATUS_LIFT_WALK,
                                              PLAYERTYPES::STATUS_THROW,
                                              PLAYERTYPES::STATUS_THROW_BACK,
                                              PLAYERTYPES::STATUS_THROWN,
                                              PLAYERTYPES::STATUS_THROWN_MISS,
                                              PLAYERTYPES::STATUS_PUSH,
                                              PLAYERTYPES::STATUS_CONSOLE });


    void CKnockCommon::OnAttach(void)
    {
        Character().ClearStatusParameter();

        RwV3d vecAttackDir = Character().AttackParameter().m_vDirection;
        Character().SetDirectionFromVector(&vecAttackDir);

        RwV3d vecAttackVel = Character().AttackParameter().m_vVelocity;
        Character().SetVelocity(&vecAttackVel);

        RwV3d vecAccel = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vecAccel, &vecAttackVel, -(1.0f / Character().GetMotionEndTime()));        
        Character().SetAcceleration(&vecAccel);

        IGamepad::StartVibration(Character().GetPadID(), IGamepad::VIBRATIONTYPE_LOW, 0.2f);        
    };


    void CKnockCommon::OnDetach(void)
    {
        ;
    };


    void CKnockCommon::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };

    
    //
    // *********************************************************************************
    //
    

    void CKnockFront::OnAttach(void)
    {
        if (Character().GetStatusPrev() == PLAYERTYPES::STATUS_KNOCK_FRONT)
            Character().ReplayMotion();
        else
            Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::KNOCK_FRONT);
        
        CKnockCommon::OnAttach();
    };

    
    //
    // *********************************************************************************
    //

    
    void CKnockBack::OnAttach(void)
    {
        if (Character().GetStatusPrev() == PLAYERTYPES::STATUS_KNOCK_BACK)
            Character().ReplayMotion();
        else
            Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::KNOCK_BACK);

        CKnockCommon::OnAttach();
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CFlyawayCommon, { PLAYERTYPES::STATUS_IDLE,
                                                PLAYERTYPES::STATUS_WALK,
                                                PLAYERTYPES::STATUS_RUN,
                                                PLAYERTYPES::STATUS_DASH,
                                                PLAYERTYPES::STATUS_DASH_FINISH,
                                                PLAYERTYPES::STATUS_JUMP_READY,
                                                PLAYERTYPES::STATUS_TOUCHDOWN,
                                                PLAYERTYPES::STATUS_GUARD_READY,
                                                PLAYERTYPES::STATUS_GUARD,
                                                PLAYERTYPES::STATUS_GUARD_FINISH,
                                                PLAYERTYPES::STATUS_ATTACK_A,
                                                PLAYERTYPES::STATUS_ATTACK_AA,
                                                PLAYERTYPES::STATUS_ATTACK_AAB,
                                                PLAYERTYPES::STATUS_ATTACK_AAC,
                                                PLAYERTYPES::STATUS_ATTACK_AABB,
                                                PLAYERTYPES::STATUS_ATTACK_AABC,
                                                PLAYERTYPES::STATUS_ATTACK_AABBB,
                                                PLAYERTYPES::STATUS_ATTACK_AABBC,
                                                PLAYERTYPES::STATUS_ATTACK_B_CHARGE,
                                                PLAYERTYPES::STATUS_ATTACK_B,
                                                PLAYERTYPES::STATUS_ATTACK_RUN,
                                                PLAYERTYPES::STATUS_ATTACK_JUMP,
                                                PLAYERTYPES::STATUS_ATTACK_KNIFE,
                                                PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP,
                                                PLAYERTYPES::STATUS_JUMP,
                                                PLAYERTYPES::STATUS_JUMP_2ND,
                                                PLAYERTYPES::STATUS_JUMP_WALL,
                                                PLAYERTYPES::STATUS_AERIAL,
                                                PLAYERTYPES::STATUS_AERIAL_MOVE,
                                                PLAYERTYPES::STATUS_DINDLE,
                                                PLAYERTYPES::STATUS_STUN,
                                                PLAYERTYPES::STATUS_SLEEP,
                                                PLAYERTYPES::STATUS_FREEZE,
                                                PLAYERTYPES::STATUS_KNOCK_FRONT,
                                                PLAYERTYPES::STATUS_KNOCK_BACK,
                                                PLAYERTYPES::STATUS_FLYAWAY_BACK,
                                                PLAYERTYPES::STATUS_FLYAWAY_FRONT,
                                                PLAYERTYPES::STATUS_THROW,
                                                PLAYERTYPES::STATUS_THROW_BACK,
                                                PLAYERTYPES::STATUS_THROWN_BACK,
                                                PLAYERTYPES::STATUS_THROWN,
                                                PLAYERTYPES::STATUS_LIFT_CHALLENGE,
                                                PLAYERTYPES::STATUS_LIFT_SUCCESS,
                                                PLAYERTYPES::STATUS_LIFT,
                                                PLAYERTYPES::STATUS_LIFT_WALK,
                                                PLAYERTYPES::STATUS_PUSH,
                                                PLAYERTYPES::STATUS_CONSOLE,
                                                PLAYERTYPES::STATUS_DEAD_FLYAWAY });


    void CFlyawayCommon::OnAttach(void)
    {
        if ((Character().GetStatusPrev() == PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT) ||
            (Character().GetStatusPrev() == PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK))
        {
            Character().ResetAcceleration();
            Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        }
        else
        {
            Character().ClearStatusParameter();

            RwV3d vAttackDir = Character().AttackParameter().m_vDirection;
            Character().SetDirectionFromVector(&vAttackDir);

            RwV3d vAttackVel = Character().AttackParameter().m_vVelocity;
            Character().SetVelocity(&vAttackVel);
            
            Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);

            IGamepad::StartVibration(Character().GetPadID(), IGamepad::VIBRATIONTYPE_HARD, 0.2f);
        };
    };


    void CFlyawayCommon::OnDetach(void)
    {
        ;
    };


    void CFlyawayCommon::OnRun(void)
    {
        Character().SetAerialMotionTime(10.0f);
        if (Character().IsWallCrashPossible())
            OnCrashWall();
    };


    //
    // *********************************************************************************
    //


    void CFlyawayFront::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::FLYAWAY_FRONT);
        CFlyawayCommon::OnAttach();
    };


    void CFlyawayFront::OnCrashWall(void)
    {
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_CRASH_WALL_FRONT);
    };
    

    //
    // *********************************************************************************
    //


    void CFlyawayBack::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::FLYAWAY_BACK);
        CFlyawayCommon::OnAttach();
    };

    
    void CFlyawayBack::OnCrashWall(void)
    {
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_CRASH_WALL_BACK);
    };


    //
    // *********************************************************************************
    //


    void CFlyawayBoundCommon::OnAttach(void)
    {
        Character().ChangeMotion(GetMotionName());

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vVelocity);
        Math::Vec3_Scale(&vVelocity, &vVelocity, -(1.0f / Character().GetMotionEndTime()));
        Character().SetAcceleration(&vVelocity);

        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_REQUEST_PASSIVE);
        Character().SetEnableCatchHit(false);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetFootPosition(&vPosition);
        vPosition.y = CWorldMap::GetMapHeight(&vPosition);

        const CPlayerCharacter::COLLISIONGROUNDINFO* pGroundInfo = Character().GetCollisionGround();
        ASSERT(pGroundInfo);

        EFFECT_GENERIC::CallTouchDownEffect(pGroundInfo->m_attribute, &vPosition);
        
        if (pGroundInfo->m_attribute != MAPTYPES::ATTRIBUTE_DEATH)
            CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4121));

        IGamepad::StartVibration(Character().GetPadID(), IGamepad::VIBRATIONTYPE_HARD, 0.2f);
    };


    void CFlyawayBoundCommon::OnDetach(void)
    {
        Character().SetEnableCatchHit(true);
    };


    void CFlyawayBoundCommon::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() < 0.2f &&
            Character().TestPlayerFlag(PLAYERTYPES::FLAG_REQUEST_PASSIVE))
        {
            RwV3d vOffset = Math::VECTOR3_ZERO;
            
            CEffectManager::PlayTrace(PLAYERTYPES::EFFECTNAMES::PASSIVE, new CPlayerTracer(m_pPlayerChr), &vOffset);

            PlayerUtil::CallVoiceOfPassive(m_pPlayerChr);
            
            Character().SetInvincibilityEnable(true, 1.0f);
            StateMachine().ChangeStatus(GetNextStatusOfPassive());
        }
        else
        {
            if (Character().IsMotionEnd())
                StateMachine().ChangeStatus(GetNextStatus());
        };
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CFlyawayBoundFront, { PLAYERTYPES::STATUS_FLYAWAY_FRONT });


    const char* CFlyawayBoundFront::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::FLYAWAY_BOUND_FRONT;
    };


    PLAYERTYPES::STATUS CFlyawayBoundFront::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_DOWN_FRONT;
    };


    PLAYERTYPES::STATUS CFlyawayBoundFront::GetNextStatusOfPassive(void) const
    {
        return PLAYERTYPES::STATUS_TUMBLER_FRONT;
    };


    //
    // *********************************************************************************
    //

    
    DEFINE_ENABLED_STATUS_FOR(CFlyawayBoundBack, { PLAYERTYPES::STATUS_THROWN,
                                                   PLAYERTYPES::STATUS_FLYAWAY_BACK });


    void CFlyawayBoundBack::OnDetach(void)
    {
        Character().SetDirection(Character().GetDirection() + Math::PI);
        CFlyawayBoundCommon::OnDetach();
    };


    const char* CFlyawayBoundBack::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::FLYAWAY_BOUND_BACK;
    };


    PLAYERTYPES::STATUS CFlyawayBoundBack::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_DOWN_BACK;
    };


    PLAYERTYPES::STATUS CFlyawayBoundBack::GetNextStatusOfPassive(void) const
    {
        return PLAYERTYPES::STATUS_TUMBLER_BACK;
    };


    //
    // *********************************************************************************
    //

    
    void CCrashWallCommon::OnAttach(void)
    {
        Character().ChangeMotion(GetMotionName());
        Character().ResetVelocity();
        Character().ResetAcceleration();
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        Character().SetEnableCatchHit(false);

        const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = Character().GetCollisionWall();
        ASSERT(pWallInfo);

        float fDirection = Character().GetDirection();
        RwV3d vPosition = pWallInfo->m_vPosition;
        
        uint32 hEffect = CEffectManager::Play(PLAYERTYPES::EFFECTNAMES::CRASH_WALL, &vPosition, fDirection);
        ASSERT(hEffect);

        CGameObjectManager::SendMessage(m_pPlayerChr,
                                        CHARACTERTYPES::MESSAGEID_RECVDMG,
                                        reinterpret_cast<void*>(10));

        int32 damageRequested = Character().GetRequestedDamage();
        if (damageRequested)
        {
            CGameObjectManager::SendMessage(m_pPlayerChr,
                                            CHARACTERTYPES::MESSAGEID_RECVDMG,
                                            reinterpret_cast<void*>(damageRequested));
            Character().RequestDamage(0);
        };

        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4121));

        IGamepad::StartVibration(Character().GetPadID(), IGamepad::VIBRATIONTYPE_HARD, 0.2f);
    };


    void CCrashWallCommon::OnDetach(void)
    {
        Character().SetEnableCatchHit(true);
    };


    void CCrashWallCommon::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() >= 1.0f)
            StateMachine().ChangeStatus(GetNextStatus());
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CCrashWallFront, { PLAYERTYPES::STATUS_FLYAWAY_FRONT });


    const char* CCrashWallFront::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::CRASHWALL_FRONT;
    };


    PLAYERTYPES::STATUS CCrashWallFront::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_CRASH_WALL_FALL_FRONT;
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CCrashWallBack, { PLAYERTYPES::STATUS_FLYAWAY_BACK });


    const char* CCrashWallBack::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::CRASHWALL_BACK;
    };


    PLAYERTYPES::STATUS CCrashWallBack::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_CRASH_WALL_FALL_BACK;
    };
    

    //
    // *********************************************************************************
    //
    

    void CCrashWallFallCommon::OnAttach(void)
    {
        RwV3d vVelocity = { 0.0f, -2.0f, 0.0f };
        Character().SetVelocity(&vVelocity);
        Character().SetEnableCatchHit(false);
    };


    void CCrashWallFallCommon::OnDetach(void)
    {
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
        Character().SetEnableCatchHit(true);
    };


    void CCrashWallFallCommon::OnRun(void)
    {
        ;
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CCrashWallFallFront, { PLAYERTYPES::STATUS_CRASH_WALL_FRONT });


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CCrashWallFallBack, { PLAYERTYPES::STATUS_CRASH_WALL_BACK });


    //
    // *********************************************************************************
    //

    
    void CCrashWallTouchdownCommon::OnAttach(void)
    {
        Character().ChangeMotion(GetMotionName());
        Character().ResetVelocity();
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        Character().SetEnableCatchHit(false);

        IGamepad::StartVibration(Character().GetPadID(), IGamepad::VIBRATIONTYPE_LOW, 0.2f);
    };


    void CCrashWallTouchdownCommon::OnDetach(void)
    {
        Character().SetEnableCatchHit(true);
    };


    void CCrashWallTouchdownCommon::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(GetNextStatus());
    };

    
    //
    // *********************************************************************************
    //

    
    DEFINE_ENABLED_STATUS_FOR(CCrashWallTouchdownFront, { PLAYERTYPES::STATUS_CRASH_WALL_FALL_FRONT });


    const char* CCrashWallTouchdownFront::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::CRASHWALL_TOUCHDOWN_FRONT;
    };


    PLAYERTYPES::STATUS CCrashWallTouchdownFront::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_DOWN_FRONT;
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CCrashWallTouchdownBack, { PLAYERTYPES::STATUS_CRASH_WALL_FALL_BACK });


    const char* CCrashWallTouchdownBack::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::CRASHWALL_TOUCHDOWN_BACK;
    };


    PLAYERTYPES::STATUS CCrashWallTouchdownBack::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_DOWN_BACK;
    };


    //
    // *********************************************************************************
    //


    bool CStatusDamage::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_DASH_FINISH,
            PLAYERTYPES::STATUS_DASH_FINISH,
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
            PLAYERTYPES::STATUS_JUMP_READY,
            PLAYERTYPES::STATUS_TOUCHDOWN,
            PLAYERTYPES::STATUS_ATTACK_A,
            PLAYERTYPES::STATUS_ATTACK_AA,
            PLAYERTYPES::STATUS_ATTACK_AAB,
            PLAYERTYPES::STATUS_ATTACK_AAC,
            PLAYERTYPES::STATUS_ATTACK_AABB,
            PLAYERTYPES::STATUS_ATTACK_AABC,
            PLAYERTYPES::STATUS_ATTACK_AABBB,
            PLAYERTYPES::STATUS_ATTACK_AABBC,
            PLAYERTYPES::STATUS_ATTACK_B_CHARGE,
            PLAYERTYPES::STATUS_ATTACK_B,
            PLAYERTYPES::STATUS_KNOCK_FRONT,
            PLAYERTYPES::STATUS_KNOCK_BACK,
            PLAYERTYPES::STATUS_ATTACK_KNIFE,
            PLAYERTYPES::STATUS_DINDLE,
            PLAYERTYPES::STATUS_STUN,
            PLAYERTYPES::STATUS_SLEEP,
            PLAYERTYPES::STATUS_BIND,
            PLAYERTYPES::STATUS_FREEZE,
            PLAYERTYPES::STATUS_GUARD,
            PLAYERTYPES::STATUS_GUARD_BACK,
            PLAYERTYPES::STATUS_GUARD_IMPACT,
            PLAYERTYPES::STATUS_GUARD_FINISH,
            PLAYERTYPES::STATUS_DASH,
            PLAYERTYPES::STATUS_DASH_CANCEL,
            PLAYERTYPES::STATUS_LIFT_CHALLENGE,
            PLAYERTYPES::STATUS_LIFT_SUCCESS,
            PLAYERTYPES::STATUS_LIFT,
            PLAYERTYPES::STATUS_LIFT_WALK,
            PLAYERTYPES::STATUS_THROW,
            PLAYERTYPES::STATUS_THROW_BACK,
            PLAYERTYPES::STATUS_PUSH,
            PLAYERTYPES::STATUS_CONSOLE,
        };

        if (GetID() == status)
            return false;

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };

    
    void CStatusDamage::OnAttach(void)
    {
        Character().ClearStatusParameter();
        Character().SetEnableBodyHitSelfToOther(false);

        m_fRecoverWait  = 0.1f;
        m_fEndTime      = Character().AttackParameter().m_fTroubleTime;

        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_REQUEST_RECOVER);

        IGamepad::StartVibration(Character().GetPadID(), IGamepad::VIBRATIONTYPE_NORMAL, 0.2f);
    };


    void CStatusDamage::OnDetach(void)
    {
        Character().SetEnableBodyHitSelfToOther(true);
    };


    void CStatusDamage::OnRun(void)
    {
        if (IsEnd())
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        }
        else
        {
            PLAYERTYPES::STATUS currentStatus = GetID();

            if ((currentStatus == PLAYERTYPES::STATUS_STUN) ||
                (currentStatus == PLAYERTYPES::STATUS_BIND) ||
                (currentStatus == PLAYERTYPES::STATUS_DINDLE))
            {
                if (StateMachine().GetStatusDuration() >= m_fRecoverWait)
                {
                    if (Character().TestPlayerFlag(PLAYERTYPES::FLAG_REQUEST_RECOVER))
                    {
                        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_REQUEST_RECOVER);

                        m_fEndTime     -= 0.1f;
                        m_fRecoverWait += 0.1f;
                    };
                };
            };
        };
    };


    bool CStatusDamage::IsEnd(void) const
    {
        return (StateMachine().GetStatusDuration() >= m_fEndTime);
    };


    //
    // *********************************************************************************
    //
    

    void CDindle::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::DINDLE);
        Character().ApplyStatusEffect();
    };


    //
    // *********************************************************************************
    //


    void CStun::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::STUN);
        Character().ApplyStatusEffect();
    };    

    
    //
    // *********************************************************************************
    //


    void CSleep::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::SLEEP);
        Character().ApplyStatusEffect();
    };


    //
    // *********************************************************************************
    //
    

    bool CBind::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        if (GetID() != status)
        {
            if (CStatusDamage::IsEnableChangeStatus(status))
            {
                return ((status != PLAYERTYPES::STATUS_KNOCK_FRONT) &&
                        (status != PLAYERTYPES::STATUS_KNOCK_BACK));
            };
        };

        return false;
    };


    void CBind::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::BIND);
        Character().ApplyStatusEffect();
    };


    //
    // *********************************************************************************
    //

    
    void CFreeze::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().StopMotion();
        Character().ApplyStatusEffect();
    };
};