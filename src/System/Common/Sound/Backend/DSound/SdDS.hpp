#pragma once

#include "SdLog.hpp"

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>


#define PRIhr PRIxPTR


#ifdef _DEBUG
#define hrCall(x) 											\
do 															\
{															\
    HRESULT hResult = (x);                                  \
                                                            \
    if (FAILED(hResult)) {									\
        SDLOG(#x" call error: 0x%" PRIhr "\n", hResult);	\
    };                                                      \
} while (0)
#else /* _DEBUG */
#define hrCall(x) (x)
#endif /* _DEBUG */


struct SdWaveDataHdr_t;


bool SdDSInitialize(HWND _hWnd);
void SdDSTerminate(void);
int32 SdDSSetWaveFormat(WAVEFORMATEX* WaveFormat, SdWaveDataHdr_t* Header);
bool SdDSBuffCreate(LPDIRECTSOUNDBUFFER* Buff, DSBUFFERDESC* BuffDesc);
void SdDSBuffDestroy(LPDIRECTSOUNDBUFFER Buff);
void SdDSBuffLoadData(LPDIRECTSOUNDBUFFER Buff, const void* Data, size_t _size);
void SdDSBuffLoadDataEx(LPDIRECTSOUNDBUFFER Buff, int32 _offset, const void* Data, uint32 _size);
void SdDSBuffClear(LPDIRECTSOUNDBUFFER Buff);
int32 SdDSBuffSetPan(LPDIRECTSOUNDBUFFER Buff, int32 _chLeft, int32 _chRight);
int32 SdDSConvVol(int32 _vol);