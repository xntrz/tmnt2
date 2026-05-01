#include "SdThread.hpp"
#include "SdTypes.hpp"
#include "SdMem.hpp"
#include "SdIncWin.hpp"

#include <mmsystem.h>


typedef struct SdMutex_t
{
    CRITICAL_SECTION CS;
} SdMutex_t;


static bool               SdDrvThLoopFlag = false;
static HANDLE             SdDrvThHandle = NULL;
static DWORD              SdDrvThId = 0;
static HANDLE             SdDrvEvHandle = NULL;
static MMRESULT        	  SdDrvTmHandle = 0;
static CRITICAL_SECTION   SdDrvSetSoundCodeQueueCS;
static SdThreadCallback_t SdThreadCallback = nullptr;
static void*              SdThreadCallbackParam = nullptr;


static void CALLBACK
SoundTimerCB(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    SetEvent(SdDrvEvHandle);
};


static DWORD WINAPI
SoundThread(LPVOID lpParam)
{
    SdThreadSetName("SoundThread");

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    SdDrvThLoopFlag = true;

    while (SdDrvThLoopFlag)
    {
        ResetEvent(SdDrvEvHandle);
        WaitForSingleObject(SdDrvEvHandle, 20);

        SdThreadCallback(SdThreadCallbackParam);
    };

    return 0;
};


bool SdThreadInitialize(SdThreadCallback_t _cb, void* _param)
{
    SdDrvEvHandle = CreateEventA(NULL, TRUE, FALSE, "SdDriverEventFlag_2024");
    if (SdDrvEvHandle == NULL)
        return false;

    SdDrvTmHandle = timeSetEvent(SD_PERIOD, 0, SoundTimerCB, NULL, TIME_PERIODIC);
    if (SdDrvTmHandle == NULL)
        return false;

    SdThreadCallback = _cb;
    SdThreadCallbackParam = _param;

    SdDrvThHandle = CreateThread(NULL, NULL, SoundThread, NULL, NULL, &SdDrvThId);
    if (SdDrvThHandle == NULL)
        return false;

    return true;
};


void SdThreadTerminate(void)
{
    if (SdDrvThHandle)
    {
        SdDrvThLoopFlag = false;

        WaitForSingleObject(SdDrvThHandle, INFINITE);
        SdDrvThHandle = NULL;
    };

    SdThreadCallback = nullptr;;
    SdThreadCallbackParam = nullptr;

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
};


void SdThreadSetName(const char* _name)
{
#if defined(_DEBUG)
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
    ThreadNameInfo.dwType = 0x1000;
    ThreadNameInfo.szName = _name;
    ThreadNameInfo.dwThreadID = GetCurrentThreadId();
    ThreadNameInfo.dwFlags = 0;

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

#endif /* defined(_DEBUG) */
};


SdMutex_t* SdMutexCreate(void)
{
    SdMutex_t* SdMutex = (SdMutex_t*)SdMemAlloc(sizeof(SdMutex_t));
    if (SdMutex)
    {
        InitializeCriticalSection(&SdMutex->CS);
    };
    
    return SdMutex;
};


void SdMutexDestroy(SdMutex_t* _mutex)
{
    DeleteCriticalSection(&_mutex->CS);
    SdMemFree(_mutex);
};


void SdMutexLock(SdMutex_t* _mutex)
{
    EnterCriticalSection(&_mutex->CS);
};


void SdMutexUnlock(SdMutex_t* _mutex)
{
    LeaveCriticalSection(&_mutex->CS);
};