#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusJump.hpp"


namespace Michelangero
{
    const RwV3d BANDANA_OFFSET = { 0.0f, 0.15f, 0.05f };

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
        void ClearTimer(void);
        float GetTimer(void) const;

    protected:
        float m_fStartTime;
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
}; /* namespace Michelangero */


class CMichelangero : public CPlayerCharacter
{
public:
    CMichelangero(GAMETYPES::COSTUME costume);
    virtual ~CMichelangero(void);
};