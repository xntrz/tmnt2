#pragma once

#include "BaseChr6045.hpp"


namespace BASEMOUSER
{
    namespace MOTIONNAMES
    {
        static const char* ATTACK_C1            = "C1";
        static const char* ATTACK_C2            = "C2";
        static const char* FLYAWAY_FRONT2       = "YFrontFuttobi2";
        static const char* FLYAWAY_BACK2        = "YBackFuttobi2";
        static const char* FLYAWAY_BOUND_FRONT  = "YFrontFuttobi3";
        static const char* FLYAWAY_BOUND_BACK   = "YBackFuttobi3";
        static const char* GETUP_FRONT          = "YFrontFuttobi4";
        static const char* GETUP_BACK           = "YBackFuttobi4";
    }; /* namespace MOTIONNAMES */
}; /* namespace BASEMOUSER */


class CBaseMouserChr : public CEnemyCharacter
{
public:
    class CHideStatusObserver : public CBaseChr6045::CNothingStatusObserver
    {
    public:
        CHideStatusObserver(void);
    };    

    class CThinkingStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CAttackAStatusObserver : public CBaseChr6045::COneShotMotionForAttack
    {
    public:
        CAttackAStatusObserver(void);
    };

    class CAttackBStatusObserver : public CBaseChr6045::COneShotMotionForAttack
    {
    public:
        CAttackBStatusObserver(void);
    };

    class CAttackCStatusObserver : public CStatusObserver
    {
    public:
        CAttackCStatusObserver(CBaseMouserChr* pBaseMouserChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void InitElecEffect(void);
        void TermElecEffect(void);
        void InitBodyColorChange(void);
        void TermBodyColorChange(void);
        void ProcBodyColorChange(void);

    private:
        float m_fTimer;
        bool m_bMotionChangeFlag;
        uint32 m_hEffectElectric;
        CEnemyUtils::CModelColorControl m_modelColorCtrl;
    };

    class CMoveStatusObserver : public CCommonEnemyObserver::CMoveStatus
    {
    public:
        virtual void OnStart(void) override;
    };

    // TODO ps2 demo version is not based on common enemy observer
    class CFlyawayStatusObserver : public CCommonEnemyObserver::CFlyawayStatus
    {
    public:
        CFlyawayStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;

    private:
        bool m_bMotionChangeFlag;
    };

    // TODO ps2 demo version is not based on common enemy observer
    class CFlyawayBoundStatusObserver : public CCommonEnemyObserver::CFlyawayBoundStatus
    {
    public:
        virtual void OnStart(void) override;
    };

    class CGetupStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bBackFlag;
    };

    class CDeathStatusObserver : public CBaseChr6045::CDeathStatusObserver
    {
    public:
        CDeathStatusObserver(CBaseMouserChr* pBaseMouserChr);
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    CBaseMouserChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseMouserChr(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter) override;
    virtual void Run(void) override;
};