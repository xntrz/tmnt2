#include "PlayerStatusBasic.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace PlayerStatus
{
    bool CIdle::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return true;
    };


    void CIdle::OnAttach(void)
    {
        Character().SetDefaultFlags();
        Character().ResetVelocity();
        Character().ResetAcceleration();
        Character().ChangeMotion("Idle");
    };


    void CIdle::OnDetach(void)
    {
        ;
    };


    void CIdle::OnRun(void)
    {
        ;
    };

    
    //
    // *********************************************************************************
    //


    bool CWalk::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_RUN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CWalk::OnAttach(void)
    {
        Character().ChangeMotion("Walk");
    };


    void CWalk::OnDetach(void)
    {
        ;
    };


    void CWalk::OnRun(void)
    {
        RwV3d vVelocity =
        {
            0.0f,
            0.0f,
            Character().Feature().m_fWalkMoveSpeed
        };

        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        Character().SetVelocity(&vVelocity);
    };


    //
    // *********************************************************************************
    //


    bool CRun::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CRun::OnAttach(void)
    {
        Character().ChangeMotion("Run");
    };


    void CRun::OnDetach(void)
    {
        ;
    };


    void CRun::OnRun(void)
    {
        RwV3d vVelocity =
        {
            0.0f,
            0.0f,
            Character().Feature().m_fRunMoveSpeed
        };

        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        Character().SetVelocity(&vVelocity);
    };

    
    //
    // *********************************************************************************
    //

    
    bool CDashCancel::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
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
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CDashCancel::OnAttach(void)
    {
        ;
    };


    void CDashCancel::OnDetach(void)
    {
        ;
    };


    void CDashCancel::OnRun(void)
    {
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_DASH);
    };

    
    //
    // *********************************************************************************
    //
    

    bool CDash::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
            PLAYERTYPES::STATUS_DASH_CANCEL,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CDash::OnAttach(void)
    {
        Character().ChangeMotion("Dash1");

        RwV3d vVelocity =
        {
            0.0f,
            0.0f,
            Character().Feature().m_fDashMoveSpeed
        };
        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        Character().SetVelocity(&vVelocity);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, true);

        RwV3d vOffset = Math::VECTOR3_ZERO;
        CEffectManager::PlayTrace("all_dash1", new CPlayerTracer(m_pPlayerChr), &vOffset);
        CEffectManager::PlayTrace("all_dash2", new CPlayerTracer(m_pPlayerChr), &vOffset);
        
        CGameSound::PlayObjectSE(m_pPlayerChr, SDCODE_SE(4122));
    };


    void CDash::OnDetach(void)
    {
        ;
    };


    void CDash::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() >= Character().Feature().m_fDashTime)
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_DASH_FINISH);
    };

    
    //
    // *********************************************************************************
    //

    
    bool CDashFinish::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_DASH);
    };


    void CDashFinish::OnAttach(void)
    {
        Character().ChangeMotion("Dash2");
        Character().ResetVelocity();
    };


    void CDashFinish::OnDetach(void)
    {
        ;
    };


    void CDashFinish::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    void CDownCommon::OnAttach(void)
    {
        Character().ChangeMotion(GetMotionName());
        Character().ResetVelocity();
        Character().ResetAcceleration();
        Character().SetEnableCatchHit(false);
    };


    void CDownCommon::OnDetach(void)
    {
        Character().SetEnableCatchHit(true);
    };


    void CDownCommon::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(GetNextStatus());
    };


    //
    // *********************************************************************************
    //

    
    bool CDownFront::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT,
            PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_FRONT,            
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    const char* CDownFront::GetMotionName(void) const
    {
        return "YFrontFuttobi3";
    };


    PLAYERTYPES::STATUS CDownFront::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_TUMBLER_FRONT;
    };


    //
    // *********************************************************************************
    //

    
    bool CDownBack::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK,
            PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_BACK,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    const char* CDownBack::GetMotionName(void) const
    {
        return "YFrontFuttobi3";
    };


    PLAYERTYPES::STATUS CDownBack::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_TUMBLER_BACK;
    };


    //
    // *********************************************************************************
    //


    void CTumblerCommon::OnAttach(void)
    {
        Character().SetEnableCatchHit(false);
        Character().ChangeMotion(GetMotionName());

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vVelocity);

        if (Math::Vec3_Length(&vVelocity) > 0.0f)
        {
            float fMotionEndT = Character().GetMotionEndTime();
            
            RwV3d vAcceleration = Math::VECTOR3_ZERO;
            vAcceleration.x = vVelocity.x * -(1.0f / fMotionEndT);
            vAcceleration.y = vVelocity.y * -(1.0f / fMotionEndT);
            vAcceleration.z = vVelocity.z * -(1.0f / fMotionEndT);

            Character().SetAcceleration(&vAcceleration);
        };
    };


    void CTumblerCommon::OnDetach(void)
    {
        Character().SetEnableCatchHit(true);
    };


    void CTumblerCommon::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    bool CTumblerFront::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_DOWN_FRONT,
            PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    const char* CTumblerFront::GetMotionName(void) const
    {
        return "YFrontFuttobi4";
    };


    //
    // *********************************************************************************
    //


    bool CTumblerBack::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_DOWN_BACK,
            PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    const char* CTumblerBack::GetMotionName(void) const
    {
        return "YFrontFuttobi4";
    };
};