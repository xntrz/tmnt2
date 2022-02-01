#include "GameTime.hpp"


CGameTime::CGameTime(uint32 uTotalTime)
{
    Init(uTotalTime);
};


CGameTime::CGameTime(void)
: m_uTotalTime(0)
{
    ;
};


CGameTime::~CGameTime(void)
{
    ;
};


void CGameTime::Init(uint32 uTotalTime)
{
    m_uTotalTime = uTotalTime;
    clamp();
};


void CGameTime::Clear(void)
{
    Init(0);
};


void CGameTime::Add(const CGameTime& Gametime)
{
    m_uTotalTime += Gametime.m_uTotalTime;
    clamp();
};


uint32 CGameTime::GetTotalSecond(void) const
{
    return m_uTotalTime;
};


uint32 CGameTime::GetHour(void) const
{
    return m_uTotalTime / 3600u;
};


uint32 CGameTime::GetMinute(void) const
{
    return (m_uTotalTime / 60u) % 60u;
};


uint32 CGameTime::GetSecond(void) const
{
    return m_uTotalTime % 60u;
};


bool CGameTime::operator<(const CGameTime& Gametime) const
{
    return (m_uTotalTime < Gametime.m_uTotalTime);
};


bool CGameTime::operator>(const CGameTime& Gametime) const
{
    return (m_uTotalTime > Gametime.m_uTotalTime);
};


bool CGameTime::operator<=(const CGameTime& Gametime) const
{
    return (m_uTotalTime <= Gametime.m_uTotalTime);
};


bool CGameTime::operator>=(const CGameTime& Gametime) const
{
    return (m_uTotalTime >= Gametime.m_uTotalTime);
};


bool CGameTime::operator!=(const CGameTime& Gametime) const
{
    return (m_uTotalTime != Gametime.m_uTotalTime);
};


bool CGameTime::operator==(const CGameTime& Gametime) const
{
    return (m_uTotalTime == Gametime.m_uTotalTime);
};


void CGameTime::clamp(void)
{
    static const uint32 HMS_MAX = GAMETIME_HMS(99, 59, 59);
    
    if (m_uTotalTime >= HMS_MAX)
        m_uTotalTime = HMS_MAX;
};

