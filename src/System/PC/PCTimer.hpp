#pragma once


class CPCTimer
{
private:
    class IGetTimeFunctor;
    class CGetTimeFunctorOS;
    class CGetTimeFunctorCPU;
    class CGetTimeFunctorQPS;

public:
    static CPCTimer& Instance(void);

    CPCTimer(void);
    virtual ~CPCTimer(void);
    uint64 GetElapsedTime(void) const;
    uint64 GetFreqMs(void) const;
    uint64 GetFreq(void) const;
    
private:
    static CPCTimer* m_pInstance;
    uint64           m_startTime;
    uint64           m_ticksPerMSec;
    uint64           m_ticksPerSec;
    IGetTimeFunctor* m_pGetTimerFunctor;
    bool             m_bIsUseQPS;
};