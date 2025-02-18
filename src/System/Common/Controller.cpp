#include "Controller.hpp"
#include "InputsDevice.hpp"


/*static*/ uint32 CController::DIGITAL_OK     = CController::DIGITAL_RDOWN;
/*static*/ uint32 CController::DIGITAL_CANCEL = CController::DIGITAL_RRIGHT;

/*static*/ const uint32 CController::DIGITAL_MASK = CController::DIGITAL_LUP
                                                  | CController::DIGITAL_LDOWN 	
                                                  | CController::DIGITAL_LLEFT 	
                                                  | CController::DIGITAL_LRIGHT 
                                                  | CController::DIGITAL_RUP 	
                                                  | CController::DIGITAL_RDOWN 	
                                                  | CController::DIGITAL_RLEFT 	
                                                  | CController::DIGITAL_RRIGHT 
                                                  | CController::DIGITAL_SELECT 
                                                  | CController::DIGITAL_START 	
                                                  | CController::DIGITAL_L1 	
                                                  | CController::DIGITAL_L2 	
                                                  | CController::DIGITAL_L3 	
                                                  | CController::DIGITAL_R1 	
                                                  | CController::DIGITAL_R2 	
                                                  | CController::DIGITAL_R3;


/*static*/ int32 CController::Max(void)
{
    return CInputsDevice::Instance().Max();
};


/*static*/ bool CController::Lock(int32 iController)
{
    ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
    ASSERT(iController < Max());
    
    return CInputsDevice::Instance().Lock(iController);
};


/*static*/ bool CController::Unlock(int32 iController)
{
    ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
    ASSERT(iController < Max());
    
    return CInputsDevice::Instance().Unlock(iController);
};


/*static*/ bool CController::IsLocked(int32 iController)
{
    ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
    ASSERT(iController < Max());
    
    return CInputsDevice::Instance().IsLocked(iController);
};


/*static*/ CController::STATE CController::GetState(int32 iController)
{
    ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
    ASSERT(iController < Max());
    
    return CInputsDevice::Instance().ControllerResource(iController).Info().m_eState;
};


/*static*/ int32 CController::GetPhysicalPort(int32 iController)
{
    ASSERT(iController != CONTROLLER_LOCKED_ON_VIRTUAL && iController != CONTROLLER_UNLOCKED_ON_VIRTUAL);
    ASSERT(iController < Max());
    
    return CInputsDevice::Instance().ControllerResource(iController).Info().m_iPhysicalPort;
};


/*static*/ int32 CController::GetController(int32 iPhysicalPort)
{
    for (int32 i = 0; i < Max(); i++)
    {
        if (GetPhysicalPort(i) == iPhysicalPort)
            return i;
    };

    return -1;
};


/*static*/ uint32 CController::GetDigital(int32 iController)
{
    switch (iController)
    {
    case CONTROLLER_LOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetLockedVirtualController().m_digital;
        
    case CONTROLLER_UNLOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetUnlockedVirtualController().m_digital;

    default:
        return CInputsDevice::Instance().ControllerResource(iController).Info().m_digital;
    };
};


/*static*/ bool CController::GetDigital(int32 iController, uint32 btn)
{
    return (GetDigital(iController) & btn) != 0;
};


/*static*/ uint32 CController::GetDigitalRepeat(int32 iController)
{
    switch (iController)
    {
    case CONTROLLER_LOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetLockedVirtualController().m_digitalRepeat;

    case CONTROLLER_UNLOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetUnlockedVirtualController().m_digitalRepeat;

    default:
        return CInputsDevice::Instance().ControllerResource(iController).Info().m_digitalRepeat;
    };
};


/*static*/ bool CController::GetDigitalRepeat(int32 iController, uint32 btn)
{
    return (GetDigitalRepeat(iController) & btn) != 0;
};


/*static*/ uint32 CController::GetDigitalTrigger(int32 iController)
{
    switch (iController)
    {
    case CONTROLLER_LOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetLockedVirtualController().m_digitalTrigger;

    case CONTROLLER_UNLOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetUnlockedVirtualController().m_digitalTrigger;

    default:
        return CInputsDevice::Instance().ControllerResource(iController).Info().m_digitalTrigger;
    };
};


/*static*/ bool CController::GetDigitalTrigger(int32 iController, uint32 btn)
{
    return (GetDigitalTrigger(iController) & btn) != 0;
};


/*static*/ uint32 CController::GetDigitalRelease(int32 iController)
{
    switch (iController)
    {
    case CONTROLLER_LOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetLockedVirtualController().m_digitalRelease;

    case CONTROLLER_UNLOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetUnlockedVirtualController().m_digitalRelease;

    default:
        return CInputsDevice::Instance().ControllerResource(iController).Info().m_digitalRelease;
    };
};


/*static*/ bool CController::GetDigitalRelease(int32 iController, uint32 btn)
{
    return (GetDigitalRelease(iController) & btn) != 0;
};


/*static*/ int16 CController::GetAnalog(int32 iController, ANALOG analog)
{
    ASSERT(analog < ANALOG_NUM);
    
    switch (iController)
    {
    case CONTROLLER_LOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetLockedVirtualController().m_aAnalog[analog];

    case CONTROLLER_UNLOCKED_ON_VIRTUAL:
        return CInputsDevice::Instance().GetUnlockedVirtualController().m_aAnalog[analog];

    default:
        ASSERT(iController < Max());
        return CInputsDevice::Instance().ControllerResource(iController).Info().m_aAnalog[analog];
    };
};


/*static*/ void CController::EnableStickToDigitalMapping(int32 iController, STICK stick, bool bEnable)
{
    if ((iController == CONTROLLER_LOCKED_ON_VIRTUAL) ||
        (iController == CONTROLLER_UNLOCKED_ON_VIRTUAL))
        return;

    ASSERT(iController < Max());

    CInputsDevice::Instance().ControllerResource(iController).EnableStickToDigitalMapping(stick, bEnable);
};


/*static*/ void CController::EnableVibration(int32 iController, bool bEnable)
{
    ASSERT(iController < Max());
    
    if (!bEnable)
        CInputsDevice::Instance().ControllerResource(iController).SetVibration(0, 0);
    
    CInputsDevice::Instance().ControllerResource(iController).EnableVibration(bEnable);
};


/*static*/ bool CController::StartVibration(int32 iController, uint32 uVibMax, int32 iVibFrame)
{
    ASSERT(iController < Max());
    
    CInputsDevice::Instance().ControllerResource(iController).SetVibration(uVibMax, iVibFrame);
    return true;
};


/*static*/ void CController::Mapping(int32 iController, uint32 btnPhysical, uint32 btnVirtual)
{
    ASSERT(iController < Max());

    *CInputsDevice::Instance().ControllerResource(iController).GetDigitalMapping(btnPhysical) = btnVirtual;
};


/*static*/ uint32 CController::GetMapping(int32 iController, uint32 btnPhysical)
{
    ASSERT(iController < Max());
    
    return *CInputsDevice::Instance().ControllerResource(iController).GetDigitalMapping(btnPhysical);
};


/*static*/ void CController::Mapping(int32 iController, ANALOG analogPhysical, ANALOG analogVirtual)
{
    ASSERT(iController < Max());

    *CInputsDevice::Instance().ControllerResource(iController).GetAnalogMapping(analogPhysical) = analogVirtual;
};


/*static*/ CController::ANALOG CController::GetMapping(int32 iController, ANALOG analogPhysical)
{
    ASSERT(iController < Max());

    return *CInputsDevice::Instance().ControllerResource(iController).GetAnalogMapping(analogPhysical);
};


/*static*/ uint32 IPhysicalController::ConvertAnalogStickToDigital(CController::STICK stick, int16 x, int16 y)
{
    static uint32 dirbtnRightStick[4] =
    {
        CController::DIGITAL_RUP,
        CController::DIGITAL_RDOWN,
        CController::DIGITAL_RLEFT,
        CController::DIGITAL_RRIGHT,
    };

    static uint32 dirbtnLeftStick[4] =
    {
        CController::DIGITAL_LUP,
        CController::DIGITAL_LDOWN,
        CController::DIGITAL_LLEFT,
        CController::DIGITAL_LRIGHT,
    };

    const int16 trigger_max = TYPEDEF::SINT16_MAX / 2;
    const int16 trigger_min = TYPEDEF::SINT16_MIN / 2;

    uint32 digital = 0;
    uint32* dirbtn = (stick == CController::STICK_RIGHT ? dirbtnRightStick : dirbtnLeftStick);

    if ((x <  trigger_min) ||
        (x >= trigger_max) ||
        (y <  trigger_min) ||
        (y >= trigger_max))
    {
        if (x >= y)
            digital = (-x >= y ? dirbtn[1] : dirbtn[3]);
        else
            digital = (-x >= y ? dirbtn[2] : dirbtn[0]);
    };

    return digital;
};


IPhysicalController::IPhysicalController(void)
{
    m_info = { 0 };
    m_info.m_eState = CController::STATE_MAX;

    uint32 aDigitalMapping[] =
    {
          CController::DIGITAL_LUP,
        CController::DIGITAL_LDOWN,
          CController::DIGITAL_LLEFT,
          CController::DIGITAL_LRIGHT,
          CController::DIGITAL_RUP,
          CController::DIGITAL_RDOWN,
          CController::DIGITAL_RLEFT,
          CController::DIGITAL_RRIGHT,
          CController::DIGITAL_SELECT,
          CController::DIGITAL_START,
          CController::DIGITAL_L1,
          CController::DIGITAL_L2,
          CController::DIGITAL_L3,
          CController::DIGITAL_R1,
          CController::DIGITAL_R2,
          CController::DIGITAL_R3,
    };

    CController::ANALOG aAnalogMapping[] =
    {
        CController::ANALOG_LSTICK_X,
        CController::ANALOG_LSTICK_Y,
        CController::ANALOG_RSTICK_X,
        CController::ANALOG_RSTICK_Y,
        CController::ANALOG_RUP,
        CController::ANALOG_RDOWN,
        CController::ANALOG_RLEFT,
        CController::ANALOG_RRIGHT,
        CController::ANALOG_L1,
        CController::ANALOG_L2,
        CController::ANALOG_L3,
        CController::ANALOG_R1,
        CController::ANALOG_R2,
        CController::ANALOG_R3,
    };

    static_assert(sizeof(aDigitalMapping) == (sizeof(m_aDigitalMapping) / 2), "check out");
    static_assert(sizeof(aAnalogMapping) == sizeof(m_aAnalogMapping), "check out");

    std::memcpy(m_aDigitalMapping, aDigitalMapping, sizeof(aDigitalMapping));
    std::memcpy(m_aAnalogMapping, aAnalogMapping, sizeof(aAnalogMapping));
};


IPhysicalController::~IPhysicalController(void)
{
    ;
};


void IPhysicalController::Update(void)
{
    //
    //	Convert stick to digital
    //
    for (int32 i = 0; i < COUNT_OF(m_info.m_abStickToDigital); ++i)
    {
        if (m_info.m_abStickToDigital[i])
            m_info.m_digital |= ConvertAnalogStickToDigital(CController::STICK(i), m_info.m_aAnalog[0], m_info.m_aAnalog[1]);
    };

    //
    //	Apply analog mapping
    //
    int16 aAnalog[CController::ANALOG_NUM] = { 0 };
    
    for (int32 i = 0; i < COUNT_OF(aAnalog); ++i)
        aAnalog[i] = m_info.m_aAnalog[m_aAnalogMapping[i]];

    for (int32 i = 0; i < COUNT_OF(m_info.m_aAnalog); ++i)
        m_info.m_aAnalog[i] = aAnalog[i];

    //
    //	Apply digital mapping
    //
    uint32 digital = 0;
    
    for (int32 i = 0; i < CController::DIGITAL_NUM; ++i)
    {
        if (m_info.m_digital & (1 << i))
            digital |= m_aDigitalMapping[i];
    };

    //
    //	Update digital
    //
    m_info.m_digital 		= digital;
    m_info.m_digitalTrigger = m_info.m_digital & ~m_info.m_digitalOld;
    m_info.m_digitalRelease = m_info.m_digitalOld & ~m_info.m_digital;
    m_info.m_digitalRepeat 	= 0;

    //
    //	Update digital repeat
    //
    for (int32 i = 0; i < CController::DIGITAL_NUM; ++i)
    {
        static const int32 REPEAT_TIME_START = 12;
        static const int32 REPEAT_DELAY_START = 6;
        static const int32 REPEAT_DELAY_MIN = 2;

        if (m_info.m_digital & (1 << i))
        {
            if (m_info.m_aDigitalRepeatTime[i] == REPEAT_TIME_START)
                m_info.m_digitalRepeat |= (1 << i);

            if (m_info.m_aDigitalRepeatTime[i]-- == 1)
            {
                m_info.m_digitalRepeat |= (1 << i);
                m_info.m_aDigitalRepeatTime[i] = m_info.m_aDigitalRepeatDelay[i]--;
                if (m_info.m_aDigitalRepeatDelay[i] < REPEAT_DELAY_MIN)
                    m_info.m_aDigitalRepeatDelay[i] = REPEAT_DELAY_MIN;
            };
        }
        else
        {
            m_info.m_digitalRepeat &= ~(1 << i);
            m_info.m_aDigitalRepeatTime[i] = REPEAT_TIME_START;
            m_info.m_aDigitalRepeatDelay[i] = REPEAT_DELAY_START;
        };
    };
};


void IPhysicalController::Clear(void)
{
    m_info.m_digitalOld = m_info.m_digital;
    
    m_info.m_digital = 0;
    m_info.m_digitalTrigger = 0;
    m_info.m_digitalRelease = 0;
    m_info.m_digitalRepeat = 0;

    for (int32 i = 0; i < COUNT_OF(m_info.m_aAnalog); ++i)
        m_info.m_aAnalog[i] = 0;
};


void IPhysicalController::SetVibration(uint32 uVibMax, int32 iVibFrame)
{
    ASSERT(iVibFrame >= 0);

    m_info.m_uVibMax = (m_info.m_bVibEnable ? uVibMax : 0);
    m_info.m_iVibFrame = (m_info.m_bVibEnable ? iVibFrame : 0);
};


void IPhysicalController::EnableVibration(bool bEnable)
{
    m_info.m_bVibEnable = bEnable;
};


void IPhysicalController::EnableStickToDigitalMapping(CController::STICK stick, bool bEnable)
{
    ASSERT(stick >= 0);
    ASSERT(stick < CController::STICK_NUM);

    m_info.m_abStickToDigital[stick] = bEnable;
};


uint32* IPhysicalController::GetDigitalMapping(uint32 digital)
{
    //
    //	Extract one button
    //
    
    int32 i = 0;

    while (!(digital & 1))
    {
        digital >>= 1;
        ++i;
        
        ASSERT(i < BITSOF(CController::DIGITAL));
    };

    return &m_aDigitalMapping[i];
};


CController::ANALOG* IPhysicalController::GetAnalogMapping(CController::ANALOG analogPhysical)
{
    ASSERT(analogPhysical >= 0);
    ASSERT(analogPhysical < COUNT_OF(m_aAnalogMapping));

    return &m_aAnalogMapping[analogPhysical];
};