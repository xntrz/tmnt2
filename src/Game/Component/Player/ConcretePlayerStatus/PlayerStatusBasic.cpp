#include "PlayerStatusBasic.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace PlayerStatus
{
    DEFINE_EMPTY_ENABLED_STATUS_FOR(CIdle, true);


    void CIdle::OnAttach(void)
    {
        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
        
        CHARACTERTYPES::FLAG cflag  = CHARACTERTYPES::FLAG_FIXED_DIRECTION
                                    | CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION
                                    | CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ
                                    | CHARACTERTYPES::FLAG_CANCEL_GRAVITY;
        Character().ClearCharacterFlag(cflag);

        Character().ResetVelocity();
        Character().ResetAcceleration();

        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::IDLE);
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


    DEFINE_ENABLED_STATUS_FOR(CWalk, { PLAYERTYPES::STATUS_IDLE,
                                       PLAYERTYPES::STATUS_RUN, });


    void CWalk::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::WALK);
    };


    void CWalk::OnDetach(void)
    {
        ;
    };


    void CWalk::OnRun(void)
    {
        float fMoveSpeed = Character().Feature().m_fWalkMoveSpeed;
        RwV3d vVelocity = { 0.0f, 0.0f, fMoveSpeed };
        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        Character().SetVelocity(&vVelocity);
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CRun, { PLAYERTYPES::STATUS_IDLE,
                                      PLAYERTYPES::STATUS_WALK });


    void CRun::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::RUN);
    };


    void CRun::OnDetach(void)
    {
        ;
    };


    void CRun::OnRun(void)
    {
        float fMoveSpeed = Character().Feature().m_fRunMoveSpeed;
        RwV3d vVelocity = { 0.0f, 0.0f, fMoveSpeed };
        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        Character().SetVelocity(&vVelocity);
    };

    
    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CDashCancel, { PLAYERTYPES::STATUS_ATTACK_A,
                                             PLAYERTYPES::STATUS_ATTACK_AA,
                                             PLAYERTYPES::STATUS_ATTACK_AAB,
                                             PLAYERTYPES::STATUS_ATTACK_AAC,
                                             PLAYERTYPES::STATUS_ATTACK_AABB,
                                             PLAYERTYPES::STATUS_ATTACK_AABC,
                                             PLAYERTYPES::STATUS_ATTACK_AABBB,
                                             PLAYERTYPES::STATUS_ATTACK_AABBC,
                                             PLAYERTYPES::STATUS_ATTACK_B,
                                             PLAYERTYPES::STATUS_ATTACK_B_CHARGE});


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
    

    DEFINE_ENABLED_STATUS_FOR(CDash, { PLAYERTYPES::STATUS_IDLE,
                                       PLAYERTYPES::STATUS_WALK,
                                       PLAYERTYPES::STATUS_RUN,
                                       PLAYERTYPES::STATUS_DASH_CANCEL });


    void CDash::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::DASH);

        float fMoveSpeed = Character().Feature().m_fDashMoveSpeed;
        RwV3d vVelocity = { 0.0f, 0.0f, fMoveSpeed };
        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        Character().SetVelocity(&vVelocity);

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);

        RwV3d vOffset = Math::VECTOR3_ZERO;
        CEffectManager::PlayTrace(PLAYERTYPES::EFFECTNAMES::DASH, new CPlayerTracer(m_pPlayerChr), &vOffset);
        CEffectManager::PlayTrace(PLAYERTYPES::EFFECTNAMES::DASH_SMOKE, new CPlayerTracer(m_pPlayerChr), &vOffset);
        
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


    DEFINE_ENABLED_STATUS_FOR(CDashFinish, { PLAYERTYPES::STATUS_DASH });


    void CDashFinish::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::DASH_FINISH);
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

    DEFINE_ENABLED_STATUS_FOR(CDownFront, { PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT,
                                            PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_FRONT });


    const char* CDownFront::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::DOWN_FRONT;
    };


    PLAYERTYPES::STATUS CDownFront::GetNextStatus(void) const
    {
        return PLAYERTYPES::STATUS_TUMBLER_FRONT;
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CDownBack, { PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK,
                                           PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_BACK });


    const char* CDownBack::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::DOWN_BACK;
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

    
    DEFINE_ENABLED_STATUS_FOR(CTumblerFront, { PLAYERTYPES::STATUS_DOWN_FRONT,
                                               PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT });


    const char* CTumblerFront::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::TUMBLER_FRONT;
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CTumblerBack, { PLAYERTYPES::STATUS_DOWN_BACK,
                                              PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK });


    const char* CTumblerBack::GetMotionName(void) const
    {
        return PLAYERTYPES::MOTIONNAMES::TUMBLER_BACK;
    };
};