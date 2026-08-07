#include "WebSystem.hpp"
#include "WebFramework.hpp"
#include "WebSpecific.hpp"
#include "WebMemory.hpp"

#include "File/WebFileManager.hpp"

#include "System/Common/Sound/SdDrv.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Screen.hpp"

#include "SDL3/SDL_platform.h"
#include "SDL3/SDL_locale.h"
#include "SDL3/SDL_hints.h"

#include "gl4esinit.h"

#include <emscripten.h>
#include <emscripten/html5.h>


/*static*/ const char* CWebSystem::WND_NAME = "TMNT2";


/*static*/ void
CWebSystem::PreInitializeGL4ES(void)
{
    //
    //  Reading some gl4es env vars from args
    //  Full list: https://github.com/ptitSeb/gl4es/blob/master/USAGE.md
    //

    const char* pszValue = nullptr;

    setenv("LIBGL_BATCH", "100", 1);
    setenv("LIBGL_BEGINEND", "1", 1);
    setenv("LIBGL_ES", "2", 1);
    setenv("LIBGL_USEVBO", "1", 1);

    if (CConfigure::CheckArgValue("glcfg", &pszValue))
    {
        int32 cfg = std::atoi(pszValue);
        
        switch (cfg)
        {
        case 1:
            setenv("LIBGL_BATCH", "0", 1);
            setenv("LIBGL_BEGINEND", "0", 1);
            break;

        case 2:
            setenv("LIBGL_BATCH", "1", 1);
            setenv("LIBGL_BEGINEND", "0", 1);
            break;

        case 3:
            setenv("LIBGL_BATCH", "100", 1);
            setenv("LIBGL_BEGINEND", "0", 1);
            break;

        default:
            break;
        };
    };

    if (CConfigure::CheckArgValue("glshrink", &pszValue))
        setenv("LIBGL_SHRINK", pszValue, 1);

    if (CConfigure::CheckArgValue("glmipmap", &pszValue))
        setenv("LIBGL_MIPMAP", pszValue, 1);

    if (CConfigure::CheckArgValue("glalphahack", &pszValue))
        setenv("LIBGL_ALPHAHACK", pszValue, 1);

    if (CConfigure::CheckArgValue("gltexcopy", &pszValue))
        setenv("LIBGL_TEXCOPY", pszValue, 1);

    if (CConfigure::CheckArgValue("glfb", &pszValue))
        setenv("LIBGL_FB", pszValue, 1);

    if (CConfigure::CheckArgValue("gldefaultwrap", &pszValue))
        setenv("LIBGL_DEFAULTWRAP", pszValue, 1);
};


/*static*/ bool
CWebSystem::EmscriptenOnFullscreen(int type, const EmscriptenFullscreenChangeEvent* event, void* data)
{
    CWebSystem* pSystem = reinterpret_cast<CWebSystem*>(data);

    pSystem->m_bIsInFullscreen = event->isFullscreen;
    pSystem->ScreenSizeChanged();

    return false;
};


/*static*/ bool
CWebSystem::EmscriptenOnSizeChanged(int type, const EmscriptenUiEvent* event, void* data)
{
    CWebSystem* pSystem = reinterpret_cast<CWebSystem*>(data);

    pSystem->ScreenSizeChanged();
    
    return false;
};


/*static*/ void CWebSystem::EmscriptenMainLoopCallback(void* pParam)
{
    CWebSystem* pSystem = reinterpret_cast<CWebSystem*>(pParam);

    pSystem->MainLoopFrame();
};


CWebSystem::CWebSystem(CWebFramework* pFramework)
: m_pFramework(pFramework)
, m_pWindow(nullptr)
, m_dTimePrev(emscripten_get_now())
, m_dTimeAccum(0.0)
, m_bFocused(true)
, m_bIsInFullscreen(false)
, m_bLangChangeRequest(false)
, m_bSyncInputAndMove(true)
, m_iResizeFrameCounter(0)
, m_iMaxMoveCount(3)
{
    ;
};


CWebSystem::~CWebSystem(void)
{
    ;
};


bool CWebSystem::Initialize(void)
{
    //
    //  Init SDL
    //
    if (!SDL_Init(GetSDLInitFlags()))
    {
        OUTPUT("SDL3 init failed!\n");
        return false;
    };

    //
    //  Set lang & create window
    //
#ifdef TMNT2_BUILD_EU
    SetLanguage();
#endif /* TMNT2_BUILD_EU */

    if (!WindowCreate())
    {
        OUTPUT("Create window failed\n");
        return false;
    };

    //
    //  Init GL4ES
    //
    PreInitializeGL4ES();

    initialize_gl4es();

    //
    //  Init vars from args
    //
    const char* pszValue = nullptr;

    if (CConfigure::CheckArgValue("movemax", &pszValue))
    {
        m_iMaxMoveCount = std::atoi(pszValue);
        m_iMaxMoveCount = Clamp(m_iMaxMoveCount, 1, 7);
    };

    if (CConfigure::CheckArgValue("moveinp", &pszValue))
    {
        m_bSyncInputAndMove = (std::atoi(pszValue) > 0);
    };

    return true;
};


void CWebSystem::Terminate(void)
{
    close_gl4es();

    WindowDestroy();

    SDL_QuitSubSystem(GetSDLInitFlags());
};


void CWebSystem::Run(void)
{
    //
    //  NOTE:  firefox 127.0 for test with RAF interval greater than vsync 
    //         https://ftp.mozilla.org/pub/firefox/releases/127.0/win64/  
    //         (about:config -> layout.frame_rate)
    // 
    //  NOTE2: to disable auto updates do following:
    //         win+r -> regedit -> HKEY_LOCAL_MACHINE\SOFTWARE\Policies\Mozilla\Firefox -> DisableAppUpdate = 1
    //         (create Mozilla\Firefox keys and DisableAppUpdate value if not exist)
    //
    
    emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false,
                                             &CWebSystem::EmscriptenOnFullscreen);

    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false,
                                   &CWebSystem::EmscriptenOnSizeChanged);

    emscripten_set_main_loop_arg(&CWebSystem::EmscriptenMainLoopCallback, this, -1, true);
};


#ifdef TMNT2_BUILD_EU

void CWebSystem::SetLanguage(void)
{
    //
    //  NOTE: for test -> dev tools (F12) -> ESC -> sensors -> location
    //

    struct LANGINFO
    {
        const char*          langName;
        const char*          langCode;
        TYPEDEF::CONFIG_LANG langConfig;
    };

    static const LANGINFO s_aLangInfo[] =
    {
        { "english", "en", TYPEDEF::CONFIG_LANG_ENGLISH },
        { "german",  "de", TYPEDEF::CONFIG_LANG_GERMAN  },
        { "french",  "fr", TYPEDEF::CONFIG_LANG_FRENCH  },
        { "spanish", "es", TYPEDEF::CONFIG_LANG_SPANISH },
        { "italian", "it", TYPEDEF::CONFIG_LANG_ITALIAN },
    };

    static_assert(COUNT_OF(s_aLangInfo) == TYPEDEF::CONFIG_LANG_NUM, "update table pls");

    //
    //  Check for forcefully set
    //
    const char* pszArgValue;
    if (CConfigure::CheckArgValue("lang", &pszArgValue))
    {
        for (int32 i = 0; i < COUNT_OF(s_aLangInfo); ++i)
        {
            if (!std::strcmp(s_aLangInfo[i].langCode, pszArgValue))
            {
                OUTPUT("WARNING! Language was forcefully set to \"%s\" by command line arg.\n", s_aLangInfo[i].langName);
                CConfigure::SetLanguage(s_aLangInfo[i].langConfig);
                return;
            };
        };
    };

    //
    //  Now check for current lang
    //
    char szLangBuff[4096];
    std::sprintf(szLangBuff, "%s", "User langs: ");
    
    int32 langIdx = -1;

    int32 localeIndex = 0;
    int32 localeNum = 0;
    SDL_Locale** ppLocale = SDL_GetPreferredLocales(&localeNum);

    while (localeIndex < localeNum)
    {
        SDL_Locale* pLocale = ppLocale[localeIndex];

        char szLang[256];
        szLang[0] = '\0';

        if (pLocale->country)
            std::sprintf(szLang, "\"%s-%s\" ", pLocale->language, pLocale->country);
        else
            std::sprintf(szLang, "\"%s\" ", pLocale->language);

        std::strcat(szLangBuff, szLang);

        for (int32 i = 0; i < COUNT_OF(s_aLangInfo); ++i)
        {
            if (!std::strcmp(pLocale->language, s_aLangInfo[i].langCode))
            {
                langIdx = i;
                break;
            };
        };

        ++localeIndex;
    };

    OUTPUT("%s\n", szLangBuff);
        
    if (langIdx == -1)
    {
        OUTPUT("Supported langs not found! Set to default \"%s\"\n",
               s_aLangInfo[TYPEDEF::CONFIG_LANG_ENGLISH].langName);

        langIdx = TYPEDEF::CONFIG_LANG_ENGLISH;
    };

    OUTPUT("Game lang: %s\n", s_aLangInfo[langIdx].langName);
    CConfigure::SetLanguage(s_aLangInfo[langIdx].langConfig);
};

#endif /* TMNT2_BUILD_EU */


bool CWebSystem::WindowCreate(void)
{
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_ASYNCIFY, "0");

    SDL_DisplayID displayId = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* displayMode = SDL_GetDesktopDisplayMode(displayId);

    m_pWindow = SDL_CreateWindow(WND_NAME, displayMode->w, displayMode->h,
                                 SDL_WINDOW_RESIZABLE | SDL_WINDOW_FILL_DOCUMENT);
    if (m_pWindow)
    {
        SDL_ShowWindow(m_pWindow);
        
        CWebSpecific::m_pWindow = m_pWindow;
        CWebSpecific::SetCanvasFocus();        
    }
    else
    {
        OUTPUT("SDL_CreateWindow failed: %s\n", SDL_GetError());
    };

    return (m_pWindow != nullptr);
};


void CWebSystem::WindowDestroy(void)
{
    if (m_pWindow)
    {
        CWebSpecific::m_pWindow = nullptr;

        SDL_DestroyWindow(m_pWindow);
        m_pWindow = nullptr;
    };
};


void CWebSystem::WindowPollEvents(void)
{
    SDL_Event event = {};
    
    while (SDL_PollEvent(&event))
        WindowDispatchEvent(&event);

    UpdateResize();
    UpdateLanguage();
};


void CWebSystem::WindowDispatchEvent(SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_EVENT_JOYSTICK_ADDED:
        m_pFramework->OnJoystickAdd(event->gdevice.which);
        break;

    case SDL_EVENT_JOYSTICK_REMOVED:
        m_pFramework->OnJoystickRem(event->jdevice.which);
        break;

    case SDL_EVENT_RENDER_DEVICE_RESET:
        OUTPUT("gx device reset\n");
        break;

    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
        ScreenSizeChanged();
        break;

    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
        ScreenSizeChanged();
        break;

    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        m_bFocused = true;
        SdDrvWindowFocusRestore();
        break;

    case SDL_EVENT_WINDOW_FOCUS_LOST:
        m_bFocused = false;
        SdDrvWindowFocusLost();
        break;

    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        ScreenSizeChanged();
        break;

    case SDL_EVENT_LOCALE_CHANGED:
        m_bLangChangeRequest = true;
        break;

    default:
        break;
    };
};


void CWebSystem::UpdateLanguage(void)
{
#ifdef TMNT2_BUILD_EU
    if (m_bLangChangeRequest)
    {
        m_bLangChangeRequest = false;
        
        SetLanguage();
        CWebFileManager::Instance().ResetData();
    };
#endif /* TMNT2_BUILD_EU */
};


void CWebSystem::UpdateResize(void)
{
    if (m_iResizeFrameCounter > 0)
    {
        if (--m_iResizeFrameCounter == 0)
        {
            int32 w = 0;
            int32 h = 0;
            SDL_GetWindowSizeInPixels(m_pWindow, &w, &h);
            m_pFramework->OnScreenSizeChanged(w, h);
        };
    };
};


void CWebSystem::MainLoopFrame(void)
{
    WindowPollEvents();

    if (m_bFocused && (m_iResizeFrameCounter == 0))
    {
        const double FRAME_TIME = (1.0 / 60.0);

        double dTimeNow = emscripten_get_now();
        double dTimeElapsed = (dTimeNow - m_dTimePrev) / 1000.0;

        if (dTimeElapsed > 0.1)
            dTimeElapsed = 0.1;

        m_dTimePrev = dTimeNow;
        m_dTimeAccum += dTimeElapsed;

        CScreen::TimerStride(FRAME_TIME);

        int32 iMoveCount = 0;
        while (m_dTimeAccum >= FRAME_TIME)
        {        
            m_pFramework->Move();

            if (m_bSyncInputAndMove)
                m_pFramework->SyncInput();

            m_dTimeAccum -= FRAME_TIME;

            if (++iMoveCount >= m_iMaxMoveCount)
            {
                m_dTimeAccum = 0.0;
                break;
            };
        };

        CScreen::TimerStride(dTimeElapsed);
        m_pFramework->Render();
        m_pFramework->Flip();

        if (!m_bSyncInputAndMove)
            m_pFramework->SyncInput();
    };

    m_pFramework->SyncFS();

    SdDrvTask();
};


SDL_InitFlags CWebSystem::GetSDLInitFlags(void) const
{
    SDL_InitFlags initFlags = SDL_INIT_EVENTS;

    // window subsystem
    initFlags |= (SDL_INIT_VIDEO);

    // inputs device subsystem
    initFlags |= (SDL_INIT_JOYSTICK |
                  SDL_INIT_HAPTIC);

    return initFlags;
};


void CWebSystem::ScreenSizeChanged(void)
{
    /* we might get wrong sizes while orientation changing on mobile device
       and we will fail in framebuffer recreation so delay this for some frame count
       until we get stable sizes after orientation changes completely */

    m_iResizeFrameCounter = 30;
};