#pragma once

#include "System/Common/ClockDevice.hpp"


class CWebClockDevice final : public IClockDevice
{
public:
    virtual void GetDateTime(CTimeObj& timeobj) override;
};