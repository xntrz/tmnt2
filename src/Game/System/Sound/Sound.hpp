#pragma once

#define SD_STRBGM_NUM         	(0x2C)
#define SD_VOICE_PER_BANK       (0x1000)

#define SD_NOCODE               (0x0)

#define SD_CODE_SE_BEGIN 		(0x1000)
#define SD_CODE_SE_END 			(0x1FFF)
#define SD_CODE_TRSE_BEGIN 		(0x2000)
#define SD_CODE_TRSE_END 		(0x2FFF)
#define SD_CODE_BGM_BEGIN 		(0x3000)
#define SD_CODE_BGM_END 		(0x3FFF)
#define SD_CODE_VOICE_BEGIN 	(0x4000)
#define SD_CODE_VOICE_END 		(0xBFFF)

#define SD_MASTERCODE_SURCTRL 	(0x40000000)
#define SD_MASTERCODE_FADE 		(0x80000000)


void SoundFirstLoad(void);
void SoundLoad(int32 Command, int32 Param);
void SoundSet(int32 Code);
void SoundSetEx(int32 Code, uint32 Param1, uint32 Param2, uint32 Param3);
bool SoundPlaybackCheck(int32 Code);
bool SoundVoiceBusyCheck(void);
bool SoundGetLoadInfo(void);
int32 SoundVoxCodeCheck(int32 SrvNo);