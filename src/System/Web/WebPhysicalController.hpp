#pragma once

#include "System/Common/Controller.hpp"

#include <SDL3/SDL_scancode.h>


class CWebPhysicalController
{
public:
    static int32 PHYSICALCONTROLLER_MAX;
    
public:
    static bool Initialize(void);
    static void Terminate(void);
    static bool OnJoystickAdd(uint32 joystickID);
    static IPhysicalController* Open(int32 iController, uint32 joystickID = 0);
    static void MapDigital(uint32 btn, int32 sdlScancode);
    static void MapDigitalFixed(uint32 btn, int32 sdlScancode);
    static void MapAnalog(CController::ANALOG analog, int32 sdlScancodeX, int32 sdlScancodeY);
    static bool IsKeyDown(int32 sdlScancode);
    static bool IsKeyTrigger(int32 sdlScancode);
    static bool IsKeyNotFixed(int32 sdlScancode);
    static int32 GetDownKey(void);
    static int32 GetPort(void);
    static int32 GetTouchPort(void);
};