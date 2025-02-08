#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusJump.hpp"


namespace Slashuur
{
    const RwV3d CHARGE_ATTACK_LOCAL_POSITION    = { 0.0f, 0.0f, 1.0f };
    const RwV3d BANDANA_OFFSET                  = { -0.02f, 0.237f, -0.038f };

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
}; /* namespace Slashuur */


class CSlashuur : public CPlayerCharacter
{
public:
    CSlashuur(GAMETYPES::COSTUME costume);
    virtual ~CSlashuur(void);
};