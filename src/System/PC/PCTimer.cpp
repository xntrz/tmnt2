#include "PCTimer.hpp"
#include "PCTypedefs.hpp"

#include <timeapi.h>
#include <intrin.h>


class CPCTimer::IGetTimeFunctor
{
public:
    inline virtual ~IGetTimeFunctor(void) {};
    virtual uint64 operator()(void) const = 0;
};


class CPCTimer::CGetTimeFunctorOS final : public CPCTimer::IGetTimeFunctor
{
public:
    CGetTimeFunctorOS(void);
    virtual ~CGetTimeFunctorOS(void);
    virtual uint64 operator()(void) const override;
};


class CPCTimer::CGetTimeFunctorCPU final : public CPCTimer::IGetTimeFunctor
{
public:
    static bool IsAvailable(void);

    CGetTimeFunctorCPU(void);
    virtual ~CGetTimeFunctorCPU(void);
    virtual uint64 operator()(void) const override;
};


class CPCTimer::CGetTimeFunctorQPS final : public CPCTimer::IGetTimeFunctor
{
public:
    static bool IsAvailable(void);
    static uint64 GetFrequency(void);

    CGetTimeFunctorQPS(void);
    virtual ~CGetTimeFunctorQPS(void);
    virtual uint64 operator()(void) const override;

private:
    static LARGE_INTEGER m_freq;
};


//
// *********************************************************************************
//


CPCTimer::CGetTimeFunctorOS::CGetTimeFunctorOS(void)
{
    timeBeginPeriod(1);
};


CPCTimer::CGetTimeFunctorOS::~CGetTimeFunctorOS(void)
{
    timeEndPeriod(1);
};


uint64 CPCTimer::CGetTimeFunctorOS::operator()(void) const
{
    return static_cast<uint32>(timeGetTime());
};


//
// *********************************************************************************
//


/*static*/ bool CPCTimer::CGetTimeFunctorCPU::IsAvailable(void)
{
    int Registers[4] = {}; // EAX, EBX, ECX, EDX
    int Function = 1;      // EAX=1: Processor Info and Feature Bits

    __cpuid(Registers, Function);

    if ((Registers[3] & (1 << 4)) != 0) //  EDX bit 4 - tsc - Time Stamp Counter and RDTSC instruction
        return true;

    return false;
};


CPCTimer::CGetTimeFunctorCPU::CGetTimeFunctorCPU(void)
{
    ;
};


CPCTimer::CGetTimeFunctorCPU::~CGetTimeFunctorCPU(void)
{
    ;
};


uint64 CPCTimer::CGetTimeFunctorCPU::operator()(void) const
{
    return static_cast<uint64>(__rdtsc());
};


//
// *********************************************************************************
//


/*static*/ LARGE_INTEGER CPCTimer::CGetTimeFunctorQPS::m_freq = {};


/*static*/ bool CPCTimer::CGetTimeFunctorQPS::IsAvailable(void)
{    
    if (QueryPerformanceFrequency(&m_freq))
        return true;

    return false;
};


/*static*/ uint64 CPCTimer::CGetTimeFunctorQPS::GetFrequency(void)
{
    return static_cast<uint64>(m_freq.QuadPart);
};


CPCTimer::CGetTimeFunctorQPS::CGetTimeFunctorQPS(void)
{
    ;
};


CPCTimer::CGetTimeFunctorQPS::~CGetTimeFunctorQPS(void)
{
    ;
};


uint64 CPCTimer::CGetTimeFunctorQPS::operator()(void) const
{
    LARGE_INTEGER counter = {};
    QueryPerformanceCounter(&counter);

    return static_cast<uint64>(counter.QuadPart);
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
: m_startTime(0)
, m_ticksPerMSec(0)
, m_ticksPerSec(0)
, m_pGetTimerFunctor(nullptr)
, m_bIsUseQPS(false)
{
    m_pInstance = this;

    if (CGetTimeFunctorQPS::IsAvailable())
    {
        m_pGetTimerFunctor = new CGetTimeFunctorQPS;
        m_bIsUseQPS = true;
    }
    else if (CGetTimeFunctorCPU::IsAvailable())
    {
        m_pGetTimerFunctor = new CGetTimeFunctorCPU;
    }
    else
    {
        m_pGetTimerFunctor = new CGetTimeFunctorOS;
    };

    HANDLE hCurrentThread = GetCurrentThread();
    DWORD dwCurrentThreadPriority = GetThreadPriority(hCurrentThread);
    SetThreadPriority(hCurrentThread, THREAD_PRIORITY_TIME_CRITICAL);
    
    DWORD time = timeGetTime();
    DWORD timeNow = 0;
    do
    {
        timeNow = timeGetTime();
    } while (time == timeNow);

    uint64 begin = (*m_pGetTimerFunctor)();
    while ((timeGetTime() - timeNow) < 1000)
        ;    
    uint64 end = (*m_pGetTimerFunctor)();

    m_startTime = end;
    m_ticksPerSec = (m_bIsUseQPS ? CGetTimeFunctorQPS::GetFrequency() : (end - begin));
    m_ticksPerMSec = (m_ticksPerSec / 1000);

    SetThreadPriority(hCurrentThread, dwCurrentThreadPriority);
};


CPCTimer::~CPCTimer(void)
{
    if (m_pGetTimerFunctor)
    {
        delete m_pGetTimerFunctor;
        m_pGetTimerFunctor = nullptr;
    };

    m_pInstance = nullptr;
};


uint64 CPCTimer::GetElapsedTime(void) const
{
    return ((*m_pGetTimerFunctor)() - m_startTime);
};


uint64 CPCTimer::GetFreqMs(void) const
{
    return m_ticksPerMSec;
};


uint64 CPCTimer::GetFreq(void) const
{
    return m_ticksPerSec;
};