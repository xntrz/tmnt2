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
    DEFINE_ENABLED_STATUS_FOR(CJumpReady, { PLAYERTYPES::STATUS_IDLE,
                                            PLAYERTYPES::STATUS_WALK,
                                            PLAYERTYPES::STATUS_RUN });


    void CJumpReady::OnAttach(void)
    {
		PLAYERTYPES::STATUS status = Character().GetStatusPrev();
		Character().SetLastGroundingStatus(status);
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::JUMP_READY);
        Character().ResetVelocity();
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
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
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
    };


    void CJumpCommon::OnRun(void)
    {
        Character().SetAerialMotionTime(Character().Feature().m_fJumpInitializeSpeed);
    };

    
    //
    // *********************************************************************************
    //

    
    DEFINE_ENABLED_STATUS_FOR(CJump, { PLAYERTYPES::STATUS_JUMP_READY });

    
    void CJump::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::JUMP);
        
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
        
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND);
        
        CJumpCommon::OnAttach();

        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4124));
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
        };

        return bResult;
    };


    void CJump2nd::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::JUMP_2ND);

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
        
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND);

        CJumpCommon::OnAttach();

        CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_JUMP_DOUBLE);

        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4124));
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
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);

        const CCharacter::COLLISIONWALLINFO* pWallInfo = Character().GetCollisionWall();
        ASSERT(pWallInfo);
        ASSERT(pWallInfo->m_bHit);
        ASSERT(pWallInfo->m_bJumpWallHit);

        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::JUMP_WALL);

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

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND);

        CJumpCommon::OnAttach();

        RwV3d vEffectPos = Math::VECTOR3_ZERO;
        Character().GetFootPosition(&vEffectPos);
        vEffectPos.x = pWallInfo->m_vJumpWallPosition.x;
        vEffectPos.z = pWallInfo->m_vJumpWallPosition.z;
        CEffectManager::Play(PLAYERTYPES::EFFECTNAMES::JUMP_WALL, &vEffectPos);
        
        CGameEvent::SetPlayerTechnicalAction(Character().GetPlayerNo(), GAMETYPES::TECACT_JUMP_WALL);
        
        PlayerUtil::CallVoiceOfJumpWall(m_pPlayerChr);
        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4124));
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

    
    DEFINE_EMPTY_ENABLED_STATUS_FOR(CJumpWallFailure, false);

    
    void CJumpWallFailure::OnAttach(void)
    {
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND);
        Character().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        
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

    DEFINE_ENABLED_STATUS_FOR(CAerialCommon, { PLAYERTYPES::STATUS_IDLE,
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
                                               PLAYERTYPES::STATUS_GUARD_FINISH });


    void CAerialCommon::OnAttach(void)
    {
        if (!Character().TestPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS))
            Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::AERIAL);

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
        
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

        float fAerialAccel = Character().Feature().m_fAerialAcceleration;
        RwV3d vAcceleration = { 0.0f, 0.0f, fAerialAccel };
        Character().RotateVectorByDirection(&vAcceleration, &vAcceleration);
        Character().SetAcceleration(&vAcceleration);
    };

    
    //
    // *********************************************************************************
    //



    DEFINE_ENABLED_STATUS_FOR(CTouchdown, { PLAYERTYPES::STATUS_JUMP,
                                            PLAYERTYPES::STATUS_JUMP_2ND,
                                            PLAYERTYPES::STATUS_JUMP_WALL,
                                            PLAYERTYPES::STATUS_AERIAL,
                                            PLAYERTYPES::STATUS_AERIAL_MOVE,
                                            PLAYERTYPES::STATUS_ATTACK_JUMP,
                                            PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP });


    void CTouchdown::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::TOUCHDOWN);
        
        Character().ResetVelocity();
        Character().ResetAcceleration();

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND);
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);

        RwV3d vFootPos = Math::VECTOR3_ZERO;
        Character().GetFootPosition(&vFootPos);

        MAPTYPES::ATTRIBUTE attribute = Character().GetCollisionGround()->m_attribute;
        EFFECT_GENERIC::CallTouchDownEffect(attribute, &vFootPos);

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