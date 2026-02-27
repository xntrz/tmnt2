#include "PCTimer.hpp"
#include "PCTypedefs.hpp"

#include <timeapi.h>
#include <intrin.h>


class CPCTimer::IGetTimeFunctor
{
public:
    inline virtual ~IGetTimeFunctor(void) {};
    virtual uint32 operator()(void) const = 0;
};


class CPCTimer::CGetTimeFunctorOS final : public CPCTimer::IGetTimeFunctor
{
public:
    inline CGetTimeFunctorOS(void) { timeBeginPeriod(1); };
    inline virtual ~CGetTimeFunctorOS(void) { timeEndPeriod(1); };
    virtual uint32 operator()(void) const override { return static_cast<uint32>(timeGetTime()); };
};


class CPCTimer::CGetTimeFunctorCPU final : public CPCTimer::IGetTimeFunctor
{
public:
    inline CGetTimeFunctorCPU(void) {};
    inline virtual ~CGetTimeFunctorCPU(void) {};
    virtual uint32 operator()(void) const override { return static_cast<uint32>(__rdtsc()); };
};


//
// *********************************************************************************
//


/*static*/ CPCTimer* CPCTimer::m_pInstance = nullptr;


/*static*/ CPCTimer& CPCTimer::Instance(void)
{
    ASSERT(m_pInstance);
    return *m_pInstance;
};


CPCTimer::CPCTimer(void)
{
    ASSERT(!m_pInstance);
    m_pInstance = this;

    int Registers[4] = {};   // EAX, EBX, ECX, EDX
    int Function = 1;           // EAX=1: Processor Info and Feature Bits
    __cpuid(Registers, Function);

    if ((Registers[3] & (1 << 4)) != 0) //  EDX bit 4 - tsc - Time Stamp Counter and RDTSC instruction
        m_pGetTimerFunctor = new CGetTimeFunctorCPU;
    else
        m_pGetTimerFunctor = new CGetTimeFunctorOS;

    ASSERT(m_pGetTimerFunctor);

    HANDLE hCurrentThread = GetCurrentThread();
    DWORD dwCurrentThreadPriority = GetThreadPriority(hCurrentThread);
    SetThreadPriority(hCurrentThread, THREAD_PRIORITY_TIME_CRITICAL);
    
    uint32 time = timeGetTime();
    uint32 timeNow = 0;
    do
    {
        timeNow = timeGetTime();
    } while (time == timeNow);

    uint32 begin = (*m_pGetTimerFunctor)();
    
    while ((timeGetTime() - timeNow) < 1000)
        ;
    
    uint32 end = (*m_pGetTimerFunctor)();

    m_startTime = end;
    m_freq      = (end - begin);
    m_freqMs    = ((end - begin) / 1000);

    SetThreadPriority(hCurrentThread, dwCurrentThreadPriority);
};


CPCTimer::~CPCTimer(void)
{
    ASSERT(m_pInstance);
    ASSERT(m_pInstance == this);

    if (m_pGetTimerFunctor)
    {
        delete m_pGetTimerFunctor;
        m_pGetTimerFunctor = nullptr;
    };

    m_pInstance = nullptr;
};


uint32 CPCTimer::GetElapsedTime(void) const
{
    return ((*m_pGetTimerFunctor)() - m_startTime);
};


uint32 CPCTimer::GetFreqMs(void) const
{
    return m_freqMs;
};


uint32 CPCTimer::GetFreq(void) const
{
    return m_freq;
};