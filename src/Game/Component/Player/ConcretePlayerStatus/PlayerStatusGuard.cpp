#include "PlayerStatusGuard.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace PlayerStatus
{
    DEFINE_ENABLED_STATUS_FOR(CGuardReady, { PLAYERTYPES::STATUS_IDLE,
                                             PLAYERTYPES::STATUS_WALK,
                                             PLAYERTYPES::STATUS_RUN });


    void CGuardReady::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::GUARD_READY);
        Character().ResetAcceleration();
        Character().ResetVelocity();
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
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


    DEFINE_ENABLED_STATUS_FOR(CGuard, { PLAYERTYPES::STATUS_GUARD_READY,
                                        PLAYERTYPES::STATUS_GUARD_BACK });


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


    DEFINE_ENABLED_STATUS_FOR(CGuardBack, { PLAYERTYPES::STATUS_GUARD_READY,
                                            PLAYERTYPES::STATUS_GUARD,
                                            PLAYERTYPES::STATUS_GUARD_BACK });


    void CGuardBack::OnAttach(void)
    {
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::GUARD);

        RwV3d vAttackDir = Character().AttackParameter().m_vDirection;
        Character().SetDirectionFromVector(&vAttackDir);

        RwV3d vAttackVel = Character().AttackParameter().m_vVelocity;
        Character().SetVelocity(&vAttackVel);

        RwV3d vAcceleration = Math::VECTOR3_ZERO;
        vAcceleration.x = Character().AttackParameter().m_vVelocity.x * -2.5f;
        vAcceleration.y = Character().AttackParameter().m_vVelocity.y * -2.5f;
        vAcceleration.z = Character().AttackParameter().m_vVelocity.z * -2.5f;

        Character().SetAcceleration(&vAcceleration);

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION);
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


    DEFINE_ENABLED_STATUS_FOR(CGuardImpact, { PLAYERTYPES::STATUS_GUARD_READY });


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


    DEFINE_ENABLED_STATUS_FOR(CGuardFinish, { PLAYERTYPES::STATUS_GUARD });    


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