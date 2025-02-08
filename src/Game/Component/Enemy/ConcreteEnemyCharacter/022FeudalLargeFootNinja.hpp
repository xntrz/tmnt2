#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C022FeudalLargeFootNinja : public CBaseGrapplerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C022FeudalLargeFootNinja(void);
    virtual ~C022FeudalLargeFootNinja(void);

private:
    CSpecialityConsider m_specialityConsider;
};