#pragma once

#include "BaseGunnerEnemyChr.hpp"


class C010Mobster : public CBaseGunnerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C010Mobster(void);
    virtual ~C010Mobster(void);

private:
    CSpecialityConsider m_specialityConsider;
};