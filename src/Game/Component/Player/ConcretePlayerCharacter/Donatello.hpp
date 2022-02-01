#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusJump.hpp"


namespace Donatello
{
    class CAttackJump : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };

    class CAttackLaser : public PlayerStatus::CAttackKnife
    {
    public:
        virtual void OnRun(void) override;
    };

    class CAttackLaserJump : public PlayerStatus::CAttackKnifeJump
    {
    public:        
        virtual void OnRun(void) override;
    };

    class CTouchdown : public PlayerStatus::CTouchdown
    {
    public:
        virtual void OnAttach(void) override;
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

    class CConsole : public CStatus
    {
    public:        
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;

    protected:
        float m_fStartTimer;
        int32 m_iMotionStep;
    };
};


class CDonatello : public CPlayerCharacter
{
public:
    CDonatello(GAMETYPES::COSTUME costume);
    virtual ~CDonatello(void);
    virtual void Run(void) override;
    virtual void OnChangeMotion(void) override;
    virtual void ShootingKnife(void) override;
};