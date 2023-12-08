#pragma once


class CPCTimer
{
private:
    class IGetTimeFunctor;
    class CGetTimeFunctorOS;
    class CGetTimeFunctorCPU;
    
public:
    static CPCTimer& Instance(void);

    CPCTimer(void);
    virtual ~CPCTimer(void);
    uint32 ElapsedTime(void) const;

    inline uint32 GranularitySecond(void) const { return m_uGranularitySecond; };
    inline uint32 GranularityMillisecond(void) const { return m_uGranularityMillisecond; };
    
private:
    static CPCTimer* m_pInstance;
    uint32 m_uTimeStart;
    uint32 m_uGranularitySecond;
    uint32 m_uGranularityMillisecond;
    IGetTimeFunctor* m_pGetTimerFunctor;
};