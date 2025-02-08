#pragma once

#include "BaseGunnerEnemyChr.hpp"


class C013FederationShooter : public CBaseGunnerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C013FederationShooter(void);
    virtual ~C013FederationShooter(void);

private:
    CSpecialityConsider m_specialityConsider;
};