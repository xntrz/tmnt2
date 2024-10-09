#include "SdDrvCtrl.hpp"
#include "SdDrv.hpp"
#include "SdStr.hpp"
#include "SdSeq.hpp"
#include "SdLog.hpp"


/*extern*/ SdSetDriverCode_t SdDrvNextCodeBuffer;
/*extern*/ int32 SdDrvVolumeDownData = 0;
/*extern*/ int32 SdDrvVolumeDownSpeed = 0;
/*extern*/ int32 SdDrvAllFadeOutData = 0;
/*extern*/ int32 SdDrvAllFadeOutSpeed = 0;
/*extern*/ bool SdDrvPause = false;
/*extern*/ bool SdDrvStereoFlag = false;
/*extern*/ int32 SdDrvTempoUpSpeed = 0;
/*extern*/ int32 SdDrvTempoUpData = 0;


static void SdDrvCtrlAllOff(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SdSeqAllOff(SD_SEQ_OPT_0x40);
    SdVoxReset(100, SDSTR_FADEOUT_FLAG_RST_QUE);
    SdVagReset(100, SDSTR_FADEOUT_FLAG_RST_QUE);

    SdDrvPause = false;
    SdDrvNextCodeBuffer.Code = 0;
    SdDrvVolumeDownData = 0;
    SdDrvVolumeDownSpeed = 0;
    SdDrvAllFadeOutData = 0;
    SdDrvAllFadeOutSpeed = 0;
};


static void SdDrvCtrlBgmOff(uint32 _param1, uint32 _param2)
{
    (void)_param2;

    switch (_param1)
    {
    case SD_DRVCTRL_FADEOUT_ALL:
        {
            SdDrvSetSeqBGMOff();
            SdVagReset(100, SDSTR_FADEOUT_FLAG_RST_QUE);
            SdDrvNextCodeBuffer.Code = 0;
        }
        break;

    case SD_DRVCTRL_FADEOUT_SEQ_NEXT:
        {
            SdDrvSetSeqBGMOff();
            SdDrvNextCodeBuffer.Code = 0;
        }
        break;

    case SD_DRVCTRL_FADEOUT_VAG_NEXT:
        {
            SdVagReset(100, SDSTR_FADEOUT_FLAG_RST_QUE);
            SdDrvNextCodeBuffer.Code = 0;
        }
        break;

    case SD_DRVCTRL_FADEOUT_AUTO:
        {
            SdDrvSetSeqBGMOff();
            SdVagReset(100, SDSTR_FADEOUT_FLAG_RST_QUE);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


static void SdDrvCtrlPauseSet(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;
    
    SdDrvPause = true;

    SdSeqPauseSet(SD_SEQ_OPT_0x4);
    SdStrPauseSet();
};


static void SdDrvCtrlPauseClr(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;
    
    SdDrvPause = false;
    
    SdSeqPauseClr(SD_SEQ_OPT_0x4);
    SdStrPauseClr();
};


static void SdDrvCtrlStereo(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;
    
    SdDrvStereoFlag = true;
    SdDrvSurMode = false;

    SdStrSetStereo();
};


static void SdDrvCtrlMono(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;
    
    SdDrvStereoFlag = false;
    SdDrvSurMode = false;

    SdStrSetMonaural();
};


static void SdDrvCtrlSurSet(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;
    
    SdDrvStereoFlag = true;
    SdDrvSurMode = true;
};


static void SdDrvCtrlFadeOut1(uint32 _param1, uint32 _param2)
{
    /* NOTE: _param2 and time are in milliseconds */
    int32 Time = SD_FADEOUT_SPEED_FAST;

    if (_param2)
        Time = (int32)_param2;

    switch (_param1)
    {
    case SD_DRVCTRL_FADEOUT_ALL:
        {
            SdDrvNextCodeBuffer.Code = 0;
            SdDrvSetSeqBGMFadeOut(Time);
            SdVagReset(Time, SDSTR_FADEOUT_FLAG_RST_QUE);            
        }
        break;

    case SD_DRVCTRL_FADEOUT_SEQ_NEXT:
        {
            SdDrvNextCodeBuffer.Code = 0;
            SdDrvSetSeqBGMFadeOut(Time);
        }
        break;

    case SD_DRVCTRL_FADEOUT_VAG_NEXT:
        {
            SdDrvNextCodeBuffer.Code = 0;
            SdVagReset(Time, SDSTR_FADEOUT_FLAG_RST_QUE);
        }
        break;

    case SD_DRVCTRL_FADEOUT_AUTO:
        {
            SdDrvSetSeqBGMFadeOut(Time);
            SdVagReset(Time, SDSTR_FADEOUT_FLAG_RST_QUE);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


static void SdDrvCtrlFadeOut2(uint32 _param1, uint32 _param2)
{
    SdDrvCtrlFadeOut1(_param1, (_param2 ? _param2 : SD_FADEOUT_SPEED_NORMAL));
};


static void SdDrvCtrlFadeOut3(uint32 _param1, uint32 _param2)
{
    SdDrvCtrlFadeOut1(_param1, (_param2 ? _param2 : SD_FADEOUT_SPEED_SLOW));
};


static void SdDrvCtrlAllFadeOut(uint32 _param1, uint32 _param2)
{
    (void)_param2;
    
    int32 Time = SD_FADEOUT_SPEED_SLOW;
    if (_param1)
        Time = int32(_param1);
    
    SdDrvSetSeqAllFadeOut(Time, SD_SEQ_OPT_0x80);
    SdVoxReset(Time, SDSTR_FADEOUT_FLAG_RST_QUE);
    SdVagReset(Time, SDSTR_FADEOUT_FLAG_RST_QUE);
};


static void SdDrvCtrlTempoUpSet(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlTempoUpClr(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVolDownSet(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVolDownClr(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxFadeOut(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxFadeOut2(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxOff(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxClr(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxRevOn(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxRevOff(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxCorOn(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxCorOff(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVoxCorNo(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVagRevOn(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVagRevOff(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVagCorOn(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVagCorOff(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDrvCtrlVagCorNo(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDriverCtrPrgFadeOut(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
};


static void SdDriverCtrPrgOff(uint32 _param1, uint32 _param2)
{
    (void)_param1;
    (void)_param2;

    SDNOIMPL();
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


void SdDrvCtrlFunc(int32 Code, uint32 _param1, uint32 _param2)
{
    int32 CodeKind = SdDrvGetCodeKind(Code);
    ASSERT(CodeKind == SD_CODEKIND_CTRL);
    
    if (CodeKind != SD_CODEKIND_CTRL)
        return;

    int32 Func = Code - SD_CODE_CTRL_MIN;    
    ASSERT(Func >= 0 && Func < COUNT_OF(SdDrvCtrlTbl));
    
    if (Func >= 0 && Func < COUNT_OF(SdDrvCtrlTbl))
        SdDrvCtrlTbl[Func](_param1, _param2);
};