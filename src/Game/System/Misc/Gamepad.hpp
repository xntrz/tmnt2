#pragma once


class CGamepad
{
public:
    enum FUNCTION
    {
        FUNCTION_ATTACK_A = 0,
        FUNCTION_ATTACK_B,
        FUNCTION_SHOT,
        FUNCTION_JUMP,
        FUNCTION_DASH,
        FUNCTION_GUARD,
        FUNCTION_SWITCH_CHR,
        FUNCTION_SWITCH_CAM,
        FUNCTION_SWITCH_GAUGE,

        FUNCTION_MAX,
    };

    enum VIBRATIONTYPE
    {
        VIBRATIONTYPE_LOW,
        VIBRATIONTYPE_NORMAL,
        VIBRATIONTYPE_HARD,
        
        VIBRATIONTYPE_MAX,
    };

public:
    static int32 Invalid(void);
    static int32 Max(void);
    static bool CheckFunction(uint32 uDigital, FUNCTION function);
    static void AppendButtonFunction(int32 iController, FUNCTION function, uint32 uBasicButton);
    static void ClearAllButtonFunction(int32 iController);    
    static bool StartVibration(int32 iController, VIBRATIONTYPE type, float fTime);
    static void EnableStickToDigitalMapping(bool bEnable);
    static uint32 ConvertToVirtualButton(uint32 uBasicButton);
    static bool IsKeyboard(int32 iController);
    static int32 GetKeyboardController(void);
    static void DigitalCancelToFunction(bool bEnable);
};