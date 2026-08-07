#pragma once

#include "System/Common/Controller.hpp"


typedef struct SDL_Window SDL_Window;


class CWebSpecific
{
public:
    struct SYSTEMTIME
    {
        int32 year;
        int32 month;
        int32 dayOfWeek;
        int32 day;
        int32 hours;
        int32 minutes;
        int32 seconds;
    };

public:
    static int32 GetKeyboradPort(void);
    static int32 GetTouchPort(void);
    static void MapDigital(uint32 btn, int32 sdlScancode);
    static void MapDigitalFixed(uint32 btn, int32 sdlScancode);
    static void MapAnalog(CController::ANALOG analog, int32 sdlScancodeX, int32 sdlScancodeY);
    static int32 GetDownKey(void);
    static bool IsKeyDown(int32 sdlScancode);
    static bool IsKeyTrigger(int32 sdlScancode);
    static bool IsKeyValid(int32 sdlScancode);
    static const char* GetKeyName(int32 sdlScancode);
    static void GetLocalTime(SYSTEMTIME* st);
    static bool IsMobilePlatform(void);
    static void VibrateMobile(int32 milliseconds);
    static void GetCanvasSize(int32* w, int32* h);
    static void SetCanvasFocus(void);
    static void RequestFullscreen(bool bLock = true);
    static bool IsInFullscreen(void);
    static void WaitForMainModuleReady(void);
    static void WaitUserActive(void);

public:
    static SDL_Window* m_pWindow;
};