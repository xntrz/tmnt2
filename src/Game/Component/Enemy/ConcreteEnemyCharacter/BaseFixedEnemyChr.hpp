#pragma once

#include "BaseChr6045.hpp"


class CBaseFixedEnemyChr : public CEnemyCharacter
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

    class CAttackBStatusObserver : public CBaseChr6045::CAttackSalivaShotStatusObserver
    {
    public:
        CAttackBStatusObserver(void);        
    };    

    class CKnockStatusObserver : public CCommonEnemyObserver::CKnockStatus
    {
    public:
        virtual void OnStart(void) override;
    };

    class CDindleStatusObserver : public CCommonEnemyObserver::CCharacterEffectStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        uint32 m_hEffectBiribiri;
    };

    class CDeathStatusObserver : public CBaseChr6045::CDeathStatusObserver
    {
    public:
        CDeathStatusObserver(void);
    };

public:
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_SPSM_TBSHOT });

    CBaseFixedEnemyChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseFixedEnemyChr(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter) override;
};