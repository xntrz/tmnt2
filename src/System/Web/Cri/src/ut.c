#include "ut.h"

#include <stdio.h>
#include <stdarg.h>

#if defined(CRI_TARGET_WIN)
    #include <windows.h>
#elif defined(CRI_TARGET_WEB)
    #include <emscripten.h>
#endif


CriChar8 __ut_c_cristr[1024];


void UT_OsSleep(CriUint32 ms)
{
#if defined(CRI_TARGET_WIN)
    Sleep((DWORD)ms);
#elif defined(CRI_TARGET_WEB)
    emscripten_sleep((unsigned int)ms);
#endif
};


void UT_OsIdle(void)
{
    UT_OsSleep(0);
};


void UT_Sprintf(CriChar8* buf, const CriChar8* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    vsprintf(buf, fmt, vl);
    va_end(vl);
};