#pragma once

#define SD_NOCODE 0
#define SD_STRBGM_NUM (0x2C)
#define SDCODE_CTRL_STOP (0x80000000)
#define SDCODE_CTRL_SURROUND (0x40000000)


void SoundInitialize(void);
void SoundTerminate(void);
void SoundSetPath(const char* Path);
void SoundFirstLoad(void);
void SoundLoad(int32 Command, uint32 Param);
void SoundSet(uint32 Code);
void SoundSetEx(uint32 Code, uint32 Param1, uint32 Param2, uint32 Param3);
bool SoundPlaybackCheck(uint32 Code);
bool SoundVoiceBusyCheck(void);
bool SoundGetLoadInfo(void);
int32 SoundVoxCodeChk(int32 _no);