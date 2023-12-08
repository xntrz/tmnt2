#pragma once


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
    uint32 MasterCode;
    uint32 Code[3];
};


struct SdSeq_t
{
    uint32          CheckKeyTop;
    uint32          ChNo;
    uint32          Code;
    uint16          CtrlPan;
    uint16          CtrlVol;
    uint16          Sno;
    uint16          Priority;
    uint16          NoiseCnt;
    uint16          KeyOffWait;
    uint16          SdWait;
    uint16          Mode;
    uint16          RrRate;
    uint16          PhaseReversal;
    uint16          TransWaveNo;
    uint16          shRdxCrxBank;
    uint32          Address;
    uint32          SpuAddress;
    uint32          Option;
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
    uint32          RdcParam[4];
    int32           nCtrlSur;
    int32           nTableIndex;
    uint32          ExtFlag;
    uint32          Mask;
    uint32          CheckKeyEnd;
};


void SdSeqInit(void);
void SdSeqTerm(void);
void SdSeqTask(void);
