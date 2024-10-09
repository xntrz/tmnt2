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
#include "SdSeqPc.hpp"
#include "SdLog.hpp"

#include <mmsystem.h>


/*extern*/ uint32 SdDrvOptFlag = 0;
/*extern*/ bool SdDrvSurMode = false;


static bool             SdDrvThLoopFlag = false;
static HANDLE           SdDrvThHandle = NULL;
static DWORD            SdDrvThId = 0;
static HANDLE           SdDrvEvHandle = NULL;
static MMRESULT        	SdDrvTmHandle = 0;
static SdQueue_t*       SdDrvSetSoundCodeQueueR;
static SdQueue_t*       SdDrvSetSoundCodeQueueW;
static CRITICAL_SECTION SdDrvSetSoundCodeQueueCS;
static SdQueue_t* 		SdDrvSetUseSoundCodeQueue; /* this queue is used internally by sound thread itself only so there is no locks required */


static void SdDrvSetCode(int32 _reason, SdSetDriverCode_t* CodeBuff)
{
    if (!CodeBuff->Code)
        return;

    int32 CodeKind = SdDrvGetCodeKind(CodeBuff->Code);

    if (_reason != SD_REASON_SETNEXT)
    {
        SdDrvSetBeforeCB(_reason, CodeBuff);

        if (!CodeBuff->Code)
            return;
        
        CodeKind = SdDrvGetCodeKind(CodeBuff->Code);
    };

    switch (CodeKind)
    {
    case SD_CODEKIND_VOX:
        {
            int32 VoxSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VOX);
            if (VoxSrvCnt <= 0)
                break;

            int32 VoxSrv = SdDrvVoiceServerSelectionCB(CodeBuff);
            SdVoxSet(VoxSrv, CodeBuff);
        }
        break;

    case SD_CODEKIND_CTRL:
        {
            SdDrvCtrlFunc(CodeBuff->Code, CodeBuff->Param1, CodeBuff->Param2);
        }
        break;

    case SD_CODEKIND_SE:
        {
            if ((CodeBuff->ExtFlag & SD_MASTERCODE_FADEOUT) != 0)
            {
                if (((CodeBuff->ExtFlag & SD_MASTERCODE_SUR) == 0) || !CodeBuff->SurCtrl.Id)
                {
                    int32 time = SD_FADEOUT_SPEED_FAST;
                    if (CodeBuff->Param1)
                        time = CodeBuff->Param1;
                    SdDrvFadeOutX(CodeBuff->Code, time);
                    break;
                };

                SdSurroundCtrl(CodeBuff);
                break;
            };

            if (!CodeBuff->SurCtrl.Id || !SdSurroundCtrl(CodeBuff))
                SdDrvSetSE(CodeBuff);
        }
        break;

    case SD_CODEKIND_TRANS_SE:
    case SD_CODEKIND_TRANS_BGM:
        {
            if (!(CodeBuff->ExtFlag & SD_MASTERCODE_FADEOUT))
            {
                if (!CodeBuff->SurCtrl.Id || !SdSurroundCtrl(CodeBuff))
                    SdDrvSetTRSE(CodeBuff);
            }
            else if ((CodeBuff->ExtFlag & SD_MASTERCODE_SUR) && CodeBuff->SurCtrl.Id)
            {
                SdSurroundCtrl(CodeBuff);
            }
            else
            {
                int32 time = SD_FADEOUT_SPEED_FAST;
                if (CodeBuff->Param1)
                    time = CodeBuff->Param1;
                SdDrvFadeOutX(CodeBuff->Code, time);
            };
        }
        break;

    case SD_CODEKIND_VAG:
        {
            int32 VagSrvCnt = SdStrGetServerNum(SDSTR_TYPE_VAG);
            if (VagSrvCnt <= 0)
                break;

            if ((CodeBuff->MasterCode & SD_MASTERCODE_FADEOUT) != 0)
            {
                int32 time = SD_FADEOUT_SPEED_FAST;
                if (CodeBuff->Param1)
                    time = int32(CodeBuff->Param1);

                SdVagFadeOutX(CodeBuff->Code, time);
            }
            else
            {
                int32 VagSrv = SdDrvBGMServerSelectionCB(CodeBuff);
                int32 VagDefFadeoutMode = SdVagGetDefaultFadeOut(VagSrv);

                if (!SdDrvAutoFadeout(VagDefFadeoutMode, CodeBuff))
                    SdVagSet(VagSrv, CodeBuff);
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

    SdDrvSetAfterCB(_reason, CodeBuff);
};


static void SdDrvNextSet(void)
{
    ;
};


static void SdDrvSoundUseSet(void)
{
    while (!SdQueueIsEmpty(SdDrvSetUseSoundCodeQueue))
    {
        SdSetSoundCode_t SdSetSndCode;
        std::memset(&SdSetSndCode, 0, sizeof(SdSetSndCode));

        std::memcpy(&SdSetSndCode, SdQueueFront(SdDrvSetUseSoundCodeQueue), sizeof(SdSetSndCode));
        SdQueuePop(SdDrvSetUseSoundCodeQueue);

        SdSetDriverCode_t SdSetDrvCode;
        std::memset(&SdSetDrvCode, 0, sizeof(SdSetDrvCode));
        SdSetDrvCode.Code 		= SdSetSndCode.Code;
        SdSetDrvCode.MasterCode = SdSetSndCode.MasterCode;
        SdSetDrvCode.ExtFlag 	= SdSetSndCode.MasterCode;
        SdSetDrvCode.Param1 	= SdSetSndCode.Param1;
        SdSetDrvCode.Param2 	= SdSetSndCode.Param2;
        SdSetDrvCode.Param3 	= SdSetSndCode.Param3;

        std::memcpy(&SdSetDrvCode.SurCtrl, &SdSetSndCode.SurCtrl, sizeof(SdSetDrvCode.SurCtrl));

        if(!SdDrvAllFadeOutData)
            SdDrvSetCode(SD_REASON_SETUSE, &SdSetDrvCode);
    };
};


static void SdDrvSoundCrxSet(void)
{
    ;
};


static void SdDrvSoundRdcSet(void)
{
    ;
};


static void SdDrvParamChangeSet(void)
{
    ;
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

        std::memcpy(&SdSetDrvCode.SurCtrl, &SdSetSndCode.SurCtrl, sizeof(SdSetDrvCode.SurCtrl));
        
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
    SdDrvSetCurrentThreadName("SoundThread");
#endif /* _DEBUG */
    
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

        SdSeqMain();
        SdDrvSeqCorrectionCB();
        SdIOSetTask();
        SdSeqPcTask();		

        SdStrTask();
        
        SdWaveLoadTask();
        SdTransLoadTask();
    };

    return 0;
};


bool SdDrvInitialize(HWND _hwnd, const char* _path, void* _heap, size_t _heapSize, int32 _option)
{
    //
    //	Init sd drv option
    //
    SdDrvOptFlag = _option;

//#ifdef _DEBUG
//	BOOL bOrgGameRunning = FALSE;
//	HANDLE hEv = OpenEventA(EVENT_ALL_ACCESS, FALSE, "SdDriverEventFlag");
//	if (hEv)
//	{
//		OUTPUT("sddrv already running! supressing stream subsystem\n");
//		bOrgGameRunning = TRUE;
//		CloseHandle(hEv);
//	};
//
//	if (bOrgGameRunning)
//		SdDrvOptFlag |= (SD_OPTFLAG_NOVAG | SD_OPTFLAG_NOVOX);	
//#endif /* _DEBUG */    
    
    if (SdDrvOptFlag & SD_OPTFLAG_LOG)
        SdLogEnabled = true;
    
    //
    //  Init sd drv subsystems
    //
    SdMemInitialize();
    SdLoadTaskInit();
    SdStrInitialize();

    if (!SdDsInitialize(_hwnd))
    {
        OUTPUT("Device init failed!\n");
        return false;
    };

    /* init seq system */
    SdSeqInit();
    SdSeqPcInit();

    /* init stream system */
    SdSetHeap(_heap, _heapSize);
    SdDrvInitCB();
    
    if (!SdLoaderInit(_path))
    {
        OUTPUT("Loader init failed!\n");
        return false;
    };
    
    SdStreamLoad();

    //
    //  Init sd drv set code queues
    //

    /* init set sound code queues */
    SdDrvSetSoundCodeQueueR = SdQueueCreate(48, sizeof(SdSetSoundCode_t));
    if (!SdDrvSetSoundCodeQueueR)
        return false;
    
    SdDrvSetSoundCodeQueueW = SdQueueCreate(48, sizeof(SdSetSoundCode_t));
    if (!SdDrvSetSoundCodeQueueW)
        return false;
    
    InitializeCriticalSection(&SdDrvSetSoundCodeQueueCS);

    /* init set use sound code queue */
    SdDrvSetUseSoundCodeQueue = SdQueueCreate(32, sizeof(SdSetSoundCode_t));
    if (!SdDrvSetUseSoundCodeQueue)
        return false;

    //
    //  Init sd drv thread resources
    //
    SdDrvEvHandle = CreateEventA(NULL, TRUE, FALSE, "SdDriverEventFlag_2024");
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

#define SDDRV_QUEUE_DESTROY(q)  \
    if(q)                       \
    {                           \
        SdQueueDestroy(q);      \
        q = nullptr;            \
    }

    SDDRV_QUEUE_DESTROY(SdDrvSetUseSoundCodeQueue);

    DeleteCriticalSection(&SdDrvSetSoundCodeQueueCS);
    SDDRV_QUEUE_DESTROY(SdDrvSetSoundCodeQueueW);
    SDDRV_QUEUE_DESTROY(SdDrvSetSoundCodeQueueR);

    //
    //  Shutdown sd drv subsystems
    //

    /* shutdown stream system */
    SdStrTerminate();

    /* shutdown seq system */
    SdSeqPcTerm();
    SdSeqTerm();

    /* shutdown direct sound */
    SdDsTerminate();

    /* shutdown loader & mem man */
    SdLoaderTerm();
    SdLoadTaskTerm();
    SdMemTerminate();
};


void SdDrvCodeSet(int32 _code, uint32 _param1, uint32 _param2, uint32 _param3)
{
    int32 Code = (_code & SD_SOUNDCODE_MASK);    
    if (!Code)
        return;

    if (SdDrvGetCodeKind(Code) == SD_CODEKIND_UNKNOWN)
        return;

    SdSetSoundCode_t CodeBuff;
    std::memset(&CodeBuff, 0, sizeof(CodeBuff));
    
    CodeBuff.MasterCode	= (_code & SD_MASTERCODE_MASK);
    CodeBuff.Code 		= (_code & SD_SOUNDCODE_MASK);

    if ((CodeBuff.MasterCode & SD_MASTERCODE_SUR) != 0)
    {
        if ((CodeBuff.MasterCode & SD_MASTERCODE_FADEOUT) != 0)
        {
            CodeBuff.SurCtrl.Id = _param3;
        }
        else
        {
            if (_param1 > 0xFFFF)
                _param1 = _param1;

            if (_param2 > 0xFFFF)
                _param2 = 0xFFFF;

            if (_param3 > 0x7FFF)
                _param3 = 0x7FFF;

            CodeBuff.SurCtrl.Rot = (uint16)(-(int32)_param1);
            CodeBuff.SurCtrl.Dist = _param2;
            CodeBuff.SurCtrl.Id = _param3;
        };
    }
    else
    {
        CodeBuff.Param1 = (int16)_param1;
        CodeBuff.Param2 = (int16)_param2;
        CodeBuff.Param3 = (int16)_param3;
    };

    EnterCriticalSection(&SdDrvSetSoundCodeQueueCS);
    if (!SdQueueIsFull(SdDrvSetSoundCodeQueueW))
        SdQueuePush(SdDrvSetSoundCodeQueueW, &CodeBuff);
    LeaveCriticalSection(&SdDrvSetSoundCodeQueueCS);
};


void SdDrvSetUseCode(int32 _code, uint32 _param1, uint32 _param2, uint32 _param3)
{
    SdSetSoundCode_t CodeBuff;
    std::memset(&CodeBuff, 0, sizeof(CodeBuff));

    CodeBuff.Code 		= (_code & SD_SOUNDCODE_MASK);
    CodeBuff.Param1 	= (int16)_param1;
    CodeBuff.Param2 	= (int16)_param2;
    CodeBuff.Param3 	= (int16)_param3;
    CodeBuff.MasterCode = (_code & SD_MASTERCODE_MASK);

    if (!SdQueueIsFull(SdDrvSetUseSoundCodeQueue))
        SdQueuePush(SdDrvSetUseSoundCodeQueue, &CodeBuff);
};


bool SdDrvGetLoadInfo(void)
{
    return SdGetLoadInfo();
};


int32 SdDrvGetCodeKind(int32 _code)
{
    /* code is SE */
    if ((_code >= SD_CODE_SE_MIN) &&
        (_code <= SD_CODE_SE_MAX))
        return SD_CODEKIND_SE;

    /* code is RDC */
    if ((_code >= SD_CODE_RDC_MIN) &&
        (_code <= SD_CODE_RDC_MAX))
        return SD_CODEKIND_RDC;

    /* code is VOX */
    if ((_code >= SD_CODE_VOICE_MIN) &&
        (_code <= SD_CODE_VOICE_MAX))
        return SD_CODEKIND_VOX;

    /* code is CTRL */
    if ((_code >= SD_CODE_CTRL_MIN) &&
        (_code <= SD_CODE_CTRL_MAX))
        return SD_CODEKIND_CTRL;

    /* code is VAG */
    if ((_code >= SD_CODE_BGM_MIN) &&
        (_code <= SD_CODE_BGM_MAX))
        return SD_CODEKIND_VAG;

    /* code is TRANS */
    if ((_code >= SD_CODE_TRANS_MIN) &&
        (_code <= SD_CODE_TRANS_MAX))
        return SdBgmCheck(_code); 

    ASSERT(false, "unknown sd code 0x%" PRIx32 "(%" PRIi32 ")", _code, _code);

    return SD_CODEKIND_UNKNOWN;
};


void SdDrvSetSeqBGMOff(void)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (!Work->Code)
            continue;

        if (SdSeqMaskCheck(Work))
            continue;

        if (!(Work->Option & SD_SEQ_OPT_0x4))
            continue;

        SdSeqOff(Work, 3);
    };
};


void SdDrvSetSeqBGMFadeOut(int32 _time)
{
    int32 FadeSpeed = SD_FADEOUT_SPEED(_time);

    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (!Work->Code)
            continue;

        if (SdSeqMaskCheck(Work))
            continue;

        if (!(Work->Option & SD_SEQ_OPT_0x4))
            continue;

        Work->FadeOut.Speed = FadeSpeed;
        Work->FadeOut.Data = SD_FADE_MAX;
    };
};


void SdDrvSetSeqAllFadeOut(int32 _time, int32 _option)
{
    if (SdDrvAllFadeOutData)
        return;

    SdDrvAllFadeOutSpeed = SD_FADEOUT_SPEED(_time);
    SdDrvAllFadeOutData = SD_FADE_MAX;

    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];
        
        if (!Work->Code)
            continue;

        if (SdSeqMaskCheck(Work))
            continue;

        if ((Work->Option & _option) != 0)
            continue;

        Work->FadeOut.Speed = SdDrvAllFadeOutSpeed;
        Work->FadeOut.Data = SdDrvAllFadeOutData;
    };
};


bool SdDrvPlaybackCheck(int32 _code)
{
    if (SdDrvAllFadeOutData)
        return true;
    
    if (!_code)
        return false;

    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (SdSeqMaskCheck(Work))
            continue;
        
        if (Work->Code == _code)
            return true;
    };

    return false;
};


bool SdDrvVoiceBusyCheck(void)
{
    return SdVoxGetAllPlayVoice();
};


int32 SdDrvVoxCodeCheck(int32 _srv)
{
    return SdVoxGetPlayCode(_srv);
};


bool SdDrvAutoFadeout(int32 _mode, SdSetDriverCode_t* CodeBuff)
{
    /* TODO SdDrvAutoFadeout */
    return false;
};


void SdDrvLoad(int32 _command, int32 _mode, int32 _param)
{
    (void)_mode;

    SdDrvLoaderCommandCB(_command, _param);
};


void SdDrvSetCurrentThreadName(const char* _name)
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
    ThreadNameInfo.szName       = _name;
    ThreadNameInfo.dwThreadID   = GetCurrentThreadId();
    ThreadNameInfo.dwFlags      = 0;

#ifdef _MSC_VER
    __try
    {
#endif /* _MSC_VER */
        RaiseException(
            MS_VC_EXCEPTION,
            0,
            sizeof(ThreadNameInfo) / sizeof(ULONG_PTR),
            PULONG_PTR(&ThreadNameInfo)
        );
#ifdef _MSC_VER
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        ;
    };
#endif /* _MSC_VER */

#endif /* _DEBUG */
};


int32 SdBgmCheck(int32 _code)
{
    if ((_code >= SD_CODE_BGM_MIN) &&
        (_code <= SD_CODE_BGM_MAX))
        return SD_CODEKIND_VAG;

    int32 BGMBankNo = 0;
    for (int32 i = 0; i < COUNT_OF(SdTransCodeTable); ++i)
    {
        if (_code >= SdTransCodeTable[i].CodeStart &&
            _code <= SdTransCodeTable[i].CodeEnd)
        {
            BGMBankNo = i;
            break;
        };
    };

    if (BGMBankNo >= COUNT_OF(SdTransCodeTable))
    {
        SDLOG("TRANS SE CHECK FAILED: Cannot find bank! Code %" PRIx32 " (%" PRIi32 ")\n", _code, _code);
        return SD_CODEKIND_UNKNOWN;
    };

    if (SdTransCodeTable[BGMBankNo].Table == nullptr)
    {
        SDLOG("TRANS SE CHECK FAILED: Bank table is NULL! Code %" PRIx32 " (%" PRIi32 ")\n", _code, _code);
        return SD_CODEKIND_UNKNOWN;
    };

    int32 Offset = (_code - (int32)SdTransCodeTable[BGMBankNo].CodeStart);
    if (Offset >= SdTransCodeTable[BGMBankNo].Count)
    {
        SDLOG("TRANS SE CHECK FAILED: Offset for code in bank is incorrect! Code %" PRIx32 " (%" PRIi32 ")\n", _code, _code);
        return SD_CODEKIND_UNKNOWN;
    };

    if (SdTransCodeTable[BGMBankNo].Table[Offset].Count >= (uint8)52)
    {
        SDLOG("TRANS SE CHECK FAILED: Bank table count is greater than 52! Code %" PRIx32 " (%" PRIi32 ")\n", _code, _code);
        return SD_CODEKIND_UNKNOWN;
    };

    if (SdTransCodeTable[BGMBankNo].Table[Offset].Option & SD_SEQ_OPT_0x4)
        return SD_CODEKIND_TRANS_BGM;

    return SD_CODEKIND_TRANS_SE;
};


int32 GetBGMCode(void)
{
    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (SdSeqMaskCheck(Work))
            continue;

        if (!Work->Code)
            continue;

        if (Work->Option & SD_SEQ_OPT_0x4)
            return Work->Code;
    };

    return 0;
};


void SdDrvFadeOutX(int32 _code, int32 _time)
{
    if (!_code)
        return;

    int32 FadeSpeed = SD_FADEOUT_SPEED(_time);

    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (SdSeqMaskCheck(Work))
            continue;

        if (!Work->Code)
            continue;

        if ((Work->RdcParent.MasterCode == _code) || (Work->Code == _code))
        {
            Work->FadeOut.Speed = FadeSpeed;
            if (!Work->FadeOut.Data)
                Work->FadeOut.Data = SD_FADE_MAX;
        };
    };
};


bool SdSurroundCtrl(SdSetDriverCode_t* CodeBuff)
{
    if (!CodeBuff->SurCtrl.Id)
        return false;

    int32 SurCtrlDone = 0;

    for (int32 i = 0; i < COUNT_OF(SdSeqWork); ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i];

        if (!SdSeqMaskCheck(Work) && Work->Code && (Work->SurCtrl.Id == CodeBuff->SurCtrl.Id))
        {
            if ((CodeBuff->ExtFlag & SD_MASTERCODE_FADEOUT) == 0)
            {
                static_assert(sizeof(Work->SurCtrl) == sizeof(CodeBuff->SurCtrl), "should equal");

                std::memcpy(&Work->SurCtrl, &CodeBuff->SurCtrl, sizeof(Work->SurCtrl));

                ++SurCtrlDone;
            }
            else
            {
                int32 time = SD_FADEOUT_SPEED_FAST;
                if (CodeBuff->Param1)
                    time = CodeBuff->Param1;
    
                Work->FadeOut.Speed = SD_FADEOUT_SPEED(time);
                if (!Work->FadeOut.Data)
                {
                    Work->FadeOut.Data = SD_FADE_MAX;
                    ++SurCtrlDone;
                };
            };
        };
    };

    return (SurCtrlDone > 0);
};


bool SdDrvSetSE(SdSetDriverCode_t* CodeBuff)
{
    int32 BankNo = SD_SE_BANK_NO(CodeBuff->Code);

    ASSERT(BankNo >= 0);
    ASSERT(BankNo < COUNT_OF(SdSeCodeTable));

    SdKdtTableData_t* TableDataArray = (SdKdtTableData_t*)SdSeCodeTable[BankNo].Table;
    if (!TableDataArray)
        return false;
    
    int32 Offset = SD_SE_BANK_OFFSET(CodeBuff->Code);
    if (SdSeCodeTable[BankNo].Count <= Offset)
        return false;

    SdKdtTableData_t* TableData = &TableDataArray[Offset];
    int32 Option = (int32)TableData->Option;
    if (!SdDrvAutoFadeout(Option, CodeBuff) && ((Option & SD_SEQ_OPT_0x4) != 0))
        return false;

    int32 Channel = SdDrvSetChannelSECB(CodeBuff, TableData, Option);
    if (!Channel)
    {
        Channel = SdDrvFindChannel(
            CodeBuff->Code,
            (int32)TableData->SetMode,
            (int32)TableData->Channel,
            Option,
            (int32)TableData->Priority,
            (int32)TableData->Count
        );
    };

    if (Channel < 0)
        return false;

    CodeBuff->Channel = Channel;
    CodeBuff->Option = Option;

    SdDrvSetSeqWork(CodeBuff, TableDataArray, Offset);

    return true;
};


bool SdDrvSetTRSE(SdSetDriverCode_t* CodeBuff)
{
    int32 BankNo = 0;
    int32 Code = (CodeBuff->Code & 0x3FFF);

    for (int32 i = 0; i < COUNT_OF(SdTransCodeTable); ++i)
    {
        if ((Code >= (int32)SdTransCodeTable[i].CodeStart) &&
            (Code <= (int32)SdTransCodeTable[i].CodeEnd))
        {
            BankNo = i;
            break;
        };
    };

    if (BankNo >= COUNT_OF(SdTransCodeTable))
        return false;

    if (!SdTransCodeTable[BankNo].Table)
        return false;
    
    if ((int32)SdTransCodeTable[BankNo].CodeStart > Code)
        return false;

    int32 Offset = Code - (int32)SdTransCodeTable[BankNo].CodeStart;
    if (Offset >= SdTransCodeTable[BankNo].Count)
        return false;

    SdKdtTableData_t* TableData = SdTransCodeTable[BankNo].Table;
    if (TableData->Count >= 52)
        return false;

    if (!TableData->TableAddress)
        return false;

    if (TableData->Option & SD_SEQ_OPT_0x4)
    {
        if ((TableData->Option & SD_SEQ_OPT_0x200) && SdDrvSameCodeFind(CodeBuff->Code, 0, COUNT_OF(SdSeqWork) - 1, 1) >= 0)
            return false;

        if (!SdDrvAutoFadeout((int32)TableData->Option, CodeBuff))
            return false;

        int32 Priority = (int32)TableData->Priority;
        if (TableData->Option & SD_SEQ_OPT_0x10)
            --Priority;

        int32 Channel = SdDrvSetChannelBGMCB(CodeBuff, TableData, (int32)TableData->Option, Priority);
        if (Channel < 0)
            return false;

        SDNOIMPL();
    }
    else
    {
        int32 Channel = SdDrvSetChannelSECB(CodeBuff, TableData, (int32)TableData->Option);
        if (!Channel)
        {
            Channel = SdDrvFindChannel(
                CodeBuff->Code,
                (int32)TableData->SetMode,
                (int32)TableData->Channel,
                (int32)TableData->Option,
                (int32)TableData->Priority,
                (int32)TableData->Count
            );
        };

        if (Channel < 0)
            return false;

        CodeBuff->Channel = Channel;
        CodeBuff->Option = (int32)TableData->Option;
    };

    SdDrvSetSeqWork(CodeBuff, TableData, Offset);

    return true;
};


int32 SdDrvFindChannel(int32 _code, int32 _mode, int32 _channel, int32 _option, int32 _priority, int32 _count)
{
    ASSERT(_channel >= 0);
    ASSERT(_channel < COUNT_OF(SdSeqWork));

    if ((_channel >= COUNT_OF(SdSeqWork)) ||
        (_channel < 0))
        return -1;

    if ((_option & SD_SEQ_OPT_0x10) != 0)
        --_priority;
    
    if (_mode == 0xFF)
    {
        if (_count <= 0)
            return _channel;

        for (int32 i = _channel; i < _count; ++i)
        {
            SdSeqWork_t* Work = &SdSeqWork[i];

            if (SdSeqMaskCheck(Work))
                return -1;

            if (Work->Code)
            {
                if (_priority < Work->Priority)
                    return -1;
            };
        };

        return _channel;
    };

    int32 ChannelCount = (_channel - _mode + 1);

    if ((ChannelCount <= 0) ||
        (_count > ChannelCount))
        return -1;
    
    if (((_option & SD_SEQ_OPT_0x8) == 0) ||
        ((_option & SD_SEQ_OPT_0x200) != 0))
    {
        int32 Channel = SdDrvSameCodeFind(_code, _mode, ChannelCount, _count);
        if (Channel >= 0)
        {
            if ((_option & SD_SEQ_OPT_0x200) == 0)
                return Channel;

            return -1;
        };
    };

    int32 FindStart = 0;
    int32 FindEnd = 0;
    int32 FindAdd = 0;
    int32 FindCount = 0;
    int32 EmptyCount = 0;
    int32 KeepPriority = 256;
    int32 Port = -1;
    int32 Priority = 256;

    if (_option & SD_SEQ_OPT_0x100)
    {
        FindStart = _mode + ChannelCount - 1;
        FindEnd = _mode;
        FindAdd = -1;
    }
    else
    {
        FindStart = _mode;
        FindEnd = _mode + ChannelCount;
        FindAdd = 1;
    };

    ASSERT(FindStart >= 0);
    ASSERT(FindStart < COUNT_OF(SdSeqWork));

    SdSeqWork_t* Work = &SdSeqWork[FindStart];
    while (FindStart != FindEnd)
    {
        if (SdSeqMaskCheck(Work) || Work->Code && (_priority < Work->Priority))
        {
            EmptyCount = 0;
            FindCount = 0;
            KeepPriority = 256;
        }
        else
        {
            if (Work->Code)
            {
                EmptyCount = 0;
            }
            else
            {
                EmptyCount = EmptyCount + 1;
                if (EmptyCount >= _count)
                {
                    Port = FindStart;
                    break;
                };
            };

            FindCount = FindCount + 1;

            if (KeepPriority >= Work->Priority)
                KeepPriority = Work->Priority;

            if (FindCount >= _count)
            {
                if (KeepPriority < Priority)
                {
                    Priority = KeepPriority;
                    Port = FindStart;
                }
                FindCount = 0;
                KeepPriority = 256;
            };
        };

        ++Work;

        FindStart = FindStart + FindAdd;
    };

    if (!(_option & SD_SEQ_OPT_0x100))
        Port += (1 - _count);
    
    return Port;
};


int32 SdDrvSameCodeFind(int32 _code, int32 _start, int32 _portCount, int32 _count)
{
    ASSERT(_start >= 0);
    ASSERT(_start < COUNT_OF(SdSeqWork));
    ASSERT((_start + _portCount) < COUNT_OF(SdSeqWork));

    int32 FindCount = 0;
    SdSeqWork_t* Work = &SdSeqWork[_start];

    for (int32 i = 0; i < _portCount; ++i)
    {
        if (SdSeqMaskCheck(Work) || (Work->Code != _code))
            FindCount = 0;
        else if (++FindCount >= _count)
            return (i - _count + _start + 1);

        ++Work;
    };

    return -1;
};


void SdDrvSetSeqWork(SdSetDriverCode_t* CodeBuff, SdKdtTableData_t* TableData, int32 Offset)
{
    ASSERT(CodeBuff->Channel >= 0);
    ASSERT(CodeBuff->Channel < COUNT_OF(SdSeqWork));

    SdKdtTableData_t* table = &TableData[Offset];
    ASSERT((CodeBuff->Channel + table->Count) < COUNT_OF(SdSeqWork));

    if (!SdWaveTopAddress[table->WaveBank])
    {
        SDLOG("set SE failed - wavebank %" PRIu8 " is not loaded\n", table->WaveBank);
        return;
    };

    int32 Count = (int32)table->Count;
    for (int32 i = 0; i < Count; ++i)
    {
        SdSeqWork_t* Work = &SdSeqWork[i + CodeBuff->Channel];

        std::memset(Work, 0, sizeof(*Work));

        Work->CheckKeyTop           = SD_CANARY_TOP;
        Work->ChNo                  = (uint16)(i + CodeBuff->Channel);
        Work->Code                  = CodeBuff->Code;
        Work->CtrlVol               = CodeBuff->Param2;
        Work->CtrlPan               = CodeBuff->Param1;
        Work->nCtrlSur              = CodeBuff->Param3;
        Work->SPan.Data             = 0x1400;
        Work->SPan.MoveData         = 0x1400;
        std::memcpy(&Work->SurCtrl, &CodeBuff->SurCtrl, sizeof(Work->SurCtrl));
        Work->Tempo.Count           = 0x8000;
        Work->Tempo.Data            = 0x8000;
        Work->Step.StepCount        = 1;
        Work->Step.StepTime         = 1;
        Work->Step.GateTime         = 1;
        Work->Step.GateLast         = 1;
        Work->Volume.Velocity       = 1;
        Work->KeyOffWait            = 1;
        Work->SdWait                = 3;
        Work->Priority              = table->Priority;
        Work->Option                = CodeBuff->Option;
        Work->TransWaveNo           = table->WaveBank;
        Work->RdcParam[0]           = CodeBuff->FRange;
        Work->RdcParam[1]           = CodeBuff->VRange;
        Work->RdcParam[2]           = CodeBuff->PRange;
        Work->RdcParam[3]           = CodeBuff->SRange;
        Work->RdcParent.MasterCode  = CodeBuff->MasterCode;
        Work->CheckKeyEnd           = SD_CANARY_END;

        if (SdDrvAllFadeOutData && (CodeBuff->Option & SD_SEQ_OPT_0x80))
        {
            Work->FadeOut.Data  = SdDrvAllFadeOutData;
            Work->FadeOut.Speed = SdDrvAllFadeOutSpeed;
        }
        else
        {
            Work->FadeOut.Data  = CodeBuff->FadeData;
            Work->FadeOut.Speed = CodeBuff->FadeSpeed;
        };

        uint32 DataOffset = ((uint32*)TableData[Offset].TableAddress)[i];
        Work->Address = (uint32)TableData[Offset].TableAddress + DataOffset;
    };
};


/***********************************************************
 *
 *  Following functions are presented only for test or debug
 *
 ***********************************************************/

int32 GetSECount(void)
{
    int32 Count = 0;

    for (int32 i = 0; i < COUNT_OF(SdSeCodeTable); ++i)
        Count += SdSeCodeTable[i].Count;

    return Count;
};


int32 GetSEBank(int32 _code)
{
    int32 Code = (_code & SD_SOUNDCODE_MASK);
    int32 CodeKind = SdDrvGetCodeKind(Code);

    if (CodeKind != SD_CODEKIND_SE)
        return -1;

    int32 BankNo = SD_SE_BANK_NO(Code);
    if ((BankNo < 0) || (BankNo >= COUNT_OF(SdSeCodeTable)))
        return -1;	

    SdKdtTableData_t* TableDataArray = (SdKdtTableData_t*)SdSeCodeTable[BankNo].Table;
    if (!TableDataArray)
        return false;

    int32 Offset = (Code & 0xFF);
    if (SdSeCodeTable[BankNo].Count <= Offset)
        return -1;

    return BankNo;
};


bool LoadSEBank(int32 _bank)
{
    for (int32 i = 2; i < 128; ++i)
        SdWaveRelease(i);

    /* skip loading first two banks due some of SE depends on each other (for example some SE from bank 1 depends on wavedata from bank 0) */
    uint32 addr = (uint32)SdGetWaveAddr();
    addr += SdGetWaveSize(0);
    addr += SdGetWaveSize(1);

    if (_bank > 1)
        SdWaveLoad(_bank, addr);
    
    return false;
};


int32 GetTRSECount(void)
{
    int32 Count = 0;

    for (int32 i = 0; i < COUNT_OF(SdTransCodeTable); ++i)
        Count += SdTransCodeTable[i].Count;

    return Count;
};


int32 GetTRSEBank(int32 _code)
{
    int32 Code = (_code & SD_SOUNDCODE_MASK);

    if ((Code >= SD_CODE_TRANS_MIN) &&
        (Code <= SD_CODE_TRANS_MAX))
    {
        Code &= 0x3FFF;

        for (int32 i = 0; i < COUNT_OF(SdTransCodeTable); ++i)
        {
            if ((Code >= (int32)SdTransCodeTable[i].CodeStart) &&
                (Code <= (int32)SdTransCodeTable[i].CodeEnd))
                return i;
        };
    };

    return -1;
};


bool LoadTRSEBank(int32 _bank)
{
    for (int32 i = 1; i < 64; ++i)
        SdTransRelease(i);
    SdTransLoad(_bank);
    return false;
};


int32 GetTRSEWaveBank(int32 _code)
{
    int32 Code = (_code & SD_SOUNDCODE_MASK);

    if ((Code >= SD_CODE_TRANS_MIN) &&
        (Code <= SD_CODE_TRANS_MAX))
    {
        Code &= 0x3FFF;

        int32 BankNo = 0;

        for (int32 i = 0; i < COUNT_OF(SdTransCodeTable); ++i)
        {
            if ((Code >= (int32)SdTransCodeTable[i].CodeStart) &&
                (Code <= (int32)SdTransCodeTable[i].CodeEnd))
            {
                BankNo = i;
                break;
            };
        };

        if (BankNo >= COUNT_OF(SdTransCodeTable))
            return -1;

        if (!SdTransCodeTable[BankNo].Table)
            return -1;

        int32 Offset = Code - (int32)SdTransCodeTable[BankNo].CodeStart;
        if (Offset >= SdTransCodeTable[BankNo].Count)
            return -1;

        SdKdtTableData_t* TableData = SdTransCodeTable[BankNo].Table;
        if (TableData->Count >= 52)
            return -1;

        return (int32)TableData[Offset].WaveBank;
    };

    return -1;
};


bool IsTRSEWaveBankLoaded(int32 _bank)
{
    ASSERT(_bank >= 0);
    ASSERT(_bank < COUNT_OF(SdWaveTopAddress));

    return (SdWaveTopAddress[_bank] != nullptr);
};