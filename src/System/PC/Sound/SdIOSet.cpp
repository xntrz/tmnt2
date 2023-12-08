#include "SdIOSet.hpp"
#include "SdTypes.hpp"


#define SDIO_MESSAGE_TYPE_FREQ  (0)
#define SDIO_MESSAGE_TYPE_ADDR  (1)
#define SDIO_MESSAGE_TYPE_LVOL  (2)
#define SDIO_MESSAGE_TYPE_RVOL  (3)


struct SdIOMessage_t
{
    int32 Channel;
    int32 Type;
    int32 Value;
    uint32 Code;
};


struct SdIOAdsr_t
{
    uint8 Ar;
    uint8 Dr;
    uint8 Sl;
    uint8 Sr;
    uint8 Rr;
};


struct SdIOEffect_t
{
    uint16 Flag;
    uint16 Mode;
    uint32 Delay;
    uint32 Feedback;
    uint32 Depth_L;
    uint32 Depth_R;
};


struct SdIOPack_t
{
    int32           KeyOn[SD_SEQ_CHANNEL_NUM];
    int32           KeyOff[SD_SEQ_CHANNEL_NUM];
    int32           EffectOn[SD_SEQ_CHANNEL_NUM];
    int32           EffectOff[SD_SEQ_CHANNEL_NUM];
    SdIOEffect_t    Effect[2];
    SdIOAdsr_t      Adsr[SD_SEQ_CHANNEL_NUM];
    SdIOMessage_t   Message[512];
    int32           MessageCount;
};


static SdIOPack_t SdIOPack;


static inline int32 SdIORangeCheck(int32 ch)
{
    return ((ch >= 0) && (ch < SD_SEQ_CHANNEL_NUM));
};


static inline int32 SdIOGetCoreNo(int32 ch)
{
    return (ch >= (SD_SEQ_CHANNEL_NUM / 2));
};


static bool SdIOSetMessage(int32 Channel, int32 Type, int32 Value)
{
    if (SdIOPack.MessageCount >= COUNT_OF(SdIOPack.Message))
        return false;

    int32 idx = SdIOPack.MessageCount++;

    SdIOPack.Message[idx].Channel   = Channel;
    SdIOPack.Message[idx].Type      = Type;
    SdIOPack.Message[idx].Value     = Value;
    SdIOPack.Message[idx].Code      = 0;

    return true;
};


static void SdIODispatchADSR(void)
{

};


static void SdIODispatchMessage(void)
{
    for (int32 i = 0; i < SdIOPack.MessageCount; ++i)
    {    
        switch (SdIOPack.Message[i].Type)
        {
        case SDIO_MESSAGE_TYPE_FREQ:
            break;

        case SDIO_MESSAGE_TYPE_ADDR:
            break;

        case SDIO_MESSAGE_TYPE_LVOL:
            break;

        case SDIO_MESSAGE_TYPE_RVOL:
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    SdIOPack.MessageCount = 0;
};


static void SdIODispatchSetEffect(void)
{

};


static void SdIODispatchEffect(void)
{

};


static void SdIODispatchKey(void)
{

};


void SdIOSetInitialize(void)
{
    std::memset(&SdIOPack, 0, sizeof(SdIOPack));
};


void SdIOSetTerminate(void)
{
    ;
};


void SdIOSetTask(void)
{
    SdIODispatchADSR();
    SdIODispatchMessage();
    SdIODispatchEffect();
    SdIODispatchSetEffect();
    SdIODispatchKey();
};


void SdIOSetKeyOff(int32 ch)
{
    if (SdIORangeCheck(ch))
        SdIOPack.KeyOff[ch] = -1;
};


void SdIOSetKeyOn(int32 ch)
{
    if (SdIORangeCheck(ch))
        SdIOPack.KeyOn[ch] = -1;
};


void SdIOSetAds(int32 ch, uint8 ar, uint8 dr, uint8 sl)
{
    if (SdIORangeCheck(ch))
    {
        SdIOPack.Adsr[ch].Ar = (0xFF - ar) | 0x80;
        SdIOPack.Adsr[ch].Dr = (0xF  - dr) | 0x80;
        SdIOPack.Adsr[ch].Sl = (0xF  - sl) | 0x80;
    };
};


void SdIOSetSrRr(int32 ch, uint8 sr, uint8 rr)
{
    if (SdIORangeCheck(ch))
    {
        SdIOPack.Adsr[ch].Sr = sr | 0x80;
        SdIOPack.Adsr[ch].Rr = (0x1F - rr) | 0x80;
    };
};


void SdIOSetSr(int32 ch, uint8 sr)
{
    if (SdIORangeCheck(ch))
        SdIOPack.Adsr[ch].Sr = sr | 0x80;
};


void SdIOSetRr(int32 ch, uint8 rr)
{
    if (SdIORangeCheck(ch))
        SdIOPack.Adsr[ch].Rr = (0x1F - rr) | 0x80;
};


void SdIOSetEffectOn(int32 ch)
{
    if (SdIORangeCheck(ch))
        SdIOPack.EffectOn[ch] = -1;
};


void SdIOSetEffectOff(int32 ch)
{
    if (SdIORangeCheck(ch))
        SdIOPack.EffectOff[ch] = -1;
};


void SdIOSetVolume(int32 ch, int32 lvol, int32 rvol)
{
    if (SdIORangeCheck(ch))
    {
        SdIOSetMessage(ch, SDIO_MESSAGE_TYPE_LVOL, (lvol & SD_SEQ_VOL_MAX));
        SdIOSetMessage(ch, SDIO_MESSAGE_TYPE_RVOL, (rvol & SD_SEQ_VOL_MAX));
    };
};


void SdIOSetSsAddress(int32 ch, int32 addr)
{
    if (SdIORangeCheck(ch))
        SdIOSetMessage(ch, SDIO_MESSAGE_TYPE_ADDR, addr);
};


void SdIOSetFreq(int32 ch, int32 freq)
{
    if (SdIORangeCheck(ch))
        SdIOSetMessage(ch, SDIO_MESSAGE_TYPE_FREQ, freq);
};


void SdIOSetEffectMode(int32 ch, int32 mode)
{
    ;
};