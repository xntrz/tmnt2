#pragma once

//
//  Sd drv generic defines
//
#define SD_WAVE_BANK_MAX        (128)
#define SD_FILENAME_LEN         (256)
#define SD_SCT_SIZE             (0x800)
#define SD_CANARY_TOP           (0x5A5A5A5A)
#define SD_CANARY_END           (0x8A8A8A8A)
#define SD_CANARY               (SD_CANARY_TOP)
#define SD_FADE_MAX             (0x10000)
#define SD_PERIOD               (5)
#define SD_SEQ_CHANNEL_NUM      (48)
#define SD_SEQ_VOL_MAX          (0x3FFF)

//
//  Sd drv opt flag
//
#define SD_OPTFLAG_NOVAG        (1 << 0)
#define SD_OPTFLAG_NOVOX        (1 << 1)
#define SD_OPTFLAG_NOSEQ        (1 << 2)

//
//  Sd drv output channel matrix
//
#define SD_PAN_CHANNEL_COUNT    (2)
#define SD_PAN_CHANNEL_LEFT     (0)
#define SD_PAN_CHANNEL_RIGHT    (1)

//
//  Sd drv code kind list
//
#define SD_CODEKIND_VOX         (0)
#define SD_CODEKIND_CTRL        (1)
#define SD_CODEKIND_SE          (2)
#define SD_CODEKIND_TRANS_SE    (3)
#define SD_CODEKIND_TRANS_BGM   (4)
#define SD_CODEKIND_VAG         (5)
#define SD_CODEKIND_TRANS       (6)
#define SD_CODEKIND_RDC         (7)
#define SD_CODEKIND_UNKNOWN     (8)

//
//  Sd drv set code reasons
//
#define SD_REASON_SETSOUND      (0)
#define SD_REASON_SETRDC        (1)
#define SD_REASON_SETCRX        (2)
#define SD_REASON_SETUSE        (3)
#define SD_REASON_RDC           (4)
#define SD_REASON_SETNEXT       (5)

//
//  Sd drv fadeout speed
//
#define SD_FADEOUT_FAST         (0)
#define SD_FADEOUT_NORMAL       (1)
#define SD_FADEOUT_SLOW         (2)

//
//  Sd drv ctrl params
//
#define SD_DRVCTRL_FADEOUT_ALL      (0)
#define SD_DRVCTRL_FADEOUT_SEQ_NEXT (1)
#define SD_DRVCTRL_FADEOUT_VAG_NEXT (2)
#define SD_DRVCTRL_FADEOUT_AUTO     (0x8000)

//
//  Sd drv code ranges from 0x0000 to 0xFFFF
//
#define SD_CODE_CTRL_MIN        (0x400)
#define SD_CODE_CTRL_MAX        (0xFFF)
#define SD_CODE_SE_MIN          (0x1000)
#define SD_CODE_SE_MAX          (0x1FFF)
#define SD_CODE_TRANS_MIN       (0x2000)
#define SD_CODE_TRANS_MAX       (0x2FFF)
#define SD_CODE_BGM_MIN         (0x3000)
#define SD_CODE_BGM_MAX         (0x3FFF)
#define SD_CODE_VOICE_MIN       (0x4000)
#define SD_CODE_VOICE_MAX       (0xBFFF)
#define SD_CODE_RDC_MIN         (0xC000)
#define SD_CODE_RDC_MAX         (0xFFFF)

//
//  Sd drv master codes
//
#define SD_MASTERCODE_SUR       (0x40000000)
#define SD_MASTERCODE_FADE      (0x80000000)

//
//  Sd drv macros
//
#define SD_INRANGE(val, min, max)           (((val) >= (min)) && ((val) <= (max)))
#define SD_INTERPOLATE(d1, d2, v)           ((d1) + (((v) * ((d2) - (d1))) / (256)))

//
//  Sd drv generic structs and enums
//
struct SdSurCtrl_t
{
    uint32 Rot;
    uint32 Dist;
    uint32 Id;
};

struct SdSetSoundCode_t
{
    uint32 Code;
    uint32 Param1;
    uint32 Param2;
    uint32 Param3;
    uint32 MasterCode;
    SdSurCtrl_t SurCtrl;
};

struct SdSetSoundCodeCrx_t
{
    uint32 Mode;
    uint32 Code;
    uint32 Param1;
    uint32 Param2;
    uint32 Param3;
    uint32 MasterCode;
};

struct SdSetDriverCode_t
{
    uint32 Code;
    uint32 Param1;
    uint32 Param2;
    uint32 Param3;
    uint32 ExtFlag;
    uint32 FadeData;
    uint32 FadeSpeed;
    int16 GroupNo;
    int16 FRange;
    int16 VRange;
    int16 PRange;
    int16 SRange;
    uint32 MasterCode;
    uint32 ParentCode[3];    
    SdSurCtrl_t SurCtrl;
    int32 Channel;
    uint32 Option;
};

struct SdEnvelopeDrum_t
{
    uint8 sno;
    uint8 sample_note;
    uint8 sample_tune;
    uint8 ar;
    uint8 dr;
    uint8 sr;
    uint8 sl;
    uint8 rr;
    uint8 pan;
    uint8 pad1;
    uint8 pad2;
    uint8 pad3;
};

struct SdPanOutput_t
{
    int32 Id;
    int32 Value;
};

struct SdPanOutputData_t
{
    int32 Count;
    SdPanOutput_t* Array;
};


struct SdWaveDataHdrX_t
{
    int32 Format;
    int32 Channels;
    int32 BlockAlign;
    int32 SamplesPerSec;

    union
    {
        struct
        {
            int32 LoopFlag;
        } seq;

        struct
        {
            int32 LoopPoint;
            int32 EndPoint;
            int32 Volume;
            int32 PlayPoint;
            int32 Page;
            int32 Size;
        } str;
    };
};


struct SdWaveDataHdr_t
{
    int32 Format;
    int32 Channels;
    int32 BlockAlign;
    int32 SamplesPerSec;
    int32 LoopPoint;
    int32 EndPoint;
    int32 Volume;
    int32 PlayPoint;
    int32 Page;
    int32 Size;
};