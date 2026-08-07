#include "ControllerMisc.hpp"
#include "Gamepad.hpp"

#if defined(TARGET_PC)
#include "System/PC/PCSpecific.hpp"
#elif defined(TARGET_WEB)
#include "System/Web/WebSpecific.hpp"
#else
#error Not implemented for current target
#endif


int32 LockTriggeredController(uint32 uButton)
{
    int32 iControllerMax = IGamepad::Max();
        
    for (int32 i = 0; i < iControllerMax; ++i)
    {
        if (IGamepad::GetDigitalTrigger(i, uButton) && !IGamepad::IsLocked(i))
            return LockAndSearchController(i);
    };

    return -1;
};


void UnlockAllControllers(void)
{
    int32 iControllerMax = IGamepad::Max();

    for (int32 i = 0; i < iControllerMax; ++i)
    {
        if (IGamepad::IsLocked(i))
            IGamepad::Unlock(i);
    };
};


int32 LockAndSearchController(int32 iController)
{
    ASSERT(iController >= 0);
    ASSERT(iController < IGamepad::Max());

    if (iController < 0)
        iController = 0;

    if (iController >= IGamepad::Max())
        iController = 0;

    int32 physicalPort = IGamepad::GetPhysicalPort(iController);

    if (IGamepad::Lock(iController))
        return IGamepad::GetController(physicalPort);
    
    return -1;
};


int32 FindTriggeredController(uint32 button, bool bCheckLocked)
{
    int32 iControllerMax = IGamepad::Max();

    for (int32 i = 0; i < iControllerMax; ++i)
    {
        if (IGamepad::GetDigitalTrigger(i, button))
        {
            if (bCheckLocked && !IGamepad::IsLocked(i))
                continue;

            return i;
        };
    };

    return -1;
};


void EnableStickToDirButton(bool bEnable)
{
    int32 iControllerMax = IGamepad::Max();

    for (int32 i = 0; i < iControllerMax; ++i)
    {
#if defined(TARGET_PC)
        if (IGamepad::GetPhysicalPort(i) == CPCSpecific::GetKeyboradPort())
            IGamepad::EnableStickToDigitalMapping(i, IGamepad::STICK_LEFT, false);
        else
            IGamepad::EnableStickToDigitalMapping(i, IGamepad::STICK_LEFT, bEnable);
#elif defined(TARGET_WEB)
        if (IGamepad::GetPhysicalPort(i) == CWebSpecific::GetKeyboradPort())
            IGamepad::EnableStickToDigitalMapping(i, IGamepad::STICK_LEFT, false);
        else
            IGamepad::EnableStickToDigitalMapping(i, IGamepad::STICK_LEFT, bEnable);
#else
        CController::EnableStickToDigitalMapping(i, CController::STICK_LEFT, bEnable);
#endif  
    };
};


bool ControllerIsKeyboard(int32 iController)
{
#if defined(TARGET_PC)
    return (CPCSpecific::GetKeyboradPort() == IGamepad::GetPhysicalPort(iController));
#elif defined(TARGET_WEB)
    return (CWebSpecific::GetKeyboradPort() == IGamepad::GetPhysicalPort(iController));
#else
    return false;
#endif    
};


bool ControllerIsTouch(int32 iController)
{
#if defined(TARGET_WEB)
    if (CWebSpecific::IsMobilePlatform())
        return (CWebSpecific::GetTouchPort() == IGamepad::GetPhysicalPort(iController));
#endif /* defined(TARGET_WEB) */
    
    return false;
};