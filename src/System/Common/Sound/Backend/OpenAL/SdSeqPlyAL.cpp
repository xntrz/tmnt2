#include "SdAL.hpp"
#include "SdSeqPlyTable.hpp"
#include "SdTypes.hpp"
#include "SdLog.hpp"


#define SD_SEQ_ADSR_STATE_ATTACK    (0)
#define SD_SEQ_ADSR_STATE_DECAY     (1)
#define SD_SEQ_ADSR_STATE_SUSTAIN   (2)
#define SD_SEQ_ADSR_STATE_RELEASE   (3)
#define SD_SEQ_ADSR_STATE_OFF       (4)

#define SD_SEQ_STOP_WAIT_TIME       (5730)

#define SD_SEQ_LOOP_ENABLE_CNT      (1)


struct SdSeqPlyWork_t
{
    ALuint           Buffer;
    ALuint           Source;
    SdWaveDataHdr_t* Header;
    int32            State;
    int32            KeyOnRq;
    int32            KeyOffRq;
    int32            Value;
    int32            ReleaseData;
    int32            AttackRate;
    int32            DecayRate;
    int32            SustainRate;
    int32            ReleaseRate;
    int32            SustainLevel;
    int32            Code;
    int32            VolumeL;
    int32            VolumeR;
    float            GainOnFocusLost;
    int32            LoopCount;
    int32            PrevCode;
    int32            ForceLoopFlag;
};


static SdSeqPlyWork_t SdSeqPlyWork[52];
static bool SdSeqPlyFocusLostFlag = false;


static inline bool SdSeqPlyIsLoopCode(int32 _code)
{
    static int32 LoopCodes[] =
    {
        0x2013,
        0x2015,
    };

    for (int32 i = 0; i < COUNT_OF(LoopCodes); ++i)
    {
        if (LoopCodes[i] == _code)
            return true;
    };

    return false;
};


static inline bool SdSeqPlyIsPortValid(int32 _port)
{
    ASSERT(_port >= 0);
    ASSERT(_port < COUNT_OF(SdSeqPlyWork));

    return ((_port >= 0) &&
            (_port < COUNT_OF(SdSeqPlyWork)));
};


static inline bool SdSeqPlyIsBuffValid(int32 _port)
{
    ASSERT(_port >= 0);
    ASSERT(_port < COUNT_OF(SdSeqPlyWork));
    //ASSERT(SdSeqPlyWorkPC[_port].Buff != nullptr);

    return (SdSeqPlyWork[_port].Buffer != 0);
};


static void SdSeqPlySetVolume(SdSeqPlyWork_t* Work)
{
    ASSERT(Work->Value >= 0);
    ASSERT(Work->Source != 0);

    ALint State = 0;
    alCall(alGetSourcei(Work->Source, AL_SOURCE_STATE, &State));

    if (State != AL_PLAYING)
        return;

    int32 VolL = SdALConvVolToDsVol(Work->VolumeL);
    int32 VolR = SdALConvVolToDsVol(Work->VolumeR);
    int32 VolH = SdALBuffSetPan(Work->Source, VolL, VolR);

    const int32 DSBVOLUME_MAX = 0;
    const int32 DSBVOLUME_MIN = -10000;

    int32 VolDS = VolH - (-SdALConvVolToDsVol(Work->Value));
    VolDS = Clamp(VolDS, DSBVOLUME_MIN, DSBVOLUME_MAX);

    float Gain = SdALConvDsVolToGain(VolDS);
    alCall(alSourcef(Work->Source, AL_GAIN, Gain));
};


void SdSeqPlyInit(void)
{
    std::memset(SdSeqPlyWork, 0, sizeof(SdSeqPlyWork));

    for (int32 i = 0; i < COUNT_OF(SdSeqPlyWork); ++i)
    {
        SdSeqPlyWork_t* Work = &SdSeqPlyWork[i];

        alCall(alGenSources(1, &Work->Source));
        alCall(alGenBuffers(1, &Work->Buffer));
    };
};


void SdSeqPlyTerm(void)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqPlyWork); ++i)
    {
        SdSeqPlyWork_t* Work = &SdSeqPlyWork[i];

        if (Work->Source)
        {
            alCall(alSourceStop(Work->Source));
            alCall(alSourcei(Work->Source, AL_BUFFER, 0));

            alCall(alDeleteSources(1, &Work->Source));
            Work->Source = 0;
        };

        if (Work->Buffer)
        {
            alCall(alDeleteBuffers(1, &Work->Buffer));
            Work->Buffer = 0;
        };
    };
};


void SdSeqPlyTask(void)
{
    if (SdSeqPlyFocusLostFlag)
        return;
    
    for (int32 i = 0; i < COUNT_OF(SdSeqPlyWork); ++i)
    {
        SdSeqPlyWork_t* Work = &SdSeqPlyWork[i];

        if (Work->KeyOnRq)
        {
            Work->KeyOnRq = 0;
            
            if ((Work->Source != 0) &&
                (Work->Buffer != 0) &&
                !Work->ForceLoopFlag)
            {
                bool IsLoop = ((Work->Header->PlayPoint & 1) != 0) || SdSeqPlyIsLoopCode(Work->Code);

                //if ((Work->LoopCount > SD_SEQ_LOOP_ENABLE_CNT)&&
                //    !SdSeqPlyIsForceLoopIgnoreCode(Work->Code))
                //{
                //    Work->ForceLoopFlag = 1;
                //    IsLoop = true;
                //};

                alCall(alSourcei(Work->Source, AL_LOOPING, IsLoop ? AL_TRUE : AL_FALSE));
                alCall(alSourcePlay(Work->Source));
                alCall(alSourcei(Work->Source, AL_BYTE_OFFSET, 0));
            };
        };
    };

    for (int32 i = 0; i < COUNT_OF(SdSeqPlyWork); ++i)
    {
        SdSeqPlyWork_t* Work = &SdSeqPlyWork[i];

        if (!Work->Buffer)
            continue;

        if (!Work->Source)
            continue;

        ALint state = 0;
        alGetSourcei(Work->Source, AL_SOURCE_STATE, &state);

        bool isPlaying = (state == AL_PLAYING);

        if (!isPlaying)
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

                SdSeqPlySetVolume(Work);
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

                SdSeqPlySetVolume(Work);
            }
            break;

        case SD_SEQ_ADSR_STATE_SUSTAIN:
            {		
                if (Work->KeyOffRq)
                {
                    if (Work->ReleaseRate)
                    {
                        int32 rd = SdSeqPlyRrTable[Work->ReleaseRate];
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

                SdSeqPlySetVolume(Work);
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

                SdSeqPlySetVolume(Work);
            }
            break;

        case SD_SEQ_ADSR_STATE_OFF:
            {
                Work->Value = 0;

                ALint looping = 0;
                alGetSourcei(Work->Source, AL_LOOPING, &looping);

                bool isLooping = (looping == AL_TRUE);

                if (!isPlaying || !isLooping)
                {
                    SdSeqPlySetVolume(Work);
                }
                else
                {
                    alSourcef(Work->Source, AL_GAIN, 0.0f);
                    alSourceStop(Work->Source);
                };
            }
            break;

        default:
            SdSeqPlySetVolume(Work);
            break;
        };
    };
};


void SdSeqPlyKeyOn(int32 _port)
{
    if (!SdSeqPlyIsPortValid(_port))
        return;
    
    SdSeqPlyWork_t* Work = &SdSeqPlyWork[_port];

    ++Work->KeyOnRq;
    Work->Value = 0;
    Work->KeyOffRq = 0;
    Work->State = SD_SEQ_ADSR_STATE_ATTACK;

    ++Work->LoopCount;
};


void SdSeqPlyKeyOff(int32 _port)
{
    if (SdSeqPlyIsPortValid(_port))
        ++SdSeqPlyWork[_port].KeyOffRq;
};


void SdSeqPlySetFreq(int32 _port, int32 _value)
{
    if (!SdSeqPlyIsPortValid(_port) ||
        !SdSeqPlyIsBuffValid(_port))
        return;

    if (SdSeqPlyWork[_port].Source == 0)
        return;

    const double reso = (48000.0 / 4096.0);

    int32 freq = (int32)((double)_value * reso);
    freq = Clamp(freq, 0x64, 0x30D40);

    int32 baseFreq = SdSeqPlyWork[_port].Header->SamplesPerSec;

    float pitch = ((float)freq / (float)baseFreq);
    //pitch = Clamp(pitch, 0.1f, 10.0f);

    alCall(alSourcef(SdSeqPlyWork[_port].Source, AL_PITCH, pitch));
};


void SdSeqPlySetAR(int32 _port, uint8 _value)
{
    ASSERT(_value >= 0);
    ASSERT(_value < COUNT_OF(SdSeqPlyArTable));

    if (!SdSeqPlyIsPortValid(_port))
        return;

    int32 ar = SdSeqPlyArTable[_value];
    if (ar == -1)
        SdSeqPlyWork[_port].AttackRate = 1;
    else
        SdSeqPlyWork[_port].AttackRate = ((0x400000 / (ar ? ar : 1)) - 1);
};


void SdSeqPlySetDR(int32 _port, uint8 _value)
{
    ASSERT(_value >= 0);
    ASSERT(_value < COUNT_OF(SdSeqPlyDrTable));

    if (!SdSeqPlyIsPortValid(_port))
        return;
    
    int32 dr = SdSeqPlyDrTable[_value];
    int32 sl = SdSeqPlyWork[_port].SustainLevel;

    SdSeqPlyWork[_port].DecayRate = ((0x3FFF - sl) << 8) / (dr ? dr : 1);
};


void SdSeqPlySetSR(int32 _port, uint8 _value)
{
    ASSERT(_value >= 0);
    ASSERT(_value < COUNT_OF(SdSeqPlySrTable));

    if (!SdSeqPlyIsPortValid(_port))
        return;

    int32 sr = SdSeqPlySrTable[_value];
    int32 sl = SdSeqPlyWork[_port].SustainLevel;

    SdSeqPlyWork[_port].SustainRate = (sr ? (sl / sr) : (0));
};


void SdSeqPlySetRR(int32 _port, uint8 _value)
{
    if (SdSeqPlyIsPortValid(_port))
        SdSeqPlyWork[_port].ReleaseRate = _value;
};


void SdSeqPlySetSL(int32 _port, uint8 _value)
{
    if (SdSeqPlyIsPortValid(_port))
        SdSeqPlyWork[_port].SustainLevel = (_value << 10) + 0x3FF;
};


void SdSeqPlySetVolL(int32 _port, int32 _value)
{
    if (SdSeqPlyIsPortValid(_port))
        SdSeqPlyWork[_port].VolumeL = _value;
};


void SdSeqPlySetVolR(int32 _port, int32 _value)
{
    if (SdSeqPlyIsPortValid(_port))
        SdSeqPlyWork[_port].VolumeR = _value;
};


void SdSeqPlySetAddress(int32 _port, int32 _code, SdWaveDataHdr_t* Header)
{
    if (!SdSeqPlyIsPortValid(_port))
        return;

    SdSeqPlyWork_t* Work = &SdSeqPlyWork[_port];

    Work->PrevCode = Work->Code;
    Work->LoopCount = 0;
    Work->ForceLoopFlag = 0;

    Work->Code = _code;
    Work->VolumeL = 0;
    Work->VolumeR = 0;
    Work->Header = Header;

    if (Work->Header && (Work->Header->Size > 0))
    {
        if (Work->Source != 0)
        {
            alCall(alSourceStop(Work->Source));
            alCall(alSourcei(Work->Source, AL_BUFFER, 0));
        }
        else
        {
            alCall(alGenSources(1, &Work->Source));
            alCall(alGenBuffers(1, &Work->Buffer));
        };

        if (Work->Source == 0)
            return;

        if (Work->Buffer == 0)
            return;

        alCall(alBufferData(Work->Buffer,
                            SdALGetWaveFormat(Work->Header),
                            &Work->Header[1],
                            Work->Header->Size,
                            Work->Header->SamplesPerSec));

        alCall(alSourcei(Work->Source, AL_BUFFER, Work->Buffer));
        alCall(alSourcef(Work->Source, AL_GAIN, 0.0f));
        alCall(alSourcei(Work->Source, AL_LOOPING, AL_FALSE));
        alCall(alSource3f(Work->Source, AL_POSITION, 0.0f, 0.0f, 0.0f));
        alCall(alSourcei(Work->Source, AL_SOURCE_RELATIVE, AL_TRUE));
        alCall(alSourcef(Work->Source, AL_ROLLOFF_FACTOR, 0.0f));
    };
};


void SdSeqPlyFocusLost(void)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqPlyWork); ++i)
    {
        SdSeqPlyWork_t* Work = &SdSeqPlyWork[i];

        if (Work->Source)
        {
            alCall(alGetSourcef(Work->Source, AL_GAIN, &Work->GainOnFocusLost));
            alCall(alSourcef(Work->Source, AL_GAIN, 0.0f));
            alCall(alSourcePause(Work->Source));
        };
    };

    SdSeqPlyFocusLostFlag = true;
};


void SdSeqPlyFocusRestore(void)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqPlyWork); ++i)
    {
        SdSeqPlyWork_t* Work = &SdSeqPlyWork[i];

        if (Work->Source)
        {
            alCall(alSourcef(Work->Source, AL_GAIN, Work->GainOnFocusLost));

            ALint State = 0;
            alCall(alGetSourcei(Work->Source, AL_SOURCE_STATE, &State));

            if (State == AL_PAUSED)
                alCall(alSourcePlay(Work->Source));
        };      
    };

    SdSeqPlyFocusLostFlag = false;
};