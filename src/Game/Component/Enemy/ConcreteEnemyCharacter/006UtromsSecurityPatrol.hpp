#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C006UtromsSecurityPatrol : public CBaseGrapplerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C006UtromsSecurityPatrol(void);
    virtual ~C006UtromsSecurityPatrol(void);

private:
    CSpecialityConsider m_specialityConsider;
};