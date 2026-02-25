#include "PCSystem.hpp"
#include "PCFramework.hpp"
#include "PCSpecific.hpp"
#include "resources.hpp"

#include "System/Common/Configure.hpp"


static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_ACTIVATE:
        {
            CPCSystem::Instance().SetFocused(wParam != WA_INACTIVE);            
            if (CPCSystem::Instance().IsFocused())
                UpdateWindow(hWnd);
        }
        break;
   
    case WM_PAINT:
        {
            PAINTSTRUCT ps;            
            HDC hDc = BeginPaint(hWnd, &ps);
            if (hDc != NULL)
            {
                CPCSystem::Instance().Framework().Render();
                CPCSystem::Instance().Framework().FlipNoSync();
                EndPaint(hWnd, &ps);
            };            
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SETCURSOR:
        CPCSpecific::DisplayCursor(false);        
        break;
        
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    };

    return 0;
};


/*static*/ const TCHAR* CPCSystem::WND_CLASS = TEXT("TMNT2");
/*static*/ const TCHAR* CPCSystem::WND_NAME = TEXT("TMNT2");
/*static*/ CPCSystem* CPCSystem::m_pInstance = nullptr;


/*static*/ CPCSystem& CPCSystem::Instance(void)
{
    return *m_pInstance;
};


CPCSystem::CPCSystem(CPCFramework* pFramework)
: m_pFramework(pFramework)
, m_szOsName()
, m_stickyKeys()
, m_toggleKeys()
, m_filterKeys()
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
    m_stickyKeys        = { 0 };
    m_stickyKeys.cbSize = sizeof(m_stickyKeys);
    SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(m_stickyKeys), &m_stickyKeys, 0);

    STICKYKEYS stickyKeys = { 0 };
    stickyKeys.cbSize   = sizeof(stickyKeys);
    stickyKeys.dwFlags  = (m_stickyKeys.dwFlags & (~SKF_HOTKEYACTIVE));
    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(stickyKeys), &stickyKeys, 0);

    //
    //  Set toggle keys
    //
    m_toggleKeys        = { 0 };
    m_toggleKeys.cbSize = sizeof(m_toggleKeys);
    SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(m_toggleKeys), &m_toggleKeys, 0);

    TOGGLEKEYS toggleKeys = { 0 };
    toggleKeys.cbSize   = sizeof(toggleKeys);
    toggleKeys.dwFlags  = (m_toggleKeys.dwFlags & (~(TKF_HOTKEYACTIVE | TKF_TOGGLEKEYSON)));
    SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(toggleKeys), &toggleKeys, 0);

    //
    //  Set filter keys
    //
    m_filterKeys        = { 0 };
    m_filterKeys.cbSize = sizeof(m_filterKeys);
    SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(m_filterKeys), &m_filterKeys, 0);

    FILTERKEYS filterKeys   = { 0 };
    filterKeys.cbSize       = sizeof(filterKeys);
    filterKeys.iWaitMSec    = m_filterKeys.iWaitMSec;
    filterKeys.iRepeatMSec  = m_filterKeys.iRepeatMSec;
    filterKeys.iDelayMSec   = m_filterKeys.iDelayMSec;
    filterKeys.iBounceMSec  = m_filterKeys.iBounceMSec;
    filterKeys.dwFlags      = (m_filterKeys.dwFlags & (~FKF_HOTKEYACTIVE));
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(filterKeys), &filterKeys, 0);

    //
    //  Set screensave
    //
    m_bScreenSavingEnabled = 0;
    SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &m_bScreenSavingEnabled, 0);    
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);

    //
    //  Check OS & create window
    //
    if (!CheckOS())
    {
        OUTPUT("OS check failed: %s\n", m_szOsName);
        return false;
    };

    OUTPUT("OS check success: %s\n", m_szOsName);

#ifdef TMNT2_BUILD_EU
    SetLanguage();
#endif /* TMNT2_BUILD_EU */

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
    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(m_stickyKeys), &m_stickyKeys, 0);
    SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(m_toggleKeys), &m_toggleKeys, 0);
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(m_filterKeys), &m_filterKeys, 0);
};


bool CPCSystem::Run(void)
{
    MSG msg = { 0 };

    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        };
    }
    else if (m_bFocused)
    {
        Framework().Move();
        Framework().Render();
        Framework().Flip();
    }
    else
    {
        WaitMessage();
    };

    return true;
};


bool CPCSystem::CheckOS(void)
{
    enum OSVERSION
    {
        OSVERSION_UNKNOWN = 0,
        OSVERSION_WIN_95,
        OSVERSION_TODO_2, // TODO possible OSR2 but idk
        OSVERSION_WIN_98,
        OSVERSION_WIN_ME,
        OSVERSION_WIN_NT,
        OSVERSION_WIN_2K,
        OSVERSION_WIN_XP_OR_ABOVE,

        OSVERSIONNUM,
    };

    static const bool s_abOsCompatibilityTable[OSVERSIONNUM] =
    {
        /*  OSVERSION_UNKNOWN           */  false,
        /*  OSVERSION_WIN_95            */  false,
        /*  OSVERSION_TODO_2            */  false,
        /*  OSVERSION_WIN_98            */  true,
        /*  OSVERSION_WIN_ME            */  true,
        /*  OSVERSION_WIN_NT            */  false,
        /*  OSVERSION_WIN_2K            */  true,
        /*  OSVERSION_WIN_XP_OR_ABOVE   */  true,
    };

    OSVERSIONINFO osver = { 0 };
    osver.dwOSVersionInfoSize = sizeof(osver);

    if (!GetVersionEx(&osver))
        return false;

    switch (osver.dwPlatformId)
    {
    case VER_PLATFORM_WIN32s:
        std::strcpy(m_szOsName, "Windows 3.1 + Win32s");
        return s_abOsCompatibilityTable[OSVERSION_WIN_95];

    case VER_PLATFORM_WIN32_WINDOWS:
        {
            if ((osver.dwMajorVersion == 4) &&
                (osver.dwMinorVersion == 0))
            {
                std::strcpy(m_szOsName, "Windows 95");
                return s_abOsCompatibilityTable[OSVERSION_WIN_95];
            }
            else if (osver.dwMinorVersion == 10)
            {
                std::strcpy(m_szOsName, "Windows 98");
                return s_abOsCompatibilityTable[OSVERSION_WIN_98];
            }
            else if (osver.dwMinorVersion == 90)
            {
                std::strcpy(m_szOsName, "Windows Me");
                return s_abOsCompatibilityTable[OSVERSION_WIN_ME];
            };
        }
        break;

    case VER_PLATFORM_WIN32_NT:
        {
            if ((osver.dwMajorVersion > 5) ||
                (osver.dwMinorVersion > 1))
            {
                std::strcpy(m_szOsName, "Windows XP or Above");
                return s_abOsCompatibilityTable[OSVERSION_WIN_XP_OR_ABOVE];
            }
            else if (osver.dwMajorVersion < 5)
            {
                return s_abOsCompatibilityTable[OSVERSION_WIN_NT];
            };

            OSVERSIONINFOEX osverex = { 0 };
            osverex.dwOSVersionInfoSize = sizeof(osverex);

            if (GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&osverex)))
            {
                if (osverex.dwMinorVersion > 0)
                {
                    if (osverex.wProductType == VER_NT_WORKSTATION)
                    {
                        std::strcpy(m_szOsName, "Windows XP");
                        if (osverex.wSuiteMask == VER_SUITE_PERSONAL)
                        {
                            std::strcat(m_szOsName, " Home Edition");
                            if (osverex.szCSDVersion[0] != '\0')
                            {
                                std::strcat(m_szOsName, " + (");
                                std::strcat(m_szOsName, osverex.szCSDVersion);
                                std::strcat(m_szOsName, ")");
                            };
                        };
                    }
                    else
                    {
                        std::strcpy(m_szOsName, "Windows .NET Server?");
                        if (osverex.szCSDVersion[0] != '\0')
                        {
                            std::strcat(m_szOsName, " + (");
                            std::strcat(m_szOsName, osverex.szCSDVersion);
                            std::strcat(m_szOsName, ")");
                        };
                    };

                    return s_abOsCompatibilityTable[OSVERSION_WIN_XP_OR_ABOVE];
                }
                else
                {
                    std::strcpy(m_szOsName, "Windows 2000");
                    std::strcat(m_szOsName, (osverex.wProductType == VER_NT_WORKSTATION) ? " Professional" : " Server");
                    if (osverex.szCSDVersion[0] != '\0')
                    {
                        std::strcat(m_szOsName, " + (");
                        std::strcat(m_szOsName, osverex.szCSDVersion);
                        std::strcat(m_szOsName, ")");
                    };

                    return s_abOsCompatibilityTable[OSVERSION_WIN_2K];
                };
            };
        }
        break;

    default:
        break;
    };

    return s_abOsCompatibilityTable[OSVERSION_UNKNOWN];
};


#ifdef TMNT2_BUILD_EU

void CPCSystem::SetLanguage(void)
{
    const char* pszArgValue = nullptr;
    bool bSpecified = CConfigure::CheckArgValue("lang", &pszArgValue);
    if (bSpecified)
    {
        struct ARG_TO_LANG
        {
            const char*             name;
            const char*             arg;
            TYPEDEF::CONFIG_LANG    lang;
        };

        static const ARG_TO_LANG s_aArgToLang[] =
        {
            { "english",    "en",   TYPEDEF::CONFIG_LANG_ENGLISH },
            { "german",     "de",   TYPEDEF::CONFIG_LANG_GERMAN  },
            { "french",     "fr",   TYPEDEF::CONFIG_LANG_FRENCH  },
            { "spanish",    "es",   TYPEDEF::CONFIG_LANG_SPANISH },
            { "italian",    "it",   TYPEDEF::CONFIG_LANG_ITALIAN },
        };

        static_assert(COUNT_OF(s_aArgToLang) == TYPEDEF::CONFIG_LANG_NUM, "update table");

        for (int32 i = 0; i < COUNT_OF(s_aArgToLang); ++i)
        {
            if (!std::strcmp(s_aArgToLang[i].arg, pszArgValue))
            {
                OUTPUT("WARNING! Language was forcefully set to \"%s\" by command line arg.\n", s_aArgToLang[i].name);
                CConfigure::SetLanguage(s_aArgToLang[i].lang);
                return;
            };
        };
    };

    LANGID langId = GetUserDefaultLangID();

    switch (PRIMARYLANGID(langId))
    {
    case LANG_FRENCH:
        CConfigure::SetLanguage(TYPEDEF::CONFIG_LANG_FRENCH);
        break;

    case LANG_GERMAN:
        CConfigure::SetLanguage(TYPEDEF::CONFIG_LANG_GERMAN);
        break;

    case LANG_ITALIAN:
        CConfigure::SetLanguage(TYPEDEF::CONFIG_LANG_ITALIAN);
        break;

    case LANG_SPANISH:
        CConfigure::SetLanguage(TYPEDEF::CONFIG_LANG_SPANISH);
        break;

    default:
        CConfigure::SetLanguage(TYPEDEF::CONFIG_LANG_ENGLISH);
        break;
    };
};

#endif /* TMNT2_BUILD_EU */


bool CPCSystem::WindowCreate(void)
{
    HWND hWndResult = NULL;

#if defined(TMNT2_SINGLE_INSTANCE)
    hWndResult = FindWindow(WND_CLASS, WND_NAME);
    if (hWndResult != NULL)
    {
        SetForegroundWindow(hWndResult);
        return false;
    };
#endif /* defined(TMNT2_SINGLE_INSTANCE) */

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = CPCSpecific::m_hInstance;
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WND_CLASS;

    if (!RegisterClassEx(&wc))
        return false;

    hWndResult = CreateWindowEx(NULL, WND_CLASS, WND_NAME, WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                                NULL, NULL, CPCSpecific::m_hInstance, NULL);
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

    UnregisterClass(WND_CLASS, NULL);
};