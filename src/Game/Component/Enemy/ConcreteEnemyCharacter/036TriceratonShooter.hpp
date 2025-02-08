#pragma once

#include "BaseGunnerEnemyChr.hpp"


class C036TriceratonShooter : public CBaseGunnerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C036TriceratonShooter(void);
    virtual ~C036TriceratonShooter(void);

private:
    CSpecialityConsider m_specialityConsider;
};