#pragma once


class CTimeSpanObj
{
public:
    CTimeSpanObj(void);
    CTimeSpanObj(uint32 timevalue);
    CTimeSpanObj(int32 totaldays, int32 hours, int32 minutes, int32 seconds);
    ~CTimeSpanObj(void);
    void Init(uint32 timevalue);
    void Init(int32 totaldays, int32 hours, int32 minutes, int32 seconds);
    int32 GetDays(void) const;
    int32 GetHours(void) const;
    int32 GetMinutes(void) const;
    int32 GetSeconds(void) const;
    uint32 GetTimevalue(void) const;

private:
    uint32 m_timevalue;
    int32 m_nTotalDays;
    int32 m_nHours;
    int32 m_nMinutes;
    int32 m_nSeconds;
};


class CTimeObj
{
public:
    CTimeObj(void);
    CTimeObj(uint32 timevalue);
    CTimeObj(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 seconds);
    ~CTimeObj(void);
    void Init(uint32 timevalue);
    void Init(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 seconds);
    int32 GetYear(void) const;
    int32 GetMonth(void) const;
    int32 GetDay(void) const;
    int32 GetHour(void) const;
    int32 GetMinute(void) const;
    int32 GetSeconds(void) const;
    uint32 GetTimevalue(void) const;
    static CTimeSpanObj CalcSpan(const CTimeObj& a, const CTimeObj& b);

private:
    bool isLeapYear(int32 year) const;
    int32 getDaysAtMonth(int32 year, int32 month) const;
    int32 getDaysAtYear(int32 year) const;

private:
    static const int32 m_nBaseYear;
    uint32 m_timevalue;
    int32 m_nTotalDays;
    int16 m_nYear;
    int16 m_nMonth;
    int8 m_bDay;
    int8 m_bHour;
    int8 m_bMinutes;
    int8 m_bSeconds;
};


class IClockDevice;


class CSystemTime
{
public:
    static CSystemTime& Instance(void);
    
    CSystemTime(void);
    void AttachClock(IClockDevice* pIClockDevice);
    void DetachClock(void);
    CTimeObj GetLocalTime(void);

private:
    IClockDevice* m_pIClockDevice;
    CTimeObj m_time;
};