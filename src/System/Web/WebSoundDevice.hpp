#pragma once

#include "System/Common/SoundDevice.hpp"


class CWebSoundDevice final : public CSoundDevice
{
public:
    CWebSoundDevice(void);
    virtual ~CWebSoundDevice(void);
    virtual bool Initialize(void) override;
    virtual void Terminate(void) override;
    virtual bool StartupFramework(void) override;
    virtual void ShutdownFramework(void) override;
};