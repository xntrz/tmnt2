#include "Gamepad.hpp"
#include "Keyboard.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Screen.hpp"


/*static*/ int32 CGamepad::Invalid(void)
{
    return CController::CONTROLLER_LOCKED_ON_VIRTUAL;
};


/*static*/ int32 CGamepad::Max(void)
{
    return CController::Max();
};


/*static*/ bool CGamepad::CheckFunction(uint32 uDigital, FUNCTION function)
{
    ASSERT(function >= 0 && function < FUNCTION_MAX);

    return FLAG_TEST(uDigital, BIT(CController::FUNCTIONAL_BEGIN + function));
};


/*static*/ void CGamepad::AppendButtonFunction(int32 iController, FUNCTION function, uint32 uBasicButton)
{
    if (iController >= 0 && iController < CController::Max())
    {
        ASSERT(function >= 0 && function < FUNCTION_MAX);
        ASSERT(uBasicButton != 0);

        uint32 uDigitalMapping = CController::GetMapping(iController, uBasicButton);
        uint32 uVirtualButton = (1 << (function + CController::FUNCTIONAL_BEGIN)) | uDigitalMapping;
        
        CController::Mapping(iController, uBasicButton, uVirtualButton);
    };
};


/*static*/ void CGamepad::ClearAllButtonFunction(int32 iController)
{
    if (iController >= 0 && iController < CController::Max())
    {
        const int32 iFunctionMax = BITSOF(CController::DIGITAL) / 2;

        for (int32 i = 0; i < iFunctionMax; ++i)
        {
            CController::Mapping(
                iController,
                1 << i,
                1 << i
            );
        };
    };    
};


/*static*/ bool CGamepad::StartVibration(int32 iController, VIBRATIONTYPE type, float fTime)
{
    ASSERT(type >= 0 && type < VIBRATIONTYPE_MAX);
    ASSERT(fTime >= 0.0f);
    
    if (iController >= 0 && iController < CController::Max() &&
        !CGameData::Attribute().IsPlayDemoMode())
    {
        static const uint32 VibrationTypeValue[] =
        {
            0x3FFF,
            0x7FFF,
            0xBFFF,
        };

        static_assert(COUNT_OF(VibrationTypeValue) == VIBRATIONTYPE_MAX, "update me");

        int32 iVibrateFrames = int32(float(CScreen::Framerate()) * fTime);
        if (iVibrateFrames > 0)
            return CController::StartVibration(iController, VibrationTypeValue[type], iVibrateFrames);
    };

    return false;
};


/*static*/ void CGamepad::EnableStickToDigitalMapping(bool bEnable)
{
    for (int32 i = 0; i < CController::Max(); ++i)
    {
#ifdef _TARGET_PC
        if (CController::GetPhysicalPort(i) == CKeyboard::GetPort())
            CController::EnableStickToDigitalMapping(i, CController::STICK_LEFT, false);
        else
            CController::EnableStickToDigitalMapping(i, CController::STICK_LEFT, bEnable);            
#else
        CController::EnableStickToDigitalMapping(i, CController::STICK_LEFT, bEnable);
#endif        
    };
};


/*static*/ uint32 CGamepad::ConvertToVirtualButton(uint32 uBasicButton)
{
    static uint32 DefaultFunctionToDigitalMapping[] =
    {
        CController::DIGITAL_A,
        CController::DIGITAL_B,
        CController::DIGITAL_Y,
        CController::DIGITAL_X,
        CController::DIGITAL_LEFT_BUMPER, 
        CController::DIGITAL_LEFT_TRIGGER,
        CController::DIGITAL_RIGHT_BUMPER,
        CController::DIGITAL_LEFT_THUMB,  
        CController::DIGITAL_RIGHT_THUMB, 
    };

    static_assert(COUNT_OF(DefaultFunctionToDigitalMapping) == FUNCTION_MAX, "update me");

    uint32 uVirtualButton = 0;
    
    for (int32 i = 0; i < FUNCTION_MAX; ++i)
    {
        if (FLAG_TEST(DefaultFunctionToDigitalMapping[i], uBasicButton))
            uVirtualButton |= (1 << (i + CController::FUNCTIONAL_BEGIN));
    };

    return uVirtualButton;
};


/*static*/ bool CGamepad::IsKeyboard(int32 iController)
{
    return (CKeyboard::GetPort() == CController::GetPhysicalPort(iController));
};


/*static*/ int32 CGamepad::GetKeyboardController(void)
{
    return CController::GetController(CKeyboard::GetPort());
};


/*static*/ void CGamepad::DigitalCancelToFunction(bool bEnable)
{
    FLAG_CHANGE(CController::DIGITAL_CANCEL, CController::DIGITAL_B, bEnable);
};