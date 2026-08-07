#pragma once

#include "System/Common/InputsDevice.hpp"


class CWebInputsDevice : public CInputsDevice
{
public:
    CWebInputsDevice(void);
    virtual ~CWebInputsDevice(void) {};
    virtual void SyncVirtualController(void) override;
    virtual void Sync(void) override;
    virtual bool Start(void) override;
    virtual void Stop(void) override;
    void OnJoystickAdd(uint32 joystickID);
    void OnJoystickRem(uint32 joystickID);

private:
    int32 m_iTouchPhysicalPort;
};