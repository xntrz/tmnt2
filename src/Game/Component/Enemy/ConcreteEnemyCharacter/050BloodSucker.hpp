#pragma once

#include "BaseBatChr.hpp"


class C050BloodSucker : public CBaseBatChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C050BloodSucker(void);
    virtual ~C050BloodSucker(void);
};