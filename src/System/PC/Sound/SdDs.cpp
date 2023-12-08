#include "SdDs.hpp"

#include <mmreg.h>


static IDirectSound8* SdDS8Instance;
static DWORD SdDSConfig = 0;
static int32 SdDS8BuffCnt = 0;


bool SdDsInitialize(HWND hWnd)
{
    HRESULT hResult = DirectSoundCreate8(0, &SdDS8Instance, 0);
    if (FAILED(hResult))
        return false;

    hResult = IDirectSound8_SetCooperativeLevel(SdDS8Instance, hWnd, DSSCL_PRIORITY);
    if (FAILED(hResult))
        return false;

    hResult = IDirectSound8_GetSpeakerConfig(SdDS8Instance, &SdDSConfig);
    if (FAILED(hResult))
        return false;

    SdDS8BuffCnt = 0;

    return true;
};


void SdDsTerminate(void)
{
    ASSERT(SdDS8BuffCnt == 0);

    if (SdDS8Instance)
    {
        IDirectSound8_Release(SdDS8Instance);
        SdDS8Instance = nullptr;
    };
};


bool SdDsBuffCreate(LPDIRECTSOUNDBUFFER* SdBuff, DSBUFFERDESC* SdBuffDesc)
{
    if (!SdDS8Instance)
        return false;

    if (FAILED(IDirectSound8_CreateSoundBuffer(SdDS8Instance, SdBuffDesc, (LPDIRECTSOUNDBUFFER*)SdBuff, NULL)))
        return false;

    ++SdDS8BuffCnt;
    
    return true;
};


void SdDsBuffDestroy(LPDIRECTSOUNDBUFFER SdBuff)
{
    ASSERT(SdDS8BuffCnt > 0);

    IDirectSoundBuffer_Release(SdBuff);
    --SdDS8BuffCnt;
};


void SdDsBuffLoadData(LPDIRECTSOUNDBUFFER SdBuff, const void* Data, uint32 Size)
{
    LPVOID  Buff1   = nullptr;
    DWORD   cbBuff1 = 0;
    LPVOID  Buff2   = nullptr;
    DWORD   cbBuff2 = 0;

    HRESULT hResult = IDirectSoundBuffer_Lock(SdBuff, 0, 0, &Buff1, &cbBuff1, &Buff2, &cbBuff2, DSBLOCK_ENTIREBUFFER);
    if (SUCCEEDED(hResult))
    {
        std::memcpy(Buff1, Data, Size);

        if (Buff2)
            std::memcpy(Buff2, &((const uint8*)Data)[cbBuff1], cbBuff2);

        IDirectSoundBuffer_Unlock(SdBuff, Buff1, cbBuff1, Buff2, cbBuff2);
    }
    else
    {
        ASSERT(false);
    };
};


void SdDsBuffLoadDataEx(LPDIRECTSOUNDBUFFER SdBuff, uint32 Offset, const void* Data, uint32 Size)
{
    LPVOID  Buff1 = nullptr;
    DWORD   cbBuff1 = 0;
    LPVOID  Buff2 = nullptr;
    DWORD   cbBuff2 = 0;

    HRESULT hResult = IDirectSoundBuffer_Lock(SdBuff, Offset, Size, &Buff1, &cbBuff1, &Buff2, &cbBuff2, 0);
    if (SUCCEEDED(hResult))
    {
        std::memcpy(Buff1, Data, Size);

        if (Buff2)
            std::memcpy(Buff2, &((const uint8*)Data)[cbBuff1], cbBuff2);

        IDirectSoundBuffer_Unlock(SdBuff, Buff1, cbBuff1, Buff2, cbBuff2);
    }
    else
    {
        ASSERT(false);
    };
};


int32 SdDsBuffSetPan(LPDIRECTSOUNDBUFFER SdBuff, int32 ch_l, int32 ch_r)
{
    int32 ch_h = ch_l;

    if (ch_h < ch_r)
        ch_h = ch_r;

    int32 l = ch_l - ch_h;
    int32 r = ch_r - ch_h;

    IDirectSoundBuffer_SetPan(SdBuff, (l <= r ? -l : r));

    return ch_h;
};


uint32 SdDsSetWaveFormat(WAVEFORMATEX* WaveFormat, SdWaveDataHdr_t* SdWaveDataHdr)
{
    std::memset(WaveFormat, 0, sizeof(*WaveFormat));

    int32 Channels      = 0;
    int32 BitsPerSample = 0;
    int32 AvgBytesPerSec= 0;

    if (SdWaveDataHdr->Format == WAVE_FORMAT_PCM)
    {
        Channels = SdWaveDataHdr->Channels;
        BitsPerSample = 8 * (SdWaveDataHdr->BlockAlign != 1) + 8;
        AvgBytesPerSec = SdWaveDataHdr->BlockAlign * SdWaveDataHdr->SamplesPerSec;

        WaveFormat->cbSize = 0;
    }
    else if (SdWaveDataHdr->Format == WAVE_FORMAT_VOXWARE_BYTE_ALIGNED)
    {
        Channels = SdWaveDataHdr->Channels;
        BitsPerSample = 4;
        AvgBytesPerSec = (SdWaveDataHdr->BlockAlign * SdWaveDataHdr->SamplesPerSec) / 64;

        ((ADPCMEWAVEFORMAT*)WaveFormat)->wfx.cbSize = 2;
        ((ADPCMEWAVEFORMAT*)WaveFormat)->wSamplesPerBlock = 64;
    }
    else
    {
        return 0;
    };

    WaveFormat->nChannels       = Channels;
    WaveFormat->wFormatTag      = SdWaveDataHdr->Format;
    WaveFormat->nSamplesPerSec  = SdWaveDataHdr->SamplesPerSec;
    WaveFormat->wBitsPerSample  = BitsPerSample;
    WaveFormat->nAvgBytesPerSec = AvgBytesPerSec;
    WaveFormat->nBlockAlign     = SdWaveDataHdr->BlockAlign;

    return sizeof(DSBUFFERDESC);
};