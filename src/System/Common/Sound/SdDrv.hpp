#pragma once

#include "SdTypes.hpp"


extern uint32 SdDrvOptFlag;
extern bool SdDrvSurMode;


bool SdDrvInitialize(void* window, const char* _path, void* _heap, size_t _heapSize, int32 _option);
void SdDrvTerminate(void);
void SdDrvTask(void);
void SdDrvWindowFocusLost(void);
void SdDrvWindowFocusRestore(void);
void SdDrvCodeSet(int32 _code, uint32 _param1, uint32 _param2, uint32 _param3);
void SdDrvSetUseCode(int32 _code, uint32 _param1, uint32 _param2, uint32 _param3);
void SdDrvFirstLoad(void);
int32 SdDrvGetLoadInfo(void);
int32 SdDrvGetCodeKind(int32 _code);
void SdDrvSetSeqBGMOff(void);
void SdDrvSetSeqBGMFadeOut(int32 _time);
void SdDrvSetSeqAllFadeOut(int32 _time, int32 _option);
bool SdDrvPlaybackCheck(int32 _code);
bool SdDrvVoiceBusyCheck(void);
int32 SdDrvVoxCodeCheck(int32 _srv);
bool SdDrvAutoFadeout(int32 _mode, SdSetDriverCode_t* CodeBuff);
void SdDrvLoad(int32 _command, int32 _mode, int32 _param);
int32 SdBgmCheck(int32 _code);
int32 GetBGMCode(void);
void SdDrvFadeOutX(int32 _code, int32 _time);
bool SdSurroundCtrl(SdSetDriverCode_t* CodeBuff);
bool SdDrvSetSE(SdSetDriverCode_t* CodeBuff);
bool SdDrvSetTRSE(SdSetDriverCode_t* CodeBuff);
int32 SdDrvFindChannel(int32 _code, int32 _mode, int32 _channel, int32 _option, int32 _priority, int32 _count);
int32 SdDrvSameCodeFind(int32 _code, int32 _start, int32 _portCount, int32 _count);
void SdDrvSetSeqWork(SdSetDriverCode_t* CodeBuff, SdKdtTableData_t* TableData, int32 _offset);

//
//  Following functions are presented only for test or debug
// 
int32 GetSECount(void);
int32 GetSECountInBank(int32 _bank);
int32 GetSEBank(int32 _code);
bool LoadSEBank(int32 _bank);
int32 GetTRSECount(void);
int32 GetTRSEBank(int32 _code);
bool LoadTRSEBank(int32 _bank);
int32 GetTRSEWaveBank(int32 _code);
bool IsTRSEWaveBankLoaded(int32 _bank);