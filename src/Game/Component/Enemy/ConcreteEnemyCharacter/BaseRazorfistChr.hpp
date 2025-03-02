#pragma once

#include "BaseChr6045.hpp"


namespace BASERAZORFIST
{
    namespace MOTIONNAMES
    {
		static const char* ATTACK_D = "D";
    }; /* namespace MOTIONNAMES */

    enum STATUS
    {
        STATUS_ATTACK_D = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_COUNTER,
    }; /* enum STATUS */
}; /* namespace BASERAZORFIST */


class CBaseRazorfistChr : public CBaseChr6045
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

    class CAttackBStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackBStatusObserver(void);
    };

    class CAttackCStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackCStatusObserver(void);
    };

    class CAttackDStatusObserver : public CBaseChr6045::CAttackSalivaShotStatusObserver
    {
    public:
        CAttackDStatusObserver(void);
    };

    class CAttackCounterStatusObserver : public CAttackAStatusObserver
    {
    public:
        CAttackCounterStatusObserver(CBaseRazorfistChr* pBaseRazorfistChr);
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        
    private:
        CBaseRazorfistChr* m_pBaseRazorfistChr;
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
        CDeathStatusObserver(CBaseRazorfistChr* pBaseRazorfistChr);
    };

public:
    DEFINE_EXTEND_STATUS_FN(BASERAZORFIST::STATUS, CBaseChr6045);

    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_TBSHOT,
                            EFFECTID::ID_TBSHOT_MARK,
                            EFFECTID::ID_WARP_START });

    CBaseRazorfistChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseRazorfistChr(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter) override;
};