#include "WebSpecific.hpp"
#include "WebTypedefs.hpp"
#include "WebPhysicalController.hpp"
#include "WebSystem.hpp"

#include "System/Common/Configure.hpp"

#include "SDL3/SDL.h"

#include <emscripten.h>
#include <emscripten/html5.h>

#pragma push_macro("new")
#pragma push_macro("delete")
#undef new
#undef delete
#include <chrono>
#pragma pop_macro("delete")
#pragma pop_macro("new")


static bool s_bIsMainModuleReady = false;


EMSCRIPTEN_KEEPALIVE
extern "C" void onMainModuleReady(void)
{
    s_bIsMainModuleReady = true;
};


/*static*/ SDL_Window* CWebSpecific::m_pWindow = nullptr;


/*static*/ int32 CWebSpecific::GetKeyboradPort(void)
{
    return CWebPhysicalController::GetPort();
};


/*static*/ int32 CWebSpecific::GetTouchPort(void)
{
    return CWebPhysicalController::GetTouchPort();
};


/*static*/ void CWebSpecific::MapDigital(uint32 btn, int32 sdlScancode)
{
    CWebPhysicalController::MapDigital(btn, sdlScancode);
};


/*static*/ void CWebSpecific::MapDigitalFixed(uint32 btn, int32 sdlScancode)
{
    CWebPhysicalController::MapDigitalFixed(btn, sdlScancode);
};


/*static*/ void CWebSpecific::MapAnalog(CController::ANALOG analog, int32 sdlScancodeX, int32 sdlScancodeY)
{
    CWebPhysicalController::MapAnalog(analog, sdlScancodeX, sdlScancodeY);
};


/*static*/ int32 CWebSpecific::GetDownKey(void)
{
    return CWebPhysicalController::GetDownKey();
};


/*static*/ bool CWebSpecific::IsKeyDown(int32 sdlScancode)
{
    return CWebPhysicalController::IsKeyDown(sdlScancode);
};


/*static*/ bool CWebSpecific::IsKeyTrigger(int32 sdlScancode)
{
    return CWebPhysicalController::IsKeyTrigger(sdlScancode);
};


/*static*/ bool CWebSpecific::IsKeyValid(int32 sdlScancode)
{
    // TODO write more restricted scancodes

    if (sdlScancode == SDL_SCANCODE_UP)
        return false;

    if (sdlScancode == SDL_SCANCODE_DOWN)
        return false;

    if (sdlScancode == SDL_SCANCODE_LEFT)
        return false;

    if (sdlScancode == SDL_SCANCODE_RIGHT)
        return false;

    if (sdlScancode == SDL_SCANCODE_ESCAPE)
        return false;

    if (sdlScancode == SDL_SCANCODE_RETURN)
        return false;

    if (sdlScancode == SDL_SCANCODE_RETURN2)
        return false;

    return true;
};


/*static*/ const char* CWebSpecific::GetKeyName(int32 sdlScancode)
{
    int32 key = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(sdlScancode), SDL_KMOD_NONE, true);
    return SDL_GetKeyName(key);
};


/*static*/ void CWebSpecific::GetLocalTime(SYSTEMTIME* st)
{
    std::time_t time =
        std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );

    std::tm* tm = std::localtime(&time);

    if (tm)
    {
        st->year      = tm->tm_year + 1900;
        st->month     = tm->tm_mon + 1;
        st->dayOfWeek = tm->tm_wday;
        st->day       = tm->tm_mday;
        st->hours     = tm->tm_hour;
        st->minutes   = tm->tm_min;
        st->seconds   = tm->tm_sec;
    };
};


/*static*/ bool CWebSpecific::IsMobilePlatform(void)
{
    if (CConfigure::CheckArg("mobile"))
        return true;

    int32 isMobile = EM_ASM_INT({
        if (navigator.userAgentData && navigator.userAgentData.mobile != undefined) {
            return navigator.userAgentData.mobile ? 1 : 0;
        } else {
            return (/Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent)) ? 1 : 0;
    }});

    return (isMobile != 0);
};


/*static*/ void CWebSpecific::VibrateMobile(int32 milliseconds)
{
    EM_ASM({
       if (typeof navigator !== "undefined" &&
           typeof navigator.vibrate === "function") {
           navigator.vibrate($0);
       }
    }, milliseconds);
};


/*static*/ void CWebSpecific::GetCanvasSize(int32* w, int32* h)
{
    emscripten_get_canvas_element_size("#canvas", w, h);
};


/*static*/ void CWebSpecific::SetCanvasFocus(void)
{
    EM_ASM({
        var canvas = document.getElementById("canvas");
        if (canvas) {
            canvas.focus();
        }
    });
};


/*static*/ void CWebSpecific::RequestFullscreen(bool bLock /*= true*/)
{
    uint64 flags = SDL_GetWindowFlags(m_pWindow);

    if ((flags & SDL_WINDOW_FULLSCREEN) == 0)
    {
        SDL_SetWindowFullscreen(CWebSpecific::m_pWindow, true);

        if (bLock)
        {
            EM_ASM({
                if (screen.orientation && screen.orientation.lock) {
                    screen.orientation.lock("landscape");
                }
            });
        };
    };
};


/*static*/ bool CWebSpecific::IsInFullscreen(void)
{
    uint64 flags = SDL_GetWindowFlags(m_pWindow);

    if ((flags & SDL_WINDOW_FULLSCREEN) != 0)
        return true;

    return false;
};


/*static*/ void CWebSpecific::WaitForMainModuleReady(void)
{
    while (!s_bIsMainModuleReady)
        emscripten_sleep(0);
};


/*static*/ void CWebSpecific::WaitUserActive(void)
{
    const char* pszScript =
        "navigator.userActivation ? navigator.userActivation.isActive : true";

    while (!emscripten_run_script_int(pszScript))
        emscripten_sleep(0);
};