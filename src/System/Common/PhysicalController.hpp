#pragma once

#include "Controller.hpp"


class IPhysicalController
{
public:
    struct INFO
    {
        int32 m_iPhysicalPort;
        CController::STATE m_state;
        uint32 m_digital;
        uint32 m_digitalOld;
        uint32 m_digitalTrigger;
        uint32 m_digitalRelease;
        uint32 m_digitalRepeat;
        int32 m_aDigitalRepeatTime[CController::DIGITAL_NUM];
        int32 m_aDigitalRepeatDelay[CController::DIGITAL_NUM];
        int16 m_aAnalog[CController::ANALOG_NUM];
    };

public:
    IPhysicalController(void);
    virtual ~IPhysicalController(void);
    virtual void Close(void) = 0;
    virtual void Update(void);
    virtual void Clear(void);
    virtual void SetVibration(uint32 type, int32 iVibFrame);
    const INFO& Info(void) const;
    void SetVibrationEnable(bool bEnable);
    uint32* GetDigitalMapping(uint32 digital);
    void EnableStickToDigitalMapping(CController::STICK stick, bool bEnable);
    static uint32 ConvertAnalogStickToDigital(CController::STICK stick, int16 x, int16 y);

protected:
    INFO m_info;
    bool m_bVibrate;
    uint32 m_uVibType;
    int32 m_iVibFrame;
    bool m_bVibrateEnable;
    bool m_abStickToDigital[CController::STICK_NUM];
    uint32 m_aDigitalMapping[CController::DIGITAL_NUM];
    uint32 m_aAnalogMapping[CController::ANALOG_NUM];
};