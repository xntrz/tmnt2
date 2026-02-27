#include "Gamepad.hpp"
#include "ControllerMisc.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Screen.hpp"


/*static*/ int32 IGamepad::m_aLockedPhysicalPort[] = { -1, -1, -1, -1 };


/*static*/ bool IGamepad::CheckFunction(uint32 uDigital, FUNCTION function)
{
    ASSERT(function >= 0);
    ASSERT(function < FUNCTION_MAX);

    uint32 uDigitalFunction = (1 << (16 + function));

    return ((uDigital & uDigitalFunction) != 0);
};


/*static*/ void IGamepad::ClearAllButtonFunction(int32 iController)
{
    if (isExist(iController))
    {
        const int32 iFunctionMax = BITSOF(CController::DIGITAL) / 2;

        for (int32 i = 0; i < iFunctionMax; ++i)
            CController::Mapping(iController, 1 << i, 1 << i);        
    };    
};


/*static*/ void IGamepad::AppendButtonFunction(int32 iController, uint32 uBasicButton, FUNCTION function)
{
    if (isExist(iController))
    {
        ASSERT(function >= 0);
        ASSERT(function < FUNCTION_MAX);
        ASSERT(uBasicButton != 0);

        uint32 uDigitalMapping = CController::GetMapping(iController, uBasicButton);
        uint32 uVirtualButton = IPadFunctionMask(function) | uDigitalMapping;

        CController::Mapping(iController, uBasicButton, uVirtualButton);
    };
};


/*static*/ uint32 IGamepad::ConvertToVirtualButton(uint32 uBasicButton)
{
#ifdef _TARGET_PS2    
    static uint32 s_aDefaultFuncToDigitMapping[] =
    {
        CController::DIGITAL_RDOWN,
        CController::DIGITAL_RLEFT,
        CController::DIGITAL_RUP,
        CController::DIGITAL_RRIGHT,
        CController::DIGITAL_L1,
        CController::DIGITAL_R1,
        CController::DIGITAL_L2,
        CController::DIGITAL_LUP,
        CController::DIGITAL_LDOWN
    };

    static_assert(COUNT_OF(s_aDefaultFuncToDigitMapping) == FUNCTION_MAX, "update me");

    uint32 uVirtualButton = 0;

    for (int32 i = 0; i < FUNCTION_MAX; ++i)
    {
        if (s_aDefaultFuncToDigitMapping[i] & uBasicButton)
            uVirtualButton |= IPadFunctionMask(i);
    };

    return uVirtualButton;
#else
    return 0;
#endif
};


/*static*/ bool IGamepad::StartVibration(int32 iController, VIBRATIONTYPE type, float fTime)
{
    ASSERT(type >= 0);
    ASSERT(type < VIBRATIONTYPE_MAX);
    ASSERT(fTime >= 0.0f);
    
    if (isExist(iController) && !CGameData::Attribute().IsPlayDemoMode())
    {
        static const uint32 aVibMax[] =
        {
//#ifdef TARGET_PC
            0x3FFF,
            0x7FFF,
            0xBFFF,
//#else
//#error Not implemented for current target
//#endif
        };

        static_assert(COUNT_OF(aVibMax) == VIBRATIONTYPE_MAX, "update me");

        int32 vibrateFrames = static_cast<int32>(CScreen::Framerate() * fTime);
        if (vibrateFrames > 0)
            return CController::StartVibration(iController, aVibMax[type], vibrateFrames);
    };

    return false;
};


/*static*/ void IGamepad::SaveLockedState(void)
{
    int32 iControllerMax = Max();

    for (int32 i = 0; i < iControllerMax; ++i)
        m_aLockedPhysicalPort[i] = (IsLocked(i) ? GetPhysicalPort(i) : -1);
};


/*static*/ void IGamepad::RestoreLockedState(void)
{
    UnlockAllControllers();
    
    int32 iControllerMax = Max();

    for (int32 i = 0; i < iControllerMax; ++i)
    {
        if (m_aLockedPhysicalPort[i] != -1)
            IGamepad::Lock(IGamepad::GetController(m_aLockedPhysicalPort[i]));
    };
};


/*static*/ bool IGamepad::isExist(int32 iController)
{
    int32 controllerMax = Max();
    
    return ((iController >= 0) &&
            (iController < controllerMax));
};