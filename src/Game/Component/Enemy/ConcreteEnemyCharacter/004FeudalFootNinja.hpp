#pragma once

#include "BaseFootNinjaChr.hpp"


class C004FeudalFootNinja : public CBaseFootNinjaChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C004FeudalFootNinja(void);
    virtual ~C004FeudalFootNinja(void);

private:
    CSpecialityConsider m_specialityConsider;
};