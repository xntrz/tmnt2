#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusJump.hpp"


namespace Raphael
{
    class CAttackJump : public CStatus
    {
    protected:
        enum PHASE
        {
            PHASE_START = 0,
            PHASE_REPEAT,
        };
        
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;

    protected:
        PHASE m_phase;
    };


    class CTouchdown : public PlayerStatus::CTouchdown
    {
    public:
        virtual void OnAttach(void) override;
    };


    class CAttackAAC : public PlayerStatus::CAttackAAC
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
    };
    

    class CAttackAABBC : public PlayerStatus::CAttackAABBC
    {
    public:
        virtual void OnDischargeWave(void) override;
    };


    class CAttackB : public PlayerStatus::CAttackB
    {
    public:
        virtual void OnDischargeWave(MAGIC_GENERIC::STEP step) override;
    };


    class CPush : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
};


class CRaphael : public CPlayerCharacter
{
public:
    CRaphael(GAMETYPES::COSTUME costume);
    virtual ~CRaphael(void);
    virtual void Run(void) override;
};