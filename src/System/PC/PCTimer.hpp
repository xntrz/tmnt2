#pragma once


class CPCTimer
{
public:
    class IGetTimeFunctor
    {
    public:
        virtual uint32 operator()(void) const = 0;
    };
    
public:
    static CPCTimer& Instance(void);

    CPCTimer(void);
    virtual ~CPCTimer(void);
    uint32 ElapsedTime(void) const;
    uint32 GranularitySecond(void) const;
    uint32 GranularityMillisecond(void) const;
    
private:
    static CPCTimer* m_pInstance;
    uint32 m_uTimeStart;
    uint32 m_uGranularitySecond;
    uint32 m_uGranularityMillisecond;
    IGetTimeFunctor* m_pGetTimerFunctor;
};