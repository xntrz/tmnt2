#pragma once

#include "BaseEliteChr.hpp"


class CAccumulateModule;
class CBarrierModule;


class CBaseFootMechSplinter : public CBaseEliteChr
{
public:
    enum STATUS
    {
        STATUS_ATTACK_KICK = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_SHOT,
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
    protected:
        enum STEP
        {
            STEP_A = 0,
            STEP_AA,
            STEP_AAX,
            STEP_END,
        };

    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void OnStepMotionA(void);
        void OnStepMotionAA(void);
        void OnStepMotionAAX(void);
        bool IsMotionChainConnectHit(void) const;
        void LeoChargeAttack(void);
        void MicChargeAttack(void);
        CBaseFootMechSplinter& FootMechSplinter(void);

    protected:
        int32 m_step;
        int32 m_attackType;
    };

    class CAttackBStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        RESULT ObservingAttackNormal(void);
        RESULT ObservingAttackCharge(void);
        CBaseFootMechSplinter& FootMechSplinter(void);

    protected:
        int32 m_step;
        int32 m_attackType;
    };

    class CAttackKickStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void RapChargeAttack(void);

    protected:
        bool m_bCharge;
        int32 m_attackType;
        float m_fJumpInitSpeed;
    };

    class CAttackShotStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void ShotKnife(void);
        void ShotLazer(void);
        void SetLazer(const RwV3d* pvecPos, const RwV3d* pvecDir);
        void GetOffsetWorldPosition(RwV3d* pvecPos, int32 no) const;
        void CalcDirectionPitch(RwV3d* pvecDirection) const;

    protected:
        int32 m_step;
        int32 m_attackType;
        float m_fJumpInitSpeed;
        int32 m_orderTargetNo;
    };

    class CThrowStatusObserver : public CCommonEnemyObserver::CBaseThrow
    {
    public:
        virtual void OnStart(void) override;
        virtual void OnLift(void) override;
        virtual void OnThrowHit(void) override;

    private:
        float m_fMessageTimer;
    };

public:
    DEFINE_EXTEND_STATUS_FN(STATUS, CBaseEliteChr);
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_WARP_START,
                            EFFECTID::ID_LASER_BEAM_ALL,
                            EFFECTID::ID_LASER_FIRE_ALL,
                            EFFECTID::ID_LASER_HIT_ALL });

    CBaseFootMechSplinter(ENEMYID::VALUE enemyId);
    virtual ~CBaseFootMechSplinter(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter) override;
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual void OnMessageTouchdown(float fHeight) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    void AttachThrowStatusObserver(void);
    void AccumulateBegin(void);
    void AccumulateEnd(void);
    void AccumulateSetStep(int32 step);    
    void AccumulateSetEffectOff(void);
    void BarrierInvoke(void);
    bool BarrierCheckEnd(void);

protected:
    CCommonEnemyObserver::CBaseThrow* m_pThrow;
    CAccumulateModule* m_pModuleAccumulate;
    CBarrierModule* m_pModuleBarrier;
};