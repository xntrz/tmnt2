#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseAI6034.hpp"


class C088Shredder : public CEnemyCharacter
{
public:
    class CThinkingObserver;
    class CSpecialDamagedObserver;
    class CElecAttackObserver;
    
public:
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_SHRD_ELEC,
                            EFFECTID::ID_SHRD_ELBALL,
                            EFFECTID::ID_SHRD_ELEND });

    C088Shredder(void);
    virtual ~C088Shredder(void);
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;

protected:
    Utils6034::ICharacterController m_chrController;
};