#pragma once

#include "BaseFootNinjaChr.hpp"


class C005FeudalFootNinja : public CBaseFootNinjaChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C005FeudalFootNinja(void);
    virtual ~C005FeudalFootNinja(void);

private:
    CSpecialityConsider m_specialityConsider;
};