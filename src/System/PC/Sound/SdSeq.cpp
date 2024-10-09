#include "SdSeq.hpp"
#include "SdIOSet.hpp"
#include "SdDrvCtrl.hpp"
#include "SdCallback.hpp"
#include "SdTable.hpp"
#include "SdLoader.hpp"
#include "SdSeqCtrl.hpp"
#include "SdDrv.hpp"
#include "SdLog.hpp"


static SdVolumeLimiter_t SdSeqVolumeLimiter[8];
/*extern*/ SdSeqWork_t SdSeqWork[52];


static void SdSeqToneNoEnvSet(SdSeqWork_t* Work, int32 _bank, int32 _no)
{
    SDNOIMPL();

    if (SdWaveTopAddress[_bank] && (_no < SdEnvelopeTable[_bank].Count))
    {
        SdSetEnvelope_t SetEnv;
        SdEnvelope_t* Env = &((SdEnvelope_t*)SdEnvelopeTable[_bank].Table)[_no];

        SetEnv.ChNo 	= Work->ChNo;
        SetEnv.Address 	= Env->addr + (uint32)SdWaveTopAddress[_bank];
        SetEnv.Note 	= 0;
        SetEnv.Tune 	= 0;
        SetEnv.Ar 		= 0x7F;
        SetEnv.Dr 		= 0;
        SetEnv.Sl 		= 0;
        SetEnv.Sr 		= 0;
        SetEnv.Rr 		= 0x1F;

        SdDrvSetToneBeforeCB(Work, SD_TONE_NOENV, &SetEnv);

        Work->Frequency.Note 	= SetEnv.Note;
        Work->Frequency.Tune 	= SetEnv.Tune;
        Work->RrRate 			= (uint16)SetEnv.Rr;
        Work->SpuAddress 		= SetEnv.Address;

        SdIOSetSsAddress(Work->ChNo, SetEnv.Address);
        SdIOSetAds(Work->ChNo, SetEnv.Ar, SetEnv.Dr, SetEnv.Sl);
        SdIOSetSrRr(Work->ChNo, SetEnv.Sr, SetEnv.Rr);
    };
};


static void SdSeqToneEnvSet(SdSeqWork_t* Work, int32 _bank, int32 _no, int32 _drumNo)
{
    if (SdWaveTopAddress[_bank] && (_no < SdEnvelopeTable[_bank].Count))
    {
        SdSetEnvelope_t SetEnv;
        int32 status;

        SdEnvelope_t* Env = &((SdEnvelope_t*)SdEnvelopeTable[_bank].Table)[_no];

        SetEnv.ChNo = Work->ChNo;
        SetEnv.Address = Env->addr + (uint32)SdWaveTopAddress[_bank];

        if (_drumNo)
        {
            SdEnvelopeDrum_t* Drm = &((SdEnvelopeDrum_t*)SdDrumTable.Table)[_drumNo - 72];

            SetEnv.Note = Drm->sample_note;
            SetEnv.Tune = Drm->sample_tune;
            SetEnv.Ar = Drm->ar;
            SetEnv.Dr = Drm->dr;
            SetEnv.Sl = Drm->sl;
            SetEnv.Sr = Drm->sr;
            SetEnv.Rr = Drm->rr;

            status = SD_TONE_ENV_DRUM;
        }
        else
        {
            SetEnv.Note = Env->sample_note;
            SetEnv.Tune = Env->sample_tune;
            SetEnv.Ar = Env->ar;
            SetEnv.Dr = Env->dr;
            SetEnv.Sl = Env->sl;
            SetEnv.Sr = Env->sr;
            SetEnv.Rr = Env->rr;

            status = SD_TONE_ENV_NODRUM;
        };

        SdDrvSetToneBeforeCB(Work, status, &SetEnv);

        Work->Frequency.Note 	= SetEnv.Note;
        Work->Frequency.Tune 	= SetEnv.Tune;
        Work->RrRate 			= (uint16)SetEnv.Rr;
        Work->SpuAddress 		= SetEnv.Address;

        SdIOSetSsAddress(Work->ChNo, SetEnv.Address);
        SdIOSetAds(Work->ChNo, SetEnv.Ar, SetEnv.Dr, SetEnv.Sl);
        SdIOSetSrRr(Work->ChNo, SetEnv.Sr, SetEnv.Rr);
    };
};


static int32 SdSeqSetSweepDelta(SdSeqWork_t* Work)
{
    if (Work->Sweep.Count)
        return (0x10000 / ((Work->Sweep.Count << 16) / Work->Tempo.Data));

    return 0;
};


static void SdSeqSetFrequency(SdSeqWork_t* Work, int32 _note)
{
    int32 Freq = _note + 4 * (Work->Frequency.Tune + (Work->Frequency.Note << 6));
    Freq = Clamp(Freq, 0, 0x4800);

    int32 idx = (Freq >> 8);

    ASSERT(idx >= 0);
    ASSERT(idx < COUNT_OF(SdFrequencyTable));

    int32 Data = SD_INTERPOLATE(SdFrequencyTable[idx], SdFrequencyTable[idx + 1], Freq);

    SdIOSetFreq(Work->ChNo, Data);
};


static int32 SdSeqRandomExe(SdSeqWork_t* Work)
{
    return 0;
};


static int32 SdSeqGetVibratoNote(SdSeqWork_t* Work)
{
    SDNOIMPL();

    int32 idx = (Work->Vibrato.Offset >> 8);

    ASSERT(idx >= 0);
    ASSERT(idx < (COUNT_OF(SdVibratoTable) - 1));

    int32 Data = SD_INTERPOLATE(SdVibratoTable[idx], SdVibratoTable[idx + 1], Work->Vibrato.Offset);

    if (Work->Vibrato.ResultDeep >= 0x8000)
        return (((Work->Vibrato.ResultDeep & 0x7FFF) + 512) * Data) / 512;
    else
        return (Work->Vibrato.ResultDeep * Data) / 0x8000;
};


static int32 SdSeqSweepNote(SdSeqWork_t* Work)
{
    if (Work->Sweep.Offset < 0x10000)
    {
        int32 idx = (Work->Sweep.Offset >> 8);

        ASSERT(idx >= 0);
        ASSERT(idx < (COUNT_OF(SdSweepTable) - 1));

        int32 Data = SD_INTERPOLATE(SdSweepTable[idx], SdSweepTable[idx + 1], Work->Sweep.Offset);

        return Work->Sweep.StartNote + (((Work->Sweep.PurposeNote - Work->Sweep.StartNote) * Data) >> 16);
    };

    return Work->Sweep.PurposeNote;
};


static int32 SdSeqGetPortamentNote(SdSeqWork_t* Work)
{
    SDNOIMPL();

    if (Work->Sweep.Offset < 0x10000)
    {
        int32 idx = (Work->Sweep.Offset >> 8);

        ASSERT(idx >= 0);
        ASSERT(idx < (COUNT_OF(SdPortamentTable) - 1));

        int32 Data = SD_INTERPOLATE(SdPortamentTable[idx], SdPortamentTable[idx + 1], Work->Sweep.Offset);

        return Work->Sweep.StartNote + (((Work->Sweep.PurposeNote - Work->Sweep.StartNote) * Data) / 0x10000);
    };

    return Work->Sweep.PurposeNote;
};


static void SdSeqPanPotDolby(SdSeqWork_t* Work, int32* _volLeft, int32* _volRight, int32 _vol)
{
    static uint8 s_acSdPanpotAngle[41 + 1] =
    {
        30,  33,  36,  39,  42,  45,  48,
        51,  54,  57,  60,  63,  66,  69,
        72,  75,  78,  81,  84,  87,  90,
        93,  96,  99,  102, 105, 108, 111,
        114, 117, 120, 123, 126, 129, 132,
        135, 138, 141, 144, 147, 150,
        150,
    };

    static uint16 s_acSdSpnAngle[41 + 1] =
    {
        185, 190, 200, 210, 220, 230, 240,
        250, 260, 270, 270, 280, 290, 300,
        310, 320, 330, 340, 345, 350, 0,
        5,   10,  20,  30,  40,  50,  60,
        70,  80,  90,  95,  100, 110, 120,
        130, 140, 150, 160, 170, 180,
        180,
    };

    int32 SurPos = 0;
    int32 PanPos = 90;

    if (SdDrvStereoFlag && !Work->PhaseReversal)
    {
        /* get pan pos */
        int32 OffsetPan = Work->Pan.Data + Work->CrxParam[2].Data + (Work->PcParam[2].Data >> 1) + ((int)Work->RdcParam[2] >> 1);
        OffsetPan = Clamp(OffsetPan, 0x0, 0x2800);

        int32 idxPan = (OffsetPan >> 8);
        
        ASSERT(idxPan >= 0);
        ASSERT(idxPan < COUNT_OF(s_acSdPanpotAngle));

        PanPos = s_acSdPanpotAngle[idxPan];

        /* get sur pos */
        int32 OffsetSur = 0;
        if (Work->nCtrlSur)
        {
            OffsetSur = 80 * Work->nCtrlSur;
            OffsetSur = Clamp(OffsetSur, 0x0, 0x2800);
        }
        else
        {
            OffsetSur = Work->CrxParam[3].Data + Work->SPan.Data + (Work->PcParam[3].Data >> 1) + ((int)Work->RdcParam[3] >> 1);
        };	

        int32 idxSur = (OffsetSur >> 8);
        
        ASSERT(idxSur >= 0);
        ASSERT(idxSur < COUNT_OF(s_acSdSpnAngle));

        SurPos = ((int32)Work->SurCtrl.Rot / 182) + s_acSdSpnAngle[idxSur];

        if (SdDrvSurMode && SurPos < 270 && SurPos > 90)
            SurPos = 180;
    };

    float c = 0.0f;
    float s = 0.0f;

    if (Work->SurCtrl.PosBuf == (SurPos + PanPos))
    {
        c = Work->SurCtrl.CosBuf;
        s = Work->SurCtrl.SinBuf;
    }
    else
    {
        float r = (((float)SurPos + (float)PanPos) * 3.141592f) / 360.0f;
        c = std::cos(r);
        s = std::sin(r);

        Work->SurCtrl.CosBuf = c;
        Work->SurCtrl.SinBuf = s;
    };

    Work->SurCtrl.PosBuf = (SurPos + PanPos);

    float vol = (float)(_vol * 2);
    
    *_volLeft  = (int32)(vol * c);
    *_volRight = (int32)(vol * s);
};


static void SdSeqPanPotOld(SdSeqWork_t* Work, int32* _volLeft, int32* _volRight, int32 _vol)
{
    (void)Work;
    (void)_volLeft;
    (void)_volRight;
    (void)_vol;
    
    SDNOIMPL();
};


static void SdSeqOutSideStep(SdSeqWork_t* Work)
{
    if (Work->Step.GateTime &&
        (Work->Step.StepCount == 1) &&
        (Work->RrRate < 0x18u))
    {
        SdIOSetRr(Work->ChNo, 24);
        ++Work->NoiseCut;
    };

    if (Work->Step.GateCount)
    {
        if (!--Work->Step.GateCount)
            SdIOSetKeyOff(Work->ChNo);
    };

    if (Work->Sweep.Count)
    {
        if (Work->Sweep.HoldCount)
        {
            --Work->Sweep.HoldCount;
        }
        else
        {
            Work->Sweep.Offset += Work->Sweep.OffsetDelta;
            if (Work->Sweep.PosFlag)
            {
                Work->Sweep.Result = SdSeqGetPortamentNote(Work);
            }
            else
            {
                if (Work->Sweep.Count == 1)
                    Work->Sweep.Offset = 0x10000;
                
                --Work->Sweep.Count;

                Work->Sweep.Result = SdSeqSweepNote(Work);
            };
        };
    };

    int32 vibNote = 0;
    if (Work->Vibrato.Deep)
    {
        if (Work->Vibrato.HoldCount)
        {
            --Work->Vibrato.HoldCount;
        }
        else
        {
            int32 deep = Work->Vibrato.Deep;

            if (Work->Vibrato.ChangCount)
            {
                --Work->Vibrato.ChangCount;
                deep = Work->Vibrato.ChangDelta + Work->Vibrato.ResultDeep;
            };

            Work->Vibrato.ResultDeep = deep;
            Work->Vibrato.Offset += Work->Vibrato.OffsetDelta;

            vibNote = SdSeqGetVibratoNote(Work);
        };
    };

    int32 note = Work->RdcParam[0] + Work->PcParam[0].Data + Work->CrxParam[0].Data + Work->Sweep.Result + vibNote;
    int32 rnd = SdSeqRandomExe(Work);

    SdSeqSetFrequency(Work, note + rnd);
};


static void SdSeqInSideStep(SdSeqWork_t* Work)
{
    ASSERT(Work->Address != 0);
    ASSERT(Work->ChNo >= 0);
    ASSERT(Work->ChNo < COUNT_OF(SdSeqWork));

    if (!Work->Address)
        return;

    if ((Work->ChNo < 0) && (Work->ChNo >= COUNT_OF(SdSeqWork)))
        return;

    bool KeyOnFlag = false;

    if (Work->Step.GateTime)
    {
        KeyOnFlag = true;
        if (Work->KeyOffWait)
        {
            Work->KeyOffWait = 0;
            SdIOSetKeyOff(Work->ChNo);
        };
    }
    else
    {
        KeyOnFlag = false;
    };

    uint8* Data = (uint8*)Work->Address;
    uint8* SeqData = nullptr;
    while (true)
    {
        SeqData = Data++;

        if (*SeqData < 0xD0)
            break;
        
        if (SdSeqCtrlFunc(*SeqData, Work, &Data))
        {
            Work->Address = (uint32)Data;
            return;
        };
    };

    if (*SeqData >= 0x68)
        Work->Step.NoteData = *SeqData - 0x68;
    else
        Work->Step.NoteData = *SeqData;

    if (Work->Step.NoteData >= 0x48)
    {
        if (Work->Step.NoteData == 0x67)
            Work->Step.NoteData = *Data++;

        SdSeqToneSet(Work, 0, Work->Step.NoteData);
        Work->Step.NoteData = 0x30;
    };

    int32 LastNote = Work->Frequency.Data;

    Work->Frequency.Data = Work->Step.NoteData;
    Work->Frequency.Data += Work->Frequency.Trn;
    Work->Frequency.Data <<= 8;
    Work->Frequency.Data += Work->Frequency.Den;
    Work->Frequency.Data += Work->L1.Freq;
    Work->Frequency.Data += Work->L2.Freq;

    if (Work->Frequency.Data >= 0x6000)
        Work->Frequency.Data -= 0x6000;

    if (*SeqData < 0x68)
        Work->Step.StepTime = *Data++;

    Work->Step.StepCount = Work->Step.StepTime;

    int16 GateLast = *Data;
    if (*Data++ < 0x80)
    {
        Work->Step.GateLast = GateLast;
        GateLast = *Data++;
    };

    Work->Step.GateTime = Work->Step.GateLast;
    if (Work->Step.GateTime)
    {
        Work->Step.GateCount = (Work->Step.StepTime * Work->Step.GateTime) >> 7;
        if (!Work->Step.GateCount)
            Work->Step.GateCount = 1;
    }
    else
    {
        Work->Step.GateCount = 0;
    };

    Work->Volume.Velocity 	= GateLast & 0x7F;
    Work->Address 			= (uint32)Data;
    Work->Vibrato.HoldCount = Work->Vibrato.HoldStep;
    Work->Vibrato.ChangCount= Work->Vibrato.ChangStep;
    Work->Vibrato.Offset 	= 0;
    Work->Vibrato.ResultDeep= 0;
    Work->Sweep.Result 		= Work->Frequency.Data;
    Work->Sweep.Count 		= Work->Sweep.Step;

    if (Work->Sweep.Count)
    {
        Work->Sweep.HoldCount = Work->Sweep.HoldStep;
        Work->Sweep.Offset = 0;
        Work->Sweep.PurposeNote = Work->Frequency.Data;

        if (Work->Sweep.PosFlag)
        {
            Work->Sweep.StartNote = LastNote;
            Work->Sweep.Result = LastNote;
            Work->Sweep.OffsetDelta = Work->Sweep.Count << 16 >> 8;
        }
        else
        {
            Work->Sweep.StartNote = Work->Frequency.Data + Work->Sweep.OffsetDeep;
            Work->Sweep.Result = Work->Frequency.Data + Work->Sweep.OffsetDeep;
            Work->Sweep.OffsetDelta = SdSeqSetSweepDelta(Work);
        };
    };

    SdSeqSetFrequency(Work, Work->Sweep.Result);

    if (KeyOnFlag)
    {
        if (Work->NoiseCut)
        {
            SdIOSetRr(Work->ChNo, Work->RrRate);
            --Work->NoiseCut;
        };

        SdIOSetKeyOn(Work->ChNo);
    };

    ASSERT(((uint8*)Work->Address) == Data, "checkout: should equal at this step");

    if (*Data++ == 228 && !Work->Sweep.Count)
    {
        Work->Sweep.HoldCount 	= *Data++;
        Work->Sweep.Count 		= *Data++;
        Work->Sweep.StartNote 	= Work->Frequency.Data;
        Work->Sweep.PurposeNote = ((*Data++ + Work->Frequency.Trn) << 8) + Work->Frequency.Den + Work->L1.Freq + Work->L2.Freq;
        Work->Sweep.Offset 		= 0;
        Work->Sweep.OffsetDelta = SdSeqSetSweepDelta(Work);
        Work->Address 			= (uint32)Data;
    };
};


static void SdSeqInSideTempo(SdSeqWork_t* Work)
{
    if (Work->Step.StepCount-- == 1)
        SdSeqInSideStep(Work);
    else
        SdSeqOutSideStep(Work);

    if (Work->Tempo.MoveCount)
    {
        if (--Work->Tempo.MoveCount)
        {
            Work->Tempo.Data += Work->Tempo.MoveData;
            Work->Tempo.Data = Clamp(Work->Tempo.Data, 0x100, 0xFF00);
        }
        else
        {
            Work->Tempo.Data = Work->Tempo.MoveDelta;
        };
    };

    if (Work->Volume.MoveCount)
    {
        if (--Work->Volume.MoveCount)
        {
            Work->Volume.Track += Work->Volume.MoveDelta;
            Work->Volume.Track = Clamp(Work->Volume.Track, 0x0, 0xFF00);
        }
        else
        {
            Work->Volume.Track = Work->Volume.MoveData;
        };
    };

    if (Work->Pan.MoveCount)
    {
        if (--Work->Pan.MoveCount)
            Work->Pan.Data += Work->Pan.MoveDelta;
        else
            Work->Pan.Data = Work->Pan.MoveData;
    };

    if (Work->SPan.MoveCount)
    {
        if (--Work->SPan.MoveCount)
            Work->SPan.Data += Work->SPan.MoveDelta;
        else
            Work->SPan.Data = Work->SPan.MoveData;
    };
};


static void SdSeqCrxStepProcess(SdSeqWork_t* Work)
{
    for (int32 i = 0; i < COUNT_OF(Work->CrxParam); ++i)
    {
        SdCrxParam_t* CrxParam = &Work->CrxParam[i];

        if (!CrxParam->MoveCount)
            continue;

        if (--CrxParam->MoveCount)
            CrxParam->Data += CrxParam->MoveDelta;
        else
            CrxParam->Data = CrxParam->MoveData;
    };
};


static void SdSeqParamChangeProcess(SdSeqWork_t* Work)
{
    for (int32 i = 0; i < COUNT_OF(Work->PcParam); ++i)
    {
        SdPcParam_t* PcParam = &Work->PcParam[i];

        if (PcParam->Data == PcParam->PurposeData)
            continue;

        PcParam->Data += PcParam->Speed;
        if (PcParam->Speed <= 0)
        {
            if (PcParam->PurposeData >= PcParam->Data)
                PcParam->Data = PcParam->PurposeData;
        }
        else
        {
            if (PcParam->Data >= PcParam->PurposeData)
                PcParam->Data = PcParam->PurposeData;
        };
    };
};


static void SdSeqCommand(SdSeqWork_t* Work)
{
    /* seq ctrl */
    SdSeqParamChangeProcess(Work);

    Work->Tempo.Count += Work->Tempo.Data;

    if (Work->Tempo.Count < 0x10000)
    {
        if (Work->Sweep.Count && !Work->Sweep.HoldCount)
        {
            Work->Sweep.Offset += Work->Sweep.OffsetDelta;
            if (Work->Sweep.PosFlag)
                Work->Sweep.Result = SdSeqGetPortamentNote(Work);
            else
                Work->Sweep.Result = SdSeqSweepNote(Work);
        };

        int32 vibNote = 0;
        if (Work->Vibrato.Deep && !Work->Vibrato.HoldCount)
        {
            Work->Vibrato.Offset += Work->Vibrato.OffsetDelta;
            vibNote = SdSeqGetVibratoNote(Work);
        };

        int32 note = Work->RdcParam[0] + Work->PcParam[0].Data + Work->CrxParam[0].Data + Work->Sweep.Result + vibNote;
        int32 rnd = SdSeqRandomExe(Work);
        SdSeqSetFrequency(Work, note + rnd);
    }
    else
    {
        Work->Tempo.Count = 0;

        SdSeqCrxStepProcess(Work);
        SdSeqInSideTempo(Work);
    };

    /* volume ctrl */
    int32 VolData = Work->Volume.Velocity + Work->L1.Volume + Work->L2.Volume;
    VolData = Clamp(VolData, 0, 0x7F);

    int32 VolTrack = Work->Volume.Track + (4 * Work->CrxParam[1].Data);
    VolTrack = Clamp(VolTrack, 0, 0xFF00);

    int32 v = (VolData * VolTrack) >> 8;
    int32 idx = v >> 7;
    int32 Vol = SD_INTERPOLATE(SdVolumeTable[idx], SdVolumeTable[idx + 1], v);

    /* pc param ctrl */
    if (Work->PcParam[1].Data)
    {
        int32 Data = (Work->PcParam[1].Data + 0x10000) >> 8;
        v = Work->PcParam[1].Data + 0x10000;

        Vol = Vol * SD_INTERPOLATE(Data, Data + 1, v);
        if (Vol > 0x1FFF)
            Vol = 0x1FFF;
    };

    /* rdc param ctrl */
    if (Work->RdcParam[1])
    {
        Vol = (Vol * (Work->RdcParam[1] + 0x100)) >> 8;
        if (Vol > 0x1FFF)
            Vol = 0x1FFF;
    };

    /* volume limiter ctrl */
    for (int32 i = 0; i < COUNT_OF(SdSeqVolumeLimiter); ++i)
    {
        SdVolumeLimiter_t* Limiter = &SdSeqVolumeLimiter[i];

        if (Work->Code >= Limiter->CodeLow && Limiter->RdcFlag >= Work->Code)
        {
            if (Limiter->BestVolume < Vol)
                Limiter->BestVolume = Vol;

            idx = (Limiter->Correction >> 8);
            ASSERT(idx >= 0);
            ASSERT(idx < COUNT_OF(SdFadeOutTable));

            Vol = (Vol * SD_INTERPOLATE(SdFadeOutTable[idx], SdFadeOutTable[idx + 1], Limiter->Correction)) >> 16;

            break;
        };
    };

    /* drv volume down ctrl */
    if (SdDrvVolumeDownData && (!(Work->Option && SD_SEQ_OPT_0x20)))
    {
        idx = (SdDrvVolumeDownData >> 8);
        ASSERT(idx >= 0);
        ASSERT(idx < COUNT_OF(SdFadeOutTable));

        Vol = (Vol * SD_INTERPOLATE(SdFadeOutTable[idx], SdFadeOutTable[idx + 1], SdDrvVolumeDownData)) >> 16;
    };

    /* surround ctrl - dist */
    if (Work->SurCtrl.Dist)
        Vol = (Vol * (0xFFFF - Work->SurCtrl.Dist)) >> 16;

    /* fade out ctrl */
    if (Work->FadeOut.Data)
    {
        Work->FadeOut.Data += Work->FadeOut.Speed;
        if (Work->FadeOut.Data < SD_FADE_MAX)
        {
            if (Work->FadeOut.Data > 0)
            {
                idx = (Work->FadeOut.Data >> 8);
                ASSERT(idx >= 0);
                ASSERT(idx < COUNT_OF(SdFadeOutTable));

                Vol = (Vol * SD_INTERPOLATE(SdFadeOutTable[idx], SdFadeOutTable[idx + 1], Work->FadeOut.Data)) >> 16;
            }
            else
            {
                Work->Code = 0;
                Work->Priority = 0;
                Work->KeyOffWait = 1;
                Work->FadeOut.Data = 0;
                Work->FadeOut.Speed = 0;
                Vol = 0;
            };
        }
        else
        {
            Work->FadeOut.Data = 0;
            Work->FadeOut.Speed = 0;
        };
    };

    /* pan ctrl */
    int32 VolLeft = 0;
    int32 VolRight = 0;
    if (SdDrvSeqPanProcSelectionCB() == 1)
        SdSeqPanPotDolby(Work, &VolLeft, &VolRight, Vol);
    else
        SdSeqPanPotOld(Work, &VolLeft, &VolRight, Vol);

    SdDrvSeqVolumeCB(Work, &VolLeft, &VolRight);

    if (VolLeft < 0)
        VolLeft = -VolLeft;

    if (VolRight < 0)
        VolRight = -VolRight;

    if (Work->PhaseReversal)
        SdIOSetVolume(Work->ChNo, -VolLeft, -VolRight);
    else
        SdIOSetVolume(Work->ChNo, VolLeft, VolRight);
};


void SdSeqInit(void)
{
    std::memset(SdSeqVolumeLimiter, 0, sizeof(SdSeqVolumeLimiter));
};


void SdSeqTerm(void)
{
    std::memset(SdSeqVolumeLimiter, 0, sizeof(SdSeqVolumeLimiter));
};


void SdSeqMain(void)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqWork);++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (!Work->Address)
            continue;

        ASSERT(Work->CheckKeyTop == SD_CANARY_TOP);
        ASSERT(Work->CheckKeyEnd == SD_CANARY_END);

        if ((Work->CheckKeyTop != SD_CANARY_TOP) ||
            (Work->CheckKeyEnd != SD_CANARY_END))
            continue;

        if (SdSeqMaskCheck(Work))
            continue;

        if (Work->KeyOffWait)
        {
            if (!--Work->KeyOffWait)
                SdIOSetKeyOff(Work->ChNo);
        };

        if (Work->Code)
        {
            if (Work->SdWait)
            {
                --Work->SdWait;
                continue;
            };

            if (Work->Option & SD_SEQ_OPT_0x4)
            {
                if (SdDrvPause)
                    continue;
                
                Work->Tempo.Data += SdDrvTempoUpData;
            };

            SdSeqCommand(Work);
        };
    };
};


int32 SdSeqMaskCheck(SdSeqWork_t* Work)
{
    return Work->Mask;
};


void SdSeqMask(int32 _no, int32 _data)
{
    SDNOIMPL();
    
    ASSERT(_no >= 0);
    ASSERT(_no < COUNT_OF(SdSeqWork));

    if (_no >= COUNT_OF(SdSeqWork))
        return;

    SdSeqWork_t* Work = &SdSeqWork[_no];

    if ((Work->CheckKeyTop == SD_CANARY_TOP) && (Work->CheckKeyEnd == SD_CANARY_END))
        Work->Mask = _data;
};


void SdSeqAllOff(int32 _option)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (!Work->Code)
            continue;

        if (SdSeqMaskCheck(Work))
            continue;

        if ((Work->Option & _option) != 0)
            continue;

        SdSeqOff(Work, 3);
    };
};


void SdSeqOff(SdSeqWork_t* Work, uint16 _keyOffWait)
{
    int32 volLeft = 0;
    int32 volRight = 0;
    SdIOSetVolume(Work->ChNo, volLeft, volRight);

    Work->Code = 0;
    Work->Volume.Track = 0;
    Work->Priority = 0;
    Work->KeyOffWait = _keyOffWait;
};


void SdSeqToneSet(SdSeqWork_t* Work, int32 _toneNo, int32 _drumNo)
{
    int32 DrumIdx = (_drumNo - 72);
    int32 no = 0;
    int32 WaveBank = 0;

    SdEnvelopeDrum_t* DrmTbl = nullptr;

    if (_drumNo)
    {
        if (!SdDrumTable.Table || (SdDrumTable.Count <= DrumIdx))
            return;

        DrmTbl = (SdEnvelopeDrum_t*)SdDrumTable.Table;
        no = DrmTbl[DrumIdx].sno;
    }
    else
    {
        no = _toneNo;
    }

    Work->Sno = (uint16)no;

    if (no >= 128)
    {
        WaveBank = Work->TransWaveNo;
        no -= 128;
    };

    ASSERT(WaveBank >= 0);
    ASSERT(WaveBank < COUNT_OF(SdEnvelopeTable));

    if (SdEnvelopeTable[WaveBank].Flag)
    {
        if (SdEnvelopeTable[WaveBank].Flag == 1)
            SdSeqToneEnvSet(Work, WaveBank, no, _drumNo);
        else
            SdSeqToneNoEnvSet(Work, WaveBank, no);

        if (!Work->CtrlPan && !Work->Pan.Mode)
        {
            if (DrmTbl)
                Work->Pan.Data = (DrmTbl[DrumIdx].pan << 8);
            else
                Work->Pan.Data = 0x1400;
        };
    };
};


void SdSeqPauseSet(int32 _option)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (!Work->Code)
            continue;

        if (SdSeqMaskCheck(Work))
            continue;

        if ((Work->Option & _option) != 0)
            continue;

        Work->KeyOffWait = 2;

        int32 volLeft = 0;
        int32 volRight = 0;
        SdIOSetVolume(Work->ChNo, volLeft, volRight);
    };
};


void SdSeqPauseClr(int32 _option)
{
    (void)_option;
};