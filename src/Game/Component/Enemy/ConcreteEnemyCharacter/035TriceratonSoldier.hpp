#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C035TriceratonSoldier : public CBaseGrapplerEnemyChr
{
public:
    class CAttackCStatusObserver : public CBaseGrapplerEnemyChr::CAttackStatusObserver
    {
    public:
        virtual void OnStart(void) override;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C035TriceratonSoldier(void);
    virtual ~C035TriceratonSoldier(void);

private:
    CSpecialityConsider m_specialityConsider;
};