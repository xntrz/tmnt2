#include "WebDebug.hpp"
#include "WebSpecific.hpp"
#include "WebMsgBox.hpp"

#include "System/Common/Debug.hpp"

#include <emscripten.h>


#define WEBDEBUG_FLAG_FATAL_RET  (1 << 0)
#define WEBDEBUG_FLAG_DISP_TFL   (1 << 1)    // t-ime f-ile l-ine


static uint32 s_uOptFlag = 0;


static inline void
SetOptFlag(uint32 flag, bool state)
{
    if (state)
        s_uOptFlag |= flag;
    else
        s_uOptFlag &= (~flag);
};


static inline bool
TestOptFlag(uint32 flag)
{
    return ((s_uOptFlag & flag) == flag);
};


static void
OutputCommon(const char* fname, int32 fline, const char* format, va_list& vl)
{
    char szOutputBuffer[4096];
    szOutputBuffer[0] = '\0';

    CWebSpecific::SYSTEMTIME st = {};
    CWebSpecific::GetLocalTime(&st);

    int32 offset = 0;

    if (TestOptFlag(WEBDEBUG_FLAG_DISP_TFL))
    {
        offset = std::sprintf(szOutputBuffer,
                              "[%02d:%02d:%02d][%s::%d]: ",
                              st.hours, st.minutes, st.seconds, fname, fline);
    };

    std::vsprintf(&szOutputBuffer[offset], format, vl);
    std::printf("%s", szOutputBuffer);
};


/*static*/ void CWebDebug::Initialize(void)
{
    CDebug::Initialize();
    CDebug::Output = CWebDebug::Output;
    CDebug::Fatal  = CWebDebug::Fatal;

    SetDispTimeFileLine(true);
    SetFatalReturn(false);
};


/*static*/ void CWebDebug::Terminate(void)
{
    CDebug::Fatal = nullptr;
    CDebug::Output = nullptr;
    CDebug::Terminate();
};


/*static*/ void CWebDebug::Output(const char* fname, int32 fline, const char* format, ...)
{
    va_list vl;
    va_start(vl, format);
    OutputCommon(fname, fline, format, vl);
    va_end(vl);
};


/*static*/ void CWebDebug::Fatal(const char* reason, ...)
{
    static char szFatalBuffer[4096];
    szFatalBuffer[0] = '\0';

    va_list vl;
    va_start(vl, reason);
    std::vsprintf(szFatalBuffer, reason, vl);
    va_end(vl);

    emscripten_log(EM_LOG_CONSOLE | EM_LOG_JS_STACK, "stacktrace:");

    CWebMsgBox::Invoke(CWebMsgBox::TYPE_OK, nullptr, szFatalBuffer);
    
    if (!TestOptFlag(WEBDEBUG_FLAG_FATAL_RET))
    {
        std::fflush(stdout);
        std::fflush(stderr);

        emscripten_force_exit(-1);
    };
};


/*static*/ void CWebDebug::SetDispTimeFileLine(bool state)
{
    SetOptFlag(WEBDEBUG_FLAG_DISP_TFL, state);
};


/*static*/ void CWebDebug::SetFatalReturn(bool state)
{
    SetOptFlag(WEBDEBUG_FLAG_FATAL_RET, state);
};