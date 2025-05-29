#pragma once

#include "BaseEliteChr.hpp"


class C085MiyamotoUsagi : public CBaseEliteChr
{
public:
    enum STATUS
    {
        STATUS_ATTACK_KICK = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_COUNTER,
        STATUS_MUTEKI,
    };

    class CThinkingStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CAttackAStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        int32 m_step;
    };

    class CAttackBStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void PlayEffectFlash(void);
        void PlayMagicWindPress(void);

    protected:
        bool m_bFlash;
        bool m_bWindPress;
    };

    class CKickAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CRunAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void GetPredictPlayerPos(int32 playerNo, RwV3d* pvecPredictPos) const;

    protected:
        RwV3d m_vecOrderPos;
        int32 m_orderTargetNo;
        float m_fMoveSpeed;
        float m_fMoveTime;
        int32 m_step;
    };

    class CBackRollStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CAvoidAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        bool m_bEffectEntry;
    };

    class CCounterAttackStatusObserver : public CBaseEliteChr::CWarpBaseStatusObserver
    {
    public:
        CCounterAttackStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        int32 m_step;
    };

    class CMutekiGuardStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        const char* m_apszMutekiMotion[2];
        int32 m_step;
        float m_fMutekiTime;
    };

    class CDeathStatusObserver : public CBaseEliteChr::CDeathStatusObserver
    {
    public:
        virtual RESULT Observing(void) override;
    };

public:
    DEFINE_EXTEND_STATUS_FN(STATUS, CBaseEliteChr);
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_WIND_PRESS,
                            EFFECTID::ID_BLADE_FLASh });

    C085MiyamotoUsagi(void);
    virtual ~C085MiyamotoUsagi(void);
    virtual bool OnMessageCanChangingAerial(float fHeight);
    virtual void OnMessageTouchdown(float fHeight);
    virtual void OnMessageAttackResult(CHitCatchData* pCatch);
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc);
    void CheckAttackBarrier(const CHitAttackData* pAttack);
    void CheckAttackDamage(const CHitAttackData* pAttack);
};