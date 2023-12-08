#include "PCDebug.hpp"
#include "PCSpecific.hpp"
#include "PCSystem.hpp"

#include "System/Common/SystemTime.hpp"
#include "System/Common/Debug.hpp"


#define PCDEBUG_FLAG_FATAL_RET  (1 << 0)
#define PCDEBUG_FLAG_DISP_TFL   (1 << 1)    // t-ime f-ile l-ine


static FILE* s_pStdIn = nullptr;
static FILE* s_pStdOut = nullptr;
static FILE* s_pStdErr = nullptr;
static HWND s_hWndConsole = NULL;
static uint32 s_uOptFlag = 0;


static void OutputCommon(bool ln, const char* fname, int32 fline, const char* format, va_list& vl)
{
    char szOutputBuffer[4096 * 16];
    szOutputBuffer[0] = '\0';
    
    SYSTEMTIME st = { 0 };
    GetLocalTime(&st);

    int32 offset = 0;

    if (FLAG_TEST(s_uOptFlag, PCDEBUG_FLAG_DISP_TFL))
    {
        offset = std::sprintf(
            szOutputBuffer,
            "[%02d:%02d:%02d.%03.0f][%s::%d]: ",
            st.wHour, st.wMinute, st.wSecond, float(st.wMilliseconds), fname, fline
        );
    };
    
    int32 Result = std::vsprintf(&szOutputBuffer[offset], format, vl);
    if (ln)
    {
        if (Result > 0)
        {
            offset += Result;
            std::sprintf(&szOutputBuffer[offset], "\n");
        };
    };

    std::printf("%s", szOutputBuffer);
    OutputDebugStringA(szOutputBuffer);
};


/*static*/ void CPCDebug::Initialize(void)
{    
    AllocConsole();
    AttachConsole(GetCurrentProcessId());

    freopen_s(&s_pStdIn, "CON", "r", stdin);
    freopen_s(&s_pStdOut, "CON", "w", stdout);
    freopen_s(&s_pStdErr, "CON", "w", stderr);

    s_hWndConsole = GetConsoleWindow();
    
    CDebug::Initialize();
    CDebug::Output  = CPCDebug::Output;
    CDebug::Fatal   = CPCDebug::Fatal;

    SetDispTimeFileLine(true);
    SetFatalReturn(false);
};


/*static*/ void CPCDebug::Terminate(void)
{
    CDebug::Fatal   = nullptr;
    CDebug::Output  = nullptr;
    CDebug::Terminate();

    std::fclose(s_pStdIn);
    std::fclose(s_pStdOut);
    std::fclose(s_pStdErr);

    FreeConsole();
    PostMessage(s_hWndConsole, WM_CLOSE, 0, 0);

    s_pStdIn = s_pStdOut = s_pStdErr = nullptr;
    s_hWndConsole = NULL;
};


/*static*/ void CPCDebug::Output(const char* fname, int32 fline, const char* format, ...)
{
    va_list vl;
    va_start(vl, format);
    OutputCommon(false, fname, fline, format, vl);
    va_end(vl);
};


/*static*/ void CPCDebug::Fatal(const char* reason, ...)
{
    static char szFatalBuffer[4096];    
    szFatalBuffer[0] = '\0';

    std::sprintf(szFatalBuffer, "%s\n\n", reason);
    
    uint32 Flags = (MB_ICONERROR);
#ifdef _DEBUG    
    std::strcat(szFatalBuffer, "Press OK to execute debugbreak or CANCEL to terminate program.");
    Flags |= (MB_OKCANCEL | MB_DEFBUTTON2);
#else
    Flags |= (MB_OK | MB_SYSTEMMODAL);
#endif    
    
    SetWindowPos(CPCSpecific::m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    CPCSpecific::DisplayCursor(true);

    int32 iResult = MessageBoxA(NULL, szFatalBuffer, CPCSystem::WNDNAME, Flags);
#ifdef _DEBUG    
    if (iResult == IDOK)
		__debugbreak();
#else
    (void)iResult;
#endif    
    
    if (!FLAG_TEST(s_uOptFlag, PCDEBUG_FLAG_FATAL_RET))
        TerminateProcess(GetCurrentProcess(), 0xFFFFFFFF);
    else
        CPCSpecific::DisplayCursor(false);
};


/*static*/ void CPCDebug::SetDispTimeFileLine(bool flag)
{
    if (flag)
        FLAG_SET(s_uOptFlag, PCDEBUG_FLAG_DISP_TFL);
    else
        FLAG_CLEAR(s_uOptFlag, PCDEBUG_FLAG_DISP_TFL);
};


/*static*/ void CPCDebug::SetFatalReturn(bool flag)
{
    if (flag)
        FLAG_SET(s_uOptFlag, PCDEBUG_FLAG_FATAL_RET);
    else
        FLAG_CLEAR(s_uOptFlag, PCDEBUG_FLAG_FATAL_RET);
};