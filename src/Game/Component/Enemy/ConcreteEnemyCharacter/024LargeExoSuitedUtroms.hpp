#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C024LargeExoSuitedUtroms : public CBaseGrapplerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C024LargeExoSuitedUtroms(void);
    virtual ~C024LargeExoSuitedUtroms(void);

private:
    CSpecialityConsider m_specialityConsider;
};