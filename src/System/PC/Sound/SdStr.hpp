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
    int32 Code;
    int16 Param1;
    int16 Param2;
    int16 Param3;    
};


struct SdStr_t
{
    int32               CheckKeyTop;
    int32               Index;
    int32               Type;
    int32               No;
    int32               PlayState;
    int32               PlayStateResume;
    int32               PlayTime;
    int32               PlayVolume;
    int32               PlayCode;
    uint32              Offset;
    int32               DataSize;
    uint32              DataPosition;
    uint32              DmaPos;
    uint32              DmaStatus;
    uint32              DmaPageOffset;
    uint32              DmaPageBlock;
    uint32              DmaPageSize;
    uint32              DmaPageRead;
    uint32              DmaPagePrev;
    void*               DmaHandle;
    void*               MemAddress;
    uint32              MemOffset;
    int32               FadeFlag;
    int32               FadeData;
    int32               FadeSpeed;
    int32               Vol_L;
    int32               Vol_R;
    uint32              DefaultFadeOut;
    uint32              DefaultTarget;
    int32               Timeout;
    int32               SetVolume;
    SdQueue_t*          Queue;
    SdWaveDataHdr_t     WaveHeader;
    SdSetDriverCode_t   SetCode;
    void*               FileHandle;
    char*               Filename;
    void*               Head;
    bool                PauseRq;
    bool                WaveTransferRq;
    bool                PlayingOnPause;
    int32               VolumeOnPause;
    int32               HighVol;
    int32               CheckKeyEnd;
};


void SdStrInitialize(void);
void SdStrTerminate(void);
void SdStrTask(void);
void SdStrPauseClr(void);
void SdStrPauseSet(void);
SdStr_t* SdStrGet(int32 _type, int32 _no);
int32 SdStrGetServerNum(int32 _type);
void SdStrSetMonaural(void);
void SdStrSetStereo(void);

int32 SdVoxInit(int32 _no);
void SdVoxSet(int32 _no, SdSetDriverCode_t* CodeBuff);
void SdVoxSetFadeOut(int32 _no, int32 _time, int32 _flag);
void SdVoxQueueClear(int32 _no);
void SdVoxQueueAllClear(void);
bool SdVoxGetPlayVoice(int32 _no);
bool SdVoxGetAllPlayVoice(void);
int32 SdVoxGetPlayCode(int32 _no);
void SdVoxReset(int32 _time, int32 _flag);

int32 SdVagInit(int32 _no);
void SdVagSet(int32 _no, SdSetDriverCode_t* CodeBuff);
bool SdVagPlaybackCheck(int32 _code);
bool SdVagFadeCheck(int32 _code);
int32 SdVagFirstPlayCode(void);
bool SdVagAutoFadeOutChek(int32 _no);
bool SdVagAllAutoFadeOutChek(void);
int32 SdVagGetDefaultFadeOut(int32 _no);
int32 SdVagGetPlayCode(int32 _no);
void SdVagSetFadeOut(int32 _no, int32 _time, int32 _flag);
void SdVagReset(int32 _time, int32 _flag);
void SdVagFadeOutX(int32 _code, int32 _time);