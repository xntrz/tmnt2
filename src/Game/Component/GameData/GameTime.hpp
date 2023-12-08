#pragma once


#define GAMETIME_HMS(h, m, s) uint32((((s) + (m) * 60) + (h) * 3600))


class CGameTime
{
public:
    CGameTime(uint32 uTotalTime);
    CGameTime(void);
    ~CGameTime(void);
    void Init(uint32 uTotalTime);
    void Clear(void);
    void Add(const CGameTime& Gametime);
    uint32 GetTotalSecond(void) const;
    uint32 GetHour(void) const;
    uint32 GetMinute(void) const;
    uint32 GetSecond(void) const;
    bool operator<(const CGameTime& Gametime) const;
    bool operator>(const CGameTime& Gametime) const;
    bool operator<=(const CGameTime& Gametime) const;
    bool operator>=(const CGameTime& Gametime) const;
    bool operator!=(const CGameTime& Gametime) const;
    bool operator==(const CGameTime& Gametime) const;

private:
    void clamp(void);
    
private:
    uint32 m_uTotalTime;
};