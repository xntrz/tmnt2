#pragma once

#include "BaseChr6045.hpp"


#define RATESPEED(x) (1.0f / (x))


namespace BASELEATHERHEAD
{
    static const int32 DAMAGE_AMOUNT_RECV_TO_SWIMMODE = 50;

    static const RwV3d EFFECT_WATERSPLASH_NORMAL_OFFSET = { 0.25f, 0.0f, 0.0f };
    static const RwV3d EFFECT_WATERSPLASH_SWIM_OFFSET = { 0.5f, 0.0f, 0.5f };

    static const float SWIMMODE_CIRCLE_CENTER_X_MIN[] = { 0.0f, 0.0f };
    static const float SWIMMODE_CIRCLE_CENTER_Z_MIN[] = { 0.0f, 0.0f };
    static const float SWIMMODE_CIRCLE_CENTER_X_MAX[] = { 6.0f, 0.0f };
    static const float SWIMMODE_CIRCLE_CENTER_Z_MAX[] = { 0.0f, 0.0f };
    static const float SWIMMODE_CIRCLE_CENTER_Y[] = { 0.0f, 0.0f };
    static const float SWIMMODE_CIRCLE_RADIUS[] = { 4.5f, 16.0f };
    static const float SWIMMODE_CIRCLE_JUMPANGLECORRECT_MIN[] = { MATH_DEG2RAD(72.0f), MATH_DEG2RAD(9.0f) };
    static const float SWIMMODE_CIRCLE_JUMPANGLECORRECT_MAX[] = { MATH_DEG2RAD(90.0f), MATH_DEG2RAD(45.0f) };
    static const float SWIMMODE_ATTACK_JUMPSPEED[] = { 17.0f, 25.0f };
    static const float SWIMMODE_ATTACK_XZSPEED_CORRECTRATE[] = { 1.2f, 1.05f };

    static const float ATTACK_E_SPEED[] = { 9.0f, 9.0f, 11.0f };
    static_assert(COUNT_OF(ATTACK_E_SPEED) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static const float ATTACK_E_AIM_ANGLESPEED[] = { 0.25f, 0.5f, 1.0f };
    static_assert(COUNT_OF(ATTACK_E_AIM_ANGLESPEED) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static const float SWIMMODE_CIRCLESPEED_MIN[] = { 8.0f, 8.0f, 9.0f };
    static_assert(COUNT_OF(SWIMMODE_CIRCLESPEED_MIN) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");
    
    static const float SWIMMODE_CIRCLESPEED_MAX[] = { 10.0f, 12.0f, 14.0f };
    static_assert(COUNT_OF(SWIMMODE_CIRCLESPEED_MAX) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    /* TODO: have no clue exactly how this was calculated but this values is not manual numbers */
    static const float SWIMMODE_CENTERRATESPEED_MIN[] = { RATESPEED(6.0f), RATESPEED(5.7f), RATESPEED(5.7f) };
    static_assert(COUNT_OF(SWIMMODE_CENTERRATESPEED_MIN) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    /* TODO: have no clue exactly how this was calculated but this values is not manual numbers */
    static const float SWIMMODE_CENTERRATESPEED_MAX[] = { RATESPEED(5.6f), RATESPEED(5.3f), RATESPEED(5.2f) };
    static_assert(COUNT_OF(SWIMMODE_CENTERRATESPEED_MAX) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static const float SWIMMODE_ATTACK_TIME[] = { 7.0f, 7.0f, 8.0f };
    static_assert(COUNT_OF(SWIMMODE_ATTACK_TIME) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    namespace MOTIONNAMES
    {
        static const char* IDLE_1       = "Idle1";
        static const char* IDLE_2       = "Idle2";
        static const char* SWIM_1       = "Swim1";
        static const char* SWIM_2       = "Swim2";
        static const char* SWIM_3       = "Swim3";
        static const char* SWIM_4       = "Swim4";
        static const char* ATTACK_AAA   = "AAA";
        static const char* ATTACK_E1    = "E1";
        static const char* ATTACK_E2    = "E2";
        static const char* ATTACK_E3    = "E3";
        static const char* KNOCK_SWIM   = "YSwim";
    }; /* namespace MOTIONNAMES */

    enum STATUS
    {
        STATUS_SWIM = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_AFTER_SWIM_JUMP,
        STATUS_AFTER_SWIM_TOUCHDOWN,
        STATUS_ATTACK_E,
        STATUS_ATTACK_E_STUN,
        STATUS_ATTACK_COUNTER,
    }; /* enum STATUS */
}; /* namespace BASELEATHERHEAD */


class CBaseLeatherheadChr : public CBaseChr6045
{
public:
    struct SWIM_PARAM
    {
        bool    bSwimModeFlag;      /* indicates whether swim mode available or not */
        bool    bNowSwimmingFlag;   /* indicates whether we are really swimming now or not */
        bool    bGoingSwimming;     /* indicates decision to change to swim mode after receiving some damage amount */
        float   fSwimTimer;         /* indicates how long to swim */
    };

    class CHideStatusObserver : public CBaseChr6045::CNothingStatusObserver
    {
    public:
        CHideStatusObserver(void);
    };

    class CAppearStatusObserver : public CBaseChr6045::CNothingStatusObserver
    {
    public:
        CAppearStatusObserver(void);
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

    class CIdleStatus : public CCommonEnemyObserver::CIdleStatus
    {
    public:
        virtual void OnStart(void) override;
    };

    class CAttackAAAStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackAAAStatusObserver(void);
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        bool CheckChainAttack(void) const;

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CAttackBStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackBStatusObserver(void);        
    };

    class CAttackEStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    private:
        enum STEP
        {
            STEP_ROAR = 0,
            STEP_CHASE,
            STEP_CHASE_HIT,
            STEP_CHASE_CRASH,
        };

    public:
        CAttackEStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void CatchAttackResult(CHitCatchData* pCatch);
        bool CheckAimContinue(int32 playerNo);
        void EntryShockwave(void) const;
        void SetSpecialSetting(void);
        void ClrSpecialSetting(void);

    private:
        int32   m_step;
        float   m_fTimer;
        bool    m_bAim;
        float   m_fRotSpeed;
        float   m_fMoveSpeed;
        bool    m_bChaseHit;
        int32   m_orderTargetNo;
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CAttackEStunStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fStunTime;
        uint32 m_hStunEffect;
    };

    class CAttackCounterStatusObserver : public CAttackBStatusObserver
    {
    public:
        CAttackCounterStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr);
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        CBaseLeatherheadChr* m_pBaseLeatherheadChr;
    };

    class CSwimStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        CSwimStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void SetSwimSpeed(void);
        bool ProcJumpToSwim(void);
        void ProcSwim(void);
        void SetJumpToSwimSetting(void);
        void GetSwimCircleCenterPoint(RwV3d* vecCircleCenterPt) const;
        void CalcSwimMarkPoint(void);
        void InitAttackReady(void);
        bool ProcAttackReady(void);
        bool SetAttackSetting(int32 playerNo);
        void SetSpecialSetting(void);
        void ClrSpecialSetting(void);

    private:
        CBaseLeatherheadChr* m_pBaseLeatherheadChr;
        RwV3d m_vecMarkPos;
        RwV3d m_vecCircleCenterStart;
        RwV3d m_vecCircleCenterEnd;
        float m_fCircleCenterRate;
        float m_fCircleCenterRateSpeed;
        float m_fCircleRadius;
        float m_fCircleSpeed;
        float m_fCircleAngle;
        float m_fCircleAngleSpeed;
        float m_fCircleSpeedMin;
        float m_fCircleSpeedMax;
        float m_fCenterRateSpeedMin;
        float m_fCenterRateSpeedMax;
        float m_fGravityRate;
        int32 m_step;
        int32 m_stageValue;
        bool  m_bJumpXZCorrectFlag;
        bool  m_bJumpTouchdownFlag;
        bool  m_bAttackOrderFlag;
        float m_fAttackReadyTimer;
        int32 m_waterSplashCountAttackReady;
    };

    class CAfterSwimJumpStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        CAfterSwimJumpStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void SetSpecialSetting(void);
        void ClrSpecialSetting(void);

    private:
        int32 m_orderTargetNo;
        float m_fGravityRate;
        int32 m_stageValue;
    };

    class CAfterSwimTouchdownStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        CAfterSwimTouchdownStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        CBaseLeatherheadChr* m_pBaseLeatherheadChr;
        bool m_bFirstTouchdownFlag;
    };

    class CThrowStatusObserver : public CCommonEnemyObserver::CBaseThrow
    {
    public:
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void OnThrowStandby(void) override;

    private:
        int32 m_orderTargetNo;
    };

    class CGuardStatusObserver : public CBaseChr6045::CGuardStatusObserver
    {
    public:
        CGuardStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr);
        virtual RESULT PassageGuardTotalTime(void) override;

    private:
        CBaseLeatherheadChr* m_pBaseLeatherheadChr;
    };

    class CJumpReadyStatusObserver : public CCommonEnemyObserver::CJumpReadyStatus
    {
    public:
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CJumpStatusObserver : public CCommonEnemyObserver::CAerialStatus
    {
    public:
        virtual RESULT Observing(void) override;
    };

    class CKnockStatusObserver : public CCommonEnemyObserver::CKnockStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bSwimFlag;
    };

    class CFlyawayStatusObserver : public CCommonEnemyObserver::CFlyawayStatus
    {
    public:
        CFlyawayStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bSwimFlag;
    };

    class CDeathStatusObserver : public CBaseChr6045::CDeathStatusObserver
    {
    public:
        CDeathStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr);
    };

public:
    DEFINE_EXTEND_STATUS_FN(BASELEATHERHEAD::STATUS, CBaseChr6045);

    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_WIND_PRESS,
                            EFFECTID::ID_WARP_START });

    CBaseLeatherheadChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseLeatherheadChr(void);
    virtual void Run(void) override;
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual void OnMessageTouchdown(float fHeight) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;
    bool CheckDamagePermission(CCharacterAttackCalculator& rCalc);
    void InitSwimParameter(void);
    void SetSwimSetting(void);
    void ProcSwimModeMustSwimming(void);
    void ProcSwimModeTimer(void);
    void ProcSwimModeDamageHeap(int32 iAmount);
    void InitWaterEffect(void);
    void TermWaterEffect(void);
    void ProcWaterEffect(void);
    void SetWaterRippleActive(bool bActive);
    void SetWaterSplashActive(bool bActive);
    void SetWaterRipplePosition(void);
    void SetWaterSplashPosition(void);
    void SetWaterEffectPosition(uint32 hEffect, const RwV3d* pvecPos);
    void AttachThrowStatusObserver(void);
    bool IsSwimming(void) const;
    void SetSwimTimer(float fTimer);
    float GetSwimTimer(void) const;

protected:
    SWIM_PARAM              m_swimParam;
    CAttackEStatusObserver* m_pStatusAttackE;
    uint32                  m_hLastMagicAttacker;
    int32                   m_iDamageHeap;
    bool                    m_bEffectWaterRippleActive;
    uint32                  m_hEffectWaterRipple;
    bool                    m_bEffectWaterSplashActive;
    uint32                  m_ahEffectWaterSplash[2];
};