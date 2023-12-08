#include "SdDrv.hpp"
#include "SdMem.hpp"
#include "SdLoader.hpp"
#include "SdLoadTask.hpp"
#include "SdDs.hpp"
#include "SdSeq.hpp"
#include "SdIOSet.hpp"
#include "SdStr.hpp"
#include "SdCallback.hpp"
#include "SdDrvCtrl.hpp"

#include <mmsystem.h>


/*extern*/ uint32 SdDrvOptFlag = 0;


static bool             SdDrvThLoopFlag = false;
static HANDLE           SdDrvThHandle = NULL;
static DWORD            SdDrvThId = 0;
static HANDLE           SdDrvEvHandle = NULL;
static uint32           SdDrvTmHandle = 0;
static SdQueue_t*       SdDrvSetSoundCodeQueueR;
static SdQueue_t*       SdDrvSetSoundCodeQueueW;
static CRITICAL_SECTION SdDrvSetSoundCodeQueueCS;


static void SdDrvSetCode(int32 Reason, SdSetDriverCode_t* SdSetDrvCode)
{
    if (!SdSetDrvCode->Code)
        return;

    int32 CodeKind = SdDrvGetCodeKind(SdSetDrvCode->Code);

    if (Reason != SD_REASON_SETNEXT)
    {
        SdDrvSetBeforeCB(Reason, SdSetDrvCode);
        
        if (!SdSetDrvCode->Code)
            return;
        
        CodeKind = SdDrvGetCodeKind(SdSetDrvCode->Code);
    };

    switch (CodeKind)
    {
    case SD_CODEKIND_VOX:
        {
            int32 VoxSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VOX);
            if (VoxSrvCnt <= 0)
                break;

            int32 VoxSrv = SdDrvVoiceServerSelectionCB(SdSetDrvCode);
            SdVoxSet(VoxSrv, SdSetDrvCode);
        }
        break;

    case SD_CODEKIND_CTRL:
        {
            SdDrvCtrlFunc(SdSetDrvCode->Code, SdSetDrvCode->Param1, SdSetDrvCode->Param2);
        }
        break;

    case SD_CODEKIND_SE:
        {
            ;
        }
        break;

    case SD_CODEKIND_TRANS_SE:
    case SD_CODEKIND_TRANS_BGM:
        {
            ;
        }
        break;

    case SD_CODEKIND_VAG:
        {
            int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);
            if (VagSrvCnt <= 0)
                break;

            if ((SdSetDrvCode->MasterCode & SD_MASTERCODE_FADE) != 0)
            {
                int32 time = 320;

                if (SdSetDrvCode->Param1)
                    time = int32(SdSetDrvCode->Param1);

                SdVagFadeOutX(SdSetDrvCode->Code, time);
            }
            else
            {
                int32 VagSrv = SdDrvBGMServerSelectionCB(SdSetDrvCode);
                int32 VagDefFadeoutMode = SdVagGetDefaultFadeOut(VagSrv);

                if (!SdDrvAutoFadeout(VagDefFadeoutMode, SdSetDrvCode))
                    SdVagSet(VagSrv, SdSetDrvCode);
            };
        }
        break;

    case SD_CODEKIND_RDC:
        {
            ;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    SdDrvSetAfterCB(Reason, SdSetDrvCode);
};


static void SdDrvNextSet(void)
{

};


static void SdDrvSoundUseSet(void)
{

};


static void SdDrvSoundCrxSet(void)
{

};


static void SdDrvSoundRdcSet(void)
{

};


static void SdDrvParamChangeSet(void)
{

};


static void SdDrvSoundSet(void)
{
    EnterCriticalSection(&SdDrvSetSoundCodeQueueCS);
    SdQueueMove(SdDrvSetSoundCodeQueueW, SdDrvSetSoundCodeQueueR);
    LeaveCriticalSection(&SdDrvSetSoundCodeQueueCS);

    while (!SdQueueIsEmpty(SdDrvSetSoundCodeQueueR))
    {
        SdSetSoundCode_t SdSetSndCode;
        std::memset(&SdSetSndCode, 0, sizeof(SdSetSndCode));
        
        std::memcpy(&SdSetSndCode, SdQueueFront(SdDrvSetSoundCodeQueueR), sizeof(SdSetSndCode));
        SdQueuePop(SdDrvSetSoundCodeQueueR);
        
        SdSetDriverCode_t SdSetDrvCode;
        std::memset(&SdSetDrvCode, 0, sizeof(SdSetDrvCode));
        SdSetDrvCode.Code       = SdSetSndCode.Code;
        SdSetDrvCode.MasterCode = SdSetSndCode.MasterCode;
        SdSetDrvCode.ExtFlag    = SdSetSndCode.MasterCode;
        SdSetDrvCode.Param1     = SdSetSndCode.Param1;
        SdSetDrvCode.Param2     = SdSetSndCode.Param2;
        SdSetDrvCode.Param3     = SdSetSndCode.Param3;

        SdDrvSetCode(SD_REASON_SETSOUND, &SdSetDrvCode);
    };
};


static void CALLBACK SoundTimerCB(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    SetEvent(SdDrvEvHandle);
};


static DWORD WINAPI SoundThread(LPVOID lpParam)
{
    (void)lpParam;

#ifdef _DEBUG
    SdDrvThreadSetName("SoundThread");
#endif
    
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    
    SdDrvThLoopFlag = true;
    
    while (SdDrvThLoopFlag)
    {
        ResetEvent(SdDrvEvHandle);
        WaitForSingleObject(SdDrvEvHandle, 20);

        SdDrvSoundSet();
        SdDrvParamChangeSet();
        SdDrvSoundRdcSet();
        SdDrvSoundCrxSet();
        SdDrvSoundUseSet();
        SdDrvNextSet();

        //SdSeqTask();        
        //SdIOSetTask();
        //SdSeqPlatTask();

        SdStrTask();
        
        SdWaveLoadTask();
        SdTransLoadTask();
    };

    return 0;
};


bool SdDrvInitialize(HWND hWnd, const char* Path, void* HeapPtr, uint32 HeapSize, uint32 OptFlag)
{
#ifdef _DEBUG
    //
    //  avoid same drv streaming resource sharing error
    //
    BOOL bOrgGameRunning = FALSE;    
    HANDLE hEv = OpenEventA(EVENT_ALL_ACCESS, FALSE, "SdDriverEventFlag");
    if (hEv)
    {
		OUTPUT("sddrv already running! supressing stream subsystem\n");
        bOrgGameRunning = TRUE;
        CloseHandle(hEv);
    };

    OptFlag |= (bOrgGameRunning ? SD_OPTFLAG_NOVAG : 0);
    OptFlag |= (bOrgGameRunning ? SD_OPTFLAG_NOVOX : 0);
#endif    

    //
    //  Init sd drv subsystems
    //
    SdMemInitialize();
    SdLoadTaskInit();
    SdStrInitialize();

    if (!SdDsInitialize(hWnd))
    {
        OUTPUT("Device init failed!\n");
        return false;
    };

	SdDrvOptFlag = OptFlag;
    SdSetHeap(HeapPtr, HeapSize);
    SdDrvInitCB();
    
    if (!SdLoaderInit(Path))
    {
        OUTPUT("Loader init failed!\n");
        return false;
    };
    
    SdStreamLoad();

    //
    //  Init sd drv set code queues
    //
    SdDrvSetSoundCodeQueueR = SdQueueCreate(48, sizeof(SdSetSoundCode_t));
    if (!SdDrvSetSoundCodeQueueR)
        return false;
    
    SdDrvSetSoundCodeQueueW = SdQueueCreate(48, sizeof(SdSetSoundCode_t));
    if (!SdDrvSetSoundCodeQueueW)
        return false;
    
    InitializeCriticalSection(&SdDrvSetSoundCodeQueueCS);

    //
    //  Init sd drv thread resources
    //
    SdDrvEvHandle = CreateEventA(NULL, TRUE, FALSE, "SdDriverEventFlag_RE");
    if (SdDrvEvHandle == NULL)
        return false;
    
    SdDrvTmHandle = timeSetEvent(SD_PERIOD, 0, SoundTimerCB, NULL, TIME_PERIODIC);
    if (SdDrvTmHandle == NULL)
        return false;

    SdDrvThHandle = CreateThread(NULL, NULL, SoundThread, NULL, NULL, &SdDrvThId);
    if (SdDrvThHandle == NULL)
        return false;

    return true;
};


void SdDrvTerminate(void)
{
    //
    //  Shutdown sd drv thread resources
    //
    if (SdDrvThHandle)
    {
        SdDrvThLoopFlag = false;

        WaitForSingleObject(SdDrvThHandle, INFINITE);
        SdDrvThHandle = NULL;
    };

    if (SdDrvTmHandle)
    {
        timeKillEvent(SdDrvTmHandle);
        SdDrvTmHandle = NULL;
    };

    if (SdDrvEvHandle)
    {
        CloseHandle(SdDrvEvHandle);
        SdDrvEvHandle = NULL;
    };

    //
    //  Cleanup sd drv set code queues
    //
    DeleteCriticalSection(&SdDrvSetSoundCodeQueueCS);

#define SDDRV_QUEUE_DESTROY(q)  \
    if(q)                       \
    {                           \
        SdQueueDestroy(q);      \
        q = nullptr;            \
    }

    SDDRV_QUEUE_DESTROY(SdDrvSetSoundCodeQueueW);
    SDDRV_QUEUE_DESTROY(SdDrvSetSoundCodeQueueR);

    //
    //  Shutdown sd drv subsystems
    //
    SdStrTerminate();

    SdDsTerminate();
    
    SdLoaderTerm();
    SdLoadTaskTerm();
    SdMemTerminate();
};


void SdDrvCodeSet(uint32 Code, uint32 Param1, uint32 Param2, uint32 Param3)
{
    if (!Code)
        return;

    EnterCriticalSection(&SdDrvSetSoundCodeQueueCS);
    if (SdQueueIsFull(SdDrvSetSoundCodeQueueW))
    {
        LeaveCriticalSection(&SdDrvSetSoundCodeQueueCS);
        return;
    };

    SdSetSoundCode_t SdSetSoundCode;
    std::memset(&SdSetSoundCode, 0, sizeof(SdSetSoundCode));

    SdSetSoundCode.MasterCode   = Code & 0xFF000000;
    SdSetSoundCode.Code         = Code & 0x00FFFFFF;

    if ((SdSetSoundCode.MasterCode & SD_MASTERCODE_SUR) != 0)        
    {
        if ((SdSetSoundCode.MasterCode & SD_MASTERCODE_FADE) != 0)
        {
            SdSetSoundCode.SurCtrl.Id = Param3;
        }
        else
        {
            if (Param1 > 0xFFFF)
                Param1 = 0xFFFF;

            if (Param2 > 0xFFFF)
                Param2 = 0xFFFF;

            if (Param3 > 0x7FFF)
                Param3 = 0x7FFF;

            SdSetSoundCode.SurCtrl.Rot = -Param1;
            SdSetSoundCode.SurCtrl.Dist = Param2;
            SdSetSoundCode.SurCtrl.Id = Param3;
        };
    }
    else
    {
        SdSetSoundCode.Param1 = Param1;
        SdSetSoundCode.Param2 = Param2;
        SdSetSoundCode.Param3 = Param3;
    };

    if (SdDrvGetCodeKind(SdSetSoundCode.Code) != SD_CODEKIND_UNKNOWN)
        SdQueuePush(SdDrvSetSoundCodeQueueW, &SdSetSoundCode);

    LeaveCriticalSection(&SdDrvSetSoundCodeQueueCS);
};


void SdDrvSetUseCode(uint32 Code, uint32 Param1, uint32 Param2, uint32 Param3)
{

};


bool SdDrvGetLoadInfo(void)
{
    return SdGetLoadInfo();
};


int32 SdDrvGetCodeKind(uint32 Code)
{
    if (SD_INRANGE(Code, SD_CODE_CTRL_MIN, SD_CODE_CTRL_MAX))
        return SD_CODEKIND_CTRL;

    if (SD_INRANGE(Code, SD_CODE_SE_MIN, SD_CODE_SE_MAX))
        return SD_CODEKIND_SE;

    if (SD_INRANGE(Code, SD_CODE_TRANS_MIN, SD_CODE_TRANS_MAX))
        return SD_CODEKIND_UNKNOWN; // TODO

    if (SD_INRANGE(Code, SD_CODE_BGM_MIN, SD_CODE_BGM_MAX))
        return SD_CODEKIND_VAG;

    if (SD_INRANGE(Code, SD_CODE_VOICE_MIN, SD_CODE_VOICE_MAX))
        return SD_CODEKIND_VOX;

    if (SD_INRANGE(Code, SD_CODE_RDC_MIN, SD_CODE_RDC_MAX))
        return SD_CODEKIND_RDC;

    return SD_CODEKIND_UNKNOWN;
};


void SdDrvSetSurMode(bool flag)
{

};


bool SdDrvGetSurMode(void)
{
    return false;
};


void SdDrvSetSeqBGMOff(void)
{

};


void SdDrvSetSeqBGMFadeOut(int32 time)
{

};


void SdDrvSetSeqAllFadeOut(int32 time, uint32 option)
{

};


bool SdDrvPlaybackCheck(uint32 Code)
{
    return false;
};


bool SdDrvVoiceBusyCheck(void)
{
	if (SdDrvOptFlag & SD_OPTFLAG_NOVOX)
		return false;

    return (SdVoxGetAllPlayVoice());
};


uint32 SdDrvVoxCodeCheck(int32 VoxSrvNo)
{
	if (SdDrvOptFlag & SD_OPTFLAG_NOVOX)
		return 0;

    return SdVoxGetPlayCode(VoxSrvNo);
};


bool SdDrvAutoFadeout(int32 mode, SdSetDriverCode_t* SetDrvCode)
{
	return false;
};


void SdDrvLoad(int32 Command, uint32 Param)
{
    SdDrvLoaderCommandCB(Command, Param);
};


bool SdDrvIsStarted(void)
{
    return SdDrvThLoopFlag;
};


void SdDrvThreadSetName(const char* Name, uint32 ThreadId)
{
#ifdef _DEBUG    
    //
    //  https://learn.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code
    //
    
    if (!IsDebuggerPresent())
        return;

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
    typedef struct {
        DWORD    dwType;     // Must be 0x1000.
        LPCSTR   szName;     // Pointer to name (in user addr space).
        DWORD    dwThreadID; // Thread ID (-1=caller thread).
        DWORD    dwFlags;    // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    THREADNAME_INFO ThreadNameInfo = {};
    ThreadNameInfo.dwType       = 0x1000;
    ThreadNameInfo.szName       = Name;
    ThreadNameInfo.dwThreadID   = (ThreadId ? ThreadId : GetCurrentThreadId());
    ThreadNameInfo.dwFlags      = 0;

    __try
    {
        RaiseException(
            MS_VC_EXCEPTION,
            0,
            sizeof(ThreadNameInfo) / sizeof(ULONG_PTR),
            PULONG_PTR(&ThreadNameInfo)
        );
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        ;
    };
#endif    
};