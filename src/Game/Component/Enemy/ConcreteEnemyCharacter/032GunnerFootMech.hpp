#pragma once

#include "BaseGunnerEnemyChr.hpp"


class C032GunnerFootMech : public CBaseGunnerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C032GunnerFootMech(void);
    virtual ~C032GunnerFootMech(void);

private:
    CSpecialityConsider m_specialityConsider;
};