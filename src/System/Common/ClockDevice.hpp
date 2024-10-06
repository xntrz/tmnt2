#pragma once


class CTimeObj;


class IClockDevice
{
public:
    virtual ~IClockDevice(void) {};
    virtual void GetDateTime(CTimeObj& timeobj) = 0;
};