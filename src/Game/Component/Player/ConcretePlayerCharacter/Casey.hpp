#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusJump.hpp"


namespace Casey
{
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


class CCasey : public CPlayerCharacter
{
public:
    CCasey(GAMETYPES::COSTUME costume);
    virtual ~CCasey(void);
    virtual void Run(void) override;
    virtual void ShootingKnife(void) override;
};