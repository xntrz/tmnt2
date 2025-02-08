#pragma once

#include "BaseFootNinjaChr.hpp"


class C003FootNinja : public CBaseFootNinjaChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C003FootNinja(void);
    virtual ~C003FootNinja(void);

private:
    CSpecialityConsider m_specialityConsider;
};