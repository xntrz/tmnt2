#pragma once

#define SD_STRBGM_NUM               (0x2C)

#define SD_NOCODE                   (0x0)


void SoundFirstLoad(void);
void SoundLoad(int32 Command, uint32 Param);
void SoundSet(uint32 Code);
void SoundSetEx(uint32 Code, uint32 Param1, uint32 Param2, uint32 Param3);
bool SoundPlaybackCheck(uint32 Code);
bool SoundVoiceBusyCheck(void);
bool SoundGetLoadInfo(void);
uint32 SoundVoxCodeCheck(int32 SrvNo);