#include "SdDs.hpp"
#include "SdDsTable.hpp"
#include "SdLog.hpp"

#include <mmreg.h>


static IDirectSound8* SdDS8Instance;
static DWORD SdDSConfig = 0;
static int32 SdDS8BuffCnt = 0;
static bool SdIsSpeaker5p1 = false;
static IDirectSoundBuffer* SdPrimaryBuff = nullptr;


bool SdDsInitialize(HWND _hWnd)
{
    HRESULT hResult = DirectSoundCreate8(0, &SdDS8Instance, 0);
    if (FAILED(hResult))
    {
        SDLOG("DirectSoundCreate8 failed (hresult = %" PRIhr ")\n", hResult);
        return false;
    };

    hResult = IDirectSound8_SetCooperativeLevel(SdDS8Instance, _hWnd, DSSCL_NORMAL);
    if (FAILED(hResult))
    {
        SDLOG("SetCooperativeLevel failed (hresult = %" PRIhr ")\n", hResult);
        return false;
    };

    hResult = IDirectSound8_GetSpeakerConfig(SdDS8Instance, &SdDSConfig);
    if (FAILED(hResult))
    {
        SDLOG("GetSpeakerConfig failed (hresult = %" PRIhr ")\n", hResult);
        return false;
    };

    if (DSSPEAKER_CONFIG(SdDSConfig) == DSSPEAKER_5POINT1)
        SdIsSpeaker5p1 = true;

    SdDS8BuffCnt = 0;

    DSBUFFERDESC SdBuffDesc;
    std::memset(&SdBuffDesc, 0, sizeof(SdBuffDesc));
    SdBuffDesc.dwSize = sizeof(DSBUFFERDESC);
    SdBuffDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    SdBuffDesc.dwBufferBytes = 0;
    SdBuffDesc.dwReserved = 0;
    SdBuffDesc.lpwfxFormat = NULL;

    SdDsBuffCreate(&SdPrimaryBuff, &SdBuffDesc);
    if (SdPrimaryBuff == nullptr)
        SDLOG("primary buffer is NULL!");

    return true;
};


void SdDsTerminate(void)
{
    if (SdPrimaryBuff != nullptr)
    {
        SdDsBuffDestroy(SdPrimaryBuff);
        SdPrimaryBuff = nullptr;
    };

    ASSERT(SdDS8BuffCnt == 0);

    if (SdDS8Instance)
    {
        IDirectSound8_Release(SdDS8Instance);
        SdDS8Instance = nullptr;
    };
};


bool SdDsBuffCreate(LPDIRECTSOUNDBUFFER* Buff, DSBUFFERDESC* BuffDesc)
{
    if (!SdDS8Instance)
        return false;

    HRESULT hResult = IDirectSound8_CreateSoundBuffer(SdDS8Instance, BuffDesc, (LPDIRECTSOUNDBUFFER*)Buff, NULL);
    if (FAILED(hResult))
    {
        SDLOG("CreateSoundBuffer failed (hresult = %" PRIhr ")\n", hResult);
        return false;
    };

    ++SdDS8BuffCnt;
    
    return true;
};


void SdDsBuffDestroy(LPDIRECTSOUNDBUFFER Buff)
{
    ASSERT(SdDS8BuffCnt > 0);

    IDirectSoundBuffer_Release(Buff);
    --SdDS8BuffCnt;
};


void SdDsBuffLoadData(LPDIRECTSOUNDBUFFER Buff, const void* Data, size_t _size)
{
    LPVOID  Buff1   = nullptr;
    DWORD   cbBuff1 = 0;
    LPVOID  Buff2   = nullptr;
    DWORD   cbBuff2 = 0;

    HRESULT hResult = IDirectSoundBuffer_Lock(Buff, 0, 0, &Buff1, &cbBuff1, &Buff2, &cbBuff2, DSBLOCK_ENTIREBUFFER);
    if (SUCCEEDED(hResult))
    {
        std::memcpy(Buff1, Data, _size);

        if (Buff2)
            std::memcpy(Buff2, &((const uint8*)Data)[cbBuff1], (size_t)cbBuff2);

        IDirectSoundBuffer_Unlock(Buff, Buff1, cbBuff1, Buff2, cbBuff2);
    }
    else
    {
        SDLOG("Lock failed (hresult = %" PRIhr ")\n", hResult);
    };
};


void SdDsBuffLoadDataEx(LPDIRECTSOUNDBUFFER Buff, int32 _offset, const void* Data, size_t _size)
{
    LPVOID  Buff1 = nullptr;
    DWORD   cbBuff1 = 0;
    LPVOID  Buff2 = nullptr;
    DWORD   cbBuff2 = 0;

    HRESULT hResult = IDirectSoundBuffer_Lock(Buff, (DWORD)_offset, (DWORD)_size, &Buff1, &cbBuff1, &Buff2, &cbBuff2, 0);
    if (SUCCEEDED(hResult))
    {
        std::memcpy(Buff1, Data, _size);

        if (Buff2)
            std::memcpy(Buff2, &((const uint8*)Data)[cbBuff1], (size_t)cbBuff2);

        IDirectSoundBuffer_Unlock(Buff, Buff1, cbBuff1, Buff2, cbBuff2);
    }
    else
    {
        SDLOG("Lock failed (hresult = %" PRIhr ")\n", hResult);
    };
};


int32 SdDsBuffSetPan(LPDIRECTSOUNDBUFFER Buff, int32 _chLeft, int32 _chRight)
{
    int32 _chHigh = _chLeft;

    if (_chHigh < _chRight)
        _chHigh = _chRight;

    int32 l = _chLeft - _chHigh;
    int32 r = _chRight - _chHigh;

    HRRET(IDirectSoundBuffer_SetPan(Buff, (l <= r ? -l : r)));

    return _chHigh;
};


int32 SdDsSetWaveFormat(WAVEFORMATEX* WaveFormat, SdWaveDataHdr_t* Header)
{
    std::memset(WaveFormat, 0, sizeof(*WaveFormat));

    int32 Channels      = 0;
    int32 BitsPerSample = 0;
    int32 AvgBytesPerSec= 0;

    if (Header->Format == WAVE_FORMAT_PCM)
    {
        Channels = Header->Channels;
        BitsPerSample = 8 * (Header->BlockAlign != 1) + 8;
        AvgBytesPerSec = Header->BlockAlign * Header->SamplesPerSec;

        WaveFormat->cbSize = 0;
    }
    else if (Header->Format == WAVE_FORMAT_VOXWARE_BYTE_ALIGNED)
    {
        Channels = Header->Channels;
        BitsPerSample = 4;
        AvgBytesPerSec = (Header->BlockAlign * Header->SamplesPerSec) / 64;

        ((ADPCMEWAVEFORMAT*)WaveFormat)->wfx.cbSize = 2;
        ((ADPCMEWAVEFORMAT*)WaveFormat)->wSamplesPerBlock = 64;
    }
    else
    {
        SDLOG("unknown header format %" PRIi32 "\n", Header->Format);
        return 0;
    };

    WaveFormat->nChannels       = (WORD)Channels;
    WaveFormat->wFormatTag      = (WORD)Header->Format;
    WaveFormat->nSamplesPerSec  = (DWORD)Header->SamplesPerSec;
    WaveFormat->wBitsPerSample  = (WORD)BitsPerSample;
    WaveFormat->nAvgBytesPerSec = (DWORD)AvgBytesPerSec;
    WaveFormat->nBlockAlign     = (WORD)Header->BlockAlign;

    return sizeof(DSBUFFERDESC);
};


int32 SdDsConvVol(int32 _vol)
{
    /* converts framework volume to directsound volume type */
    
    if (_vol > 0x3FFF)
        _vol = 0x3FFF;

    int32 idx = (_vol >> 6);
    return -SdDsVolumeTable[idx];
};