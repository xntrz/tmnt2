#include "SdCallback.hpp"
#include "SdLoader.hpp"
#include "SdDrv.hpp"
#include "SdDrvCtrl.hpp"
#include "SdStr.hpp"

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
    if (!FLAG_TEST(SdDrvOptFlag, SD_OPTFLAG_NOVAG))
    {
        SdVagInit(0);
    };

    if (!FLAG_TEST(SdDrvOptFlag, SD_OPTFLAG_NOVOX))
    {
        SdVoxInit(0);
        SdVoxInit(1);
    };
};


void SdDrvLoaderCommandCB(int32 Command, uint32 Param)
{
    uint32 WaveAddress = uint32(SdGetWaveAddr());

    if (Command > 1)
    {
        WaveAddress += SdGetWaveSize(0);
        WaveAddress += SdGetWaveSize(1);
        
        for (int32 i = 1; i < 64; ++i)      // skip first bank
            SdTransRelease(i);
        
        for (int32 i = 2; i < 128; ++i)     // skip first two banks
            SdWaveRelease(i);
    };

    switch (Command)
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

            SdWaveLoad(0, WaveAddress);
            WaveAddress += SdGetWaveSize(0);

            SdWaveLoad(1, WaveAddress);
            WaveAddress += SdGetWaveSize(1);

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
            
            switch (Param)
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
            }
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
    };
};


void SdDrvSetChannelBGMCB(void)
{
    SdDrvCtrlFunc(SD_CODE_DRVCTRL_BGMOFF, SD_DRVCTRL_FADEOUT_AUTO, 0);
};


int32 SdDrvVoiceServerSelectionCB(SdSetDriverCode_t* SdSetDrvCode)
{
    int32 result = SdSetDrvCode->Param3 & 1;
    
    SdSetDrvCode->Param3 = 0;
    
    return result;
};


int32 SdDrvBGMServerSelectionCB(SdSetDriverCode_t* SdSetDrvCode)
{
    (void)SdSetDrvCode;
    
    return 0;   // always first server
};


void SdDrvSetBeforeCB(int32 Reason, SdSetDriverCode_t* SdSetDrvCode)
{
    (void)Reason;

    int32 CodeKind = SdDrvGetCodeKind(SdSetDrvCode->Code);
    if (CodeKind == SD_CODEKIND_SE ||
        CodeKind == SD_CODEKIND_TRANS ||
        CodeKind == SD_CODEKIND_TRANS_BGM ||
        CodeKind == SD_CODEKIND_TRANS_SE)
    {
        if (FLAG_TEST(SdDrvOptFlag, SD_OPTFLAG_NOSEQ))
        {
            SdSetDrvCode->Code = 0;
            return;
        };
    };

    if ((SdSetDrvCode->ExtFlag & SD_MASTERCODE_FADE) != 0)
        return;

    switch (SdSetDrvCode->Code)
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
    };
};


void SdDrvSetAfterCB(int32 Reason, SdSetDriverCode_t* SdSetDrvCode)
{
    (void)Reason;
    (void)SdSetDrvCode;
};


void SdDrvVagVolumeCB(uint32 Code, int32* VolLeft, int32* VolRight)
{
    (void)Code;

    int32 Config = CGameSound::GetBgmRaw();

    ASSERT(Config >= 0 && Config < COUNT_OF(SdVCfg));

    float l = float(*VolLeft)  * SdVCfg[Config];
    float r = float(*VolRight) * SdVCfg[Config];

    *VolLeft  = int32(l);
    *VolRight = int32(r);
};


void SdDrvVoxVolumeCB(uint32 Code, int32* VolLeft, int32* VolRight)
{
    (void)Code;
    
    int32 Config = CGameSound::GetVoiceRaw();

    ASSERT(Config >= 0 && Config < COUNT_OF(SdVCfg));

    float l = float(*VolLeft)  * SdVCfg[Config];
    float r = float(*VolRight) * SdVCfg[Config];

    *VolLeft  = int32(l);
    *VolRight = int32(r);
};


void SdDrvSeqVolumeCB(struct SdSeq_t* SdSeq, int32* VolLeft, int32* VolRight)
{
    (void)SdSeq;
    (void)VolLeft;
    (void)VolRight;
};


int32 SdDrvSeqPanProcSelectionCB(void)
{
    //
    //  0 - OLD
    //  1 - DOLBY
    //  2 - TEST
    //
    
    return 2;
};


void SdDrvGameThreadCallBack(void)
{

};


void SdDrvUserThreadCallBack(void)
{
    //
    //  TODO ???
    //    
    if (SdTransCodeTable[14].Table
        && SdTransCodeTable[14].Table[198].TableAddress
        && !SdTransCodeTable[14].Table[198].WaveBank)
    {
        SdTransCodeTable[14].Table[198] = SdTransCodeTable[14].Table[199];
    };
};