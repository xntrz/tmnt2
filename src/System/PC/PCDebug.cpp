#include "PCDebug.hpp"
#include "PCMonitor.hpp"
#include "PCSetting.hpp"
#include "PCSpecific.hpp"
#include "PCUtils.hpp"

#include "System/Common/Debug.hpp"


static FILE* s_pStdIn = nullptr;
static FILE* s_pStdOut = nullptr;
static FILE* s_pStdErr = nullptr;
static HWND s_hWndConsole = NULL;


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


/*static*/ void CPCDebug::ReplaceConsole(void)
{
    if (CPCMonitor::GetNum() > 1)
    {
        int32 iMonitorNo = -1;

        if (CPCSetting::m_iMonitorNo == -1)
        {
            int32 iPrimaryMonitorNo = CPCMonitor::GetPrimary();
            iMonitorNo = Math::InvClamp(iPrimaryMonitorNo + 1, 0, CPCMonitor::GetNum() - 1);
        }
        else
        {
            iMonitorNo = Math::InvClamp(CPCSetting::m_iMonitorNo + 1, 0, CPCMonitor::GetNum() - 1);
        };

        RECT rc = { 0 };
        GetWindowRect(s_hWndConsole, &rc);

        int32 cxScreen = CPCMonitor::GetWidth(iMonitorNo);
        int32 cyScreen = CPCMonitor::GetHeight(iMonitorNo);

        int32 w = rc.right - rc.left;
        int32 h = rc.bottom - rc.top;
        int32 x = Math::Clamp((cxScreen - w) / 2, 0, cxScreen - w);
        int32 y = Math::Clamp((cyScreen - h) / 2, 0, cyScreen - h);

        SetWindowPos(
            s_hWndConsole,
            HWND_NOTOPMOST,
            CPCMonitor::GetOffsetX(iMonitorNo) + x,
            CPCMonitor::GetOffsetY(iMonitorNo) + y,
            w,
            h,
			SWP_NOACTIVATE
        );
    };
};


/*static*/ void CPCDebug::Output(const char* format, ...)
{
    static char szOutputBuffer[4096];    
    szOutputBuffer[0] = '\0';

    va_list vl;
    va_start(vl, format);
    std::vsprintf(szOutputBuffer, format, vl);
    va_end(vl);

    std::printf("%s", szOutputBuffer);
    OutputDebugStringA(szOutputBuffer);
};


/*static*/ void CPCDebug::Fatal(const char* reason)
{
    static char szFatalBuffer[4096];    
    szFatalBuffer[0] = '\0';

    std::sprintf(szFatalBuffer, "%s\n\n", reason);
    std::strcat(szFatalBuffer, "Press OK to execute debugbreak or CANCEL to terminate program.");

    CPCUtils::DisplayCursor(true);
    
    SetWindowPos(CPCSpecific::m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    int32 iResult = MessageBoxA(NULL, szFatalBuffer, "Fatal error", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONERROR);
	if(iResult == IDOK)
		__debugbreak();

    TerminateProcess(GetCurrentProcess(), -1);
};