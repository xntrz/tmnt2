#pragma once

#include "System/Common/Controller.hpp"


#define IPadFunctionMask(Function) (1 << (16 + (Function)))


class IGamepad : public CController
{
public:
    enum
    {
        CONTROLLER_1 = 0,
        CONTROLLER_2,
        CONTROLLER_3,
        CONTROLLER_4,
    };

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
        VIBRATIONTYPE_LOW = 0,
        VIBRATIONTYPE_NORMAL,
        VIBRATIONTYPE_HARD,
        
        VIBRATIONTYPE_MAX,
    };

public:
    static bool CheckFunction(uint32 uDigital, FUNCTION function);    
    static void ClearAllButtonFunction(int32 iController);    
    static void AppendButtonFunction(int32 iController, uint32 uBasicButton, FUNCTION function);
    static uint32 ConvertToVirtualButton(uint32 uBasicButton);
    static bool StartVibration(int32 iController, VIBRATIONTYPE type, float fTime);    
    static void SaveLockedState(void);
    static void RestoreLockedState(void);
    
private:
    static bool isExist(int32 iController);

private:
    static int32 m_aLockedPhysicalPort[];
};


using IPad = IGamepad;