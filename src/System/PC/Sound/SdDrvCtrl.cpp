#include "SdDrvCtrl.hpp"
#include "SdDrv.hpp"
#include "SdStr.hpp"


/*extern*/ SdSetDriverCode_t SdDrvNextCodeBuffer;
/*extern*/ int32 SdDrvVolumeDownData = 0;
/*extern*/ int32 SdDrvVolumeDownSpeed = 0;
/*extern*/ int32 SdDrvAllFadeOutData = 0;
/*extern*/ int32 SdDrvAllFadeOutSpeed = 0;
/*extern*/ bool SdDrvPause = false;
/*extern*/ bool SdDrvStereoFlag = false;
/*extern*/ int32 SdDrvTempoUpSpeed = 0;
/*extern*/ int32 SdDrvTempoUpData = 0;


static void SdDrvCtrlAllOff(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;

    // TODO SdSeqAllOff

    SdVoxReset(100, SDSTR_FADEOUT_FLAG_RST_QUE);
    SdVagReset(100, SDSTR_FADEOUT_FLAG_RST_QUE);

    SdDrvPause = false;
    SdDrvNextCodeBuffer.Code = 0;
    SdDrvVolumeDownData = 0;
    SdDrvVolumeDownSpeed = 0;
    SdDrvAllFadeOutData = 0;
    SdDrvAllFadeOutSpeed = 0;
};


static void SdDrvCtrlBgmOff(uint32 Param1, uint32 Param2)
{
    (void)Param2;
    
    switch (Param1)
    {
    case SD_DRVCTRL_FADEOUT_ALL:
        {
            
        }
        break;

    case SD_DRVCTRL_FADEOUT_SEQ_NEXT:
        {
            
        }
        break;

    case SD_DRVCTRL_FADEOUT_VAG_NEXT:
        {
            
        }
        break;

    case SD_DRVCTRL_FADEOUT_AUTO:
        {
            
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


static void SdDrvCtrlPauseSet(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
    
    SdDrvPause = true;

    //
    //  TODO sd seq
    //

    SdStrPauseSet();
};


static void SdDrvCtrlPauseClr(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
    
    SdDrvPause = false;
    
    SdStrPauseClr();
};


static void SdDrvCtrlStereo(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
    
    SdDrvStereoFlag = false;
    
    SdVagSetStereo();
    SdDrvSetSurMode(false);
};


static void SdDrvCtrlMono(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
    
    SdDrvStereoFlag = true;

    SdVagSetMono();
    SdDrvSetSurMode(false);
};


static void SdDrvCtrlSurSet(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
    
    SdDrvStereoFlag = false;

    SdDrvSetSurMode(true);
};


static void SdDrvCtrlFadeOut1(uint32 Param1, uint32 Param2)
{
    int32 time = 320;

    if (Param2)
        time = int32(Param2);
    
    switch (Param1)
    {
    case SD_DRVCTRL_FADEOUT_ALL:
        {
            SdDrvNextCodeBuffer.Code = 0;
            
            SdDrvSetSeqBGMFadeOut(time);
            SdVagReset(time, SDSTR_FADEOUT_FLAG_RST_QUE);            
        }
        break;

    case SD_DRVCTRL_FADEOUT_SEQ_NEXT:
        {
            SdDrvNextCodeBuffer.Code = 0;

            SdDrvSetSeqBGMFadeOut(time);
        }
        break;

    case SD_DRVCTRL_FADEOUT_VAG_NEXT:
        {
            SdDrvNextCodeBuffer.Code = 0;

            SdVagReset(time, SDSTR_FADEOUT_FLAG_RST_QUE);
        }
        break;

    case SD_DRVCTRL_FADEOUT_AUTO:
        {
            SdDrvSetSeqBGMFadeOut(time);
            SdVagReset(time, SDSTR_FADEOUT_FLAG_RST_QUE);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


static void SdDrvCtrlFadeOut2(uint32 Param1, uint32 Param2)
{
    SdDrvCtrlFadeOut1(Param1, (Param2 ? Param2 : 640));
};


static void SdDrvCtrlFadeOut3(uint32 Param1, uint32 Param2)
{
    SdDrvCtrlFadeOut1(Param1, (Param2 ? Param2 : 1280));
};


static void SdDrvCtrlAllFadeOut(uint32 Param1, uint32 Param2)
{
    (void)Param2;
    
    int32 time = 320;

    if (Param1)
        time = int32(Param1);
    
    SdDrvSetSeqAllFadeOut(time, 0x80); // TODO seq opt mask
    SdVoxReset(time, SDSTR_FADEOUT_FLAG_RST_QUE);
    SdVagReset(time, SDSTR_FADEOUT_FLAG_RST_QUE);
};


static void SdDrvCtrlTempoUpSet(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlTempoUpClr(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVolDownSet(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVolDownClr(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxFadeOut(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxFadeOut2(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxOff(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxClr(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxRevOn(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxRevOff(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxCorOn(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxCorOff(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVoxCorNo(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVagRevOn(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVagRevOff(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVagCorOn(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVagCorOff(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDrvCtrlVagCorNo(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDriverCtrPrgFadeOut(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void SdDriverCtrPrgOff(uint32 Param1, uint32 Param2)
{
    (void)Param1;
    (void)Param2;
};


static void(*const SdDrvCtrlTbl[])(uint32, uint32) =
{
    SdDrvCtrlAllOff,
    SdDrvCtrlBgmOff,
    SdDrvCtrlPauseSet,
    SdDrvCtrlPauseClr,
    SdDrvCtrlStereo,
    SdDrvCtrlMono,
    SdDrvCtrlSurSet,
    SdDrvCtrlFadeOut1,
    SdDrvCtrlFadeOut2,
    SdDrvCtrlFadeOut3,
    SdDrvCtrlAllFadeOut,
    SdDrvCtrlTempoUpSet,
    SdDrvCtrlTempoUpClr,
    SdDrvCtrlVolDownSet,
    SdDrvCtrlVolDownClr,
    SdDrvCtrlAllFadeOut,
    SdDrvCtrlVoxFadeOut,
    SdDrvCtrlVoxFadeOut2,
    SdDrvCtrlVoxOff,
    SdDrvCtrlVoxClr,
    SdDrvCtrlVoxRevOn,
    SdDrvCtrlVoxRevOff,
    SdDrvCtrlVoxCorOn,
    SdDrvCtrlVoxCorOff,
    SdDrvCtrlVoxCorNo,
    SdDrvCtrlVagRevOn,
    SdDrvCtrlVagRevOff,
    SdDrvCtrlVagCorOn,
    SdDrvCtrlVagCorOff,
    SdDrvCtrlVagCorNo,
    SdDriverCtrPrgFadeOut,
    SdDriverCtrPrgOff,
};


void SdDrvCtrlFunc(uint32 Code, uint32 Param1, uint32 Param2)
{
    int32 CodeKind = SdDrvGetCodeKind(Code);

    ASSERT(CodeKind == SD_CODEKIND_CTRL);

    if (CodeKind != SD_CODEKIND_CTRL)
        return;

    int32 Func = Code - SD_CODE_CTRL_MIN;
    
    ASSERT(Func >= 0 && Func < COUNT_OF(SdDrvCtrlTbl));
    
    if (Func >= 0 && Func < COUNT_OF(SdDrvCtrlTbl))
        SdDrvCtrlTbl[Func](Param1, Param2);
};