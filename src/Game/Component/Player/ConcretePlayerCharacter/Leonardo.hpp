#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"


namespace Leonardo
{
    const RwV3d BANDANA_OFFSET                  = { 0.0f, 0.15f, 0.05f };
    const RwV3d JUMPATTACK_VELOCITY             = { 0.0f, -7.5f, 12.5f };
    const RwV3d CHARGE_ATTACK_LOCAL_POSITION    = { 0.0f, 0.145f, 1.0f };

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
    public:
        virtual void OnDischargeWave(void) override;
    };

    class CAttackB : public PlayerStatus::CAttackB
    {
    public:
        virtual void OnDischargeWave(MAGIC_GENERIC::STEP step) override;
    };
}; /* namespace Leonardo */


class CLeonardo : public CPlayerCharacter
{
public:
    CLeonardo(GAMETYPES::COSTUME costume);
    virtual ~CLeonardo(void);
};