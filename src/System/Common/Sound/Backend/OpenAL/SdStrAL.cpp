#include "SdStr.hpp"
#include "SdLoader.hpp"
#include "SdLoadTask.hpp"
#include "SdMem.hpp"
#include "SdDrv.hpp"
#include "SdAL.hpp"
#include "SdTable.hpp"
#include "SdCallback.hpp"
#include "SdLog.hpp"
#include "SdQueue.hpp"


static inline uint32
SdStrGetTotalDataSize(SdStr_t* SdStr)
{
    int32 BlockAlign = SdStr->WaveHeader.BlockAlign;
    int32 Channels = SdStr->WaveHeader.Channels;

    int32 BitsPerSample = (BlockAlign / Channels) * 8;
    int32 SamplesPerSec = SdStr->WaveHeader.SamplesPerSec;

    int32 AudioDurationMS = SdStr->DataDurationMS + 1;

    uint32 DataSize = (uint32)((((uint64)AudioDurationMS) * SamplesPerSec * BlockAlign) / 1000);
    DataSize = (DataSize / BlockAlign) * BlockAlign;

    return DataSize;
};


static inline uint32
SdStrGetPutDataSize(SdStr_t* SdStr, uint32 Size)
{
    uint32 PutSize = Size;
    uint32 DataSize = SdStrGetTotalDataSize(SdStr);

    if ((SdStr->DataTotalPlayed + PutSize) >= DataSize)
    {
        ASSERT(DataSize >= SdStr->DataTotalPlayed, "%d -- %d", DataSize, SdStr->DataTotalPlayed);
        PutSize = (uint32)(DataSize - SdStr->DataTotalPlayed);
    };

    return PutSize;
};


static inline bool SdStrIsPlaying(SdStr_t* SdStr)
{
    return ((SdStr->PlayState >= SDSTR_STATE_PLAYING) &&
        (SdStr->PlayState <= SDSTR_STATE_READ_NEXT));
};


static inline bool SdStrCheckLoadEnd(SdStr_t* SdStr)
{
    if (!SdStr->FileHandle)
        return true;

    if (SdLoadTaskIsReadEnd(SdStr->FileHandle))
    {
        SdStr->FileHandle = nullptr;
        return true;
    };

    return false;
};


static void SdStrQueueAllClear(SdStr_t* SdStr)
{
    if (!SdStr->FadeFlag)
        return;

    if (SdStr->FadeFlag & SDSTR_FADEOUT_FLAG_RST_QUE)
        SdQueueClear(SdStr->Queue);

    SdStr->FadeFlag = 0;
};


static void SdStrQueueClear(SdStr_t* SdStr)
{
    ;
};


static bool SdStrQueueCtrl(SdStr_t* SdStr)
{
    if (SdStr->Type != SDSTR_TYPE_VOX)
        return false;

    if (SdQueueIsEmpty(SdStr->Queue))
        return false;

    SdStrQueueData_t* QueueData = (SdStrQueueData_t*)SdQueueFront(SdStr->Queue);

    //SdStr->DelayTime = int32(QueueData->Param1);
    //SdStr->Reverb = bool(QueueData->Param2 > 0);
    //SdStr->PlayVolume = int32(QueueData->Param3);
    SdStrSet(SdStr, QueueData->Code);

    SdQueuePop(SdStr->Queue);

    return true;
};


static bool SdStrQueueSet2nd(SdStr_t* SdStr)
{
    if (SdStr->Type != SDSTR_TYPE_VOX)
        return false;

    if (!SdStrQueueCtrl(SdStr))
    {


        return false;
    };

    return true;
};


static bool SdStrFadeOut(SdStr_t* SdStr)
{
    if (!SdStr->FadeData)
        return true;

    SdStr->FadeData += SdStr->FadeSpeed;
    if (SdStr->FadeData >= SD_FADE_MAX)
    {
        SdStr->FadeSpeed = 0;
        SdStr->FadeData = 0;

        return true;
    };

    if (SdStr->FadeData > 0)
        return true;

    SdStr->FadeSpeed = 0;
    SdStr->FadeData = 0;

    return false;
};


static void SdStrPanCtrl(SdStr_t* SdStr)
{
    ;
};


static void SdStrPanSet(SdStr_t* SdStr)
{

};


static void SdStrVolCtrl(SdStr_t* SdStr)
{
    int32 Vol = SdStr->WaveHeader.Volume;

    if (SdStr->PlayVolume)
        Vol = SdStr->PlayVolume;

    /* convert vol to framework vol */
    Vol = Vol << 6;

    SdStr->Vol_L = Vol;
    SdStr->Vol_R = Vol;

    if (SdStr->Type == SDSTR_TYPE_VAG)
        SdDrvVagVolumeCB(SdStr->PlayCode, &SdStr->Vol_L, &SdStr->Vol_R);
    else
        SdDrvVoxVolumeCB(SdStr->PlayCode, &SdStr->Vol_L, &SdStr->Vol_R);

    float FadeVol = 0.0f;

    if (SdStrFadeOut(SdStr))
    {
        FadeVol = 1.0f;
    }
    else
    {
        SdStrQueueAllClear(SdStr);
        SdStr->PauseRq = false;
        SdStr->PlayState = SDSTR_STATE_PLAYFINISH;
    };

    if (SdStr->FadeData)
        FadeVol = ((float)SdStr->FadeData) / ((float)SD_FADE_MAX);

    SdStr->Vol_L = (int32)(((float)SdStr->Vol_L) * FadeVol);
    SdStr->Vol_R = (int32)(((float)SdStr->Vol_R) * FadeVol);
};


static void SdStrVolSet(SdStr_t* SdStr)
{
    if (SdStr->FocusLost)
        return;

    int32 vol = Max(SdStr->Vol_L, SdStr->Vol_R);
    int32 volDS = SdALConvVolToDsVol(vol);
    float gain = SdALConvDsVolToGain(volDS);

    alCall(alSourcef(SdStr->ALSource, AL_GAIN, gain));
};


static bool SdStrReadHead(SdStr_t* SdStr)
{
    if (!SdStr->Head)
        SdStr->Head = SdMemAlloc(SD_SCT_SIZE);

    if (!SdStr->Head)
    {
        SdStr->PlayState = SDSTR_STATE_IDLE;
        return false;
    };

    SdStr->Offset = SD_SCT_SIZE;
    SdStr->FileHandle = SdLoadTaskReadAsync(SdStr->Filename, SdStr->Head, SD_SCT_SIZE, SdStr->DataPosition);
    ASSERT(SdStr->FileHandle);

    return (SdStr->FileHandle ? true : false);
};


static bool SdStrReadHeadWait(SdStr_t* SdStr)
{
    if (!SdStrCheckLoadEnd(SdStr))
        return false;

    SdStr->DmaPos = 0;

    std::memcpy(&SdStr->WaveHeader, SdStr->Head, sizeof(SdStr->WaveHeader));

    if (SdStr->WaveHeader.Page)
    {
        SdStr->DmaPos = SdStr->WaveHeader.BlockAlign * (SdStr->WaveHeader.PlayPoint / SdStr->WaveHeader.BlockAlign);
        SdStr->DmaPageSize = SdStr->WaveHeader.Page * SdStr->WaveHeader.BlockAlign;
    }
    else
    {
        ;
    };

    SdStr->DataTotalSize = SdStrGetTotalDataSize(SdStr);

    SdStr->WaveHeader.LoopPoint += SD_SCT_SIZE;
    SdStr->WaveHeader.EndPoint += SD_SCT_SIZE;

    if (!SdStr->ALSource)
    {
        alCall(alGenSources(1, &SdStr->ALSource));
        if (SdStr->ALSource == 0)
        {
            SdStr->PlayState = SDSTR_STATE_IDLE;
            return false;
        };
    }
    else
    {
        alCall(alSourceStop(SdStr->ALSource));
        alCall(alSourcei(SdStr->ALSource, AL_BUFFER, 0));

        ALint NumQueued = 0;
        alCall(alGetSourcei(SdStr->ALSource, AL_BUFFERS_QUEUED, &NumQueued));

        while (NumQueued--)
        {
            ALuint BufferID = 0;
            alCall(alSourceUnqueueBuffers(SdStr->ALSource, 1, &BufferID));
        };

//      if (SdStr->ALBuffer)
//      {
//          if (SdStr->ALBufferCount > 0)
//          {
//              alCall(alDeleteBuffers(SdStr->ALBufferCount, SdStr->ALBuffer));
//              std::memset(SdStr->ALBuffer, 0, sizeof(SdStr->ALBuffer[0]) * SdStr->ALBufferCount);
//          };
//      };
    };
    
    SdStr->ALBufferNoPlaying = 0;
    SdStr->ALBufferNoReading = 0;
    SdStr->ALBufferCount = SdStr->DmaPageSize / SdStr->DmaPageBlock;

    if (SdStr->DataTotalSize < SdStr->DmaPageSize)
        SdStr->ALBufferCount = (ALIGN_ROUND_UP(SdStr->DataTotalSize, SdStr->DmaPageBlock) / SdStr->DmaPageBlock);

    if (SdStr->ALBufferCount != SdStr->ALBufferCountPrev)
    {
        if (SdStr->ALBuffer)
        {
            SdMemFree(SdStr->ALBuffer);
            SdStr->ALBuffer = nullptr;
        };

        SdStr->ALBuffer = (uint32*)SdMemAlloc(sizeof(uint32) * SdStr->ALBufferCount);
        SdStr->ALBufferCountPrev = SdStr->ALBufferCount;
    };

    for (int32 i = 0; i < SdStr->ALBufferCount; ++i)
    {
        alCall(alGenBuffers(1, &SdStr->ALBuffer[i]));
        if (!SdStr->ALBuffer[i])
        {
            SdStr->PlayState = SDSTR_STATE_IDLE;
            return false;
        };

        alCall(alBufferData(SdStr->ALBuffer[i],
                            SdALGetWaveFormat(&SdStr->WaveHeader),
                            nullptr,
                            SdStr->DmaPageBlock,
                            SdStr->WaveHeader.SamplesPerSec));
    };

    alCall(alSourcef(SdStr->ALSource, AL_GAIN, 0.0f));
    alCall(alSource3f(SdStr->ALSource, AL_POSITION, 0.0f, 0.0f, 0.0f));
    alCall(alSourcei(SdStr->ALSource, AL_SOURCE_RELATIVE, AL_TRUE));

    if (SdStr->DmaPos >= SdStr->DmaPageSize)
    {
        SdStr->DmaPos = 0;
        if (SdStr->Type == SDSTR_TYPE_VOX)
            SdStr->WaveHeader.Page = 0;
    };

    SdStr->DmaPageOffset = 0;
    SdStr->MemOffset = SdStr->DmaPageBlock;

    return true;
};


static bool SdStrReadFirst(SdStr_t* SdStr)
{
    ASSERT(SdStr->FileHandle == nullptr);

    uint32 MemOfs = SdStr->DataPosition + SdStr->Offset;
    void* MemAddr = SdStrGetMemAddress(SdStr);

    SdStr->FileHandle = SdLoadTaskReadAsync(SdStr->Filename, MemAddr, SdStr->DmaPageSize, MemOfs);
    if (!SdStr->FileHandle)
        return false;

    SdStr->Offset += SdStr->DmaPageSize;

    return true;
};


static bool SdStrReadFirstWait(SdStr_t* SdStr)
{
    if (!SdStrCheckLoadEnd(SdStr))
        return false;

    uint32 MemSize = SdStr->DmaPageSize;
    uint8* MemAddr = (uint8*)SdStrGetMemAddress(SdStr);

    if (!SdStr->WaveHeader.Page)
    {
        uint32 Size = MemSize;
        Size = SdStrGetPutDataSize(SdStr, Size);

        for (int32 i = 0; i < SdStr->ALBufferCount; ++i)
        {
            uint8* DataAddr = (MemAddr + (i * SdStr->DmaPageBlock));
            uint32 DataSize = (Min(SdStr->DmaPageBlock, Size));

            SdStr->DataTotalPlayed += DataSize;

            alCall(alBufferData(SdStr->ALBuffer[i], SdALGetWaveFormat(&SdStr->WaveHeader),
                                DataAddr, DataSize, SdStr->WaveHeader.SamplesPerSec));

            if (Size >= SdStr->DmaPageBlock)
                Size -= SdStr->DmaPageBlock;
        };

        alCall(alSourceQueueBuffers(SdStr->ALSource, SdStr->ALBufferCount, SdStr->ALBuffer));
    }
    else
    {
        for (int32 i = 0; i < SdStr->ALBufferCount; ++i)
        {
            uint8* DataAddr = (i == 0) ? (MemAddr + SdStr->DmaPos) :
                                         (MemAddr + (i * SdStr->DmaPageBlock));


            uint32 DataSize = (i == 0) ? (SdStr->DmaPageBlock - SdStr->DmaPos) :
                                         (SdStr->DmaPageBlock);

            alCall(alBufferData(SdStr->ALBuffer[i], SdALGetWaveFormat(&SdStr->WaveHeader),
                                DataAddr, DataSize, SdStr->WaveHeader.SamplesPerSec));            
        };

        alCall(alSourceQueueBuffers(SdStr->ALSource, SdStr->ALBufferCount, SdStr->ALBuffer));
    };

    SdMemFree(SdStr->Head);
    SdStr->Head = nullptr;

    return true;
};


static bool SdStrPlayStart(SdStr_t* SdStr)
{
    alCall(alSourcePlay(SdStr->ALSource));
    SdStrVolSet(SdStr);

    return true;
};


static void SdStrPlayRoot(SdStr_t* SdStr)
{
    SdStrVolCtrl(SdStr);
    SdStrPanCtrl(SdStr);
    SdStrPanSet(SdStr);
    SdStrVolSet(SdStr);
};


static bool SdStrWaveTransfer(SdStr_t* SdStr)
{
    if (SdStr->MemAddress)
    {
        if (!SdStr->WaveHeader.Page)
        {
            uint32 Size = SdStr->DmaPageBlock;
            Size = SdStrGetPutDataSize(SdStr, Size);

            if (Size > 0)
            {
                alCall(alBufferData(SdStr->ALBuffer[SdStr->ALBufferNoReading], SdALGetWaveFormat(&SdStr->WaveHeader),
                                    SdStr->MemAddress, Size, SdStr->WaveHeader.SamplesPerSec));

                alCall(alSourceQueueBuffers(SdStr->ALSource, 1, &SdStr->ALBuffer[SdStr->ALBufferNoReading]));

                SdStr->DataTotalPlayed += Size;
            };

            if (SdStr->DataTotalPlayed >= SdStr->DataTotalSize)
                return false;
        }
        else
        {
            alCall(alBufferData(SdStr->ALBuffer[SdStr->ALBufferNoReading], SdALGetWaveFormat(&SdStr->WaveHeader),
                                SdStr->MemAddress, SdStr->DmaPageBlock, SdStr->WaveHeader.SamplesPerSec));

            alCall(alSourceQueueBuffers(SdStr->ALSource, 1, &SdStr->ALBuffer[SdStr->ALBufferNoReading]));
        };

        SdStr->ALBufferNoReading = (SdStr->ALBufferNoReading + 1);
        SdStr->ALBufferNoReading %= SdStr->ALBufferCount;
    };

    SdStr->MemAddress = (void*)(uint32(SdStrGetMemAddress(SdStr)) + SdStr->DmaPageOffset);
    SdStr->DmaPagePrev = SdStr->DmaPageOffset;
    SdStr->DmaPageOffset = SdStr->MemOffset;
    SdStr->MemOffset += SdStr->DmaPageBlock;

    if (SdStr->MemOffset >= SdStr->DmaPageSize)
        SdStr->MemOffset = 0;

    if (SdStr->WaveHeader.Page)
    {
        if ((SdStr->Offset - SD_SCT_SIZE) > (SdStr->WaveHeader.EndPoint - SdStr->DmaPageBlock))
            SdStr->Offset = SdStr->WaveHeader.LoopPoint;

        return true;
    };

    if ((SdStr->Offset - SD_SCT_SIZE) < uint32(SdStr->WaveHeader.Size))
        return true;

    std::memset(SdStr->MemAddress, 0, SdStr->DmaPageBlock);

    return false;
};


static bool SdStrOffsetDownload(SdStr_t* SdStr)
{
    if (SdStr->FileHandle)
        return false;

    if (!SdStr->MemAddress)
        return false;

    SdStr->FileHandle = SdLoadTaskReadAsync(SdStr->Filename,
                                            SdStr->MemAddress,
                                            SdStr->DmaPageBlock,
                                            SdStr->DataPosition + SdStr->Offset);
    if (!SdStr->FileHandle)
        return false;

    SdStr->Offset += SdStr->DmaPageBlock;

    return true;
};


static void SdStrPause(SdStr_t* SdStr)
{
    alCall(alGetSourcef(SdStr->ALSource, AL_GAIN, &SdStr->GainOnPause));
    alCall(alSourcef(SdStr->ALSource, AL_GAIN, 0.0f));
    alCall(alSourcePause(SdStr->ALSource));
};


static void SdStrResume(SdStr_t* SdStr)
{
    SdStr->PauseRq = false;
    SdStr->PlayState = SdStr->PlayStateResume;

    alCall(alSourcef(SdStr->ALSource, AL_GAIN, SdStr->GainOnPause));
    alCall(alSourcePlay(SdStr->ALSource));
};


static bool SdStrPlayFinish(SdStr_t* SdStr)
{
    SdStrVolCtrl(SdStr);
    alCall(alSourcef(SdStr->ALSource, AL_GAIN, 0.0f));

    if (!SdStrCheckLoadEnd(SdStr))
        return false;

    ALint State = 0;
    alCall(alGetSourcei(SdStr->ALSource, AL_SOURCE_STATE, &State));

    if (State == AL_PLAYING)
    {
        alCall(alSourceStop(SdStr->ALSource));
        return false;
    };

    SdStr->DmaPos = 0;

    if (SdStr->Filename)
    {
        SdMemFree(SdStr->Filename);
        SdStr->Filename = nullptr;
    };

    if (SdStr->Type == SDSTR_TYPE_VAG)
    {
        if (SdStr->SetCode.Code)
        {
            SdStrSet(SdStr, SdStr->SetCode.Code);
            return false;
        };
    }
    else
    {
        SdVoxQueueClear(SdStr->No);
    };

    return true;
};


static void SdStrPlay(SdStr_t* SdStr)
{
    SdStrCheckLoadEnd(SdStr);

    bool IsPlaying = false;

    if (SdStr->ALSource)
    {
        ALint ProcessedCount = 0;
        alCall(alGetSourcei(SdStr->ALSource, AL_BUFFERS_PROCESSED, &ProcessedCount));

        if (ProcessedCount > 0)
        {
            SdStr->ALBufferNoPlaying += ProcessedCount;
            SdStr->ALBufferNoPlaying %= SdStr->ALBufferCount;
        };

        while (ProcessedCount--)
        {
            ALuint BufferID = 0;
            alCall(alSourceUnqueueBuffers(SdStr->ALSource, 1, &BufferID));

            ++SdStr->WaveTransferRqCount;
        };

        ALint State = 0;
        alCall(alGetSourcei(SdStr->ALSource, AL_SOURCE_STATE, &State));

        if (State == AL_PLAYING)
        {
            IsPlaying = true;
        }
        else if (State == AL_STOPPED)
        {
            if ((SdStr->PlayState == SDSTR_STATE_PLAYING) ||
                (SdStr->PlayState == SDSTR_STATE_READ_NEXT) ||
                (SdStr->PlayState == SDSTR_STATE_DOWNLOAD_WAIT))
            {
                ALint NumQueuedPage = 0;
                alCall(alGetSourcei(SdStr->ALSource, AL_BUFFERS_QUEUED, &NumQueuedPage));

                if (NumQueuedPage > 0)
                    alCall(alSourcePlay(SdStr->ALSource));
            };
        };
    };

    switch (SdStr->PlayState)
    {
    case SDSTR_STATE_PLAYING:
    case SDSTR_STATE_READ_NEXT:
    case SDSTR_STATE_DOWNLOAD_WAIT:
        {
            if (IsPlaying)
            {
                ++SdStr->PlayTime;
                SdStr->DataSize -= (SdStr->DataSize <= 0 ? 0 : SD_PERIOD);
            }
            else
            {
                if (SdStr->DataTotalPlayed >= SdStr->DataTotalSize)
                    SdStr->DataSize = 0;
            };

            if ((!SdStr->WaveHeader.Page) && (SdStr->DataSize <= 0))
            {
                std::memset(SdStrGetMemAddress(SdStr), 0, SdStr->DmaPageSize);
                SdStr->PlayState = SDSTR_STATE_PLAYFINISH;
            };
        }
        break;

    default:
        break;
    };

    switch (SdStr->PlayState)
    {
    case SDSTR_STATE_IDLE:
        {
            SdStrFadeOut(SdStr);
            SdStrQueueClear(SdStr);
            SdStrQueueSet2nd(SdStr);
        }
        break;

    case SDSTR_STATE_READ_HEAD:
        {
            if (SdStrReadHead(SdStr))
                SdStr->PlayState = SDSTR_STATE_READ_HEAD_WAIT;
            else
                ASSERT(false);
        }
        break;

    case SDSTR_STATE_READ_HEAD_WAIT:
        {
            if (SdStrReadHeadWait(SdStr))
                SdStr->PlayState = SDSTR_STATE_READ_FIRST;
        }
        break;

    case SDSTR_STATE_READ_FIRST:
        {
            if (SdStrReadFirst(SdStr))
                SdStr->PlayState = SDSTR_STATE_READ_FIRST_WAIT;
            else
                ASSERT(false);
        }
        break;

    case SDSTR_STATE_READ_FIRST_WAIT:
        {
            if (SdStrReadFirstWait(SdStr))
                SdStr->PlayState = SDSTR_STATE_PLAYSTART;
        }
        break;

    case SDSTR_STATE_PLAYSTART:
        {
            if (SdStrPlayStart(SdStr))
                SdStr->PlayState = SDSTR_STATE_PLAYING;
            else
                ASSERT(false);
        }
        break;

    case SDSTR_STATE_PLAYING:
        {
            SdStrPlayRoot(SdStr);

            if ((SdStr->WaveTransferRqCount > 0) && SdStrCheckLoadEnd(SdStr))
            {
                if (SdStrWaveTransfer(SdStr))
                    SdStrOffsetDownload(SdStr);

                --SdStr->WaveTransferRqCount;
            };
        }
        break;

    case SDSTR_STATE_READ_NEXT:
        {
            ASSERT(false, "not expected to be here while in OpenAL");
            SdStr->PlayState = SDSTR_STATE_IDLE;
        }
        break;

    case SDSTR_STATE_PAUSE:
        {
            SdStrPause(SdStr);
        }
        break;

    case SDSTR_STATE_RESUME:
        {
            SdStrResume(SdStr);
        }
        break;

    case SDSTR_STATE_DOWNLOAD_WAIT:
        {
            ASSERT(false, "not expected to be here while in OpenAL");
            SdStr->PlayState = SDSTR_STATE_IDLE;
        }
        break;

    case SDSTR_STATE_PLAYFINISH:
        {
            if (SdStrPlayFinish(SdStr) && !SdStrQueueSet2nd(SdStr))
                SdStr->PlayState = SDSTR_STATE_IDLE;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (SdStrIsPlaying(SdStr) && SdStr->PauseRq)
    {
        SdStr->PauseRq = 0;
        SdStr->PlayStateResume = SdStr->PlayState;
        SdStr->PlayState = SDSTR_STATE_PAUSE;
    };
};


void OnSdStrNew(SdStr_t* SdStr)
{
    SdStr->WaveTransferRqCount = 0;
    SdStr->ALBufferCountPrev = -1;
};


void OnSdStrDestroy(SdStr_t* SdStr)
{
    if (SdStr->ALSource)
    {
        alCall(alSourceStop(SdStr->ALSource));

        ALint NumQueued = 0;
        alCall(alGetSourcei(SdStr->ALSource, AL_BUFFERS_QUEUED, &NumQueued));

        while (NumQueued--)
        {
            ALuint BufferID = 0;
            alCall(alSourceUnqueueBuffers(SdStr->ALSource, 1, &BufferID));
        };

        alCall(alDeleteSources(1, &SdStr->ALSource));
        SdStr->ALSource = 0;
    };

    if (SdStr->ALBuffer)
    {
        if (SdStr->ALBufferCount > 0)
        {
            alCall(alDeleteBuffers(SdStr->ALBufferCount, SdStr->ALBuffer));
            std::memset(SdStr->ALBuffer, 0, sizeof(SdStr->ALBuffer[0]) * SdStr->ALBufferCount);
        };

        SdMemFree(SdStr->ALBuffer);
        SdStr->ALBuffer = nullptr;
    };
};


void OnSdStrTask(SdStr_t* SdStr)
{
    SdStrPlay(SdStr);
};


void SdStrSetMonaural(void)
{
    // currently pan is ignored for stream
};


void SdStrSetStereo(void)
{
    // currently pan is ignored for stream
};


void SdStrFocusLost(void)
{
    int32 VoxSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VOX);
    for (int32 i = 0; i < VoxSrvCnt; ++i)
    {
        SdStr_t* Str = SdStrGet(SDSTR_TYPE_VOX, i);

        if (Str->ALSource)
        {
            alCall(alGetSourcef(Str->ALSource, AL_GAIN, &Str->GainOnFocusLost));
            alCall(alSourcef(Str->ALSource, AL_GAIN, 0.0f));
            alCall(alSourcePause(Str->ALSource));
        };

        Str->FocusLost = true;
    };

    int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);
    for (int32 i = 0; i < VagSrvCnt; ++i)
    {
        SdStr_t* Str = SdStrGet(SDSTR_TYPE_VAG, i);

        if (Str->ALSource)
        {
            alCall(alGetSourcef(Str->ALSource, AL_GAIN, &Str->GainOnFocusLost));
            alCall(alSourcef(Str->ALSource, AL_GAIN, 0.0f));
            alCall(alSourcePause(Str->ALSource));
        };

        Str->FocusLost = true;
    };
};


void SdStrFocusRestore(void)
{
    int32 VoxSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VOX);
    for (int32 i = 0; i < VoxSrvCnt; ++i)
    {
        SdStr_t* Str = SdStrGet(SDSTR_TYPE_VOX, i);

        if (Str->ALSource)
        {
            alCall(alSourcef(Str->ALSource, AL_GAIN, Str->GainOnFocusLost));

            ALint State = 0;
            alCall(alGetSourcei(Str->ALSource, AL_SOURCE_STATE, &State));

            if (State == AL_PAUSED)
                alCall(alSourcePlay(Str->ALSource));
        };

        Str->FocusLost = false;
    };

    int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);
    for (int32 i = 0; i < VagSrvCnt; ++i)
    {
        SdStr_t* Str = SdStrGet(SDSTR_TYPE_VAG, i);

        if (Str->ALSource)
        {
            alCall(alSourcef(Str->ALSource, AL_GAIN, Str->GainOnFocusLost));

            ALint State = 0;
            alCall(alGetSourcei(Str->ALSource, AL_SOURCE_STATE, &State));

            if (State == AL_PAUSED)
                alCall(alSourcePlay(Str->ALSource));
        };

        Str->FocusLost = false;
    };
};