#pragma once

#include "System/Common/ClockDevice.hpp"


class CPCClockDevice final : public IClockDevice
{
public:
    virtual void GetDateTime(CTimeObj& timeobj) override;
};