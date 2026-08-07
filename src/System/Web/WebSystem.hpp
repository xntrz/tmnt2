#pragma once

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_events.h"


struct EmscriptenFullscreenChangeEvent;
struct EmscriptenUiEvent;


class CWebFramework;


class CWebSystem
{
public:
    static const char* WND_NAME;

public:
    CWebSystem(CWebFramework* pFramework);
    ~CWebSystem(void);
    bool Initialize(void);
    void Terminate(void);
    void Run(void);
#ifdef TMNT2_BUILD_EU
    void SetLanguage(void);
#endif /* TMNT2_BUILD_EU */
    bool WindowCreate(void);
    void WindowDestroy(void);
    void WindowPollEvents(void);
    void WindowDispatchEvent(SDL_Event* event);
    void UpdateLanguage(void);
    void UpdateResize(void);
    void MainLoopFrame(void);
    SDL_InitFlags GetSDLInitFlags(void) const;
    void ScreenSizeChanged(void);

private:
    static void PreInitializeGL4ES(void);
    static bool EmscriptenOnFullscreen(int type, const EmscriptenFullscreenChangeEvent* event, void* data);
    static bool EmscriptenOnSizeChanged(int type, const EmscriptenUiEvent* event, void* data);
    static void EmscriptenMainLoopCallback(void* pParam);

private:
    CWebFramework*      m_pFramework;
    SDL_Window*         m_pWindow;
    double              m_dTimePrev;
    double              m_dTimeAccum;
    bool                m_bFocused;
    bool                m_bIsInFullscreen;
    bool                m_bLangChangeRequest;
    bool                m_bSyncInputAndMove;
    int32               m_iResizeFrameCounter;
    int32               m_iMaxMoveCount;
};
