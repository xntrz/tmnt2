#pragma once

#include "SdTypes.hpp"

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>


bool SdDsInitialize(HWND hWnd);
void SdDsTerminate(void);
uint32 SdDsSetWaveFormat(WAVEFORMATEX* WaveFormat, SdWaveDataHdr_t* SdWaveDataHdr);
bool SdDsBuffCreate(LPDIRECTSOUNDBUFFER* SdBuff, DSBUFFERDESC* SdBuffDesc);
void SdDsBuffDestroy(LPDIRECTSOUNDBUFFER SdBuff);
void SdDsBuffLoadData(LPDIRECTSOUNDBUFFER SdBuff, const void* Data, uint32 Size);
void SdDsBuffLoadDataEx(LPDIRECTSOUNDBUFFER SdBuff, uint32 Offset, const void* Data, uint32 Size);
int32 SdDsBuffSetPan(LPDIRECTSOUNDBUFFER SdBuff, int32 ch_l, int32 ch_r);