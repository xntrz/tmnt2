#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C001PurpleDragonGang : public CBaseGrapplerEnemyChr
{
public:
    class CAttackGrenadeStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C001PurpleDragonGang(void);
    virtual ~C001PurpleDragonGang(void);

private:
    CSpecialityConsider m_specialityConsider;
};