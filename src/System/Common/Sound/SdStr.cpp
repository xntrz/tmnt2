#include "SdStr.hpp"
#include "SdQueue.hpp"
#include "SdLoader.hpp"
#include "SdMem.hpp"
#include "SdDrv.hpp"
#include "SdLog.hpp"


extern void OnSdStrNew(SdStr_t* SdStr);
extern void OnSdStrDestroy(SdStr_t* SdStr);
extern void OnSdStrTask(SdStr_t* SdStr);


SdStr_t* SdStrSrv[SDSTR_SRV_MAX];


static inline bool SdStrCheckCondition(SdStr_t* SdStr)
{
    return ((SdStr->CheckKeyTop == SD_CANARY_TOP) &&
            (SdStr->CheckKeyEnd == SD_CANARY_END));
};


void* SdStrGetMemAddress(SdStr_t* SdStr)
{
    ASSERT(SdStr->Index >= 0);
    ASSERT(SdStr->Index < COUNT_OF(SdStrSrv));

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

        OnSdStrDestroy(SdStr);

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

        OnSdStrDestroy(SdStr);
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
            OnSdStrTask(SdStrSrv[i]);
    };
};


SdStr_t* SdStrSrvNew(int32 _no, int32 _type)
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

    Srv->CheckKeyTop = SD_CANARY_TOP;
    Srv->Index = SrvIndex;
    Srv->Type = _type;
    Srv->No = _no;
    Srv->DmaPageSize = 0;
    Srv->DmaPageBlock = 0;
    Srv->CheckKeyEnd = SD_CANARY_END;

    OnSdStrNew(Srv);

    return Srv;
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


bool SdStrSet(SdStr_t* SdStr, int32 _code)
{
    SdLogCodeLoad(_code);

    int32 CodeKind = SdDrvGetCodeKind(_code);

    if (!((CodeKind == SD_CODEKIND_VAG) ||
          (CodeKind == SD_CODEKIND_VOX)))
        return false;

    char Filename[SD_FILENAME_LEN];
    Filename[0] = '\0';

    switch (SdStr->Type)
    {
    case SDSTR_TYPE_VAG:
        {
            SdGetBgmFilename(Filename, SD_VAG_BANK_NO(_code));

            SdStr->DataPosition = SdGetBgmPosition(_code);
            SdStr->DataSize = SdGetBgmSize(_code);
            SdStr->DataDurationMS = SdGetBgmSize(_code);
            SdStr->DmaPageSize = SdGetBgmDmaPage(_code);
            SdStr->DmaPageBlock = SdGetBgmDmaBlock(_code);

            //SdStr->PanData      = SdStr->SetCode.Param1;
            SdStr->PlayVolume = SdStr->SetCode.Param2;
            SdStr->FadeSpeed = SdStr->SetCode.FadeSpeed;
            SdStr->FadeData = SdStr->SetCode.FadeData;

            SdStr->DataTotalPlayed = 0;

            std::memset(&SdStr->SetCode, 0x00, sizeof(SdStr->SetCode));
        }
        break;

    case SDSTR_TYPE_VOX:
        {
            SdGetVoiceFilename(Filename, SD_VOX_BANK_NO(_code));

            SdStr->DataPosition = SdGetVoicePosition(_code);
            SdStr->DataSize = SdGetVoiceSize(_code);
            SdStr->DataDurationMS = SdGetVoiceSize(_code);
            SdStr->DmaPageSize = SdGetVoiceDmaPage(_code);
            SdStr->DmaPageBlock = SdGetVoiceDmaBlock(_code);

            SdStr->DataTotalPlayed = 0;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (SdStr->DataSize <= 0)
        return false;
    
    SdStr->Filename = (char*)SdMemAlloc(std::strlen(Filename) + 1);
    if (!SdStr->Filename)
        return false;

    std::strcpy(SdStr->Filename, Filename);

    SdStr->PlayCode = _code;
    SdStr->PlayState = SDSTR_STATE_READ_HEAD;
    SdStr->PlayTime = 0;
    SdStr->Offset = 0;
    SdStr->MemAddress = nullptr;
    SdStr->MemOffset = SdStr->DmaPageBlock;

    SdStr->ALBufferCount = 0;
    SdStr->ALBufferNoPlaying = 0;
    SdStr->ALBufferNoReading = 0;
    SdStr->WaveTransferRqCount = 0;
    SdStr->WaveTransferRq = false;

    std::memset(SdStrGetMemAddress(SdStr), 0, SdStr->DmaPageSize);

    if (!SdStr->SetVolume)
        SdStr->SetVolume = SdStr->WaveHeader.Volume;

    return true;
};


bool SdStrQueuePush(SdStr_t* SdStr, SdSetDriverCode_t* CodeBuff)
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


void SdStrSetFadeOut(SdStr_t* SdStr, int32 _time, uint32 _flag, int32 _waittime)
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
    if (SdStr)
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