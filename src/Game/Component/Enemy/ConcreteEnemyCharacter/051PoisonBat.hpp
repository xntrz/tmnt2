#pragma once

#include "BaseBatChr.hpp"


class C051PoisonBat : public CBaseBatChr
{
public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C051PoisonBat(void);
    virtual ~C051PoisonBat(void);
};