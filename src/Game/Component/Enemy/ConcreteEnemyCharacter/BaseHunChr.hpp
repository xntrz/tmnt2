#pragma once

#include "BaseChr6045.hpp"


namespace BASEHUN
{
    static const char* ATTACK_B_GIMMICK_NAME_HUN_1 = "GMKM58OBRM_B_0";
    static const char* ATTACK_B_GIMMICK_NAME_HUN_2 = "GMKM58OBRM_C_0";

    namespace MOTIONNAMES
    {
        static const char* ATTACK_D  = "D";
        static const char* ATTACK_F1 = "JAttack1";
        static const char* ATTACK_F2 = "JAttack2";
        static const char* ATTACK_F3 = "JAttack3";
    };

    enum STATUS
    {
        STATUS_ATTACK_D = ENEMYTYPES::STATUS_SPECIAL,        
        STATUS_ATTACK_F,
        STATUS_ATTACK_F_CONNECT,
        STATUS_BURST_FLYAWAY,
        STATUS_BURST_FLYAWAY_BOUND,
    };
};


class CBaseHunChr : public CBaseChr6045
{
public:
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
        CThinkingStatusObserver(CBaseHunChr* pBaseHunChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        bool BurstAttackThinkWaitProc(void);

    private:
        CBaseHunChr* m_pBaseHunChr;
        bool m_bBurstEffectActive;
        uint32 m_hBurstEffect;
        float m_fBurstThinkWaitTimer;
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CRunStatusObserver : public CCommonEnemyObserver::CMoveStatus
    {
    public:
        virtual RESULT Observing(void) override;
    };

    class CAttackAStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackAStatusObserver(void);
    };

    class CAttackBStatusObserver : public CBaseChr6045::COneShotMotionForAttack
    {
    public:
        CAttackBStatusObserver(CBaseHunChr* pBaseHunChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
        const char* m_pszFallRubbleMgrObjName;
        RwV3d m_vecOffset;
        float m_fScale;
    };

    class CAttackCStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackCStatusObserver(CBaseHunChr* pBaseHunChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void InitWaterSplashEffect(void);
        void TermWaterSplashEffect(void);
        void ProcWaterSplashEffect(void);

    private:
        CBaseHunChr* m_pBaseHunChr;
        RwV3d m_vecAimPos;
        float m_fTimer;
        RwV3d m_vecEffectPos;
        uint32 m_hEffect;
        bool m_bEffectActive;
    };

    class CAttackDStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackDStatusObserver(CBaseHunChr* pBaseHunChr);
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
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

    class CAttackFStatusObserver : public CBaseChr6045::CJumpActionStatusObserver
    {
    public:
        CAttackFStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void SetJumpSetting(void) override;
        virtual void SetTouchdownSetting(void) override;

    private:
        int32 m_orderTargetNo;
    };

    class CAttackFConnectFromRunStatusStatusObserver : public CAttackFStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual void SetJumpSetting(void) override;
    };

    class CGuardStatusObserver : public CBaseChr6045::CGuardStatusObserver
    {
    public:
        CGuardStatusObserver(CBaseHunChr* pBaseHunChr);
        virtual RESULT PassageGuardTotalTime(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
    };

    class CFlyawayForBurstStatusObserver : public CCommonEnemyObserver::CFlyawayStatus
    {
    public:
        CFlyawayForBurstStatusObserver(CBaseHunChr* pBaseHunChr);
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
    };

    class CFlyawayBoundForBurstStatusObserver : public CCommonEnemyObserver::CFlyawayBoundStatus
    {
    public:
        CFlyawayBoundForBurstStatusObserver(CBaseHunChr* pBaseHunChr);
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
    };

    class CDeathStatusObserver : public CBaseChr6045::CDeathStatusObserver
    {
    public:
        CDeathStatusObserver(CBaseHunChr* pBaseHunChr);
        virtual void OnStart(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
    };

public:
    DEFINE_EXTEND_STATUS_FN(BASEHUN::STATUS, CBaseChr6045);

    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_WIND_PRESS,
                            EFFECTID::ID_WARP_START });

    CBaseHunChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseHunChr(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter) override;
    virtual void Run(void) override;    
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual void OnMessageTouchdown(float fHeight) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    void InitRevengeMode(int32 damageAmountForRevenge);
    void SetRevengeMode(void);
    void ClearRevengeMode(void);
    void AddRevengeDamage(int32 amount);
    void SetBurstMode(void);
    void ClearBurstMode(void);
    void CalcBodyColor(void);
    bool IsInBurstMode(void) const;
    bool IsInRevengeMode(void) const;
    void AttachThrowStatusObserver(void);

protected:
    bool m_bRevengeModeActive;
    bool m_bBurstModeActie;
    int32 m_revengeDamageHeap;
    int32 m_revengeDamageMax;
    CEnemyUtils::CModelColorControl m_modelColorCtrl;
};