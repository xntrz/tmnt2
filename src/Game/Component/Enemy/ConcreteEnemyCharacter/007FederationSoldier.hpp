#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C007FederationSoldier : public CBaseGrapplerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C007FederationSoldier(void);
    virtual ~C007FederationSoldier(void);

private:
    CSpecialityConsider m_specialityConsider;
};