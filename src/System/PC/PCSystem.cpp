#include "PCSystem.hpp"
#include "PCFramework.hpp"
#include "PCSpecific.hpp"
#include "resources.hpp"

#ifdef _DEBUG
#include "Sound/SdDrv.hpp"
#include "Sound/SdDrvCtrl.hpp"
#endif


static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_ACTIVATE:
        {
            CPCSystem::Instance().SetFocused(wParam != WA_INACTIVE);
            
            if (!CPCSystem::Instance().IsFocused())
                break;

            UpdateWindow(hWnd);
        }
        break;
   
    case WM_PAINT:
        {
            PAINTSTRUCT Ps;

			BeginPaint(hWnd, &Ps);
			CPCSystem::Instance().Framework().Render();
			CPCSystem::Instance().Framework().FlipNoSync();
			EndPaint(hWnd, &Ps);
        }
        break;

    case WM_CLOSE:
        {
            DestroyWindow(hWnd);
        }        
        break;
        
    case WM_DESTROY:
        {
            PostQuitMessage(0);
        }        
        break;

    case WM_SETCURSOR:
        {
            CPCSpecific::DisplayCursor(false);
        }
        break;
        
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    };

    return 0;
};


/*static*/ const char* CPCSystem::WNDNAME = "TMNT2";
/*static*/ CPCSystem* CPCSystem::m_pInstance = nullptr;


/*static*/ CPCSystem& CPCSystem::Instance(void)
{
    return *m_pInstance;
};


CPCSystem::CPCSystem(CPCFramework* pFramework)
: m_pFramework(pFramework)
, m_bScreenSavingEnabled(false)
, m_bFocused(true)
{
    ASSERT(!m_pInstance);
    m_pInstance = this;
};


CPCSystem::~CPCSystem(void)
{
    ASSERT(m_pInstance == this);
    m_pInstance = nullptr;
};


bool CPCSystem::Initialize(void)
{
    //
    //  Set sticky keys
    //
    m_StickyKeys = { 0 };
    m_StickyKeys.cbSize = sizeof(m_StickyKeys);

    SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(m_StickyKeys), &m_StickyKeys, 0);

    STICKYKEYS StickyKeys = { 0 };
    StickyKeys.cbSize = sizeof(StickyKeys);
    StickyKeys.dwFlags = FLAG_CLEAR(m_StickyKeys.dwFlags, SKF_HOTKEYACTIVE);

    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(StickyKeys), &StickyKeys, 0);

    //
    //  Set toggle keys
    //
    m_ToggleKeys = { 0 };
    m_ToggleKeys.cbSize = sizeof(m_ToggleKeys);

    SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(m_ToggleKeys), &m_ToggleKeys, 0);

    TOGGLEKEYS ToggleKeys = { 0 };
    ToggleKeys.cbSize = sizeof(ToggleKeys);
    ToggleKeys.dwFlags = FLAG_CLEAR(m_ToggleKeys.dwFlags, TKF_HOTKEYACTIVE | TKF_TOGGLEKEYSON);

    SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(ToggleKeys), &ToggleKeys, 0);

    //
    //  Set filter keys
    //
    m_FilterKeys = { 0 };
    m_FilterKeys.cbSize = sizeof(m_FilterKeys);

    SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(m_FilterKeys), &m_FilterKeys, 0);

    FILTERKEYS FilterKeys = { 0 };
    FilterKeys.cbSize = sizeof(FilterKeys);
    FilterKeys.iWaitMSec = m_FilterKeys.iWaitMSec;
    FilterKeys.iRepeatMSec = m_FilterKeys.iRepeatMSec;
    FilterKeys.iDelayMSec = m_FilterKeys.iDelayMSec;
    FilterKeys.iBounceMSec = m_FilterKeys.iBounceMSec;
    FilterKeys.dwFlags = FLAG_CLEAR(m_FilterKeys.dwFlags, FKF_HOTKEYACTIVE);

    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FilterKeys), &FilterKeys, 0);

    //
    //  Set screensave
    //
    m_bScreenSavingEnabled = 0;
    SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &m_bScreenSavingEnabled, 0);    
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);

    if (!CheckOS())
    {
        OUTPUT("Operating system check failed\n");
        return false;
    };

    if (!WindowCreate())
    {
        OUTPUT("Create window failed\n");
        return false;
    };

    return true;
};


void CPCSystem::Terminate(void)
{
    WindowDestroy();

    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, m_bScreenSavingEnabled, NULL, 0);
    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(m_StickyKeys), &m_StickyKeys, 0);
    SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(m_ToggleKeys), &m_ToggleKeys, 0);
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(m_FilterKeys), &m_FilterKeys, 0);
};


bool CPCSystem::Run(void)
{
    bool bResult = true;
    MSG Msg;

    if (PeekMessageA(&Msg, 0, 0, 0, PM_REMOVE))
    {
        if (Msg.message == WM_QUIT)
        {
            bResult = false;
        }
        else
        {
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        };
    }
    else if (m_bFocused)
    {
        Framework().Move();
        Framework().Render();
        Framework().Flip();
    };

	return bResult;
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

    std::strcpy(m_szOsName, s_aWinVersionInfo[WinVersion].m_pszLabel);
    
    return s_aWinVersionInfo[WinVersion].m_bCompatibility;
};


bool CPCSystem::WindowCreate(void)
{
    WNDCLASSEXA WndClass = { 0 };
    WndClass.cbSize         = sizeof(WndClass);
    WndClass.style          = 0;
    WndClass.lpfnWndProc    = WndProc;
    WndClass.cbClsExtra     = 0;
    WndClass.cbWndExtra     = 0;
    WndClass.hInstance      = CPCSpecific::m_hInstance;
    WndClass.hIcon          = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP));
    WndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground  = HBRUSH(GetStockObject(BLACK_BRUSH));
    WndClass.lpszMenuName   = NULL;
	WndClass.lpszClassName  = WNDNAME;

    if (!RegisterClassExA(&WndClass))
        return false;

    const DWORD Styles = (WS_OVERLAPPEDWINDOW);
    
    static_assert(Styles == 0xCF0000, "checkout");

    HWND hWndResult = CreateWindowExA(
        NULL,
        WNDNAME,
        WNDNAME,
        Styles,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        NULL,
        NULL,
        CPCSpecific::m_hInstance,
        NULL
    );

	if (!hWndResult)
        return false;

    SetForegroundWindow(hWndResult);
    
    CPCSpecific::m_hWnd = hWndResult;

    return true;
};


void CPCSystem::WindowDestroy(void)
{
    if (CPCSpecific::m_hWnd)
    {
        DestroyWindow(CPCSpecific::m_hWnd);
        CPCSpecific::m_hWnd = NULL;
    };

	MSG msg;
	while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
		;

	UnregisterClassA(WNDNAME, NULL);
};