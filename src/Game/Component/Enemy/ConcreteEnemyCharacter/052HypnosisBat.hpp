#pragma once

#include "BaseBatChr.hpp"


class C052HypnosisBat : public CBaseBatChr
{
public:
    class CAttackSleepStatusObserver : public CBaseFlyingEnemyChr::CBaseFlyingStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C052HypnosisBat(void);
    virtual ~C052HypnosisBat(void);
};