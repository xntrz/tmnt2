#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C020LargeMobster : public CBaseGrapplerEnemyChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);
    
    C020LargeMobster(void);
    virtual ~C020LargeMobster(void);

private:
    CSpecialityConsider m_specialityConsider;
};