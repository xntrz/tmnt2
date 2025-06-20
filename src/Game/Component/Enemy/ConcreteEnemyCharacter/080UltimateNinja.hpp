#pragma once

#include "080UltimateNinjaUtils.hpp"

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseAI6034.hpp"


class C080UltimateNinja : public CEnemyCharacter
{
public:
    enum STATUS
    {
        STATUS_ATTACK_JUMP_READY = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_JUMP_AERIAL,
        STATUS_ATTACK_DRAGON_AERIAL,
        STATUS_ATTACK_DRAGON_TOUCHDOWN,
        STATUS_WARP_START,
        STATUS_WARP_END,
    };

    class CScatterKnifeMgr;

    class CThinkingObserver;
    class CAttackAAObserver;
    class CAttackBBBBObserver;
    class CAttackJumpReadyObserver;
    class CAttackJumpAerialObserver;
    class CAttackDragonObserver;
    class CAttackDragonAerialObserver;
    class CAttackDragonTouchdownObserver;
    class CAttackSmokeObserver;
    class CAttackThrowWeightObserver;
    class CWarpObserer;
    class CWarpEndObserver;

public:
    DEFINE_EXTEND_STATUS_FN(STATUS, CEnemyCharacter);
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_DRAGONSMOKE,
                            EFFECTID::ID_WINDWARP_START,
                            EFFECTID::ID_WINDWARP_OUT,
                            EFFECTID::ID_ULNIN_ENMAKU,
                            EFFECTID::ID_WARP_START });
    
    C080UltimateNinja(void);
    virtual ~C080UltimateNinja(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    virtual void OnStart(void) override;
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual void OnMessageTouchdown(float fHeight) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    int32 GetNoReactionDamageAmount(void) const;

protected:
    Utils6034::ICharacterController m_chrController;
    Utils6034::CFixTimer            m_timerSpinningWeightSE;
    UltimateNinjaUtils::CDragon     m_dragon;
};



