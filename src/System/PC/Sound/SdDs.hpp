#pragma once

#include "SdTypes.hpp"

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>


bool SdDsInitialize(HWND _hWnd);
void SdDsTerminate(void);
int32 SdDsSetWaveFormat(WAVEFORMATEX* WaveFormat, SdWaveDataHdr_t* Header);
bool SdDsBuffCreate(LPDIRECTSOUNDBUFFER* Buff, DSBUFFERDESC* BuffDesc);
void SdDsBuffDestroy(LPDIRECTSOUNDBUFFER Buff);
void SdDsBuffLoadData(LPDIRECTSOUNDBUFFER Buff, const void* Data, size_t _size);
void SdDsBuffLoadDataEx(LPDIRECTSOUNDBUFFER Buff, int32 _offset, const void* Data, uint32 _size);
int32 SdDsBuffSetPan(LPDIRECTSOUNDBUFFER Buff, int32 _chLeft, int32 _chRight);
int32 SdDsConvVol(int32 _vol);