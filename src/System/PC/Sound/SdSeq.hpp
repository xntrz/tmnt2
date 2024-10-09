#pragma once

#include "SdTypes.hpp"


// TODO opt
#define SD_SEQ_OPT_0x4		(0x4) 	// bgm se
#define SD_SEQ_OPT_0x8		(0x8)
#define SD_SEQ_OPT_0x10		(0x10)
#define SD_SEQ_OPT_0x20		(0x20)
#define SD_SEQ_OPT_0x40		(0x40)
#define SD_SEQ_OPT_0x80		(0x80)
#define SD_SEQ_OPT_0x100	(0x100)	// find reverse
#define SD_SEQ_OPT_0x200	(0x200)


struct SdTempo_t
{
    int32 Count;
    int32 Data;
    int32 MoveData;
    int32 MoveCount;
    int32 MoveDelta;
};


struct SdStep_t
{
    int16 StepCount;
    int16 StepTime;
    int16 GateCount;
    int16 GateTime;
    int16 GateLast;
    int16 NoteData;
};


struct SdVolume_t
{
    int32 Track;
    int32 MoveCount;
    int32 MoveDelta;
    int32 MoveData;
    int32 Velocity;
};


struct SdPan_t
{
    int32 Data;
    int32 MoveCount;
    int32 MoveDelta;
    int32 MoveData;
    int32 Mode;
};


struct SdLoop_t
{
    int32 Count;
    int32 Volume;
    int32 Freq;
    uint32 Address;
};


struct SdBrackets_t
{
    uint32 Flag;
    void* Ptr1Address;
    void* Ptr2Address;
};


struct SdFrequency_t
{
    int32 Data;
    int32 Note;
    int32 Tune;
    int32 Trn;
    int32 Den;
};


struct SdVibrato_t
{
    int16 HoldCount;
    int16 HoldStep;
    int16 ChangCount;
    int16 ChangStep;
    int32 Deep;
    int32 ResultDeep;
    int32 Offset;
    int32 OffsetDelta;
    int32 ChangDelta;
};


struct SdSweep_t
{
    int16 HoldCount;
    int16 HoldStep;
    int16 Step;
    int16 Count;
    int32 Result;
    int32 OffsetDeep;
    int32 Offset;
    int32 OffsetDelta;
    int32 PurposeNote;
    int32 StartNote;
    int32 PosFlag;
};


struct SdRandom_t
{
    int16 Count;
    int16 Offset;
    int32 Speed;
    int32 Data;
};


struct SdFadeOut_t
{
    int32 Data;
    int32 Speed;
};


struct SdCrxParam_t
{
    int32 Data;
    int32 MoveCount;
    int32 MoveDelta;
    int32 MoveData;
};


struct SdPcParam_t
{
    int32 Data;
    int32 PurposeData;
    int32 Speed;
};


struct SdRdcParent_t
{
    int32 MasterCode;
    int32 Code[3];
};


struct SdSeqWork_t
{
    int32          	CheckKeyTop;
    uint16          ChNo;
    int32          	Code;
    int16         	CtrlPan;
    int16         	CtrlVol;
    uint16          Sno;
    uint16          Priority;
    uint16          NoiseCut;
    uint16          KeyOffWait;
    uint16          SdWait;
    uint16          Mode;
    uint16          RrRate;
    uint16          PhaseReversal;
    uint16          TransWaveNo;
    uint16          shRdxCrxBank;
    uint32          Address;
    int32         	SpuAddress;
    int32         	Option;
    SdTempo_t       Tempo;
    SdStep_t        Step;
    SdVolume_t      Volume;
    SdPan_t         Pan;
    SdLoop_t        L1;
    SdLoop_t        L2;
    SdLoop_t        L3;
    SdBrackets_t    Brackets;
    SdFrequency_t   Frequency;
    SdVibrato_t     Vibrato;
    SdSweep_t       Sweep;
    SdRandom_t      Random;
    SdFadeOut_t     FadeOut;
    SdCrxParam_t    CrxParam[4];
    SdPcParam_t     PcParam[4];
    SdRdcParent_t   RdcParent;
    SdPan_t         SPan;
    SdSurCtrl_t		SurCtrl;
    uint32          RdcParam[4];
    int32           nCtrlSur;
    int32           nTableIndex;
    int32          	ExtFlag;
    int32          	Mask;
    int32          	CheckKeyEnd;
};


extern SdSeqWork_t SdSeqWork[52];


void SdSeqInit(void);
void SdSeqTerm(void);
void SdSeqMain(void);
int32 SdSeqMaskCheck(SdSeqWork_t* Work);
void SdSeqMask(int32 _no, int32 _data);
void SdSeqAllOff(int32 _option);
void SdSeqOff(SdSeqWork_t* Work, uint16 _keyOffWait);
void SdSeqToneSet(SdSeqWork_t* Work, int32 _toneNo, int32 _drumNo);
void SdSeqPauseSet(int32 _option);
void SdSeqPauseClr(int32 _option);