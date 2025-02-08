#pragma once

#include "BaseGeneralEnemyChr.hpp"


class C098Fugitoid : public CBaseGeneralEnemyChr
{
public:
    class CCrashWallStatusObserver : public CCommonEnemyObserver::CCrashWallStatus
    {
    public:
        virtual void OnStart(void) override;
    };

    class CDeathStatusObserver : public CCommonEnemyObserver::CCrashWallStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bMessageRequest;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C098Fugitoid(void);
    virtual ~C098Fugitoid(void);
    virtual void Run(void);
    virtual void OnMessageTouchdown(float fHeight);
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc);
    virtual CHRTYPE GetAttackCharacterType(void);
};