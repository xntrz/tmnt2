#pragma once

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>


class CPCSoundDevice
{
public:
    CPCSoundDevice(void);
    virtual ~CPCSoundDevice(void) {};
    bool Initialize(void);
    void Terminate(void);
    bool InitializeLib(void);
    void TerminateLib(void);
    
private:
    IDirectSound8* m_pDs;
    IDirectSoundBuffer* m_pDsBuffer;
    void* m_pSoundHeap;
};