#pragma once

#include "BaseGunnerEnemyChr.hpp"


class C014ExoSuitedUtroms : public CBaseGunnerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);
    
    C014ExoSuitedUtroms(void);
    virtual ~C014ExoSuitedUtroms(void);

private:
    CSpecialityConsider m_specialityConsider;
};