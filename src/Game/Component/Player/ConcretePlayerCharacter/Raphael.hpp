#pragma once

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusJump.hpp"
#include "Game/Component/Player/ConcretePlayerStatus/PlayerStatusThrow.hpp"


namespace Raphael
{
    const RwV3d BANDANA_OFFSET = { 0.0f, 0.15f, 0.05f };

    namespace MOTIONNAMES
    {
        static const char* ATTACK_JUMP1 = "JAttack1";
        static const char* ATTACK_JUMP2 = "JAttack2";
        static const char* ATTACK_JUMP3 = "JAttack3";
        static const char* PUSH         = "Push";
    };

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

    class CAttackAAC : public PlayerStatus::CLiftChallenge
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
}; /* namespace Raphael */


class CRaphael : public CPlayerCharacter
{
public:
    CRaphael(GAMETYPES::COSTUME costume);
    virtual ~CRaphael(void);
};