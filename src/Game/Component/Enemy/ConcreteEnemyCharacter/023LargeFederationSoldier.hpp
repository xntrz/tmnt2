#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C023LargeFederationSoldier : public CBaseGrapplerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C023LargeFederationSoldier(void);
    virtual ~C023LargeFederationSoldier(void);

private:
    CSpecialityConsider m_specialityConsider;
};