#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C021LargeFootNinja : public CBaseGrapplerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C021LargeFootNinja(void);
    virtual ~C021LargeFootNinja(void);

private:
    CSpecialityConsider m_specialityConsider;
};