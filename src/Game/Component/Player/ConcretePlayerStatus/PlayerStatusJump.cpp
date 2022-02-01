#include "PlayerStatusJump.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Player/PlayerUtil.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace PlayerStatus
{
    bool CJumpReady::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CJumpReady::OnAttach(void)
    {
		PLAYERTYPES::STATUS status = Character().GetStatusPrev();
		Character().SetLastGroundingStatus(status);
        Character().ChangeMotion("J1");
        Character().ResetVelocity();
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION, true);
    };


    void CJumpReady::OnDetach(void)
    {
        ;
    };


    void CJumpReady::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_JUMP);
    };

    
    //
    // *********************************************************************************
    //

    
    void CJumpCommon::OnAttach(void)
    {
        Character().SetMaxVelocity(Character().Feature().m_fAerialMoveSpeed);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ, true);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
    };


    void CJumpCommon::OnRun(void)
    {
        Character().SetAerialMotionTime(Character().Feature().m_fJumpInitializeSpeed);
    };

    
    //
    // *********************************************************************************
    //

    
    bool CJump::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_JUMP_READY);
    };


    void CJump::OnAttach(void)
    {
        Character().ChangeMotion("J2");
        
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        switch (Character().GetLastGroundingStatus())
        {
        case PLAYERTYPES::STATUS_WALK:
            vVelocity.z = Character().Feature().m_fWalkMoveSpeed;
            break;

        case PLAYERTYPES::STATUS_RUN:
            vVelocity.z = Character().Feature().m_fRunMoveSpeed;
            break;

        default:
            vVelocity.z = 0.0f;
            break;
        };

        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        
        vVelocity.y += Character().Feature().m_fJumpInitializeSpeed;
        Character().SetVelocity(&vVelocity);
        
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION, true);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND, true);
        
        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4124));
        CJumpCommon::OnAttach();
    };


    void CJump::OnDetach(void)
    {
        ;
    };


    void CJump::OnRun(void)
    {
        CJumpCommon::OnRun();
    };

    
    //
    // *********************************************************************************
    //

    
    bool CJump2nd::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        bool bResult = Character().IsEnableJump2nd();
        if (bResult)
        {
            PLAYERTYPES::STATUS aStatusArray[] =
            {
                PLAYERTYPES::STATUS_JUMP,
                PLAYERTYPES::STATUS_JUMP_WALL,
                PLAYERTYPES::STATUS_AERIAL,
                PLAYERTYPES::STATUS_AERIAL_MOVE,
            };

            bResult = IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
        }

        return bResult;
    };


    void CJump2nd::OnAttach(void)
    {
        Character().ChangeMotion("JDouble");

        RwV3d vVelocity = Math::VECTOR3_ZERO;        
        if (Character().GetStatusPrev() == PLAYERTYPES::STATUS_AERIAL_MOVE)
        {
            vVelocity.z = Character().Feature().m_fRunMoveSpeed;
            Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        }
        else
        {
            Character().GetVelocity(&vVelocity);
        };

        vVelocity.y = Character().Feature().m_fJumpInitializeSpeed;
        Character().SetVelocity(&vVelocity);
        
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, false);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND, false);

        CGameEvent::SetPlayerTechnicalAction(
            Character().GetPlayerNo(),
            GAMETYPES::TECACT_JUMP_DOUBLE
        );

        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4124));

        CJumpCommon::OnAttach();
    };


    void CJump2nd::OnDetach(void)
    {
        ;
    };


    void CJump2nd::OnRun(void)
    {
        CJumpCommon::OnRun();
    };

    
    //
    // *********************************************************************************
    //

    
    bool CJumpWall::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        bool bResult = Character().IsEnableJumpWall();
        if (bResult)
        {
            PLAYERTYPES::STATUS aStatusArray[] =
            {
                PLAYERTYPES::STATUS_JUMP,
                PLAYERTYPES::STATUS_JUMP_2ND,
                PLAYERTYPES::STATUS_JUMP_WALL,
                PLAYERTYPES::STATUS_AERIAL,
                PLAYERTYPES::STATUS_AERIAL_MOVE,
            };

            bResult = IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
        }

        return bResult;
    };


    void CJumpWall::OnAttach(void)
    {
        Character().ChangeMotion("JWall");
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, false);

        const CCharacter::COLLISIONWALLINFO* pWallInfo = Character().GetCollisionWall();
        ASSERT(pWallInfo);
        ASSERT(pWallInfo->m_bHit);
        ASSERT(pWallInfo->m_bJumpWallHit);

        RwV3d vWallNormal = pWallInfo->m_vJumpWallNormal;
        vWallNormal.y = 0.0f;
        
        Math::Vec3_Normalize(&vWallNormal, &vWallNormal);
        
        float fOpposite = Math::ATan2(vWallNormal.x, vWallNormal.z);
        Character().SetDirection(fOpposite);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vVelocity, &vWallNormal, Character().Feature().m_fRunMoveSpeed);
        vVelocity.y = Character().Feature().m_fJumpInitializeSpeed;
        Character().SetVelocity(&vVelocity);
        
        Character().ResetAcceleration();
        
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, true);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND, true);

        RwV3d vFootPos = Math::VECTOR3_ZERO;
        Character().GetFootPosition(&vFootPos);
        CEffectManager::Play("all_jump", &vFootPos);
        
        CGameEvent::SetPlayerTechnicalAction(
            Character().GetPlayerNo(),
            GAMETYPES::TECACT_JUMP_WALL
        );

        Character().NotifyWallJumpSuccess();
        
        PlayerUtil::CallVoiceOfJumpWall(m_pPlayerChr);
        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4124));

        CJumpCommon::OnAttach();
    };


    void CJumpWall::OnDetach(void)
    {
        ;
    };


    void CJumpWall::OnRun(void)
    {
        CJumpCommon::OnRun();
    };

    
    //
    // *********************************************************************************
    //

    
    bool CJumpWallFailure::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return false;        
    };

    
    void CJumpWallFailure::OnAttach(void)
    {
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
        
        Character().ResetAcceleration();
        
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vVelocity);

        if (vVelocity.y > 0.0f)
            vVelocity.y = 0.0f;

        vVelocity.x = 0.0f;
        vVelocity.y -= 4.0f;
        vVelocity.z = 0.0f;

        Character().SetVelocity(&vVelocity);
    };
    

    void CJumpWallFailure::OnDetach(void)
    {
        ;
    };
    

    void CJumpWallFailure::OnRun(void)
    {
        ;
    };

    
    //
    // *********************************************************************************
    //

    
    bool CAerialCommon::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
            PLAYERTYPES::STATUS_JUMP,
            PLAYERTYPES::STATUS_JUMP_2ND,
            PLAYERTYPES::STATUS_AERIAL,
            PLAYERTYPES::STATUS_AERIAL_MOVE,
            PLAYERTYPES::STATUS_ATTACK_A,
            PLAYERTYPES::STATUS_ATTACK_AA,
            PLAYERTYPES::STATUS_ATTACK_AAB,
            PLAYERTYPES::STATUS_ATTACK_AAC,
            PLAYERTYPES::STATUS_ATTACK_AABB,
            PLAYERTYPES::STATUS_ATTACK_AABC,
            PLAYERTYPES::STATUS_ATTACK_AABBB,
            PLAYERTYPES::STATUS_ATTACK_AABBC,
            PLAYERTYPES::STATUS_ATTACK_B,
            PLAYERTYPES::STATUS_ATTACK_B_CHARGE,
            PLAYERTYPES::STATUS_ATTACK_RUN,
            PLAYERTYPES::STATUS_LIFT_CHALLENGE,
            PLAYERTYPES::STATUS_LIFT_SUCCESS,
            PLAYERTYPES::STATUS_LIFT,
            PLAYERTYPES::STATUS_LIFT_WALK,
            PLAYERTYPES::STATUS_LIFTED,
            PLAYERTYPES::STATUS_LIFTED_WALK,
            PLAYERTYPES::STATUS_THROWN_MISS,
            PLAYERTYPES::STATUS_THROWN_COMBINATION_END,
            PLAYERTYPES::STATUS_DASH_CANCEL,
            PLAYERTYPES::STATUS_DASH,
            PLAYERTYPES::STATUS_DASH_FINISH,
            PLAYERTYPES::STATUS_DINDLE,
            PLAYERTYPES::STATUS_STUN,
            PLAYERTYPES::STATUS_SLEEP,
            PLAYERTYPES::STATUS_BIND,
            PLAYERTYPES::STATUS_FREEZE,
            PLAYERTYPES::STATUS_DOWN_FRONT,
            PLAYERTYPES::STATUS_DOWN_BACK,
            PLAYERTYPES::STATUS_TUMBLER_FRONT,
            PLAYERTYPES::STATUS_TUMBLER_BACK,
            PLAYERTYPES::STATUS_PUSH,
            PLAYERTYPES::STATUS_CONSOLE,
            PLAYERTYPES::STATUS_GUARD_READY,
            PLAYERTYPES::STATUS_GUARD,
            PLAYERTYPES::STATUS_GUARD_BACK,
            PLAYERTYPES::STATUS_GUARD_IMPACT,
            PLAYERTYPES::STATUS_GUARD_FINISH,
        };
        
        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CAerialCommon::OnAttach(void)
    {
        if (!Character().IsPlayerFlagSet(PLAYERTYPES::FLAG_AERIAL_STATUS))
            Character().ChangeMotion("J2");

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION, true);
        
        CJumpCommon::OnAttach();
    };

    
    //
    // *********************************************************************************
    //


    void CAerial::OnDetach(void)
    {
        ;
    };


    void CAerial::OnRun(void)
    {
        CJumpCommon::OnRun();
    };


    //
    // *********************************************************************************
    //

    
    void CAerialMove::OnDetach(void)
    {
        ;
    };


    void CAerialMove::OnRun(void)
    {
        CJumpCommon::OnRun();

        RwV3d vAcceleration =
        {
            0.0f,
            0.0f,
            Character().Feature().m_fAerialAcceleration
        };

        Character().RotateVectorByDirection(&vAcceleration, &vAcceleration);
        Character().SetAcceleration(&vAcceleration);
    };

    
    //
    // *********************************************************************************
    //

    
    bool CTouchdown::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_JUMP,
            PLAYERTYPES::STATUS_JUMP_2ND,
            PLAYERTYPES::STATUS_JUMP_WALL,
            PLAYERTYPES::STATUS_AERIAL,
            PLAYERTYPES::STATUS_AERIAL_MOVE,
            PLAYERTYPES::STATUS_ATTACK_JUMP,
            PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CTouchdown::OnAttach(void)
    {
        Character().ChangeMotion("J3");
        
        Character().ResetVelocity();
        Character().ResetAcceleration();

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION, true);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND, false);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, false);

        RwV3d vFootPos = Math::VECTOR3_ZERO;
        Character().GetFootPosition(&vFootPos);

        EFFECT_GENERIC::CallTouchDownEffect(
            Character().GetCollisionGround()->m_attribute,
            &vFootPos
        );

        PlayerUtil::CallSEOfTouchdown(m_pPlayerChr);
    };


    void CTouchdown::OnDetach(void)
    {
        Character().SetDirectionFromModelRotate();
    };


    void CTouchdown::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };
};