#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C000Dummy : public CBaseGrapplerEnemyChr
{
public:
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_ALL_DASH2,
                            EFFECTID::ID_KO_FLASH });

    C000Dummy(void);
    virtual ~C000Dummy(void);

private:
    CSpecialityConsider m_specialityConsider;
};