#include "WebClockDevice.hpp"
#include "WebSpecific.hpp"

#include "System/Common/SystemTime.hpp"


void CWebClockDevice::GetDateTime(CTimeObj& timeobj)
{
    CWebSpecific::SYSTEMTIME st = {};
    CWebSpecific::GetLocalTime(&st);
    
    timeobj.Init(st.year, st.month, st.day,
                 st.hours, st.minutes, st.seconds);
};