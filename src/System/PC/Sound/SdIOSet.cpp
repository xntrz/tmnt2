#include "SdIOSet.hpp"
#include "SdTypes.hpp"
#include "SdSeqPc.hpp"
#include "SdSeq.hpp"
#include "SdLog.hpp"


#define SDIO_MESSAGE_TYPE_FREQ  (0)
#define SDIO_MESSAGE_TYPE_ADDR  (1)
#define SDIO_MESSAGE_TYPE_LVOL  (2)
#define SDIO_MESSAGE_TYPE_RVOL  (3)


struct SdIOMessage_t
{
    int32 Channel;
    int32 Type;
    int32 Value;
    int32 Code;
};


struct SdIOAdsr_t
{
    uint8 Ar;
    uint8 Dr;
    uint8 Sl;
    uint8 Sr;
    uint8 Rr;
};


struct SdIOPack_t
{
    int32           KeyOn[SD_PC_SEQ_CHANNEL_NUM];
    int32           KeyOff[SD_PC_SEQ_CHANNEL_NUM];
    SdIOAdsr_t      Adsr[SD_SEQ_CHANNEL_NUM];
    SdIOMessage_t   Message[512];
    int32           MessageCount;
};


static SdIOPack_t SdIOPack;


static inline int32 SdIORangeCheck(int32 _ch)
{
    return ((_ch >= 0) && (_ch < SD_SEQ_CHANNEL_NUM));
};


static bool SdIOSetMessage(int32 _channel, int32 _type, int32 _value)
{
    if (SdIOPack.MessageCount >= COUNT_OF(SdIOPack.Message))
        return false;

    int32 idx = SdIOPack.MessageCount++;

    SdIOPack.Message[idx].Channel   = _channel;
    SdIOPack.Message[idx].Type      = _type;
    SdIOPack.Message[idx].Value     = _value;
    SdIOPack.Message[idx].Code      = SdSeqWork[_channel].Code;

    return true;
};


static void SdIODispatchADSR(void)
{
    for (int32 i = 0; i < COUNT_OF(SdIOPack.Adsr); ++i)
    {
        SdIOAdsr_t* Adsr = &SdIOPack.Adsr[i];

        if (Adsr->Sl & 0x80)
        {
            Adsr->Sl &= 0x7F;
            SdSeqPcSetSL(i, Adsr->Sl);
        };

        if (Adsr->Ar & 0x80)
        {
            Adsr->Ar &= 0x7F;
            SdSeqPcSetAR(i, Adsr->Ar);
        };

        if (Adsr->Dr & 0x80)
        {
            Adsr->Dr &= 0x7F;
            SdSeqPcSetDR(i, Adsr->Dr);
        };

        if (Adsr->Sr & 0x80)
        {
            Adsr->Sr &= 0x7F;
            SdSeqPcSetSR(i, Adsr->Sr);
        };

        if (Adsr->Rr & 0x80)
        {
            Adsr->Rr &= 0x7F;
            SdSeqPcSetRR(i, Adsr->Rr);
        };
    };	
};


static void SdIODispatchMessage(void)
{
    for (int32 i = 0; i < SdIOPack.MessageCount; ++i)
    {    
        switch (SdIOPack.Message[i].Type)
        {
        case SDIO_MESSAGE_TYPE_FREQ:
            SdSeqPcSetFreq(SdIOPack.Message[i].Channel, SdIOPack.Message[i].Value);
            break;

        case SDIO_MESSAGE_TYPE_ADDR:
            SdSeqPcSetAddress(SdIOPack.Message[i].Channel,
                              SdIOPack.Message[i].Code,
                              (SdWaveDataHdr_t*)SdIOPack.Message[i].Value);
            break;

        case SDIO_MESSAGE_TYPE_LVOL:
            SdSeqPcSetVolL(SdIOPack.Message[i].Channel, SdIOPack.Message[i].Value);
            break;

        case SDIO_MESSAGE_TYPE_RVOL:
            SdSeqPcSetVolR(SdIOPack.Message[i].Channel, SdIOPack.Message[i].Value);
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    SdIOPack.MessageCount = 0;
};


static void SdIODispatchKey(void)
{
    for (int32 i = 0; i < COUNT_OF(SdIOPack.KeyOff); ++i)
    {
        if (SdIOPack.KeyOff[i])
        {
            SdSeqPcKeyOff(i);
            SdIOPack.KeyOff[i] = 0;
        };
    };

    for (int32 i = 0; i < COUNT_OF(SdIOPack.KeyOn); ++i)
    {
        if (SdIOPack.KeyOn[i])
        {
            SdSeqPcKeyOn(i);
            SdIOPack.KeyOn[i] = 0;
        };
    };
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
    SdIODispatchADSR(); 		// +
    SdIODispatchMessage(); 		// +
    SdIODispatchKey(); 			// +
};


void SdIOSetKeyOff(int32 _ch)
{
    if (SdIORangeCheck(_ch))
        SdIOPack.KeyOff[_ch] = -1;
};


void SdIOSetKeyOn(int32 _ch)
{
    if (SdIORangeCheck(_ch))
        SdIOPack.KeyOn[_ch] = -1;
};


void SdIOSetAds(int32 _ch, int32 _ar, int32 _dr, int32 _sl)
{
    ASSERT(_ar >= 0);
    ASSERT(_ar <= 0xFF);
    ASSERT(_dr >= 0);
    ASSERT(_dr <= 0xF);
    ASSERT(_sl >= 0);
    ASSERT(_sl <= 0xF);

    if (SdIORangeCheck(_ch))
    {
        SdIOPack.Adsr[_ch].Ar = ((0xFF - ((uint8)_ar)) | 0x80);
        SdIOPack.Adsr[_ch].Dr = ((0xF  - ((uint8)_dr)) | 0x80);
        SdIOPack.Adsr[_ch].Sl = ((0xF  - ((uint8)_sl)) | 0x80);
    };
};


void SdIOSetSrRr(int32 _ch, int32 _sr, int32 _rr)
{
    if (SdIORangeCheck(_ch))
    {
        SdIOSetSr(_ch, _sr);
        SdIOSetRr(_ch, _rr);
    };
};


void SdIOSetSr(int32 _ch, int32 _sr)
{
    ASSERT(_sr >= 0);
    ASSERT(_sr <= 0xFF);

    if (SdIORangeCheck(_ch))
        SdIOPack.Adsr[_ch].Sr = (((uint8)_sr) | 0x80);
};


void SdIOSetRr(int32 _ch, int32 _rr)
{
    ASSERT(_rr >= 0);
    ASSERT(_rr <= 0x1F);

    if (SdIORangeCheck(_ch))
        SdIOPack.Adsr[_ch].Rr = ((0x1F - ((uint8)_rr)) | 0x80);
};


void SdIOSetVolume(int32 _ch, int32 _lvol, int32 _rvol)
{
    if (SdIORangeCheck(_ch))
    {
        SdIOSetMessage(_ch, SDIO_MESSAGE_TYPE_LVOL, (_lvol & SD_SEQ_VOL_MAX));
        SdIOSetMessage(_ch, SDIO_MESSAGE_TYPE_RVOL, (_rvol & SD_SEQ_VOL_MAX));
    };
};


void SdIOSetSsAddress(int32 _ch, int32 _address)
{
    if (SdIORangeCheck(_ch))
        SdIOSetMessage(_ch, SDIO_MESSAGE_TYPE_ADDR, _address);
};


void SdIOSetFreq(int32 _ch, int32 _freq)
{
    if (SdIORangeCheck(_ch))
        SdIOSetMessage(_ch, SDIO_MESSAGE_TYPE_FREQ, _freq);
};