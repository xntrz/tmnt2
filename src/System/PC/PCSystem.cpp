#include "PCSystem.hpp"
#include "PCFramework.hpp"
#include "PCSpecific.hpp"
#include "PCResources.hpp"


static const TCHAR* APP_WNDNAME = TEXT("TMNT2");
static const TCHAR* APP_CLASSNAME = TEXT("TMNT2APP");


/*static*/ LRESULT CALLBACK CPCSystem::WndProcRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CPCSystem* pSystem = nullptr;
    
    if (uMsg == WM_NCCREATE)
    {
        pSystem = (CPCSystem*)LPCREATESTRUCT(lParam)->lpCreateParams;
        SetWindowLong(hWnd, GWL_USERDATA, LONG(pSystem));
    }
    else
    {
        pSystem = (CPCSystem*)GetWindowLong(hWnd, GWL_USERDATA);
    };

    if (pSystem)
        return pSystem->WndProc(hWnd, uMsg, wParam, lParam);
    else
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
};


CPCSystem::CPCSystem(CPCFramework* pFramework)
: m_pFramework(pFramework)
, m_bScreenSavingEnabled(false)
, m_bIsForeground(false)
{
    ;
};


CPCSystem::~CPCSystem(void)
{
    ;
};


bool CPCSystem::Initialize(void)
{
    m_StickyKeys = { 0 };
    m_ToggleKeys = { 0 };
    m_FilterKeys = { 0 };
    
    m_StickyKeys.cbSize = sizeof(m_StickyKeys);
    m_ToggleKeys.cbSize = sizeof(m_ToggleKeys);
    m_FilterKeys.cbSize = sizeof(m_FilterKeys);

    SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &m_bScreenSavingEnabled, 0);
    SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(m_StickyKeys), &m_StickyKeys, 0);
    SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(m_ToggleKeys), &m_ToggleKeys, 0);
    SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(m_FilterKeys), &m_FilterKeys, 0);

    STICKYKEYS StickyKeys = { 0 };
    StickyKeys.cbSize = sizeof(StickyKeys);
    StickyKeys.dwFlags = FLAG_CLEAR(m_StickyKeys.dwFlags, SKF_HOTKEYACTIVE);

    TOGGLEKEYS ToggleKeys = { 0 };
    ToggleKeys.cbSize = sizeof(ToggleKeys);
    ToggleKeys.dwFlags = FLAG_CLEAR(m_ToggleKeys.dwFlags, TKF_HOTKEYACTIVE | TKF_TOGGLEKEYSON);

    FILTERKEYS FilterKeys = { 0 };
    FilterKeys.cbSize = sizeof(FilterKeys);
    FilterKeys.iWaitMSec = m_FilterKeys.iWaitMSec;
    FilterKeys.iRepeatMSec = m_FilterKeys.iRepeatMSec;
    FilterKeys.iDelayMSec = m_FilterKeys.iDelayMSec;
    FilterKeys.iBounceMSec = m_FilterKeys.iBounceMSec;
    FilterKeys.dwFlags = FLAG_CLEAR(m_FilterKeys.dwFlags, FKF_HOTKEYACTIVE);

    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);
    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(StickyKeys), &StickyKeys, 0);
    SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(ToggleKeys), &ToggleKeys, 0);
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &FilterKeys, 0);

    if (!CheckOS())
    {
        OUTPUT("[SYS] Operating system check failed\n");
        return false;
    };

    if (!WindowCreate())
    {
        OUTPUT("[SYS] Create window failed\n");
        return false;
    };    

    return true;
};


void CPCSystem::Terminate(void)
{
    WindowDestroy();

    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, m_bScreenSavingEnabled, NULL, NULL);
    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(m_StickyKeys), &m_StickyKeys, NULL);
    SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(m_ToggleKeys), &m_ToggleKeys, NULL);
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(m_FilterKeys), &m_FilterKeys, NULL);
};


void CPCSystem::Run(void)
{
    MSG Msg = { 0 };
    
    while (!m_pFramework->IsEnded())
    {
        if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD))
        {
            if (Msg.message == WM_QUIT)
            {
                break;
            }
            else
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            };
        }
        else if (m_bIsForeground)
        {
            m_pFramework->Move();
            m_pFramework->Render();
            m_pFramework->Flip();
        }
        else
        {
            WaitMessage();
        };
    };
};


void CPCSystem::WindowShow(bool bFullscreen)
{
    HWND hWnd = HWND(CPCSpecific::m_hWnd);
    
    if (bFullscreen)
    {
        ShowCursor(FALSE);
    }
    else
    {
        ShowWindow(hWnd, SW_SHOW);
        SetForegroundWindow(hWnd);
    };
};


void CPCSystem::WindowHide(bool bFullscreen)
{
    HWND hWnd = HWND(CPCSpecific::m_hWnd);
    
    if (!bFullscreen)
        ShowWindow(hWnd, SW_HIDE);
};


void CPCSystem::CursorShow(bool bState)
{
    ;
};


bool CPCSystem::IsCursorHidden(void) const
{
    return false;
};


bool CPCSystem::CheckOS(void)
{
    enum WINVERSION
    {
        WINVERSION_UNKNOWN = 0,
        WINVERSION_2000,
        WINVERSION_XP,
        WINVERSION_VISTA,
        WINVERSION_7,
        WINVERSION_8,
        WINVERSION_10,
    };

    struct WINVERSION_INFO
    {
        const char* m_pszLabel;
        bool m_bCompatibility;
    };

    static const WINVERSION_INFO s_aWinVersionInfo[] =
    {
        { "Unknown",                true,   },
        { "Windows 2000",           true,   },
        { "Windows XP",             true,   },
        { "Windows Vista",          true,   },
        { "Windows 7",              true,   },
        { "Windows 8",              true,   },
        { "Windows 10 or above",    true    },
    };

    WINVERSION WinVersion = WINVERSION_UNKNOWN;
    OSVERSIONINFOEX osverex = { 0 };
    osverex.dwOSVersionInfoSize = sizeof(osverex);

    if (!GetVersionEx(LPOSVERSIONINFO(&osverex)))
        return false;

    if (osverex.wProductType != VER_NT_WORKSTATION)
        return false;

    if (osverex.dwMajorVersion == 5 && osverex.dwMinorVersion == 0)
        WinVersion = WINVERSION_2000;
    else if (osverex.dwMajorVersion == 5 && osverex.dwMinorVersion >= 1)
        WinVersion = WINVERSION_XP;
    else if (osverex.dwMajorVersion == 6 && osverex.dwMinorVersion >= 0)
        WinVersion = WINVERSION_VISTA;
    else if (osverex.dwMajorVersion == 6 && osverex.dwMinorVersion >= 1)
        WinVersion = WINVERSION_7;
    else if (osverex.dwMajorVersion == 6 && osverex.dwMinorVersion >= 2)
        WinVersion = WINVERSION_8;
    else if (osverex.dwMajorVersion == 10 && osverex.dwMinorVersion >= 0)
        WinVersion = WINVERSION_10;
    else
        WinVersion = WINVERSION_UNKNOWN;

    strcpy(m_szOsName, s_aWinVersionInfo[WinVersion].m_pszLabel);
    
    return s_aWinVersionInfo[WinVersion].m_bCompatibility;
};


bool CPCSystem::WindowCreate(void)
{
    WNDCLASSEX WndClass = { 0 };
    WndClass.cbSize         = sizeof(WndClass);
    WndClass.style          = 0;
    WndClass.lpfnWndProc    = &CPCSystem::WndProcRouter;
    WndClass.cbClsExtra     = 0;
    WndClass.cbWndExtra     = 0;
    WndClass.hInstance      = CPCSpecific::m_hInstance;
    WndClass.hIcon          = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP));
    WndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground  = HBRUSH(COLOR_WINDOW + 1);
    WndClass.lpszMenuName   = NULL;
	WndClass.lpszClassName  = APP_CLASSNAME;

    if (!RegisterClassEx(&WndClass))
        return false;
    
    HWND hWndResult = CreateWindowEx(
        NULL,
        APP_CLASSNAME,
        APP_WNDNAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        NULL,
        NULL,
        CPCSpecific::m_hInstance,
        this
    );

	if (!hWndResult)
	{
		DWORD dwError = GetLastError();
        return false;
	};

    SetForegroundWindow(hWndResult);
    CPCSpecific::m_hWnd = hWndResult;

    return true;
};


void CPCSystem::WindowDestroy(void)
{
    HWND hWnd = HWND(CPCSpecific::m_hWnd);
    if (hWnd)
    {
        DestroyWindow(hWnd);
        CPCSpecific::m_hWnd = nullptr;
    };

	UnregisterClass(APP_CLASSNAME, NULL);

	MSG Msg = { 0 };
	while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD))
		;	
};


LRESULT CALLBACK CPCSystem::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_ACTIVATE:
        HANDLE_WM_ACTIVATE(hWnd, wParam, lParam, OnActivate);
        break;

    case WM_SIZE:
        HANDLE_WM_SIZE(hWnd, wParam, lParam, OnSize);
        break;

    case WM_SIZING:
        OnSizing(hWnd, LPRECT(lParam));
        break;

    case WM_CLOSE:
        HANDLE_WM_CLOSE(hWnd, wParam, lParam, OnClose);        
        break;

    case WM_DESTROY:
        HANDLE_WM_DESTROY(hWnd, wParam, lParam, OnDestroy);
        break;

    case WM_SETCURSOR:
        if (!HANDLE_WM_SETCURSOR(hWnd, wParam, lParam, OnSetCursor))
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        break;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    };

    return 0;
};


void CPCSystem::OnActivate(HWND hWnd, uint32 state, HWND hwndActDeact, BOOL fMinimized)
{
    if (state == WA_INACTIVE)
        m_bIsForeground = false;
    else
        m_bIsForeground = true;

    UpdateWindow(hWnd);
};


void CPCSystem::OnSize(HWND hWnd, uint32 state, int32 w, int32 h)
{
    if (w > 0 && h > 0)
        m_pFramework->UpdateSize(w, h);
};


void CPCSystem::OnSizing(HWND hWnd, RECT* pRect)
{
    m_pFramework->Render();
    m_pFramework->Flip();
    
    SetWindowPos(
        hWnd,
        HWND_TOP,
        pRect->left,
        pRect->top,
        pRect->right - pRect->left,
        pRect->bottom - pRect->top,
        SWP_NOMOVE
    );
};


void CPCSystem::OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);
};


void CPCSystem::OnDestroy(HWND hWnd)
{
    PostQuitMessage(0x0);
};


bool CPCSystem::OnSetCursor(HWND hWnd, HWND hWndCursor, uint32 codeHitTest, uint32 ms)
{
    return false;
};