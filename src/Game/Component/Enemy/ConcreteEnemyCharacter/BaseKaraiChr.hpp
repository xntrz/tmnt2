#pragma once

#include "BaseChr6045.hpp"


namespace BASEKARAI
{
    namespace MOTIONNAMES
    {
        static const char* MOVE_BACKROUND      = "BackAway";
        static const char* MOVE_JUMPESCAPE1    = "J1";
        static const char* MOVE_JUMPESCAPE2    = "J2";
        static const char* MOVE_JUMPESCAPE3    = "JDouble";
        static const char* MOVE_JUMPESCAPE4    = "J3";
        static const char* ATTACK_AA           = "AA";
        static const char* ATTACK_AAA          = "AAA";
        static const char* ATTACK_AAB          = "AAB";
        static const char* ATTACK_AAC          = "AAC";
        static const char* ATTACK_BB           = "BB";
        static const char* ATTACK_C1           = "J1";
        static const char* ATTACK_C2           = "JKunai";
        static const char* ATTACK_C3           = "J3";
        static const char* ATTACK_D1           = "Dash1";
        static const char* ATTACK_D2           = "Dash2";
        static const char* ATTACK_E1           = "JAttack1";
        static const char* ATTACK_E2           = "JAttack2";
        static const char* ATTACK_E3           = "JAttack3";
        static const char* ATTACK_F            = "D";
        static const char* ATTACK_SPECIAL1     = "C1";
        static const char* ATTACK_SPECIAL2     = "C2";
        static const char* ATTACK_SPECIAL3     = "C3";
    }; /* namespace MOTIONNAMES */

    enum STATUS
    {
        STATUS_MOVE_BACKROUND = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_MOVE_JUMPESCAPE,
        STATUS_ATTACK_C,
        STATUS_ATTACK_D,
        STATUS_ATTACK_E,
        STATUS_ATTACK_F,
        STATUS_ATTACK_SPECIAL,
        STATUS_ATTACKTODO,
        STATUS_ATTACK_COUNTER,
    }; /* enum STATUS */

    static const float GUARD_COUNTERACTION_TIME[] = { 0.5f, 0.5f, 0.5f };
    static_assert(COUNT_OF(GUARD_COUNTERACTION_TIME) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static const float ATTACK_SPECIAL_CHARGETIME[] = { 1.75f, 1.25f, 0.75f,};
    static_assert(COUNT_OF(ATTACK_SPECIAL_CHARGETIME) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");
}; /* namespace KARAITYPES */


class CBaseKaraiChr : public CBaseChr6045
{
public:    
    class CAttackDStatusObserver : public CStatusObserver
    {
    protected:
        enum STEP
        {
            STEP_DASH_BEGIN = 0,
            STEP_DASH,
            STEP_DASH_END,
        };

    public:
        CAttackDStatusObserver(void);
        CAttackDStatusObserver(const char* pszMotion);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void SetDashStart(void);
        virtual void SetDashEnd(void);
        void InitWaterSplashEffect(void);
        void ProcWaterSplashEffect(void);
        void SetDashParameter(int32 attackPlayerNo, float fDashSpeed, float fDashDistance);

    protected:
        int32 m_step;
        RwV3d m_vecDashPos;
        int32 m_attackPlayerNo;
        float m_fTimer;
        float m_fDashTime;
        bool  m_bDashEnd;
        const char* m_pszMotion;
        bool m_bEffectActive;
        uint32 m_hEffectWSplash;
    };

    class CAttackSpecialStatusObserver : public CAttackDStatusObserver
    {
    private:
        enum STEPCHARGE
        {
            STEPCHARGE_ONE = 0,
            STEPCHARGE_TWO,
            STEPCHARGE_THREE,

            STEPCHARGENUM,
        };

    public:
        CAttackSpecialStatusObserver(CBaseKaraiChr* pBaseKaraiChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void SetDashStart(void) override;
        virtual void SetDashEnd(void) override;
        void InitChargeEffect(void);
        void ProcChargeEffect(void);
        void InitElectricEffect(void);

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
        float m_fChargeTime;
        float m_fChargeStepTimer;
        int32 m_stepCharge;
        uint32 m_hEffectCharge;
        uint32 m_ahEffectElectric[2];
    };

    class CAttackEStatusObserver : public CBaseChr6045::CJumpActionStatusObserver
    {
    public:
        CAttackEStatusObserver(void);
    };
    
    class CAttackFStatusObserver : public CBaseChr6045::COneShotMotionForAttack
    {
    public:
        CAttackFStatusObserver(void);
        virtual void OnStart(void) override;
    };

    class CAttackCounterStatusObserver : public CAttackFStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CAttackCStatusObserver : public CBaseChr6045::CJumpActionStatusObserver
    {
    public:
        CAttackCStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual void SetJumpSetting(void) override;
        void SetShotContinualSetting(int32 iShotNum, float fAreaRadius, float fShotTime, int32 iAimNo);
        void SetShotSametimeSetting(int32 iShotNum, float fAreaRadius);
        void ShotKunaiContinual(void);
        void ShotKunaiSameTime(void);

    private:
        int32 m_attackPlayerNo;
        int32 m_shotCount;
        int32 m_shotContinualAimNo;
        float m_fShotDirBase;
        float m_fTimer;
        bool  m_bShotEnableFlag;
        bool  m_bShotContinualFlag;
        int32 m_shotNum;
        float m_fShotRadSize;
        float m_fShotContinualTime;
        float m_fShotContinualInterval;
    };

    class CAttackC2StatusObserver : public CAttackCStatusObserver
    {
    public:
        virtual void OnStart(void) override;
    };

    class CAttackBStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    private:
        enum STEP
        {
            STEP_BEGIN = 0,
            STEP_BB,
            STEP_END,
        };

    public:
        CAttackBStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        int32 m_orderType;
    };
    
    class CAttackAStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    private:
        enum STEP
        {
            STEP_BEGIN = 0,
            STEP_AA,
            STEP_AAA,
            STEP_END,
        };

    public:
        CAttackAStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;

    private:
        int32 m_step;
        int32 m_orderType;        
    };
    
    class CGuardStatusObserver : public CBaseChr6045::CGuardStatusObserver
    {
    public:
        CGuardStatusObserver(CBaseKaraiChr* pBaseKaraiChr);
        virtual RESULT PassageGuardTotalTime(void) override;

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
    };

    class CMoveBackRoundStatusObserver : public CBaseChr6045::COneShotMotionForAttack
    {
    public:
        CMoveBackRoundStatusObserver(void);
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CMoveJumpEscapeStatusObserver : public CBaseChr6045::CJumpActionStatusObserver
    {
    public:
        CMoveJumpEscapeStatusObserver(CBaseKaraiChr* pBaseKaraiChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void SetJumpSetting(void) override;

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
        RwV3d m_vecOrderPos;
        bool m_bDoubleJump;
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
    
    class CDeathStatusObserver : public CBaseChr6045::CDeathStatusObserver
    {
    public:
        CDeathStatusObserver(CBaseKaraiChr* pBaseKaraiChr);
        virtual void OnStart(void) override;

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
    };

    class CThinkingStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_nextStatus;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);
    static void GetDashMarkPosition(RwV3d* pvecPos,
                                    const CCharacterCompositor* pChrCompositor,
                                    int32 iPlayerNo,
                                    float fDistance);

    CBaseKaraiChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseKaraiChr(void);
    virtual void Run(void) override;
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    void SetWarningModeSetting(void);
    void SetFinalModeSetting(void);
    bool IsModeWarning(void) const;
    bool IsModeFinal(void) const;
    void ClearModeEffects(void);

    using CBaseChr6045::AttachStatusObserver;
    bool AttachStatusObserver(BASEKARAI::STATUS status, CStatusObserver* pStatusObserver);

    using CBaseChr6045::SetStatus;
    bool SetStatus(BASEKARAI::STATUS status);

private:
    bool m_bModeWarning;
    bool m_bModeFinal;
    uint32 m_hEffectWarning;
    uint32 m_hEffectFinal;
};