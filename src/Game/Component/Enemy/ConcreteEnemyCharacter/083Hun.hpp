#pragma once

#include "BaseHunChr.hpp"


class C083Hun : public CBaseHunChr
{
public:
    C083Hun(void);
    virtual ~C083Hun(void);
    virtual void OnMessageReceivedDamage(int32 iAmount) override;
};