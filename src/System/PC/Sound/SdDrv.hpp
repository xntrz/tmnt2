#pragma once

#include "SdTypes.hpp"

#include "../PCTypedefs.hpp"


extern uint32 SdDrvOptFlag;


bool SdDrvInitialize(HWND hWnd, const char* Path, void* HeapPtr, uint32 HeapSize, uint32 OptFlag);
void SdDrvTerminate(void);
void SdDrvCodeSet(uint32 Code, uint32 Param1, uint32 Param2, uint32 Param3);
void SdDrvSetCrx(uint32 Code, uint32 Param1, uint32 Param2, uint32 Param3);
void SdDrvSetRdcCode(uint32 Code, uint32 Param1, uint32 Param2, uint32 Param3);
void SdDrvSetUseCode(uint32 Code, uint32 Param1, uint32 Param2, uint32 Param3);
bool SdDrvGetLoadInfo(void);
int32 SdDrvGetCodeKind(uint32 Code);
void SdDrvSetSurMode(bool flag);
bool SdDrvGetSurMode(void);
void SdDrvSetSeqBGMOff(void);
void SdDrvSetSeqBGMFadeOut(int32 time);
void SdDrvSetSeqAllFadeOut(int32 time, uint32 option);
bool SdDrvPlaybackCheck(uint32 Code);
bool SdDrvVoiceBusyCheck(void);
uint32 SdDrvVoxCodeCheck(int32 VoxSrvNo);
bool SdDrvAutoFadeout(int32 mode, SdSetDriverCode_t* SetDrvCode);
void SdDrvLoad(int32 Command, uint32 Param);
bool SdDrvIsStarted(void);
void SdDrvThreadSetName(const char* Name, uint32 ThreadId = 0);