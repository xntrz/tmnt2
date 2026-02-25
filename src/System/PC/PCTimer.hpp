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
    uint32 GetElapsedTime(void) const;
    uint32 GetFreqMs(void) const;
    uint32 GetFreq(void) const;
    
private:
    static CPCTimer* m_pInstance;
    uint32           m_startTime;
    uint32           m_freqMs;
    uint32           m_freq;
    IGetTimeFunctor* m_pGetTimerFunctor;
};