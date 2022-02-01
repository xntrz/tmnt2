#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"


namespace Leonardo
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
};


class CLeonardo : public CPlayerCharacter
{
public:
    CLeonardo(GAMETYPES::COSTUME costume);
    virtual ~CLeonardo(void);
    virtual void Run(void) override;
};