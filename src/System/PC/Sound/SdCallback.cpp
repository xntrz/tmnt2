#include "SdCallback.hpp"
#include "SdLoader.hpp"
#include "SdDrv.hpp"
#include "SdDrvCtrl.hpp"
#include "SdStr.hpp"
#include "SdSeq.hpp"
#include "SdIOSet.hpp"
#include "SdLog.hpp"

#include "Game/System/Sound/GameSound.hpp"
#include "Game/Component/GameData/GameData.hpp"


static const float SdVCfg[] =
{
    1.0f,
    0.9f,
    0.8f,
    0.7f,
    0.6f,
    0.5f,
    0.4f,
    0.3f,
    0.2f,
    0.1f,
    0.0f
};


void SdDrvInitCB(void)
{
    SdVagInit(0);

    SdVoxInit(0);
    SdVoxInit(1);	
};


void SdDrvLoaderCommandCB(int32 _command, int32 _param)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (!Work->Code)
            continue;

        if (SdSeqMaskCheck(Work))
            continue;

        if (Work->Code <= 0x2004)
            continue;

        int32 CodeKind = SdDrvGetCodeKind(Work->Code);
        if ((CodeKind == SD_CODEKIND_TRANS_SE) || (CodeKind == SD_CODEKIND_TRANS_BGM))
            SdSeqOff(Work, 3);
    };

    uint32 WaveAddress = (uint32)SdGetWaveAddr();
    WaveAddress += SdGetWaveSize(0);
    
    if (_command > 1)
    {
        WaveAddress += SdGetWaveSize(1);

        for (int32 i = 1; i < COUNT_OF(SdTransCodeTable); ++i)      // skip first bank
            SdTransRelease(i);
        
        for (int32 i = 2; i < COUNT_OF(SdWaveTopAddress); ++i)     // skip first two banks
            SdWaveRelease(i);
    };

    switch (_command)
    {
    case 0:
        {
            SdSystemDataLoadInfo();
        }
        return;

    case 1:
        {
            SdTransReleaseAll();
            SdWaveReleaseAll();

            SdWaveLoad(0, (uint32)SdGetWaveAddr());
            SdWaveLoad(1, WaveAddress);

            SdTransLoad(0);
        }
        return;

    case 2:
        {
            ;
        }
        break;

    case 3:
        {
            SdWaveLoad(13, WaveAddress);
            WaveAddress += SdGetWaveSize(13);
            
            SdTransLoad(12);
            
            switch (_param)
            {
            case 1:
                {
                    SdWaveLoad(3, WaveAddress);
                    WaveAddress += SdGetWaveSize(3);

                    SdTransLoad(2);
                }
                break;

            case 2:
                {
                    SdWaveLoad(6, WaveAddress);
                    WaveAddress += SdGetWaveSize(6);

                    SdTransLoad(5);
                }
                break;

            case 3:
                {
                    SdWaveLoad(7, WaveAddress);
                    WaveAddress += SdGetWaveSize(7);

                    SdTransLoad(6);
                }
                break;

            case 4:
                {
                    SdWaveLoad(11, WaveAddress);
                    WaveAddress += SdGetWaveSize(11);

                    SdTransLoad(10);
                }
                break;

            default:
                break;
            };
        }
        break;
        
    case 4:
        {
            SdWaveLoad(12, WaveAddress);
            WaveAddress += SdGetWaveSize(12);
            
            SdTransLoad(11);
        }
        break;
        
    case 5:
        {
            SdWaveLoad(9, WaveAddress);
            WaveAddress += SdGetWaveSize(9);
            
            SdTransLoad(8);
        }
        break;
        
    case 6:
        {
            SdWaveLoad(3, WaveAddress);
            WaveAddress += SdGetWaveSize(3);
            
            SdTransLoad(2);
        }
        break;
        
    case 7:
        {
            SdWaveLoad(4, WaveAddress);
            WaveAddress += SdGetWaveSize(4);
            
            SdTransLoad(3);
        }
        break;

    case 8:
        {
            SdWaveLoad(8, WaveAddress);
            WaveAddress += SdGetWaveSize(8);

            SdTransLoad(7);
            
            SdWaveLoad(2, WaveAddress);
            WaveAddress += SdGetWaveSize(2);
            
            SdTransLoad(1);
        }
        break;
        
    case 9:
        {
            SdWaveLoad(11, WaveAddress);
            WaveAddress += SdGetWaveSize(11);
            
            SdTransLoad(10);
        }
        break;
        
    case 10:
        {
            SdWaveLoad(10, WaveAddress);
            WaveAddress += SdGetWaveSize(10);
            
            SdTransLoad(9);
        }
        break;
        
    case 11:
        {
            SdWaveLoad(6, WaveAddress);
            WaveAddress += SdGetWaveSize(6);
                        
            SdTransLoad(5);
            
            SdWaveLoad(7, WaveAddress);
            WaveAddress += SdGetWaveSize(7);

            SdTransLoad(6);
            
            SdWaveLoad(8, WaveAddress);
            WaveAddress += SdGetWaveSize(8);
            
            SdTransLoad(7);
        }
        break;
        
    case 12:
        {
            SdWaveLoad(14, WaveAddress);
            WaveAddress += SdGetWaveSize(14);
            
            SdTransLoad(13);
        }
        break;
        
    case 13:
        {
            SdWaveLoad(5, WaveAddress);
            WaveAddress += SdGetWaveSize(5);
            
            SdTransLoad(4);
        }
        break;

    default:
        break;
    };

    bool LoadFlag_Sla = false;
    bool LoadFlag_Kar = false;
    bool LoadFlag_Spl = false;
    int32 CharaCnt = CGameData::PlayParam().GetCharaInfoNum();
    
    for (int32 i = 0; i < CharaCnt; ++i)
    {
        switch (CGameData::PlayParam().CharaInfo(i).m_CharacterID)
        {
        case PLAYERID::ID_LEO:
            {
                SdWaveLoad(15, WaveAddress);
                WaveAddress += SdGetWaveSize(15);
            }
            break;

        case PLAYERID::ID_RAP:
            {
                SdWaveLoad(16, WaveAddress);
                WaveAddress += SdGetWaveSize(16);
            }
            break;

        case PLAYERID::ID_MIC:
            {
                SdWaveLoad(17, WaveAddress);
                WaveAddress += SdGetWaveSize(17);
            }
            break;

        case PLAYERID::ID_DON:
            {
                SdWaveLoad(18, WaveAddress);
                WaveAddress += SdGetWaveSize(18);
            }
            break;

        case PLAYERID::ID_SPL:
            {
                SdWaveLoad(19, WaveAddress);
                WaveAddress += SdGetWaveSize(19);
                LoadFlag_Spl = true;
            }
            break;

        case PLAYERID::ID_CAS:
            {
                SdWaveLoad(20, WaveAddress);
                WaveAddress += SdGetWaveSize(20);
            }
            break;

        case PLAYERID::ID_KAR:
            {
                SdWaveLoad(21, WaveAddress);
                WaveAddress += SdGetWaveSize(21);
                LoadFlag_Kar = true;
            }
            break;

        case PLAYERID::ID_SLA:
            {
                SdWaveLoad(22, WaveAddress);
                WaveAddress += SdGetWaveSize(22);
                LoadFlag_Sla = true;
            }
            break;

        default:
            break;
        };
    };
    
    SdTransLoad(14);

    switch (CGameData::PlayParam().GetStage())
    {
    case STAGEID::ID_ST18FB:
    case STAGEID::ID_ST60X_D04:
        {
            SdWaveLoad(23, WaveAddress);
            WaveAddress += SdGetWaveSize(23);
        }
        break;

    case STAGEID::ID_ST20FB:
    case STAGEID::ID_ST50NB:
    case STAGEID::ID_ST60X_D08:
        {
            if (!LoadFlag_Sla)
            {
                SdWaveLoad(22, WaveAddress);
                WaveAddress += SdGetWaveSize(22);
            };
        }
        break;

    case STAGEID::ID_ST30NB:
    case STAGEID::ID_ST60X_D03:
        {
            SdWaveLoad(24, WaveAddress);
            WaveAddress += SdGetWaveSize(24);
        }
        break;

    case STAGEID::ID_ST34N:
        {
            SdWaveLoad(13, WaveAddress);
            WaveAddress += SdGetWaveSize(13);
            
            SdTransLoad(12);
        }
        break;

    case STAGEID::ID_ST38NB:
    case STAGEID::ID_ST40OB:
    case STAGEID::ID_ST58OB1:
    case STAGEID::ID_ST58OB2:
    case STAGEID::ID_ST60X_C10:
        {
            SdWaveLoad(28, WaveAddress);
            WaveAddress += SdGetWaveSize(28);
        }
        break;

    case STAGEID::ID_ST43N:
    case STAGEID::ID_ST44N:
    case STAGEID::ID_ST44NB:
    case STAGEID::ID_ST60X_D05:
        {
            SdWaveLoad(26, WaveAddress);
            WaveAddress += SdGetWaveSize(26);
        }
        break;

    case STAGEID::ID_ST47OB:
    case STAGEID::ID_ST60X_C08:
    case STAGEID::ID_ST60X_D07:
        {
            if (!LoadFlag_Spl)
            {
                SdWaveLoad(19, WaveAddress);
                WaveAddress += SdGetWaveSize(19);
            };
        }
        break;

    case STAGEID::ID_ST52F:
    case STAGEID::ID_ST52FB:
    case STAGEID::ID_ST53F:
    case STAGEID::ID_ST53FB:
    case STAGEID::ID_ST54F:
    case STAGEID::ID_ST54FB:
    case STAGEID::ID_ST55F:
    case STAGEID::ID_ST55FB:
    case STAGEID::ID_ST60X_C02:
    case STAGEID::ID_ST60X_C04:
    case STAGEID::ID_ST60X_C07:
        {
            SdWaveLoad(25, WaveAddress);
            WaveAddress += SdGetWaveSize(25);
        }
        break;

    case STAGEID::ID_ST57NB:
    case STAGEID::ID_ST60X_C09:
        {
            if (!LoadFlag_Kar)
            {
                SdWaveLoad(21, WaveAddress);
                WaveAddress += SdGetWaveSize(21);
            };
        }
        break;

    case STAGEID::ID_ST60X_D09:
        {
            SdWaveLoad(27, WaveAddress);
            WaveAddress += SdGetWaveSize(27);
        }
        break;

    case STAGEID::ID_ST60X_D10:
        {
            SdWaveLoad(29, WaveAddress);
            WaveAddress += SdGetWaveSize(29);
        }
        break;

    default:
        break;
    };
};


int32 SdDrvVoiceServerSelectionCB(SdSetDriverCode_t* CodeBuff)
{
    int32 result = CodeBuff->Param3 & 1;
    CodeBuff->Param3 = 0;	
    return result;
};


int32 SdDrvBGMServerSelectionCB(SdSetDriverCode_t* CodeBuff)
{
    (void)CodeBuff;
    
    return 0;   // always first server
};


void SdDrvSetBeforeCB(int32 _reason, SdSetDriverCode_t* CodeBuff)
{
    /* handle drv opt */
    if ((_reason == SD_REASON_SETSOUND) ||
        (_reason == SD_REASON_SETUSE))
    {
        int32 CodeKind = SdDrvGetCodeKind(CodeBuff->Code);
        switch (CodeKind)
        {
        case SD_CODEKIND_SE:
        case SD_CODEKIND_TRANS_SE:
        case SD_CODEKIND_TRANS_BGM:
            if (FLAG_TEST(SdDrvOptFlag, SD_OPTFLAG_NOSEQ))
                CodeBuff->Code = 0;
            break;

        case SD_CODEKIND_VAG:
            if (FLAG_TEST(SdDrvOptFlag, SD_OPTFLAG_NOVAG))
                CodeBuff->Code = 0;
            break;

        case SD_CODEKIND_VOX:
            if (FLAG_TEST(SdDrvOptFlag, SD_OPTFLAG_NOVOX))
                CodeBuff->Code = 0;
            break;

        default:
            break;
        };
    };
    
    /* handle use code for characters SP */	
    if ((CodeBuff->ExtFlag & SD_MASTERCODE_FADEOUT) != 0)
        return;

    switch (CodeBuff->Code)
    {
    case 0x2031:
        SdDrvSetUseCode(0x10AD, 0, 0, 0);
        break;
        
    case 0x204A:
        SdDrvSetUseCode(0x10B5, 0, 0, 0);
        break;
        
    case 0x2063:
        SdDrvSetUseCode(0x10BD, 0, 0, 0);
        break;
        
    case 0x207C:
    case 0x2096:
        SdDrvSetUseCode(0x10C6, 0, 0, 0);
        break;

    case 0x20B0:
        SdDrvSetUseCode(0x10BD, 0, 0, 0);
        break;

    case 0x20CC:
        SdDrvSetUseCode(0x10AD, 0, 0, 0);
        break;
        
    case 0x20E8:
        SdDrvSetUseCode(0x10B5, 0, 0, 0);
        break;

    default:
        break;
    };
};


void SdDrvSetAfterCB(int32 _reason, SdSetDriverCode_t* CodeBuff)
{
    (void)_reason;
    (void)CodeBuff;
};


void SdDrvVagVolumeCB(int32 Code, int32* _volLeft, int32* _volRight)
{
    (void)Code;

    int32 Config = CGameSound::GetBgmRaw();

    ASSERT(Config >= 0 && Config < COUNT_OF(SdVCfg));

    float l = float(*_volLeft)  * SdVCfg[Config];
    float r = float(*_volRight) * SdVCfg[Config];

    *_volLeft  = int32(l);
    *_volRight = int32(r);
};


void SdDrvVoxVolumeCB(int32 Code, int32* _volLeft, int32* _volRight)
{
    (void)Code;
    
    int32 Config = CGameSound::GetVoiceRaw();

    ASSERT(Config >= 0 && Config < COUNT_OF(SdVCfg));

    float l = float(*_volLeft)  * SdVCfg[Config];
    float r = float(*_volRight) * SdVCfg[Config];

    *_volLeft  = int32(l);
    *_volRight = int32(r);
};


void SdDrvSeqVolumeCB(struct SdSeqWork_t* Work, int32* _volLeft, int32* _volRight)
{
    int32 Code = Work->Code;

    if (SdDrvPause &&
        (Code != 0x1002) &&
        (Code != 0x1003) &&
        (Code != 0x1004))
    {
        *_volLeft = 0;
        *_volRight = 0;
    };

    if ((Code == 0x10E0) ||
        (Code == 0x10DD) ||
        (Code == 0x10D5) ||
        (Code == 0x10D4) ||
        (Code == 0x10D3) ||
        (Code == 0x102F) ||
        (Code == 0x100A))
    {
        int32 cfg = CGameSound::GetVoiceRaw();

        *_volLeft  = (int32)((float)*_volLeft  * SdVCfg[cfg]);
        *_volRight = (int32)((float)*_volRight * SdVCfg[cfg]);
    }
    else
    {
        int32 cfg = CGameSound::GetSeRaw();

        *_volLeft  = (int32)((float)*_volLeft  * SdVCfg[cfg]);
        *_volRight = (int32)((float)*_volRight * SdVCfg[cfg]);
    };
};


int32 SdDrvSeqPanProcSelectionCB(void)
{
    //
    //  0 - OLD
    //  1 - DOLBY
    //
    
    return 1;
};


int32 SdDrvSetChannelBGMCB(SdSetDriverCode_t* CodeBuff, SdKdtTableData_t* TableData, int32 _option, int32 _priority)
{
    (void)CodeBuff;
    (void)TableData;
    (void)_option;
    (void)_priority;

    SdDrvCtrlFunc(SD_CODE_DRVCTRL_BGMOFF, SD_DRVCTRL_FADEOUT_AUTO, 0);
    
    return 0;
};


int32 SdDrvSetChannelSECB(SdSetDriverCode_t* CodeBuff, SdKdtTableData_t* TableData, int32 _option)
{
    (void)CodeBuff;
    (void)TableData;
    (void)_option;

    return 0;
};


void SdDrvSetToneBeforeCB(struct SdSeqWork_t* Work, int32 _status, struct SdSetEnvelope_t* SetEnv)
{
    (void)Work;
    (void)_status;
    (void)SetEnv;
};


void SdDrvSeqCorrectionCB(void)
{
    /* se seq corrections */
    if (SdSeCodeTable[0].Table)
    {
        ((SdKdtTableData_t*)SdSeCodeTable[0].Table)[252].Channel = 40;
        ((SdKdtTableData_t*)SdSeCodeTable[0].Table)[252].Option = 0xC8;
    };

    /* trans seq corrections */
    if (SdTransCodeTable[14].Table
        && SdTransCodeTable[14].Table[198].TableAddress
        && !SdTransCodeTable[14].Table[198].WaveBank)
    {
        SdTransCodeTable[14].Table[198].WaveBank = SdTransCodeTable[14].Table[199].WaveBank;
        SdTransCodeTable[14].Table[198].SetMode = SdTransCodeTable[14].Table[199].SetMode;
        SdTransCodeTable[14].Table[198].Channel = SdTransCodeTable[14].Table[199].Channel;
        SdTransCodeTable[14].Table[198].Count = SdTransCodeTable[14].Table[199].Count;
        SdTransCodeTable[14].Table[198].Option = SdTransCodeTable[14].Table[199].Option;
    };
};
