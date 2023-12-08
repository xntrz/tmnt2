#pragma once

#include "SdTypes.hpp"
#include "SdQueue.hpp"


#define SDSTR_SRV_MAX                   (8)

#define SDSTR_TYPE_VOX                  (0)
#define SDSTR_TYPE_VAG                  (1)

#define SDSTR_STATE_IDLE                (0)
#define SDSTR_STATE_READ_HEAD           (1)
#define SDSTR_STATE_READ_HEAD_WAIT      (2)
#define SDSTR_STATE_READ_FIRST          (3)
#define SDSTR_STATE_READ_FIRST_WAIT     (4)
#define SDSTR_STATE_PLAYSTART           (5)
#define SDSTR_STATE_PLAYING             (6)
#define SDSTR_STATE_READ_NEXT           (7)
#define SDSTR_STATE_PAUSE               (8)
#define SDSTR_STATE_RESUME              (9)
#define SDSTR_STATE_DOWNLOAD_WAIT       (10)
#define SDSTR_STATE_PLAYFINISH          (11)

#define SDSTR_FADEOUT_FLAG_RST_QUE      (1 << 0)


struct SdStrQueueData_t
{
    uint32 Code;
    uint32 Param1;
    uint32 Param2;
    uint32 Param3;    
};


struct SdStr_t
{
    uint32 CheckKeyTop;
    int32 Index;
    int32 Type;
    int32 No;
    int32 PlayState;
    int32 PlayStateResume;
    int32 PlayTime;
    int32 PlayVolume;
    uint32 PlayCode;
    uint32 Offset;
    int32 DataSize;
    uint32 DataPosition;
    uint32 DmaPos;
    uint32 DmaStatus;
    uint32 DmaPageOffset;
    uint32 DmaPageBlock;
    uint32 DmaPageSize;
    uint32 DmaPageRead;
    uint32 DmaPagePrev;
    void* DmaHandle;
    void* MemAddress;
    uint32 MemOffset;
    uint32 FadeFlag;
    int32 FadeData;
    int32 FadeSpeed;
    int32 Vol_L;
    int32 Vol_R;
    uint32 DefaultFadeOut;
    uint32 DefaultTarget;
    int32 Timeout;
    int32 SetVolume;
    SdQueue_t* Queue;
    SdWaveDataHdr_t WaveHeader;
    SdSetDriverCode_t SetCode;
    void* FileHandle;
    char* Filename;
    void* Head;
    bool PauseRq;
    bool WaveTransferRq;
    bool PlayingOnPause;
    int32 VolumeOnPause;
    bool Stereo;
    bool Reverb;  
    uint32 CheckKeyEnd;
};


void SdStrInitialize(void);
void SdStrTerminate(void);
void SdStrTask(void);
void SdStrPauseClr(void);
void SdStrPauseSet(void);
SdStr_t* SdStrGet(int32 Type, int32 No);
int32 SdStrGetServerNum(int32 Type);

int32 SdVoxInit(int32 No);
void SdVoxSet(int32 No, SdSetDriverCode_t* SetDrvCode);
void SdVoxSetFadeOut(int32 No, int32 time, uint32 flag);
void SdVoxQueueClear(int32 No);
void SdVoxQueueAllClear(void);
bool SdVoxGetPlayVoice(int32 No);
bool SdVoxGetAllPlayVoice(void);
uint32 SdVoxGetPlayCode(int32 No);
int32 SdVoxGetTotalPlaytime(int32 No);
int32 SdVoxGetPlayVoiceCount(int32 No);
int32 SdVoxGetAllPlayVoiceCount(void);
void SdVoxReset(int32 time, uint32 flag);

int32 SdVagInit(int32 No);
void SdVagSet(int32 No, SdSetDriverCode_t* SetDrvCode);
void SdVagSetMono(void);
void SdVagSetStereo(void);
bool SdVagPlaybackCheck(uint32 Code);
bool SdVagFadeCheck(uint32 Code);
uint32 SdVagFirstPlayCode(void);
bool SdVagAutoFadeOutChek(int32 No);
bool SdVagAllAutoFadeOutChek(void);
int32 SdVagGetDefaultFadeOut(int32 No);
uint32 SdVagGetPlayCode(int32 No);
void SdVagSetFadeOut(int32 No, int32 time, uint32 flag);
void SdVagReset(int32 time, uint32 flag);
void SdVagFadeOutX(uint32 Code, int32 time);