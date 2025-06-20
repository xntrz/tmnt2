#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseAI6034.hpp"


class C074Traximus : public CEnemyCharacter
{
public:
    enum STATUS
    {
        STATUS_ATTACK_JUMP_READY = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_JUMP_AERIAL,
        STATUS_BACK_STEP_START,
    };

    class CSharedData
    {
    public:
        bool bJumpAttack;
    };

    class CThinkingObserver;
    class CAttackAAStatusObserver;
    class CAttackBStatusObserver;
    class CBackStepStartObserver;
    class CAttackJumpReadyObserver;
    class CAttackJumpAerialObserver;
    class CAttackJumpTouchdownObserver;
    class CThrowObserver;

public:
    DEFINE_EXTEND_STATUS_FN(STATUS, CEnemyCharacter);
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_WIND_PRESS,
                            EFFECTID::ID_WARP_START });
    
    C074Traximus(void);
    virtual ~C074Traximus(void);
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual void OnMessageDoChangingAerial(void) override;
    virtual void OnMessageTouchdown(float fHeight) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    void AttachThrowStatusObserver(void);
    int32 GetNoReactionDamageAmount(void) const;

protected:
    Utils6034::ICharacterController     m_chrController;
    CSharedData                         m_sharedData;
    CCommonEnemyObserver::CBaseThrow*   m_pThrow;
};