#pragma once

#include "BaseFootNinjaChr.hpp"


class C002FootNinja : public CBaseFootNinjaChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C002FootNinja(void);
    virtual ~C002FootNinja(void);

private:
    CSpecialityConsider m_specialityConsider;
};