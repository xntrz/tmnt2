#pragma once

#include "BaseChr6045.hpp"


namespace BASESPIDERMONSTER
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
}; /* namespace BASESPIDERMONSTER */


class CBaseSpiderMonsterChr : public CBaseChr6045
{
public:
    class CHideStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CAppearStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
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

    class CSpiderWebShotObserver : public CBaseChr6045::CAttackSalivaShotStatusObserver
    {
    public:
        CSpiderWebShotObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
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

    class CAttackDStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackDStatusObserver(void);
    };

    class CAttackCounterStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackCounterStatusObserver(void);
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CGuardStatusObserver : public CBaseChr6045::CGuardStatusObserver
    {
    public:
        CGuardStatusObserver(void);
    };

    class CJumpReadyStatusObserver : public CBaseChr6045::CJumpReadyStatusObserver
    {
    public:
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CDeathStatusObserver : public CBaseChr6045::CDeathStatusObserver
    {
    public:
        CDeathStatusObserver(CBaseSpiderMonsterChr* pBaseSpiderMonsterChr);
    };

public:
    DEFINE_EXTEND_STATUS_FN(BASESPIDERMONSTER::STATUS, CBaseChr6045);

    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_SPIDERNET,
                            EFFECTID::ID_SPIDERSMOKE,
                            EFFECTID::ID_WARP_START });
    
    CBaseSpiderMonsterChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseSpiderMonsterChr(void);
    void AttachAppearStatusObserver(void);
};