#include "SdSeqCtrl.hpp"
#include "SdIOSet.hpp"
#include "SdSeq.hpp"
#include "SdLog.hpp"


static bool SdSeqCtrlTpmSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlSnoSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlTpsSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlSvlSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlSvpSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlVolSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlVomSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlAdsSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlSrsSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlRrsSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlSpnSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlTblSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlUsxSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlPanSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlPamSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlTrnSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlDtsSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlVibSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlVchSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlRdmSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlSwsSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlPorSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlL1SSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlL1ESet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlL2SSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlL2ESet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlL3SSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlL3ESet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlKksSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlKkeSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlEtoSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlEndSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlUseSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlKyuSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlTieSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlEc1Set(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlEc2Set(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlEonSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlEcfSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlDummyX(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlRdcSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlCrsSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlCrcSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlCrfSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlCrvSet(SdSeqWork_t*, uint8**);
static bool SdSeqCtrlCrpSet(SdSeqWork_t*, uint8**);


static bool SdSeqCtrlTpsSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Tempo.Data = (int32)*(*Data)++ << 8;
    return false;
};


static bool SdSeqCtrlTpmSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Tempo.MoveCount = (int32)*(*Data)++;
    Work->Tempo.MoveDelta = (int32)*(*Data)++ << 8;

    int32 TpmData = Work->Tempo.MoveDelta - Work->Tempo.Data;
    TpmData = Clamp(TpmData, -0x7F00, 0x7F00);
    TpmData /= Work->Tempo.MoveCount;
    TpmData = Clamp(TpmData, -0x7F0, 0x7F0);

    return false;
};


static bool SdSeqCtrlSnoSet(SdSeqWork_t* Work, uint8** Data)
{
    if (Work->CtrlPan)
    {
        int32 PanData = Work->CtrlPan * 80;
        PanData = Clamp(PanData, 0, 0x2800);

        Work->Pan.Data = PanData;
        Work->Pan.MoveCount = 0;
    };

    if ((Work->Sno != (uint16)**Data) && Work->Step.GateTime)
        SdIOSetKeyOff(Work->ChNo);
    
    SdSeqToneSet(Work, (int32)*(*Data)++, 0);

    return false;
};


static bool SdSeqCtrlSvlSet(SdSeqWork_t* Work, uint8** Data)
{
    SdSeqCtrlVolSet(Work, Data);
    SdSeqCtrlSnoSet(Work, Data);
    
    return false;
};


static bool SdSeqCtrlSvpSet(SdSeqWork_t* Work, uint8** Data)
{
    SdSeqCtrlVolSet(Work, Data);
    SdSeqCtrlSnoSet(Work, Data);
    SdSeqCtrlPanSet(Work, Data);
    
    return false;
};


static bool SdSeqCtrlVolSet(SdSeqWork_t* Work, uint8** Data)
{
    int32 VolData = (int32)*(*Data)++;

    if (Work->CtrlVol)
        VolData *= Work->CtrlVol;
    else
        VolData <<= 8;

    Work->Volume.Track = VolData;
    Work->Volume.MoveCount = 0;
    
    return false;
};


static bool SdSeqCtrlVomSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Volume.MoveCount = (int32)*(*Data)++;
    Work->Volume.MoveData = (int32)*(*Data)++ << 8;

    int32 MoveCount = Work->Volume.MoveCount;
    if (!MoveCount)
        MoveCount = 1;

    int32 VolData = (Work->Volume.MoveData - Work->Volume.Track) / MoveCount;
    VolData = Clamp(VolData, -0x7F0, 0x7F0);

    Work->Volume.MoveDelta = VolData;
    
    return false;
};


static bool SdSeqCtrlAdsSet(SdSeqWork_t* Work, uint8** Data)
{
    uint8 ar = *(*Data)++;
    uint8 drsl = *(*Data)++;

    SdIOSetAds(Work->ChNo, ar, (drsl >> 4) & 0xF, drsl & 0xF);
    
    return false;
};


static bool SdSeqCtrlSrsSet(SdSeqWork_t* Work, uint8** Data)
{
    SdIOSetSr(Work->ChNo, *(*Data)++);
    
    return false;
};


static bool SdSeqCtrlRrsSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->RrRate = (uint16)*(*Data) & 0x1F;
    SdIOSetRr(Work->ChNo, *(*Data)++ & 0x1F);

    return false;
};


static bool SdSeqCtrlSpnSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->SPan.MoveCount = (int32)*(*Data)++;
    Work->SPan.MoveDelta = (int32)((*(*Data)++ & 0x7F) << 8);

    int32 MoveCount = Work->SPan.MoveCount;
    if (!MoveCount)
        MoveCount = 1;

    int32 PanData = (Work->SPan.MoveData - Work->SPan.Data) / MoveCount;
    PanData = Clamp(PanData, -0x7F0, 0x7F0);

    Work->SPan.MoveDelta = PanData;
    if (!Work->SPan.MoveCount)
        Work->SPan.Data = Work->SPan.MoveData;

    return false;
};


static bool SdSeqCtrlTblSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->nTableIndex = (int32)*(*Data);

    ++*Data;
    ++*Data;
    ++*Data;

    return false;
};


static bool SdSeqCtrlUsxSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    ASSERT(false);

    return false;
};


static bool SdSeqCtrlPanSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Pan.Mode = (int32)(*(*Data) >> 7);
    Work->Pan.Data = (int32)((*(*Data)++ & 0x7F) << 8);
    Work->Pan.MoveCount = 0;

    return false;
};


static bool SdSeqCtrlPamSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Pan.MoveCount = (int32)*(*Data)++;
    Work->Pan.MoveData = (int32)((*(*Data)++ & 0x7F) << 8);

    int32 PanData = (Work->Pan.MoveData - Work->Pan.Data) / Work->Pan.MoveCount;
    PanData = Clamp(PanData, -0x7F0, 0x7F0);

    Work->Pan.MoveDelta = PanData;

    return false;
};


static bool SdSeqCtrlTrnSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Frequency.Trn = (int32)*(*Data)++;

    return false;
};


static bool SdSeqCtrlDtsSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Frequency.Den = 4 * (int32)*(*Data)++;

    return false;
};


static bool SdSeqCtrlVibSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Vibrato.HoldStep = (int16)*(*Data)++;

    int32 VibData = (int32)*(*Data)++;
    int32 Speed = 0x2000;

    if (VibData != 255)
        Speed = 32 * VibData;

    Work->Vibrato.OffsetDelta = Speed;
    Work->Vibrato.Deep = (int32)(*(*Data)++ << 8);

    if (Work->Vibrato.HoldStep > 512)
    {
        Work->Vibrato.ChangStep = Work->Vibrato.HoldStep - 512;
        Work->Vibrato.ChangDelta = Work->Vibrato.Deep / Work->Vibrato.ChangStep;
        Work->Vibrato.HoldStep = 0;
    };

    return false;
};


static bool SdSeqCtrlVchSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Vibrato.ChangStep = (int16)*(*Data)++;
    if (Work->Vibrato.ChangStep)
        Work->Vibrato.ChangDelta = Work->Vibrato.Deep / Work->Vibrato.ChangStep;

    return false;
};


static bool SdSeqCtrlRdmSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Random.Speed = (int32)*(*Data)++;
    Work->Random.Data = (int32)(*(*Data)++ << 8);
    Work->Random.Data += (int32)*(*Data)++;
    Work->Random.Count = 0;
    Work->Random.Offset = 0;
    
    return false;
};


static bool SdSeqCtrlSwsSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Sweep.PosFlag = 0;
    Work->Sweep.HoldStep = (int16)*(*Data)++;
    Work->Sweep.Step = (int16)*(*Data)++;
    Work->Sweep.OffsetDeep = (int32)(*(*Data)++ << 8);

    return false;
};


static bool SdSeqCtrlPorSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Sweep.HoldStep = 0;
    Work->Sweep.Step = (int16)*(*Data)++;
    Work->Sweep.PosFlag = Work->Sweep.Step != 0;

    return false;
};


static bool SdSeqCtrlL1SSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->L1.Address = (uint32)*Data;
    Work->L1.Count = 0;
    Work->L1.Volume = 0;
    Work->L1.Freq = 0;

    return false;
};


static bool SdSeqCtrlL1ESet(SdSeqWork_t* Work, uint8** Data)
{
    ++Work->L1.Count;

    int32 Count = (int32)*(*Data)++;

    if ((Work->L1.Count == Count) && Count)
    {
        Work->L1.Freq = 0;
        Work->L1.Volume = 0;
        ++*Data;
        ++*Data;
    }
    else
    {
        Work->L1.Volume += (int32)*(*Data)++;
        Work->L1.Freq += (int32)(*(*Data)++) * 8;
        *Data = (uint8*)Work->L1.Address;
    };

    return false;
};


static bool SdSeqCtrlL2SSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->L2.Address = (uint32)*Data;
    Work->L2.Count = 0;
    Work->L2.Volume = 0;
    Work->L2.Freq = 0;

    return false;
};


static bool SdSeqCtrlL2ESet(SdSeqWork_t* Work, uint8** Data)
{
    ++Work->L2.Count;

    int32 Count = (int32)*(*Data)++;

    if ((Work->L2.Count == Count) && Count)
    {
        Work->L2.Freq = 0;
        Work->L2.Volume = 0;
        ++*Data;
        ++*Data;
    }
    else
    {
        Work->L2.Volume += (int32)*(*Data)++;
        Work->L2.Freq += (int32)*(*Data)++ * 8;
        *Data = (uint8*)Work->L2.Address;
    };

    return false;
};


static bool SdSeqCtrlL3SSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->L3.Address = (uint32)*Data;

    return false;
};


static bool SdSeqCtrlL3ESet(SdSeqWork_t* Work, uint8** Data)
{
    *Data = (uint8*)Work->L3.Address;

    return false;
};


static bool SdSeqCtrlKksSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Brackets.Ptr1Address = (void*)*Data;
    Work->Brackets.Flag = 0;

    return false;
};


static bool SdSeqCtrlKkeSet(SdSeqWork_t* Work, uint8** Data)
{
    if (Work->Brackets.Flag == 2)
    {
        Work->Brackets.Flag = 1;
        *Data = (uint8*)Work->Brackets.Ptr2Address;
    }
    else if (Work->Brackets.Flag == 1)
    {
        Work->Brackets.Flag = 2;
        Work->Brackets.Ptr2Address = (void*)*Data;
        *Data = (uint8*)Work->Brackets.Ptr1Address;
    }
    else if (Work->Brackets.Flag == 0)
    {
        Work->Brackets.Flag = 1;
    };

    return false;
};


static bool SdSeqCtrlEtoSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->PhaseReversal = (uint16)*(*Data)++;

    return false;
};


static bool SdSeqCtrlUseSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;

    ++*Data;
    ++*Data;
    ++*Data;

    return false;
};


static bool SdSeqCtrlKyuSet(SdSeqWork_t* Work, uint8** Data)
{
    SdIOSetVolume(Work->ChNo, 0, 0);

    Work->KeyOffWait = 1;
    Work->Step.StepCount = (int16)*(*Data)++;
    Work->Step.GateTime = 1;

    return true;
};


static bool SdSeqCtrlTieSet(SdSeqWork_t* Work, uint8** Data)
{
    Work->Step.StepCount = (int16)*(*Data)++;
    Work->Step.GateTime = (int16)*(*Data)++;

    if (!Work->Step.GateTime)
    {
        Work->Step.GateCount = 0;
        return true;
    };

    Work->Step.GateCount = (Work->Step.GateTime * Work->Step.StepCount) >> 7;
    if (Work->Step.GateCount)
        return true;

    Work->Step.GateCount = 1;

    return true;
};


static bool SdSeqCtrlEc1Set(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();
    
    return false;
};


static bool SdSeqCtrlEc2Set(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();

    return false;
};


static bool SdSeqCtrlEonSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();

    return false;
};


static bool SdSeqCtrlEcfSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();

    return false;
};


static bool SdSeqCtrlRdcSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();

    return false;
};


static bool SdSeqCtrlCrsSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();

    return false;
};


static bool SdSeqCtrlCrcSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();
    
    return false;
};


static bool SdSeqCtrlCrfSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();

    return false;
};


static bool SdSeqCtrlCrvSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();

    return false;
};


static bool SdSeqCtrlCrpSet(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;
    (void)Data;

    SDNOIMPL();

    return false;
};


static bool SdSeqCtrlEndSet(SdSeqWork_t* Work, uint8** Data)
{
    SdSeqOff(Work, 1);

    return true;
};


static bool SdSeqCtrlDummyX(SdSeqWork_t* Work, uint8** Data)
{
    (void)Work;

    ++*Data;
    ++*Data;
    ++*Data;

    return false;
};


static bool(*const SdSeqCtrlTbl[])(SdSeqWork_t*, uint8**) =
{
    SdSeqCtrlTpsSet,
    SdSeqCtrlTpmSet,
    SdSeqCtrlSnoSet,
    SdSeqCtrlSvlSet,
    SdSeqCtrlSvpSet,
    SdSeqCtrlVolSet,
    SdSeqCtrlVomSet,
    SdSeqCtrlAdsSet,
    SdSeqCtrlSrsSet,
    SdSeqCtrlRrsSet,
    SdSeqCtrlSpnSet,
    SdSeqCtrlTblSet,
    SdSeqCtrlUseSet,
    SdSeqCtrlPanSet,
    SdSeqCtrlPamSet,
    SdSeqCtrlTrnSet,
    SdSeqCtrlDtsSet,
    SdSeqCtrlVibSet,
    SdSeqCtrlVchSet,
    SdSeqCtrlRdmSet,
    SdSeqCtrlDummyX,
    SdSeqCtrlSwsSet,
    SdSeqCtrlPorSet,
    SdSeqCtrlL1SSet,
    SdSeqCtrlL1ESet,
    SdSeqCtrlL2SSet,
    SdSeqCtrlL2ESet,
    SdSeqCtrlL3SSet,
    SdSeqCtrlL3ESet,
    SdSeqCtrlKksSet,
    SdSeqCtrlKkeSet,
    SdSeqCtrlEtoSet,
    SdSeqCtrlEndSet,
    SdSeqCtrlUseSet,
    SdSeqCtrlKyuSet,
    SdSeqCtrlTieSet,
    SdSeqCtrlEc1Set,
    SdSeqCtrlEc2Set,
    SdSeqCtrlEonSet,
    SdSeqCtrlEcfSet,
    SdSeqCtrlDummyX,
    SdSeqCtrlRdcSet,
    SdSeqCtrlCrsSet,
    SdSeqCtrlCrcSet,
    SdSeqCtrlCrfSet,
    SdSeqCtrlCrvSet,
    SdSeqCtrlCrpSet,
    SdSeqCtrlEndSet,
};


bool SdSeqCtrlFunc(uint8 _code, struct SdSeqWork_t* Work, uint8** Data)
{
    uint8 idx = _code + 48;
    if (_code >= 208)
        return SdSeqCtrlTbl[idx](Work, Data);
    else
        return SdSeqCtrlEndSet(Work, Data);
};