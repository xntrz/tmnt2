#pragma once

#include "System/Common/SoundDevice.hpp"

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>


class CPCSoundDevice final : public CSoundDevice
{
public:
    CPCSoundDevice(void);
    virtual ~CPCSoundDevice(void);
    virtual bool Initialize(void) override;
    virtual void Terminate(void) override;
    virtual bool StartupFramework(void) override;
    virtual void ShutdownFramework(void) override;
    
private:
    uint8* m_pSoundHeap;
    IDirectSound8* m_pDs;
    IDirectSoundBuffer* m_pDsBuffer;
};