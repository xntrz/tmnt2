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

    class CDeathStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bMessageRequest;
    };

public:
    DEFINE_EMPTY_NEEDED_EFFECTS();

    C098Fugitoid(void);
    virtual ~C098Fugitoid(void);
    virtual void Run(void);
    virtual void OnMessageTouchdown(float fHeight);
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc);
    virtual CHRTYPE GetAttackCharacterType(void);
};