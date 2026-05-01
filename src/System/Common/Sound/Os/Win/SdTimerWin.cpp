#include "SdTimer.hpp"
#include "SdIncWin.hpp"

#include <timeapi.h>


static LARGE_INTEGER SdQPCFreq;
static BOOL SdQPCAvailable = FALSE;


bool SdTimerInitialize(void)
{
    SdQPCAvailable = QueryPerformanceFrequency(&SdQPCFreq);
    if (!SdQPCAvailable)
        timeBeginPeriod(1);

    return true;
};


void SdTimerTerminate(void)
{
    if (!SdQPCAvailable)
        timeEndPeriod(1);

    SdQPCFreq.QuadPart = 0;
    SdQPCAvailable = FALSE;
};


double SdTimerGetNow(void)
{
    if (SdQPCAvailable)
    {
        LARGE_INTEGER Counter;
        QueryPerformanceCounter(&Counter);

        return (((double)Counter.QuadPart * 1000.0) / (double)SdQPCFreq.QuadPart);
    };

    return ((double)timeGetTime());
};