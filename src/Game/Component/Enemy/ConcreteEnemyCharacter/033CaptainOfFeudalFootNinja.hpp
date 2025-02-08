#pragma once

#include "BaseFootNinjaChr.hpp"


class C033CaptainOfFeudalFootNinja : public CBaseFootNinjaChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C033CaptainOfFeudalFootNinja(void);
    virtual ~C033CaptainOfFeudalFootNinja(void);

private:
    CSpecialityConsider m_specialityConsider;
};