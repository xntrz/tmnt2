#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusJump.hpp"


namespace Donatello
{
    const RwV3d BANDANA_OFFSET = { 0.0f, 0.15f, 0.05f };

    namespace MOTIONNAMES
    {
        static const char* CONSOLE1        = "Panel1";
        static const char* CONSOLE2        = "Panel2";
        static const char* CONSOLE3        = "Panel3";
        static const char* ATTACK_LASER    = "Shuriken";
        static const char* ATTACK_JUMP1    = "JAttack1";
        static const char* ATTACK_JUMP2    = "JAttack2";
    };

    namespace EFFECTNAMES
    {
        static const char* LASER_BEAM = "don_laser_beam";
    };

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
        virtual void OnAttach(void) override;
        virtual void OnRun(void) override;
        void ShootingLaser(void);
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
    virtual void OnChangeMotion(void) override;
};