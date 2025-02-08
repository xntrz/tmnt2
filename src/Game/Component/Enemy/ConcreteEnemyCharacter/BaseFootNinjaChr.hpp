#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class CBaseFootNinjaChr : public CBaseGrapplerEnemyChr
{
public:
    class CBackAwayStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CJAttackStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

public:
    CBaseFootNinjaChr(ENEMYID::VALUE idEnemy);
    virtual ~CBaseFootNinjaChr(void);
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
};