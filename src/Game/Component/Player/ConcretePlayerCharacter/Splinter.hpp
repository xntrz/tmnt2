#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusJump.hpp"


namespace Splinter
{
    namespace MOTIONNAMES
    {
        static const char* ATTACK_JUMP = "JAttack";
    };
    
    class CAttackJump : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };

    class CAttackAABBC : public PlayerStatus::CAttackAABBC
    {
    protected:
        enum PHASE
        {
            PHASE_WAIT_TIMING = 0,
            PHASE_INVOKE_BARRIER,
            PHASE_FINISH,
        };

    public:
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
        virtual void OnDischargeWave(void) override;

    protected:
        PHASE m_phase;
    };

    class CAttackB : public PlayerStatus::CAttackB
    {
    protected:
        enum PHASE
        {
            PHASE_WAIT_TIMING = 0,
            PHASE_INVOKE_BARRIER,
            PHASE_FINISH,
        };

    public:
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
        virtual void OnDischargeWave(MAGIC_GENERIC::STEP step) override;

    protected:
        PHASE m_phase;
    };
}; /* namespace Splinter */


class CSplinter : public CPlayerCharacter
{
public:
    CSplinter(GAMETYPES::COSTUME costume);
    virtual ~CSplinter(void);
};