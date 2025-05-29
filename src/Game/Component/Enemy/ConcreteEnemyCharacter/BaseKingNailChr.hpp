#pragma once

#include "BaseChr6045.hpp"


namespace BASEKINGNAIL
{
    namespace MOTIONNAMES
    {
        static const char* ATTACK_D1 = "D1";
        static const char* ATTACK_D2 = "D2";
        static const char* ATTACK_D3 = "D3";
        static const char* ATTACK_D4 = "D4";
    };

    enum STATUS
    {
        STATUS_ATTACK_D = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_COUNTER,
    };
};


class CBaseKingNailChr : public CBaseChr6045
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

    class CAttackAStatusObserver : public CBaseChr6045::CAttackSalivaShotStatusObserver
    {
    public:
        CAttackAStatusObserver(void);
    };

    class CAttackBStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackBStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;

    private:
        bool m_bRotateEnableFlag;
    };

    class CAttackCStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackCStatusObserver(void);
    };

    class CAttackDStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackDStatusObserver(CBaseKingNailChr* pBaseKingNailChr);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        CBaseKingNailChr* m_pBaseKingNailChr;
        int32 m_iStep;
        float m_fTimer;
        float m_fSpinTime;
    };

    class CAttackCounterStatusObserver : public CAttackDStatusObserver
    {
    public:
        CAttackCounterStatusObserver(CBaseKingNailChr* pBaseKingNailChr);
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CGuardStatusObserver : public CBaseChr6045::CGuardStatusObserver
    {
    public:
        CGuardStatusObserver(void);
        virtual RESULT PassageGuardTotalTime(void) override;
    };

    class CJumpReadyStatusObserver : public CBaseChr6045::CJumpReadyStatusObserver
    {
    public:
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CDeathStatusObserver : public CBaseChr6045::CDeathStatusObserver
    {
    public:
        CDeathStatusObserver(CBaseKingNailChr* pBaseKingNailChr);
    };

public:
    DEFINE_EXTEND_STATUS_FN(BASEKINGNAIL::STATUS, CBaseChr6045);

    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_TBSHOT,
                            EFFECTID::ID_TBSHOT_MARK,
                            EFFECTID::ID_WARP_START });

    CBaseKingNailChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseKingNailChr(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter) override;
};