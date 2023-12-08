#include "PlayerStatusGuard.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace PlayerStatus
{
    bool CGuardReady::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CGuardReady::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::GUARD_READY);
        Character().ResetAcceleration();
        Character().ResetVelocity();
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, true);
    };


    void CGuardReady::OnDetach(void)
    {
        ;
    };


    void CGuardReady::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_GUARD);
    };

    
    //
    // *********************************************************************************
    //

    
    bool CGuard::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_GUARD_READY,
            PLAYERTYPES::STATUS_GUARD_BACK,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };



    void CGuard::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::GUARD);
    };


    void CGuard::OnDetach(void)
    {
        ;
    };


    void CGuard::OnRun(void)
    {
        ;
    };


    //
    // *********************************************************************************
    //

    
    bool CGuardBack::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_GUARD_READY,
            PLAYERTYPES::STATUS_GUARD,
            PLAYERTYPES::STATUS_GUARD_BACK,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CGuardBack::OnAttach(void)
    {
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, false);
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::GUARD);
        Character().SetDirectionFromVector(
            &Character().AttackParameter().m_vDirection
        );
        Character().SetVelocity(
            &Character().AttackParameter().m_vVelocity
        );

        RwV3d vAcceleration = Math::VECTOR3_ZERO;
        vAcceleration.x = Character().AttackParameter().m_vVelocity.x * -2.5f;
        vAcceleration.y = Character().AttackParameter().m_vVelocity.y * -2.5f;
        vAcceleration.z = Character().AttackParameter().m_vVelocity.z * -2.5f;

        Character().SetAcceleration(&vAcceleration);

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION, true);
    };


    void CGuardBack::OnDetach(void)
    {
        ;
    };


    void CGuardBack::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() >= 0.4f)
        {
            Character().ResetVelocity();
            Character().ResetAcceleration();
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_GUARD);
        };
    };


    //
    // *********************************************************************************
    //


    bool CGuardImpact::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_GUARD_READY);
    };


    void CGuardImpact::OnAttach(void)
    {

    };


    void CGuardImpact::OnDetach(void)
    {

    };


    void CGuardImpact::OnRun(void)
    {
        StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };


    //
    // *********************************************************************************
    //


    bool CGuardFinish::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_GUARD);
    };


    void CGuardFinish::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::GUARD_FINISH);
    };


    void CGuardFinish::OnDetach(void)
    {
        ;
    };


    void CGuardFinish::OnRun(void)
    {
        if (Character().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };
};