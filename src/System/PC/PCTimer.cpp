#include "PCTimer.hpp"
#include "PCTypedefs.hpp"


class CPCTimer::IGetTimeFunctor
{
public:
    virtual uint32 operator()(void) const = 0;
};


class CPCTimer::CGetTimeFunctorOS final : public CPCTimer::IGetTimeFunctor
{
public:
    CGetTimeFunctorOS(void)
    {
        timeBeginPeriod(1);
    };

    
    virtual ~CGetTimeFunctorOS(void)
    {
        timeEndPeriod(1);
    };

    
    virtual uint32 operator()(void) const override
    {
        return timeGetTime();
    };
};


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

    m_uTimeStart = end;
    m_uGranularityMillisecond = end - begin;
    m_uGranularitySecond = (end - begin) / 1000;
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


uint32 CPCTimer::ElapsedTime(void) const
{
    return ((*m_pGetTimerFunctor)() - m_uTimeStart);
};


uint32 CPCTimer::GranularitySecond(void) const
{
    return m_uGranularitySecond;
};


uint32 CPCTimer::GranularityMillisecond(void) const
{
    return m_uGranularityMillisecond;
};