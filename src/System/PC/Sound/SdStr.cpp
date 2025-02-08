#include "SdStr.hpp"
#include "SdLoader.hpp"
#include "SdLoadTask.hpp"
#include "SdMem.hpp"
#include "SdDrv.hpp"
#include "SdDs.hpp"
#include "SdTable.hpp"
#include "SdCallback.hpp"
#include "SdLog.hpp"


/**
 *  TODO: there is some really rare trouble when voice not stop playing and playing above loaded data (got it twice for long time)
 */

static SdStr_t* SdStrSrv[SDSTR_SRV_MAX];


static inline IDirectSoundBuffer* SdStrGetDMA(SdStr_t* SdStr)
{
    return (IDirectSoundBuffer*)(SdStr->DmaHandle);
};


static inline bool SdStrIsPlaying(SdStr_t* SdStr)
{
    return ((SdStr->PlayState >= SDSTR_STATE_PLAYING) &&
            (SdStr->PlayState <= SDSTR_STATE_READ_NEXT));
};


static inline bool SdStrCheckCondition(SdStr_t* SdStr)
{
    return ((SdStr->CheckKeyTop == SD_CANARY_TOP) &&
            (SdStr->CheckKeyEnd == SD_CANARY_END));
};


static inline bool SdStrCheckLoadEnd(SdStr_t* SdStr)
{
    if (!SdStr->FileHandle)
        return true;
    
    if (SdLoadTaskIsEnd(SdStr->FileHandle))
    {
        SdStr->FileHandle = nullptr;
        return true;
    };

    return false;
};


static inline void* SdStrGetMemAddress(SdStr_t* SdStr)
{
    ASSERT( (SdStr->Index >= 0) && (SdStr->Index < COUNT_OF(SdStrSrv)) );

    void* Result = SdGetStrMemAddr();

    for (int32 i = 0; i < SdStr->Index; ++i)
    {
        if (SdStrSrv[i]->Type == SDSTR_TYPE_VAG)
            Result = ((uint8*)Result + SdGetStrVagMemSize());

        if (SdStrSrv[i]->Type == SDSTR_TYPE_VOX)
            Result = ((uint8*)Result + SdGetStrVoxMemSize());
    };

    return Result;
};


static bool SdStrSet(SdStr_t* SdStr, int32 _code)
{
    int32 CodeKind = SdDrvGetCodeKind(_code);

    if (!(CodeKind == SD_CODEKIND_VAG || CodeKind == SD_CODEKIND_VOX))
        return false;

    char Filename[SD_FILENAME_LEN];
    Filename[0] = '\0';

    switch (SdStr->Type)
    {
    case SDSTR_TYPE_VAG:
        {
            SdGetBgmFilename(Filename, SD_VAG_BANK_NO(_code));
            
            SdStr->DataPosition = SdGetBgmPosition(_code);
            SdStr->DataSize     = SdGetBgmSize(_code);
            SdStr->DmaPageSize	= SdGetBgmDmaPage(_code);
            SdStr->DmaPageBlock	= SdGetBgmDmaBlock(_code);

            //SdStr->PanData      = SdStr->SetCode.Param1;
            SdStr->PlayVolume   = SdStr->SetCode.Param2;
            SdStr->FadeSpeed    = SdStr->SetCode.FadeSpeed;
            SdStr->FadeData     = SdStr->SetCode.FadeData;

            std::memset(&SdStr->SetCode, 0x00, sizeof(SdStr->SetCode));
        }
        break;

    case SDSTR_TYPE_VOX:
        {
            SdGetVoiceFilename(Filename, SD_VOX_BANK_NO(_code));

            SdStr->DataPosition = SdGetVoicePosition(_code);
            SdStr->DataSize     = SdGetVoiceSize(_code);
            SdStr->DmaPageSize	= SdGetVoiceDmaPage(_code);
            SdStr->DmaPageBlock	= SdGetVoiceDmaBlock(_code);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    SdStr->Filename = (char*)SdMemAlloc(std::strlen(Filename));
    if (!SdStr->Filename)
        return false;

    std::strcpy(SdStr->Filename, Filename);

    SdStr->PlayCode     = _code;
    SdStr->PlayState    = SDSTR_STATE_READ_HEAD;
    SdStr->PlayTime     = 0;
    SdStr->Offset       = 0;
    SdStr->MemAddress   = nullptr;
    SdStr->MemOffset    = SdStr->DmaPageBlock;

    std::memset(SdStrGetMemAddress(SdStr), 0, SdStr->DmaPageSize);

    if (!SdStr->SetVolume)
        SdStr->SetVolume = SdStr->WaveHeader.Volume;

    return true;
};


static SdStr_t* SdStrSrvNew(int32 _no, int32 _type)
{
    SdStr_t* Srv = nullptr;
    int32 SrvIndex = 0;

    while (SdStrSrv[SrvIndex])
    {
        ++SrvIndex;
        
        if (SrvIndex >= COUNT_OF(SdStrSrv))
            return Srv;
    };

    SdStrWorkMemReserve(sizeof(SdStr_t));

    Srv = &((SdStr_t*)SdGetStrWorkAddr())[SrvIndex];
    SdStrSrv[SrvIndex] = Srv;

    std::memset(Srv, 0, sizeof(*Srv));

    Srv->CheckKeyTop    = SD_CANARY_TOP;
    Srv->Index          = SrvIndex;
    Srv->Type           = _type;
    Srv->No             = _no;
    Srv->DmaPageSize    = 0;
    Srv->DmaPageBlock   = 0;
    Srv->CheckKeyEnd    = SD_CANARY_END;

    return Srv;
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


static bool SdStrQueuePush(SdStr_t* SdStr, SdSetDriverCode_t* CodeBuff)
{
    if (SdStr->Type != SDSTR_TYPE_VOX)
        return false;

    if (SdStr->PauseRq)
        return false;

    if (SdQueueIsFull(SdStr->Queue))
        return false;

    SdStrQueueData_t QueueData;
    std::memset(&QueueData, 0, sizeof(QueueData));
    QueueData.Code = CodeBuff->Code;
    QueueData.Param1 = CodeBuff->Param1;
    QueueData.Param2 = CodeBuff->Param2;
    QueueData.Param3 = CodeBuff->Param3;

    SdQueuePush(SdStr->Queue, &QueueData);

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


static void SdStrSetFadeOut(SdStr_t* SdStr, int32 _time, uint32 _flag, int32 _waittime)
{
    (void)_waittime;

    if (!SdStr->PlayCode)
        return;
    
    if (SdStr->PlayState == SDSTR_STATE_IDLE)
        return;

    if (SdStr->FadeData)
        return;

    SdStr->FadeSpeed = SD_FADEOUT_SPEED(_time);
    SdStr->FadeData = SD_FADE_MAX;
    
    if (SdStr->Type == SDSTR_TYPE_VOX)
        SdStr->FadeFlag = _flag;
};


static void SdStrPanCtrl(SdStr_t* SdStr)
{
    ;
};


static void SdStrPanSet(SdStr_t* SdStr)
{
    SdStr->HighVol = SdDsBuffSetPan(SdStrGetDMA(SdStr), SdStr->Vol_L, SdStr->Vol_R);
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
    int32 DmaVol = SdDsConvVol(SdStr->HighVol);

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
    SdStr->FileHandle = SdLoadTaskAsync(SdStr->Filename, SdStr->Head, SD_SCT_SIZE, SdStr->DataPosition);
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
    uint32 SdBuffSize = SdDsSetWaveFormat(&SdWaveFmt, &SdStr->WaveHeader);

    if (SdStr->WaveHeader.Page)
    {
        SdStr->DmaPos = SdStr->WaveHeader.BlockAlign * (SdStr->WaveHeader.PlayPoint / SdStr->WaveHeader.BlockAlign);
        SdStr->DmaPageSize = SdStr->WaveHeader.Page * SdStr->WaveHeader.BlockAlign;
    }
    else
    {
        ;
    };

    SdStr->WaveHeader.LoopPoint += SD_SCT_SIZE;
    SdStr->WaveHeader.EndPoint += SD_SCT_SIZE;

    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
    if (Dma)
    {
        IDirectSoundBuffer8_SetFormat(Dma, &SdWaveFmt);
    }
    else
    {
        DSBUFFERDESC SdBuffDesc;
        std::memset(&SdBuffDesc, 0, sizeof(SdBuffDesc));
        SdBuffDesc.dwSize       = SdBuffSize;
        SdBuffDesc.dwBufferBytes= SdStr->DmaPageSize;
        SdBuffDesc.dwFlags      = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
        SdBuffDesc.lpwfxFormat  = &SdWaveFmt;
        
        if (!SdDsBuffCreate(&Dma, &SdBuffDesc))
        {
            SdStr->PlayState = SDSTR_STATE_IDLE;
            return false;
        }
        else
        {
            SdStr->DmaHandle = Dma;
        };
    };
    
    IDirectSoundBuffer_SetVolume(Dma, DSBVOLUME_MIN);

    if (SdStr->DmaPos >= SdStr->DmaPageSize)
    {
        SdStr->DmaPos = 0;
        if (SdStr->Type == SDSTR_TYPE_VOX)
            SdStr->WaveHeader.Page = 0;
    };

    IDirectSoundBuffer8_SetCurrentPosition(Dma, SdStr->DmaPos);

    SdStr->DmaPageOffset = 0;
    SdStr->MemOffset = SdStr->DmaPageBlock;
    
    return true;
};


static bool SdStrReadFirst(SdStr_t* SdStr)
{
    ASSERT(SdStr->FileHandle == nullptr);

    uint32 MemOfs = SdStr->DataPosition + SdStr->Offset;
    void* MemAddr = SdStrGetMemAddress(SdStr);

    SdStr->FileHandle = SdLoadTaskAsync(SdStr->Filename, MemAddr, SdStr->DmaPageSize, MemOfs);
    if (!SdStr->FileHandle)
        return false;

    SdStr->Offset += SdStr->DmaPageSize;

    return true;
};


static bool SdStrReadFirstWait(SdStr_t* SdStr)
{
    if (!SdStrCheckLoadEnd(SdStr))
        return false;
    
    IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
    uint32 MemSize = SdStr->DmaPageSize;
    void* MemAddr = SdStrGetMemAddress(SdStr);

    SdDsBuffLoadData(Dma, MemAddr, MemSize);

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
        uint32 MemOfs = uint32(MemAddrCur) - uint32(MemAddrOrg);
        
        SdDsBuffLoadDataEx(Dma, MemOfs, MemAddrCur, SdStr->DmaPageBlock);
    };

    SdStr->MemAddress    = (void*)(uint32(SdStrGetMemAddress(SdStr)) + SdStr->DmaPageOffset);
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

    SdStr->FileHandle = SdLoadTaskAsync(SdStr->Filename, SdStr->MemAddress, SdStr->DmaPageBlock, SdStr->DataPosition + SdStr->Offset);
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
            };

            if ((!SdStr->WaveHeader.Page) && (SdStr->DataSize <= 0))
            {                
                std::memset(SdStrGetMemAddress(SdStr), 0, SdStr->DmaPageSize);
                SdStr->PlayState = SDSTR_STATE_PLAYFINISH;
            };
        }
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
            bool IsStateNotChanged = (SdStr->PlayState == SDSTR_STATE_PLAYING); // make sure that state was not changed by fade ctrl in playroot

            if (IsTransfered && IsStateNotChanged)
                SdStr->PlayState = SDSTR_STATE_READ_NEXT;
        }
        break;
        
    case SDSTR_STATE_READ_NEXT:
        {
            SdStrPlayRoot(SdStr);

            bool IsDownloadRqOK = SdStrOffsetDownload(SdStr);
            bool IsStateNotChanged = (SdStr->PlayState == SDSTR_STATE_READ_NEXT); // make sure that state was not changed by fade ctrl in playroot

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


void SdStrInitialize(void)
{
    for (int32 i = 0; i < COUNT_OF(SdStrSrv); ++i)
        SdStrSrv[i] = nullptr;
};


void SdStrTerminate(void)
{
    for (int32 i = 0; i < COUNT_OF(SdStrSrv); ++i)
    {
        SdStr_t* SdStr = SdStrSrv[i];
        
        if (!SdStr)
            continue;

        if (SdStr->Queue)
        {
            SdQueueDestroy(SdStr->Queue);
            SdStr->Queue = nullptr;
        };

        if (SdStr->Filename)
        {
            SdMemFree(SdStr->Filename);
            SdStr->Filename = nullptr;
        };

        if (SdStr->Head)
        {
            SdMemFree(SdStr->Head);
            SdStr->Head = nullptr;
        };

        if (SdStr->DmaHandle)
        {
            IDirectSoundBuffer* Dma = SdStrGetDMA(SdStr);
            SdDsBuffDestroy(Dma);
            SdStr->DmaHandle = nullptr;
        };
    };
};


void SdStrTask(void)
{
    for (int32 i = 0; i < COUNT_OF(SdStrSrv); ++i)
    {
        if (!SdStrSrv[i])
            continue;

        ASSERT(SdStrCheckCondition(SdStrSrv[i]));

        if (SdStrCheckCondition(SdStrSrv[i]))
            SdStrPlay(SdStrSrv[i]);
    };
};


void SdStrPauseClr(void)
{
    for (int32 i = 0; i < COUNT_OF(SdStrSrv); ++i)
    {
        SdStr_t* SdStr = SdStrSrv[i];
        
        if (!SdStr)
            continue;

        if (SdStr->PlayState == SDSTR_STATE_IDLE)
            continue;

        if ((SdStr->PlayState == SDSTR_STATE_PAUSE) && (SdStr->PlayState != SDSTR_STATE_RESUME))
            SdStr->PlayState = SDSTR_STATE_RESUME;    
    };
};


void SdStrPauseSet(void)
{
    for (int32 i = 0; i < COUNT_OF(SdStrSrv); ++i)
    {
        SdStr_t* SdStr = SdStrSrv[i];
        
        if (!SdStr)
            continue;

        if (SdStr->PlayState == SDSTR_STATE_IDLE)
            continue;

        if ((SdStr->PlayState != SDSTR_STATE_PAUSE) && (SdStr->PlayState != SDSTR_STATE_RESUME))
            SdStr->PauseRq = true;
    };
};


SdStr_t* SdStrGet(int32 _type, int32 _no)
{
    for (int32 i = 0; i < COUNT_OF(SdStrSrv); ++i)
    {
        if (SdStrSrv[i] == nullptr)
            continue;

        if (SdStrSrv[i]->Type != _type)
            continue;

        if (SdStrSrv[i]->No != _no)
            continue;

        return SdStrSrv[i];
    };

    SDLOG("an attempt to get invalid stream server type=%" PRIi32 " no=%" PRIi32 "\n", _type, _no);

    return nullptr;
};


int32 SdStrGetServerNum(int32 _type)
{
    int32 Result = 0;

    for (int32 i = 0; i < COUNT_OF(SdStrSrv); ++i)
    {
        if (SdStrSrv[i])
            Result += (SdStrSrv[i]->Type == _type ? 1 : 0);
    };

    return Result;
};


void SdStrSetMonaural(void)
{
    /* not used at all */
};


void SdStrSetStereo(void)
{
    /* not used at all */
};


int32 SdVoxInit(int32 _no)
{
    SdStr_t* SdStr = SdStrSrvNew(_no, SDSTR_TYPE_VOX);
    if (SdStr)
        SdStr->Queue = SdQueueCreate(48, sizeof(SdStrQueueData_t));

    return (SdStr ? SdStr->Index : -1);
};


void SdVoxSet(int32 _no, SdSetDriverCode_t* CodeBuff)
{
    SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VOX, _no);
    if (SdStr)
        SdStrQueuePush(SdStr, CodeBuff);
};


void SdVoxSetFadeOut(int32 _no, int32 _time, int32 _flag)
{
    SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VOX, _no);
    if (SdStr)
        SdStrSetFadeOut(SdStr, _time, _flag, 0);
};


void SdVoxQueueClear(int32 _no)
{
    ;
};


void SdVoxQueueAllClear(void)
{
    ;
};


bool SdVoxGetPlayVoice(int32 _no)
{
    SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VOX, _no);
    if(SdStr)
        return (SdStr->PlayState != SDSTR_STATE_IDLE);

    return false;
};


bool SdVoxGetAllPlayVoice(void)
{
    int32 VoxSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VOX);

    for (int32 i = 0; i < VoxSrvCnt; ++i)
    {
        if (SdVoxGetPlayVoice(i))
            return true;
    };

    return false;        
};


int32 SdVoxGetPlayCode(int32 _no)
{
    SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VOX, _no);
    if (SdStr)
        return (SdStr->PlayState != SDSTR_STATE_IDLE ? SdStr->PlayCode : 0);

    return 0;
};


void SdVoxReset(int32 _time, int32 _flag)
{
    int32 VoxSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VOX);

    for (int32 i = 0; i < VoxSrvCnt; ++i)
        SdVoxSetFadeOut(i, _time, _flag);
};


int32 SdVagInit(int32 _no)
{
    SdStr_t* SdStr = SdStrSrvNew(_no, SDSTR_TYPE_VAG);    
    if (SdStr)
        SdStr->DefaultFadeOut = SD_FADEOUT_MODE_NORMAL;

    return (SdStr ? SdStr->Index : -1);
};


void SdVagSet(int32 _no, SdSetDriverCode_t* CodeBuff)
{
    SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VAG, _no);
    
    if (!SdStr)
        return;

    if (SdStr->PlayState != SDSTR_STATE_IDLE)
    {
        SdVagSetFadeOut(_no, 100, SDSTR_FADEOUT_FLAG_RST_QUE);
        std::memcpy(&SdStr->SetCode, CodeBuff, sizeof(*CodeBuff));
    }
    else
    {
        SdStr->SetCode.Param1 = CodeBuff->Param1;
        SdStr->SetCode.Param2 = CodeBuff->Param2;
        SdStr->SetCode.Param3 = CodeBuff->Param3;
        SdStr->SetCode.FadeData = CodeBuff->FadeData;
        SdStr->SetCode.FadeSpeed = CodeBuff->FadeSpeed;
        
        SdStrSet(SdStr, CodeBuff->Code);
    };
};


bool SdVagPlaybackCheck(int32 _code)
{
    int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);

    for (int32 i = 0; i < VagSrvCnt; ++i)
    {
        int32 VagSrvPlyCode = SdVagGetPlayCode(i);        
        if (VagSrvPlyCode == _code)
            return true;
    };

    return false;
};


bool SdVagFadeCheck(int32 _code)
{
    int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);

    for (int32 i = 0; i < VagSrvCnt; ++i)
    {
        SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VAG, i);
        ASSERT(SdStr);

        bool IsCodeMatch= (SdStr->PlayCode == _code);
        bool IsFading 	= (SdStr->FadeData > 0) && (SdStr->FadeSpeed < 0);

        if (IsCodeMatch && IsFading)
            return true;
    };

    return false;
};


int32 SdVagFirstPlayCode(void)
{
    int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);

    for (int32 i = 0; i < VagSrvCnt; ++i)
    {
        int32 VagSrvPlyCode = SdVagGetPlayCode(i);
        if (VagSrvPlyCode)
            return VagSrvPlyCode;
    };

    return 0;
};


bool SdVagAutoFadeOutChek(int32 _no)
{
    return true;
};


bool SdVagAllAutoFadeOutChek(void)
{
    return true;
};


int32 SdVagGetDefaultFadeOut(int32 _no)
{
    SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VAG, _no);
    if (SdStr)
        return (SdStr->DefaultFadeOut);
    
    return SD_FADEOUT_MODE_0x3;
};


int32 SdVagGetPlayCode(int32 _no)
{
    SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VAG, _no);
    if (SdStr)
        return (SdStr->PlayState != SDSTR_STATE_IDLE ? SdStr->PlayCode : 0);
    
    return 0;
};


void SdVagSetFadeOut(int32 _no, int32 _time, int32 _flag)
{
    SdStr_t* SdStr = SdStrGet(SDSTR_TYPE_VAG, _no);
    if (SdStr)
        SdStrSetFadeOut(SdStr, _time, _flag, 0);
};


void SdVagReset(int32 _time, int32 _flag)
{
    int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);

    for (int32 i = 0; i < VagSrvCnt; ++i)
        SdVagSetFadeOut(i, _time, _flag);
};


void SdVagFadeOutX(int32 _code, int32 _time)
{
    int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);

    for (int32 i = 0; i < VagSrvCnt; ++i)
    {
        if (SdVagGetPlayCode(i) == _code)
            SdVagSetFadeOut(i, _time, SDSTR_FADEOUT_FLAG_RST_QUE);
    };
};