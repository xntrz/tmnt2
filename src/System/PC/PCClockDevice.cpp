#include "PCClockDevice.hpp"
#include "PCTypedefs.hpp"

#include "System/Common/SystemTime.hpp"


void CPCClockDevice::GetDateTime(CTimeObj& timeobj)
{
    SYSTEMTIME SystemTime = { 0 };
    GetLocalTime(&SystemTime);
    
    timeobj.Init(
        int32(SystemTime.wYear),
        int32(SystemTime.wMonth),
        int32(SystemTime.wDay),
        int32(SystemTime.wHour),
        int32(SystemTime.wMinute),
        int32(SystemTime.wSecond)
    );
};