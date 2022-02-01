#pragma once


class CTimeObj;


class IClockDevice
{
public:
    virtual void GetDateTime(CTimeObj& timeobj) = 0;
};