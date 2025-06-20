#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseAI6034.hpp"


class C089FeudalJapanShredder : public CEnemyCharacter
{
public:
    class CThinkingObserver;
    class CThrowObserver;
    class CElecAttackObserver;
    
public:
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_TENGSWORD,
                            EFFECTID::ID_SHRD_ELEC,
                            EFFECTID::ID_SHRD_ELBALL,
                            EFFECTID::ID_SHRD_ELEND });

    C089FeudalJapanShredder(void);
    virtual ~C089FeudalJapanShredder(void);
    virtual void Run(void) override;
    virtual void OnStart(void) override;
    virtual void OnStop(void) override;
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    void AttachThrowStatusObserver(void);
    int32 GetNoReactionDamageAmount(void) const;
    void SetupTenguSwordEffect(void);

protected:
    Utils6034::ICharacterController m_chrController;
    Utils6034::CBaseThrowObserver*  m_pThrow;
    Utils6034::CBoneTracer          m_swordBoneTracer;
    Utils6034::CTraceEffect         m_swordTraceEffect;
};