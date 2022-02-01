#include "PlayerStatusDamage.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Player/PlayerUtil.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Map/WorldMap.hpp"


namespace PlayerStatus
{
    bool CKnockCommon::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_ATTACK_B,
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
            PLAYERTYPES::STATUS_CONSOLE,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CKnockCommon::OnAttach(void)
    {
        Character().ClearSatusParameter();
        
        Character().SetDirectionFromVector(
            &Character().AttackParameter().m_vDirection
        );
        
        Character().SetVelocity(
            &Character().AttackParameter().m_vVelocity
        );

        RwV3d vAccleration = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(
            &vAccleration,
            &Character().AttackParameter().m_vVelocity,
            -(1.0f / Character().GetMotionEndTime())
        );

        CGamepad::StartVibration(Character().GetPadID(), CGamepad::VIBRATIONTYPE_LOW, 0.2f);        
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
            Character().ChangeMotion("YFront");
        
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
            Character().ChangeMotion("YBack");

        CKnockCommon::OnAttach();
    };

    
    //
    // *********************************************************************************
    //


    bool CFlyawayCommon::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
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
            PLAYERTYPES::STATUS_DEAD_FLYAWAY,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CFlyawayCommon::OnAttach(void)
    {
        if (Character().GetStatusPrev() == PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT ||
            Character().GetStatusPrev() == PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK)
        {
            Character().ResetAcceleration();
            Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
        }
        else
        {
            Character().ClearSatusParameter();

            Character().SetDirectionFromVector(
                &Character().AttackParameter().m_vDirection
            );

            Character().SetVelocity(
                &Character().AttackParameter().m_vVelocity
            );
            
            Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);

            CGamepad::StartVibration(Character().GetPadID(), CGamepad::VIBRATIONTYPE_HARD, 0.2f);
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
        Character().ChangeMotion("YFrontFuttobi1");
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
        Character().ChangeMotion("YBackFuttobi1");
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

        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_PASSIVE, false);
        Character().SetEnableCatchHit(false);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetFootPosition(&vPosition);
        vPosition.y = CWorldMap::GetMapHeight(&vPosition);

        const CPlayerCharacter::COLLISIONGROUNDINFO* pGroundInfo = Character().GetCollisionGround();
        ASSERT(pGroundInfo);

        EFFECT_GENERIC::CallTouchDownEffect(pGroundInfo->m_attribute, &vPosition);
        
        if (!IS_FLAG_SET(pGroundInfo->m_attribute, MAPTYPES::ATTRIBUTE_DEATH))
            CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4121));

        CGamepad::StartVibration(Character().GetPadID(), CGamepad::VIBRATIONTYPE_HARD, 0.2f);
    };


    void CFlyawayBoundCommon::OnDetach(void)
    {
        Character().SetEnableCatchHit(true);
    };


    void CFlyawayBoundCommon::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() < 0.2f &&
            Character().IsPlayerFlagSet(PLAYERTYPES::FLAG_REQUEST_PASSIVE))
        {
            RwV3d vOffset = Math::VECTOR3_ZERO;
            CEffectManager::PlayTrace(
                "all_dash2",
                new CPlayerTracer(m_pPlayerChr),
                &vOffset
            );

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

    
    bool CFlyawayBoundFront::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_FLYAWAY_FRONT);
    };


    const char* CFlyawayBoundFront::GetMotionName(void) const
    {
        return "YFrontFuttobi2";
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

    
    bool CFlyawayBoundBack::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_THROWN ||
                status == PLAYERTYPES::STATUS_FLYAWAY_BACK);
    };


    void CFlyawayBoundBack::OnDetach(void)
    {
        Character().SetDirection(Character().GetDirection() + Math::PI);
        CFlyawayBoundCommon::OnDetach();
    };


    const char* CFlyawayBoundBack::GetMotionName(void) const
    {
        return "YBackFuttobi2";
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
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, true);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
        Character().SetEnableCatchHit(false);

        const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = Character().GetCollisionWall();
        ASSERT(pWallInfo);
        
		RwV3d vPosition = pWallInfo->m_vPosition;
		vPosition.y -= 0.5f;

        CEffectManager::Play(
            "all_pitan",
            &vPosition,
            Character().GetDirection()
        );

        CGameObjectManager::SendMessage(
            m_pPlayerChr,
            CHARACTERTYPES::MESSAGEID_RECVDMG,
            (void*)10
        );

        if (Character().IsDamageRequested())
        {
            CGameObjectManager::SendMessage(
                m_pPlayerChr,
                CHARACTERTYPES::MESSAGEID_RECVDMG,
                (void*)20
            );
            
            Character().RequestDamage(0);
        };

        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4121));

        CGamepad::StartVibration(Character().GetPadID(), CGamepad::VIBRATIONTYPE_HARD, 0.2f);
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

    
    bool CCrashWallFront::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_FLYAWAY_FRONT);
    };


    const char* CCrashWallFront::GetMotionName(void) const
    {
        return "YWall1";
    };


    PLAYERTYPES::STATUS CCrashWallFront::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_CRASH_WALL_FALL_FRONT;
    };


    //
    // *********************************************************************************
    //

    
    bool CCrashWallBack::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_FLYAWAY_BACK);
    };


    const char* CCrashWallBack::GetMotionName(void) const
    {
        return "YWall1";
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
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, false);
        Character().SetEnableCatchHit(true);
    };


    void CCrashWallFallCommon::OnRun(void)
    {
        ;
    };


    //
    // *********************************************************************************
    //

    
    bool CCrashWallFallFront::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_CRASH_WALL_FRONT);
    };


    //
    // *********************************************************************************
    //

    
    bool CCrashWallFallBack::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_CRASH_WALL_BACK);
    };


    //
    // *********************************************************************************
    //

    
    void CCrashWallTouchdownCommon::OnAttach(void)
    {
        Character().ChangeMotion(GetMotionName());
        Character().ResetVelocity();
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, false);
        Character().SetEnableCatchHit(false);

        CGamepad::StartVibration(Character().GetPadID(), CGamepad::VIBRATIONTYPE_LOW, 0.2f);
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

    
    bool CCrashWallTouchdownFront::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_CRASH_WALL_FALL_FRONT);
    };


    const char* CCrashWallTouchdownFront::GetMotionName(void) const
    {
        return "YWall2";
    };


    PLAYERTYPES::STATUS CCrashWallTouchdownFront::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_DOWN_FRONT;
    };


    //
    // *********************************************************************************
    //
    

    bool CCrashWallTouchdownBack::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_CRASH_WALL_FALL_BACK);
    };


    const char* CCrashWallTouchdownBack::GetMotionName(void) const
    {
        return "YWall2";
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

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };

    
    void CStatusDamage::OnAttach(void)
    {
        Character().ClearSatusParameter();
        Character().SetEnableBodyHitSelfToOther(false);

        m_fTime = 0.1f;
        m_fEndTime = Character().AttackParameter().m_fTroubleTime;

        CGamepad::StartVibration(Character().GetPadID(), CGamepad::VIBRATIONTYPE_NORMAL, 0.2f);
    };


    void CStatusDamage::OnDetach(void)
    {
        Character().SetEnableBodyHitSelfToOther(true);
    };


    void CStatusDamage::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() < m_fEndTime)
        {
            if (GetID() == PLAYERTYPES::STATUS_STUN ||
                GetID() == PLAYERTYPES::STATUS_BIND)
            {
                if (StateMachine().GetStatusDuration() >= m_fTime)
                {
                    if (Character().IsPlayerFlagSet(PLAYERTYPES::FLAG_REQUEST_RECOVER))
                    {
                        m_fEndTime -= 0.1f;
                        m_fTime += 1.0f;
                        Character().SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_RECOVER, false);
                    };
                };
            };
        }
        else
        {
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };
    };


    //
    // *********************************************************************************
    //
    

    void CDindle::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().ChangeMotion("BiriBiri");
        Character().ApplyStatusEffect();
    };


    //
    // *********************************************************************************
    //


    void CStun::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().ChangeMotion("Piyori");
        Character().ApplyStatusEffect();
    };    

    
    //
    // *********************************************************************************
    //


    void CSleep::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().ChangeMotion("Piyori");
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
                return (status != PLAYERTYPES::STATUS_KNOCK_FRONT &&
                        status != PLAYERTYPES::STATUS_KNOCK_BACK);
            };
        };

        return false;
    };


    void CBind::OnAttach(void)
    {
        CStatusDamage::OnAttach();
        Character().ChangeMotion("BiriBiri");
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