#include "SystemTime.hpp"
#include "ClockDevice.hpp"


CHECK_SIZE(CTimeObj,    0x10);
CHECK_SIZE(CTimeSpanObj,0x14);
CHECK_SIZE(CSystemTime, 0x14);


CTimeSpanObj::CTimeSpanObj(void)
: m_timevalue(0)
, m_nTotalDays(0)
, m_nHours(0)
, m_nMinutes(0)
, m_nSeconds(0)
{
    ;
};


CTimeSpanObj::CTimeSpanObj(uint32 timevalue)
: m_timevalue(0)
, m_nTotalDays(0)
, m_nHours(0)
, m_nMinutes(0)
, m_nSeconds(0)
{
    Init(timevalue);
};


CTimeSpanObj::CTimeSpanObj(int32 totaldays, int32 hours, int32 minutes, int32 seconds)
: m_timevalue(0)
, m_nTotalDays(0)
, m_nHours(0)
, m_nMinutes(0)
, m_nSeconds(0)
{
    Init(totaldays, hours, minutes, seconds);
};


CTimeSpanObj::~CTimeSpanObj(void)
{
    ;
};


void CTimeSpanObj::Init(uint32 timevalue)
{
    m_timevalue = timevalue;
    m_nSeconds  = m_timevalue % 60;
    m_nMinutes  = m_timevalue / 60 % 60;
    m_nHours    = m_timevalue / 60 / 60 % 24;
    m_nTotalDays= m_timevalue / 60 / 60 / 24;
};


void CTimeSpanObj::Init(int32 totaldays, int32 hours, int32 minutes, int32 seconds)
{
    ASSERT(hours > -24 && hours < 24);
    ASSERT(minutes > -60 && minutes < 60);
    ASSERT(seconds > -60 && seconds < 60);

    m_nTotalDays= totaldays;
    m_nSeconds  = seconds;
    m_nHours    = hours;
    m_nMinutes  = minutes;
    m_timevalue = seconds + 60 * (minutes + 60 * (hours + 24 * totaldays));
};


int32 CTimeSpanObj::GetDays(void) const
{
    return m_nTotalDays;
};


int32 CTimeSpanObj::GetHours(void) const
{
    return m_nHours;
};


int32 CTimeSpanObj::GetMinutes(void) const
{
    return m_nMinutes;
};


int32 CTimeSpanObj::GetSeconds(void) const
{
    return m_nSeconds;
};


uint32 CTimeSpanObj::GetTimevalue(void) const
{
    return m_timevalue;
};


/*static*/ const int32 CTimeObj::m_nBaseYear = 2000;


CTimeObj::CTimeObj(void)
: m_timevalue(0)
, m_nTotalDays(0)
, m_nYear(0)
, m_nMonth(0)
, m_bDay(0)
, m_bHour(0)
, m_bMinutes(0)
, m_bSeconds(0)
{
    ;
};


CTimeObj::CTimeObj(uint32 timevalue)
: m_timevalue(0)
, m_nTotalDays(0)
, m_nYear(0)
, m_nMonth(0)
, m_bDay(0)
, m_bHour(0)
, m_bMinutes(0)
, m_bSeconds(0)
{
    Init(timevalue);
};


CTimeObj::CTimeObj(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 seconds)
: m_timevalue(0)
, m_nTotalDays(0)
, m_nYear(0)
, m_nMonth(0)
, m_bDay(0)
, m_bHour(0)
, m_bMinutes(0)
, m_bSeconds(0)
{
    Init(year, month, day, hour, minute, seconds);
};


CTimeObj::~CTimeObj(void)
{
    ;
};


void CTimeObj::Init(uint32 timevalue)
{
    m_timevalue = timevalue;
    
    CTimeSpanObj span(m_timevalue);    
    m_nTotalDays= span.GetDays();
    m_nYear     = m_nBaseYear;
    m_nMonth    = 1;
    m_bDay      = 1;
	m_bHour		= int8(span.GetHours());
	m_bMinutes	= int8(span.GetMinutes());
	m_bSeconds	= int8(span.GetSeconds());

    int32 nTotalDays = m_nTotalDays;
    ASSERT(nTotalDays);

    while (getDaysAtYear(m_nYear) <= nTotalDays)
        nTotalDays -= getDaysAtYear(m_nYear++);

    while (getDaysAtMonth(m_nYear, m_nMonth) <= nTotalDays)
        nTotalDays -= getDaysAtMonth(m_nYear, m_nMonth++);

	m_bDay = int8(nTotalDays + 1);
};


void CTimeObj::Init(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 seconds)
{
    ASSERT(year >= m_nBaseYear);
    
	m_nYear			= int16(year);
	m_nMonth		= int16(month);
	m_bDay			= int8(day);
	m_bHour			= int8(hour);
	m_bMinutes		= int8(minute);
	m_bSeconds		= int8(seconds);
	m_nTotalDays	= int32(m_bDay - 1);

    for (int32 nMonth = 1; nMonth < m_nMonth; ++nMonth)
        m_nTotalDays += getDaysAtMonth(m_nYear, nMonth);
    
    for (int32 nYear = m_nBaseYear; nYear < m_nYear; ++nYear)
        m_nTotalDays += getDaysAtYear(nYear);

    m_timevalue = CTimeSpanObj(m_nTotalDays, m_bHour, m_bMinutes, m_bSeconds).GetTimevalue();
};


int32 CTimeObj::GetYear(void) const
{
    return m_nYear;
};


int32 CTimeObj::GetMonth(void) const
{
    return m_nMonth;
};


int32 CTimeObj::GetDay(void) const
{
    return m_bDay;
};


int32 CTimeObj::GetHour(void) const
{
    return m_bHour;
};


int32 CTimeObj::GetMinute(void) const
{
    return m_bMinutes;
};


int32 CTimeObj::GetSeconds(void) const
{
    return m_bSeconds;
};


uint32 CTimeObj::GetTimevalue(void) const
{
    return m_timevalue;
};


/*static*/ CTimeSpanObj CTimeObj::CalcSpan(const CTimeObj& a, const CTimeObj& b)
{
    return CTimeSpanObj(a.GetTimevalue() - b.GetTimevalue());    
};


bool CTimeObj::isLeapYear(int32 year) const
{
    return !(year % 4) && (year % 100 || !(year % 400));    
};


int32 CTimeObj::getDaysAtMonth(int32 year, int32 month) const
{
    static const int32 anDaysOfMonth[] =
    {
        0,
        31,
        28,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31,
    };

    if (month == 2 && isLeapYear(year))
        return anDaysOfMonth[month] + 1;
    else
        return anDaysOfMonth[month];
};


int32 CTimeObj::getDaysAtYear(int32 year) const
{
    if (!isLeapYear(year))
        return 365;
    else
        return 366;
};


/*static*/ CSystemTime& CSystemTime::Instance(void)
{
    static CSystemTime s_theSystemTime;
    return s_theSystemTime;
};


CSystemTime::CSystemTime(void)
: m_pIClockDevice(nullptr)
, m_time()
{
    ;
};


void CSystemTime::AttachClock(IClockDevice* pIClockDevice)
{
    m_pIClockDevice = pIClockDevice;
};


void CSystemTime::DetachClock(void)
{
    m_pIClockDevice = nullptr;
};


CTimeObj CSystemTime::GetLocalTime(void)
{
    if (m_pIClockDevice)
        m_pIClockDevice->GetDateTime(m_time);
    
    return m_time;
};

