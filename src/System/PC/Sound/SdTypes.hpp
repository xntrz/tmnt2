#pragma once

//
//  Sd drv generic
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
#define SD_PC_SEQ_CHANNEL_NUM	(52)
#define SD_SEQ_VOL_MAX          (0x3FFF)

#define SD_INTERPOLATE(d1, d2, v) \
    ((d1) + (((v & 0xFF) * ((d2) - (d1))) >> 8))


//
//  Sd drv opt flag
//

#define SD_OPTFLAG_NOVAG        (1 << 0)
#define SD_OPTFLAG_NOVOX        (1 << 1)
#define SD_OPTFLAG_NOSEQ        (1 << 2)
#define SD_OPTFLAG_LOG			(1 << 3)


//
//  Sd drv code kind list
//

#define SD_CODEKIND_VOX         (0)
#define SD_CODEKIND_CTRL        (1)
#define SD_CODEKIND_SE          (2)
#define SD_CODEKIND_TRANS_SE	(3)
#define SD_CODEKIND_TRANS_BGM	(4)
#define SD_CODEKIND_VAG         (5)
                            //	(6) TODO unknown code kind
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
//  Sd drv fadeout
//

#define SD_FADEOUT_MODE_NORMAL     	(1)
#define SD_FADEOUT_MODE_0x3			(3) // TODO

#define SD_FADEOUT_SPEED_FAST 		(320)
#define SD_FADEOUT_SPEED_NORMAL 	(SD_FADEOUT_SPEED_FAST * 2)
#define SD_FADEOUT_SPEED_SLOW 		(SD_FADEOUT_SPEED_NORMAL * 2)

#define SD_FADEOUT_SPEED(t) \
    -(SD_FADE_MAX / ((((t) / SD_PERIOD) <= 0) ? (1) : ((t) / SD_PERIOD)))


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
#define SD_CODE_VOICE_MAX       (0x7FFF)
                            //	(0x8000) VOICE??
                            //	(0xBFFF) VOICE??
#define SD_CODE_RDC_MIN         (0xC000)
#define SD_CODE_RDC_MAX         (0xFFFF)

//
//  Sd drv master codes
//

#define SD_MASTERCODE_SUR      	(0x40000000)
#define SD_MASTERCODE_FADEOUT  	(0x80000000)
#define SD_MASTERCODE_MASK		(0xFF000000)
#define SD_SOUNDCODE_MASK		(0x00FFFFFF)

/*
 *	Sd set tone status
 */

#define SD_TONE_ENV_DRUM 	(0)
#define SD_TONE_ENV_NODRUM 	(1)
#define SD_TONE_NOENV 		(2)


//
//  Sd drv generic structs and enums
//

struct SdSurCtrl_t
{
    uint32 Rot;
    uint32 Dist;
    uint32 Id;
    float SinBuf;
    float CosBuf;
    int32 PosBuf;
};


struct SdSetSoundCode_t
{
    int32 Code;
    int16 Param1;
    int16 Param2;
    int16 Param3;
    int32 MasterCode;
    SdSurCtrl_t SurCtrl;
};


struct SdSetDriverCode_t
{
    int32 Code;
    int16 Param1;
    int16 Param2;
    int16 Param3;
    int32 ExtFlag;
    int32 FadeData;
    int32 FadeSpeed;
    uint16 GroupNo;
    int16 FRange;
    int16 VRange;
    int16 PRange;
    int16 SRange;
    int32 MasterCode;
    int32 ParentCode[3];    
    SdSurCtrl_t SurCtrl;
    int32 Channel;
    int32 Option;
};


struct SdEnvelope_t
{
    int32 addr;
    int8 sample_note;
    int8 sample_tune;
    uint8 ar;
    uint8 dr;
    uint8 sr;
    uint8 sl;
    uint8 rr;
    uint8 pad;
};

CHECK_SIZE(SdEnvelope_t, 0xC);


struct SdEnvelopeDrum_t
{
    uint8 sno;
    int8 sample_note;
    int8 sample_tune;
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

CHECK_SIZE(SdEnvelopeDrum_t, 0xC);


struct SdSetEnvelope_t
{
    uint16 ChNo;
    int32 Address;
    int32 Note;
    int32 Tune;
    int32 Ar;
    int32 Dr;
    int32 Sl;
    int32 Sr;
    int32 Rr;
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


struct SdKdtTableData_t
{
    uint8 Priority;
    uint8 WaveBank;
    uint8 SetMode;
    uint8 Channel;
    uint8 Count;
    uint8 pad[3];
    int32 Option;
    uint8** TableAddress;
};

CHECK_SIZE(SdKdtTableData_t, 0x10);


struct SdVolumeLimiter_t
{
    int32 CodeLow;
    int32 CodeHi;
    int16 Flag;
    int32 RdcFlag;
    int32 LowestVolume;
    int32 BestVolume;
    int32 Correction;
    int32 Decrease;
    int32 Increase;
};
