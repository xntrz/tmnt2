#include "PCSoundDevice.hpp"

#include <dsound.h>
#include "cri_adxcs.h"
#include "adx_dsound8.h"

#include <ks.h>
#include <ksmedia.h>

static IDirectSound8* s_pDirectSound;
static IDirectSoundBuffer* s_pSoundBuffer;


bool PCSddInitializeADX(void* hWnd)
{
    if (SUCCEEDED(DirectSoundCreate8(0, &s_pDirectSound, 0)))
    {
        if (SUCCEEDED(s_pDirectSound->SetCooperativeLevel(HWND(hWnd), DSSCL_PRIORITY)))
        {
            DSBUFFERDESC DsBufferDesc;
            std::memset(&DsBufferDesc, 0x00, sizeof(DsBufferDesc));

            DsBufferDesc.dwSize = sizeof(DsBufferDesc);
            DsBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
            if (SUCCEEDED(s_pDirectSound->CreateSoundBuffer(&DsBufferDesc, &s_pSoundBuffer, 0)))
            {
                WAVEFORMATEX WaveFormatEx;
                std::memset(&WaveFormatEx, 0x00, sizeof(WaveFormatEx));
                WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
                WaveFormatEx.nChannels = 2;
                WaveFormatEx.nSamplesPerSec = 48000;
                WaveFormatEx.wBitsPerSample = 16;
                WaveFormatEx.nBlockAlign = 4;
                WaveFormatEx.nAvgBytesPerSec = 192000;
                if (FAILED(s_pSoundBuffer->SetFormat(&WaveFormatEx)) || (WaveFormatEx.nSamplesPerSec != 48000))
                {
                    WaveFormatEx.nSamplesPerSec = 44100;
                    WaveFormatEx.nAvgBytesPerSec = 44100 * WaveFormatEx.nBlockAlign;
                    s_pSoundBuffer->SetFormat(&WaveFormatEx);
                };
                
                ADXPC_SetupSoundDirectSound8(s_pDirectSound);
                
                return true;
            };
        };
    };

    return false;
};


void PCSddTerminateADX(void)
{
    ADXPC_ShutdownSound();

    if (s_pSoundBuffer)
    {
        s_pSoundBuffer->Release();
        s_pSoundBuffer = nullptr;
    };

    if (s_pDirectSound)
    {
        s_pDirectSound->Release();
        s_pDirectSound = nullptr;
    };
};


bool PCSddInitializeFramework(void)
{
    return true;
};


void PCSddTerminateFramework(void)
{
    ;
};