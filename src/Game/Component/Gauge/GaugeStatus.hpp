#pragma once


class CGaugeStatus
{
public:
    enum EX_GAUGE_TYPE
    {
        EX_GAUGE_ZAKO_FUGITOID_LIVE = 0,
        EX_GAUGE_ZAKO_COUNT_BANDIT,
        EX_GAUGE_ZAKO_COUNT_FOOT,
        EX_GAUGE_ZAKO_COUNT_MAFIA,
        EX_GAUGE_ZAKO_NUM,
    };
    
public:
    static void Initialize(void);
    static void Termiante(void);
    static void Period(void);
    static void Draw(void);
    static void TimerDispEnable(bool bEnable);
    static void TimerDispSet(int32 no, float fValue);
    static void CounterDispEnable(bool bEnable);
    static void CounterDispSet(int32 no, int32 value);
    static void ExGaugeDispEnable(EX_GAUGE_TYPE type, bool bEnable);
    static void ExGaugeDispInit(EX_GAUGE_TYPE type, int32 value);
    static void ExGaugeDispSet(int32 no, EX_GAUGE_TYPE type, int32 value);
};