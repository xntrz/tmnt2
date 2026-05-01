#include "SdTimer.hpp"

#include <emscripten.h>


bool SdTimerInitialize(void)
{
    return true;
};


void SdTimerTerminate(void)
{
    ;
};


double SdTimerGetNow(void)
{
    return emscripten_get_now();
};