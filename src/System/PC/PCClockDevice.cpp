#include "PCClockDevice.hpp"
#include "PCTypedefs.hpp"

#include "System/Common/SystemTime.hpp"


void CPCClockDevice::GetDateTime(CTimeObj& timeobj)
{
    SYSTEMTIME sysTime = {};
    GetLocalTime(&sysTime);
    
    timeobj.Init(static_cast<int32>(sysTime.wYear),
                 static_cast<int32>(sysTime.wMonth),
                 static_cast<int32>(sysTime.wDay),
                 static_cast<int32>(sysTime.wHour),
                 static_cast<int32>(sysTime.wMinute),
                 static_cast<int32>(sysTime.wSecond));
};