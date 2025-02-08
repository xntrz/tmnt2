#pragma once

#include "BaseGunnerEnemyChr.hpp"


class C015CaptainOfFederation : public CBaseGunnerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C015CaptainOfFederation(void);
    virtual ~C015CaptainOfFederation(void);

private:
    CSpecialityConsider m_specialityConsider;
};