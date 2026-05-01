#include "SdStr.hpp"
#include "SdLoader.hpp"
#include "SdLoadTask.hpp"
#include "SdMem.hpp"
#include "SdDrv.hpp"
#include "SdDS.hpp"
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

    int32 AudioDurationMS = SdStr->DataDurationMS;

    uint32 DataSize = (uint32)((((uint64)AudioDurationMS) * SamplesPerSec * BlockAlign) / 1000);
    DataSize = (DataSize / BlockAlign) * BlockAlign;

    return ALIGN_ROUND_UP(DataSize, SdStr->DmaPageBlock);
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


static inline IDirectSoundBuffer* SdStrGetDMA(SdStr_t* SdStr)
{
    return (IDirectSoundBuffer*)(SdStr->DmaHandle);
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
    SdStr->HighVol = SdDSBuffSetPan(SdStrGetDMA(SdStr), SdStr->Vol_L, SdStr->Vol_R);
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
        FadeVol = float(SdStr->FadeData) / float(SD_FADE_MAX);

    SdStr->Vol_L = int32(float(SdStr->Vol_L) * FadeVol);
    SdStr->Vol_R = int32(float(SdStr->Vol_R) * FadeVol);
};


static void SdStrVolSet(SdStr_t* SdStr)
{
    int32 DmaVol = SdDSConvVol(SdStr->HighVol);

    IDirectSoundBuffer8_SetVolume(SdStrGetDMA(SdStr), DmaVol);
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

    WAVEFORMATEX SdWaveFmt;
    std::memset(&SdWaveFmt, 0, sizeof(SdWaveFmt));    
    uint32 SdBuffSize = SdDSSetWaveFormat(&SdWaveFmt, &SdStr->WaveHeader);

    if (SdStr->WaveHeader.Page)
    {
        SdStr->DmaPos =
            SdStr->WaveHeader.BlockAlign * (SdStr->WaveHeader.PlayPoint / SdStr->WaveHeader.BlockAlign);
        
        SdStr->DmaPageSize =
            SdStr->WaveHeader.Page * SdStr->WaveHeader.BlockAlign;
    }
    else
    {
        ;
    };

    SdStr->DataTotalSize = SdStrGetTotalDataSize(SdStr);

    SdStr->WaveHeader.LoopPoint += SD_SCT_SIZE;
    SdStr->WaveHeader.EndPoint += SD_SCT_SIZE;

    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
    if (Dma)
    {
        hrCall(IDirectSoundBuffer8_SetFormat(Dma, &SdWaveFmt));

        SdDSBuffClear(Dma);
    }
    else
    {
        DSBUFFERDESC SdBuffDesc;
        std::memset(&SdBuffDesc, 0, sizeof(SdBuffDesc));
        SdBuffDesc.dwSize       = SdBuffSize;
        SdBuffDesc.dwBufferBytes= SdStr->DmaPageSize;
        SdBuffDesc.dwFlags      = (DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY);
        SdBuffDesc.lpwfxFormat  = &SdWaveFmt;
        
        if (!SdDSBuffCreate(&Dma, &SdBuffDesc))
        {
            SdStr->PlayState = SDSTR_STATE_IDLE;
            return false;
        }
        else
        {
            SdStr->DmaHandle = Dma;
        };
    };

    hrCall(IDirectSoundBuffer_SetVolume(Dma, DSBVOLUME_MIN));

    if (SdStr->DmaPos >= SdStr->DmaPageSize)
    {
        SdStr->DmaPos = 0;
        if (SdStr->Type == SDSTR_TYPE_VOX)
            SdStr->WaveHeader.Page = 0;
    };

    hrCall(IDirectSoundBuffer8_SetCurrentPosition(Dma, SdStr->DmaPos));

    SdStr->DmaPageOffset = 0;
    SdStr->MemOffset = SdStr->DmaPageBlock;
    
    return true;
};


static bool SdStrReadFirst(SdStr_t* SdStr)
{
    ASSERT(SdStr->FileHandle == nullptr);

    uint32 MemOfs = SdStr->DataPosition + SdStr->Offset;
    void* MemAddr = SdStrGetMemAddress(SdStr);
    uint32 MemSize = SdStr->DmaPageSize;

    SdStr->FileHandle = SdLoadTaskReadAsync(SdStr->Filename, MemAddr, MemSize, MemOfs);
    if (!SdStr->FileHandle)
        return false;

    SdStr->Offset += MemSize;

    return true;
};


static bool SdStrReadFirstWait(SdStr_t* SdStr)
{
    if (!SdStrCheckLoadEnd(SdStr))
        return false;
    
    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
    uint32 MemSize = SdStr->DmaPageSize;
    void* MemAddr = SdStrGetMemAddress(SdStr);

    if (!SdStr->WaveHeader.Page)
    {
        MemSize = SdStrGetPutDataSize(SdStr, MemSize);
        SdDSBuffLoadData(Dma, MemAddr, MemSize);

        SdStr->DataTotalPlayed += MemSize;
    }
    else
    {
        SdDSBuffLoadData(Dma, MemAddr, MemSize);
    };

    SdMemFree(SdStr->Head);
    SdStr->Head = nullptr;

    return true;
};


static bool SdStrPlayStart(SdStr_t* SdStr)
{
    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);

    if (FAILED(IDirectSoundBuffer8_SetCurrentPosition(Dma, SdStr->DmaPos)))
        return false;

    if (FAILED(IDirectSoundBuffer8_Play(Dma, 0, 0, DSBPLAY_LOOPING)))
        return false;

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
    if (!SdStr->WaveTransferRq)
        return false;

    if (SdStr->FileHandle && (SdStr->DmaPageRead == SdStr->DmaPagePrev))
    {
        SdStr->Timeout = 4000;
        SdStr->PlayState = SDSTR_STATE_DOWNLOAD_WAIT;
        return false;
    };

    if (SdStr->MemAddress)
    {
        IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
        void* MemAddrCur = SdStr->MemAddress;
        void* MemAddrOrg = SdStrGetMemAddress(SdStr);
        uint32 MemOfs = (uintptr_t)MemAddrCur - (uintptr_t)MemAddrOrg;

        if (!SdStr->WaveHeader.Page)
        {
            uint32 Size = SdStr->DmaPageBlock;
            //Size = SdStrGetPutDataSize(SdStr, Size);

            if (Size > 0)
            {
                SdDSBuffLoadDataEx(Dma, MemOfs, MemAddrCur, Size);
                SdStr->DataTotalPlayed += Size;
            };
        }
        else
        {
            SdDSBuffLoadDataEx(Dma, MemOfs, MemAddrCur, SdStr->DmaPageBlock);
        };
    };

    SdStr->MemAddress    = (void*)( (uintptr_t)SdStrGetMemAddress(SdStr) + (uintptr_t)SdStr->DmaPageOffset );
    SdStr->DmaPagePrev   = SdStr->DmaPageOffset;
    SdStr->DmaPageOffset = SdStr->MemOffset;
    SdStr->MemOffset    += SdStr->DmaPageBlock;

    if (SdStr->MemOffset >= SdStr->DmaPageSize)
        SdStr->MemOffset = 0;

    if (SdStr->WaveHeader.Page)
    {
        if ((SdStr->Offset - SD_SCT_SIZE) > (SdStr->WaveHeader.EndPoint - SdStr->DmaPageBlock))
            SdStr->Offset = SdStr->WaveHeader.LoopPoint;

        return true;
    }
    else
    {
//        if (SdStr->DataTotalPlayed >= SdStr->DataTotalSize)
//        {
//            IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
//            void* MemAddrCur = SdStr->MemAddress;
//            void* MemAddrOrg = SdStrGetMemAddress(SdStr);
//            uint32 MemOfs = (uintptr_t)MemAddrCur - (uintptr_t)MemAddrOrg;
//
//            std::memset(SdStr->MemAddress, 0, SdStr->DmaPageBlock);
//            
//            SdDSBuffLoadDataEx(Dma, MemOfs, MemAddrCur, SdStr->DmaPageBlock);
//        };        
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
    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
    if (Dma && !SdStr->VolumeOnPause)
    {
        IDirectSoundBuffer8_GetVolume(Dma, LPLONG(&SdStr->VolumeOnPause));
        IDirectSoundBuffer8_SetVolume(Dma, DSBVOLUME_MIN);
        IDirectSoundBuffer_GetCurrentPosition(Dma, LPDWORD(&SdStr->DmaPos), 0);
        IDirectSoundBuffer_Play(Dma, 0, 0, 0);
        IDirectSoundBuffer_Stop(Dma);
    };

    SdStrVolCtrl(SdStr);
};


static void SdStrResume(SdStr_t* SdStr)
{
    SdStr->PauseRq = false;
    SdStr->PlayState = SdStr->PlayStateResume;

    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
    if (Dma && SdStr->VolumeOnPause)
    {
        SdStr->VolumeOnPause = 0;

        IDirectSoundBuffer_SetCurrentPosition(Dma, SdStr->DmaPos);
        IDirectSoundBuffer_SetVolume(Dma, SdStr->VolumeOnPause);
        IDirectSoundBuffer_Play(Dma, 0, 0, DSBPLAY_LOOPING);
    };

    SdStrVolCtrl(SdStr);
};


static void SdStrDownloadWait(SdStr_t* SdStr)
{
    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
    if (Dma)
    {
        Dma->SetVolume(DSBVOLUME_MIN);
		Dma->Stop();
    };

    if (SdStr->FileHandle)
    {
        SdStr->Timeout -= 1;
        if (SdStr->Timeout <= 0)
            SdStr->PlayState = SDSTR_STATE_PLAYFINISH;
    }
    else
    {
        SdStr->PlayState = SDSTR_STATE_PLAYSTART;
    };
};


static bool SdStrPlayFinish(SdStr_t* SdStr)
{
    SdStrVolCtrl(SdStr);

    IDirectSoundBuffer_SetVolume(SdStrGetDMA(SdStr), DSBVOLUME_MIN);

    if (!SdStrCheckLoadEnd(SdStr))
        return false;

    if ((SdStr->DmaStatus & DSBSTATUS_PLAYING) != 0)
    {
        IDirectSoundBuffer_Stop(SdStrGetDMA(SdStr));
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

    uint32 DmaStatus = 0;
    uint32 DmaPos = 0;

    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
    if (Dma)
    {
        Dma->GetStatus(LPDWORD(&DmaStatus));
        if (SUCCEEDED(Dma->GetCurrentPosition(LPDWORD(&DmaPos), NULL)))
        {
            SdStr->DmaPos = DmaPos;
            SdStr->DmaPageRead = ALIGN_ROUND_DOWN(SdStr->DmaPos, SdStr->DmaPageBlock);
            SdStr->WaveTransferRq = false;

            if (SdStr->DmaPageRead != SdStr->DmaPageOffset)
                SdStr->WaveTransferRq = true;
        };
    };

    switch (SdStr->PlayState)
    {
    case SDSTR_STATE_PLAYING:
    case SDSTR_STATE_READ_NEXT:
    case SDSTR_STATE_DOWNLOAD_WAIT:
        {            
            if (DmaStatus & DSBSTATUS_PLAYING)
            {
                ++SdStr->PlayTime;
                SdStr->DataSize -= (SdStr->DataSize <= 0 ? 0 : SD_PERIOD);
            }
            else
            {
                //if (SdStr->DataTotalPlayed >= SdStr->DataTotalSize)
                //    SdStr->DataSize = 0;
            };

            if ((!SdStr->WaveHeader.Page) && (SdStr->DataSize <= 0))
            {
                IDirectSoundBuffer_Stop(Dma);
                IDirectSoundBuffer_SetVolume(Dma, DSBVOLUME_MIN);
                IDirectSoundBuffer_SetCurrentPosition(Dma, 0);
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
            
            bool IsTransfered = SdStrWaveTransfer(SdStr);

            /* make sure that state was not changed by fade ctrl in playroot */
            bool IsStateNotChanged = (SdStr->PlayState == SDSTR_STATE_PLAYING);

            if (IsTransfered && IsStateNotChanged)
                SdStr->PlayState = SDSTR_STATE_READ_NEXT;
        }
        break;
        
    case SDSTR_STATE_READ_NEXT:
        {
            SdStrPlayRoot(SdStr);

            bool IsDownloadRqOK = SdStrOffsetDownload(SdStr);

            /* make sure that state was not changed by fade ctrl in playroot */
            bool IsStateNotChanged = (SdStr->PlayState == SDSTR_STATE_READ_NEXT);

            if (IsDownloadRqOK && IsStateNotChanged)
                SdStr->PlayState = SDSTR_STATE_PLAYING;
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
            SDLOG("DOWNLOAD_WAIT\n");
            SdStrDownloadWait(SdStr);
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
    ;
};


void OnSdStrDestroy(SdStr_t* SdStr) 
{
    if (SdStr->DmaHandle)
    {
        IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
        SdDSBuffDestroy(Dma);
        SdStr->DmaHandle = nullptr;
    };
};


void OnSdStrTask(SdStr_t* SdStr) 
{
    SdStrPlay(SdStr);
};


void SdStrSetMonaural(void)
{
    
};


void SdStrSetStereo(void)
{
    
};


void SdStrFocusLost(void)
{

};


void SdStrFocusRestore(void)
{
   
};