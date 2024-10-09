#include "SdSeqPc.hpp"
#include "SdDs.hpp"
#include "SdSeqPcTable.hpp"
#include "SdDsTable.hpp"
#include "SdLog.hpp"


#define SD_SEQ_ADSR_STATE_ATTACK    (0)
#define SD_SEQ_ADSR_STATE_DECAY     (1)
#define SD_SEQ_ADSR_STATE_SUSTAIN   (2)
#define SD_SEQ_ADSR_STATE_RELEASE   (3)
#define SD_SEQ_ADSR_STATE_OFF       (4)

#define SD_SEQ_STOP_WAIT_TIME       (5730)


struct SdSeqWorkPC_t
{
    IDirectSoundBuffer* Buff;
    SdWaveDataHdr_t* Header;
    int32 State;
    int32 KeyOnRq;
    int32 KeyOffRq;
    int32 Value;
    int32 ReleaseData;
    int32 AttackRate;
    int32 DecayRate;
    int32 SustainRate;
    int32 ReleaseRate;
    int32 SustainLevel;
    int32 Code;
    int32 VolumeL;
    int32 VolumeR;
};


static SdSeqWorkPC_t SdSeqWorkPC[52];


static inline bool SdSeqPcIsPortValid(int32 _port)
{
    ASSERT(_port >= 0);
    ASSERT(_port < COUNT_OF(SdSeqWorkPC));

    return ((_port >= 0) &&
            (_port < COUNT_OF(SdSeqWorkPC)));
};


static inline bool SdSeqPcIsBuffValid(int32 _port)
{
    ASSERT(_port >= 0);
    ASSERT(_port < COUNT_OF(SdSeqWorkPC));
    ASSERT(SdSeqWorkPC[_port].Buff != nullptr);

    return (SdSeqWorkPC[_port].Buff != nullptr);
};


static void SdSeqPcSetVolume(SdSeqWorkPC_t* Work)
{
    ASSERT(Work->Value >= 0);
    ASSERT(Work->Buff != nullptr);

    DWORD dwStatus = 0;
    HRRET(IDirectSoundBuffer8_GetStatus(Work->Buff, &dwStatus));
    if (!(dwStatus & DSBSTATUS_PLAYING))
        return;

    int32 VolL = SdDsConvVol(Work->VolumeL);
    int32 VolR = SdDsConvVol(Work->VolumeR);
    int32 VolH = SdDsBuffSetPan(Work->Buff, VolL, VolR);
    
    int32 VolDS = VolH - (-SdDsConvVol(Work->Value));
    VolDS = Clamp(VolDS, DSBVOLUME_MIN, DSBVOLUME_MAX);
    
    HRRET(IDirectSoundBuffer8_SetVolume(Work->Buff, VolDS));
};


void SdSeqPcInit(void)
{
    std::memset(SdSeqWorkPC, 0, sizeof(SdSeqWorkPC));
};


void SdSeqPcTerm(void)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqWorkPC); ++i)
    {
        SdSeqWorkPC_t* Work = &SdSeqWorkPC[i];

        if (Work->Buff)
        {
            HRRET(IDirectSoundBuffer8_Stop(Work->Buff));

            int32 counter = (SD_SEQ_STOP_WAIT_TIME * 10);
            while (--counter)
            {
                DWORD dwStatus = 0;
                HRRET(IDirectSoundBuffer_GetStatus(Work->Buff, &dwStatus));
                if (!(dwStatus & DSBSTATUS_PLAYING))
                    break;
            };

            SdDsBuffDestroy(Work->Buff);
            Work->Buff = nullptr;
        };
    };
};


void SdSeqPcTask(void)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqWorkPC); ++i)
    {
        SdSeqWorkPC_t* Work = &SdSeqWorkPC[i];

        if (Work->KeyOnRq)
        {
            Work->KeyOnRq = 0;
            
            DWORD dwFlags = 0;
            if (((Work->Header->PlayPoint & 1) != 0) || (Work->Code == 0x2013))
                dwFlags |= DSBPLAY_LOOPING;

            if (Work->Buff)
            {
                HRRET(IDirectSoundBuffer8_SetCurrentPosition(Work->Buff, 0));
                HRRET(IDirectSoundBuffer8_Play(Work->Buff, 0, 0, dwFlags));
            };
        };
    };

    for (int32 i = 0; i < COUNT_OF(SdSeqWorkPC); ++i)
    {
        SdSeqWorkPC_t* Work = &SdSeqWorkPC[i];

        if (!Work->Buff)
            continue;

        DWORD dwStatus = 0;
        HRESULT hr = IDirectSoundBuffer8_GetStatus(Work->Buff, &dwStatus);
        if (FAILED(hr))
            continue;

        if (!(dwStatus & DSBSTATUS_PLAYING))
        {
            Work->Value = -10000;
            Work->State = SD_SEQ_ADSR_STATE_OFF;
        };

        switch (Work->State)
        {
        case SD_SEQ_ADSR_STATE_ATTACK:
            {
                Work->Value = ((Work->Value << 8) + Work->AttackRate) >> 8;
                if (Work->Value >= SD_SEQ_VOL_MAX)
                {
                    Work->Value = SD_SEQ_VOL_MAX;
                    Work->State = SD_SEQ_ADSR_STATE_DECAY;
                };

                SdSeqPcSetVolume(Work);
            }
            break;

        case SD_SEQ_ADSR_STATE_DECAY:
            {
                Work->Value = ((Work->Value << 8) - Work->DecayRate) >> 8;
                if (Work->Value <= Work->SustainLevel)
                {
                    Work->Value = Work->SustainLevel;
                    Work->State = SD_SEQ_ADSR_STATE_SUSTAIN;
                };

                SdSeqPcSetVolume(Work);
            }
            break;

        case SD_SEQ_ADSR_STATE_SUSTAIN:
            {		
                if (Work->KeyOffRq)
                {
                    if (Work->ReleaseRate)
                    {
                        int32 rd = SdSeqPcRrTable[Work->ReleaseRate];
                        if (rd)
                            Work->ReleaseData = (Work->Value / rd) << 8;

                        Work->Value -= Work->SustainRate;
                        Work->State = SD_SEQ_ADSR_STATE_RELEASE;
                    }
                    else
                    {
                        Work->Value -= Work->SustainRate;
                        Work->State = SD_SEQ_ADSR_STATE_OFF;
                    };
                }
                else
                {
                    Work->Value -= Work->SustainRate;
                };

                if (Work->Value <= 0)
                {
                    Work->Value = 0;
                    Work->State = SD_SEQ_ADSR_STATE_OFF;
                };

                SdSeqPcSetVolume(Work);
            }
            break;

        case SD_SEQ_ADSR_STATE_RELEASE:
            {
                Work->Value = ((Work->Value << 8) - Work->ReleaseData) >> 8;
                if (Work->Value <= 0)
                {
                    Work->Value = 0;
                    Work->State = SD_SEQ_ADSR_STATE_OFF;
                };

                SdSeqPcSetVolume(Work);
            }
            break;

        case SD_SEQ_ADSR_STATE_OFF:
            {
                Work->Value = 0;

                if (!(dwStatus & DSBSTATUS_PLAYING) ||
                    ((dwStatus & DSBSTATUS_LOOPING) == 0))
                {
                    SdSeqPcSetVolume(Work);
                }
                else
                {
                    HRRET(IDirectSoundBuffer8_SetVolume(Work->Buff, SdDsConvVol(0)));
                    HRRET(IDirectSoundBuffer8_Stop(Work->Buff));

                    int32 counter = SD_SEQ_STOP_WAIT_TIME;
                    while (true)
                    {
                        HRRET(IDirectSoundBuffer8_GetStatus(Work->Buff, &dwStatus));

                        if (!(dwStatus & DSBSTATUS_PLAYING))
                            break;

                        if (!--counter)
                        {
                            SdSeqPcSetVolume(Work);
                            break;
                        };
                    };
                };
            }
            break;

        default:
            break;
        };
    };
};


void SdSeqPcKeyOn(int32 _port)
{
    if (!SdSeqPcIsPortValid(_port))
        return;
    
    SdSeqWorkPC_t* Work = &SdSeqWorkPC[_port];

    ++Work->KeyOnRq;
    Work->Value = 0;
    Work->KeyOffRq = 0;
    Work->State = SD_SEQ_ADSR_STATE_ATTACK;
};


void SdSeqPcKeyOff(int32 _port)
{
    if (SdSeqPcIsPortValid(_port))
        ++SdSeqWorkPC[_port].KeyOffRq;
};


void SdSeqPcSetFreq(int32 _port, int32 _value)
{
    if (!SdSeqPcIsPortValid(_port) ||
        !SdSeqPcIsBuffValid(_port))
        return;
    
    const double reso = (48000.0 / 4096.0);

    int32 freq = (int32)((double)_value * reso);
    freq = Clamp(freq, 0x64, 0x30D40);

    HRRET(IDirectSoundBuffer8_SetFrequency(SdSeqWorkPC[_port].Buff, freq));
};


void SdSeqPcSetAR(int32 _port, uint8 _value)
{
    ASSERT(_value >= 0);
    ASSERT(_value < COUNT_OF(SdSeqPcArTable));

    if (!SdSeqPcIsPortValid(_port))
        return;

    int32 ar = SdSeqPcArTable[_value];
    if (ar == -1)
        SdSeqWorkPC[_port].AttackRate = 1;
    else
        SdSeqWorkPC[_port].AttackRate = (0x400000 / (ar ? ar : 1) - 1);
};


void SdSeqPcSetDR(int32 _port, uint8 _value)
{
    ASSERT(_value >= 0);
    ASSERT(_value < COUNT_OF(SdSeqPcDrTable));

    if (!SdSeqPcIsPortValid(_port))
        return;
    
    int32 dr = SdSeqPcDrTable[_value];
    int32 sl = SdSeqWorkPC[_port].SustainLevel;

    SdSeqWorkPC[_port].DecayRate = ((0x3FFF - sl) << 8) / (dr ? dr : 1);
};


void SdSeqPcSetSR(int32 _port, uint8 _value)
{
    ASSERT(_value >= 0);
    ASSERT(_value < COUNT_OF(SdSeqPcSrTable));

    if (!SdSeqPcIsPortValid(_port))
        return;

    int32 sr = SdSeqPcSrTable[_value];
    int32 sl = SdSeqWorkPC[_port].SustainLevel;

    SdSeqWorkPC[_port].SustainRate = (sr ? (sl / sr) : (0));
};


void SdSeqPcSetRR(int32 _port, uint8 _value)
{
    if (SdSeqPcIsPortValid(_port))
        SdSeqWorkPC[_port].ReleaseRate = _value;
};


void SdSeqPcSetSL(int32 _port, uint8 _value)
{
    if (SdSeqPcIsPortValid(_port))
        SdSeqWorkPC[_port].SustainLevel = (_value << 10) + 0x3FF;
};


void SdSeqPcSetVolL(int32 _port, int32 _value)
{
    if (SdSeqPcIsPortValid(_port))
        SdSeqWorkPC[_port].VolumeL = _value;
};


void SdSeqPcSetVolR(int32 _port, int32 _value)
{
    if (SdSeqPcIsPortValid(_port))
        SdSeqWorkPC[_port].VolumeR = _value;
};


void SdSeqPcSetAddress(int32 _port, int32 _code, SdWaveDataHdr_t* Header)
{
    if (!SdSeqPcIsPortValid(_port))
        return;

    SdSeqWorkPC_t* Work = &SdSeqWorkPC[_port];

    Work->Code = _code;
    Work->VolumeL = 0;
    Work->VolumeR = 0;
    Work->Header = Header;

    DSBUFFERDESC SdBuffDesc;
    std::memset(&SdBuffDesc, 0, sizeof(SdBuffDesc));

    WAVEFORMATEX SdWaveFmt;
    std::memset(&SdWaveFmt, 0, sizeof(SdWaveFmt));

    int32 SdBuffSize = SdDsSetWaveFormat(&SdWaveFmt, Work->Header);

    DWORD dwFlags = 0;
    dwFlags |= DSBCAPS_LOCSOFTWARE;
    dwFlags |= DSBCAPS_CTRLFREQUENCY;
    dwFlags |= DSBCAPS_CTRLPAN;
    dwFlags |= DSBCAPS_CTRLVOLUME;
    //dwFlags |= DSBCAPS_GLOBALFOCUS;
    dwFlags |= DSBCAPS_GETCURRENTPOSITION2;

    SdBuffDesc.dwSize 		= (DWORD)SdBuffSize;
    SdBuffDesc.dwFlags 		= dwFlags;
    SdBuffDesc.dwBufferBytes= (DWORD)Work->Header->Size;
    SdBuffDesc.lpwfxFormat 	= &SdWaveFmt;

    if (SdBuffDesc.dwSize && SdBuffDesc.dwBufferBytes)
    {
        if (Work->Buff)
        {
            HRRET(IDirectSoundBuffer8_Stop(Work->Buff));

            int32 counter = (SD_SEQ_STOP_WAIT_TIME * 10);
            while (--counter)
            {
                DWORD dwStatus = 0;
                HRRET(IDirectSoundBuffer_GetStatus(Work->Buff, &dwStatus));
                if (!(dwStatus & DSBSTATUS_PLAYING))
                    break;
            };
            
            SdDsBuffDestroy(Work->Buff);
            Work->Buff = nullptr;
        };
        
        if (SdDsBuffCreate(&Work->Buff, &SdBuffDesc))
        {
            HRRET(IDirectSoundBuffer8_SetVolume(Work->Buff, DSBVOLUME_MIN));
            SdDsBuffSetPan(Work->Buff, DSBVOLUME_MIN, DSBVOLUME_MIN);
            SdDsBuffLoadData(Work->Buff, &Work->Header[1], Work->Header->Size);
        };
    };
};