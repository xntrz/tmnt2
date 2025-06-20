#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseAI6034.hpp"


class C090UltimateShredder : public CEnemyCharacter
{
public:
    enum STATUS
    {
        STATUS_ATTACK_THUNDER = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_JUMP_READY,
        STATUS_ATTACK_JUMP_AERIAL,
        STATUS_ATTACK_JUMP_TOUCHDOWN,
    };

    class CSharedData
    {
    public:
        bool bJumpAttack;
    };

    class CThinkingObserver;
    class CAttackJumpReadyObserver;
    class CAttackJumpAerialObserver;
    class CAttackJumpTouchdownObserver;
    class CThunderAttackObserver;
    class CElecAttackObserver;
    class CThrowObserver;

public:
    DEFINE_EXTEND_STATUS_FN(STATUS, CEnemyCharacter);
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_TENGSWORD,
                            EFFECTID::ID_SHRD_ELEC,
                            EFFECTID::ID_SHRD_ELBALL,
                            EFFECTID::ID_SHRD_ELEND,
                            EFFECTID::ID_THUNDER,
                            EFFECTID::ID_WIND_PRESS,
                            EFFECTID::ID_WARP_START });

    C090UltimateShredder(void);
    virtual ~C090UltimateShredder(void);
    virtual void Run(void) override;
    virtual void OnStart(void) override;
    virtual void OnStop(void) override;
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual void OnMessageDoChangingAerial(void) override;
    virtual void OnMessageTouchdown(float fHeight) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    void AttachThrowStatusObserver(void);
    int32 GetNoReactionDamageAmount(void) const;
    void SetupTenguSwordEffect(void);
    void SetupBodyElecEffect(void);
    void SetupSharedData(void);

protected:
    Utils6034::ICharacterController     m_chrController;
    Utils6034::CBaseThrowObserver*      m_pThrow;
    Utils6034::CBoneTracer              m_swordBoneTracer;
    Utils6034::CTraceEffect             m_swordTraceEffect;
    CSharedData                         m_sharedData;
    Utils6034::CBodyElecEffectManager   m_elecEffectMgr;
    Utils6034::CFixTimer                m_elecEffectTimer;
};