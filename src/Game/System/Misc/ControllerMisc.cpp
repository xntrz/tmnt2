#include "ControllerMisc.hpp"
#include "Gamepad.hpp"

#ifdef TARGET_PC
#include "System/PC/PCSpecific.hpp"
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
    if (iController < 0 || iController > IGamepad::Max())
        iController = 0;
    
    if (IGamepad::Lock(iController))
        return IGamepad::GetController(IGamepad::GetPhysicalPort(iController));
    
    return -1;
};


int32 FindTriggeredController(uint32 uButton, bool bUnlocked)
{
    int32 iControllerMax = IGamepad::Max();

    for (int32 i = 0; i < iControllerMax; ++i)
    {
        if (IGamepad::GetDigitalTrigger(i, uButton) && !IGamepad::IsLocked(i))
            return i;
    };

    return -1;
};


void EnableStickToDirButton(bool bEnable)
{
    int32 iControllerMax = IGamepad::Max();

    for (int32 i = 0; i < iControllerMax; ++i)
    {
#ifdef TARGET_PC
        if (IGamepad::GetPhysicalPort(i) == CPCSpecific::GetKeyboradPort())
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
#ifdef TARGET_PC
    return (CPCSpecific::GetKeyboradPort() == IGamepad::GetPhysicalPort(iController));
#else
    return false;
#endif    
};