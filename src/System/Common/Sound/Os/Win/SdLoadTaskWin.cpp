#include "SdLoadTask.hpp"
#include "SdQueue.hpp"
#include "SdThread.hpp"
#include "SdIncWin.hpp"


struct SdLoadTask_t
{
    HANDLE		Handle;
    void*		ReadAddr;
    uint32		ReadSize;
    char		Filepath[MAX_PATH];
    bool		State;
    uint32		cbReaded;
    void*       UserData;
};


static bool             SdLoadThreadLoopFlag = false;
static HANDLE           SdLoadThreadHandle = NULL;
static SdQueue_t* 		SdLoadTaskQueueR = nullptr;
static SdQueue_t* 		SdLoadTaskQueueW = nullptr;
static SdMutex_t*       SdLoadTaskQueueMutex;
static SdLoadTask_t		SdLoadTask[64];
static int32 			SdLoadTaskPos = 0;


static DWORD WINAPI
SdLoadThreadProc(void* pParam)
{
#ifdef _DEBUG
    SdThreadSetName("SoundLoadThread");
#endif /* _DEBUG */
    
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    SdLoadThreadLoopFlag = true;
    
    while (SdLoadThreadLoopFlag)
    {
        Sleep(10);

        SdMutexLock(SdLoadTaskQueueMutex);
        SdQueueMove(SdLoadTaskQueueW, SdLoadTaskQueueR);
        SdMutexUnlock(SdLoadTaskQueueMutex);

        while (!SdQueueIsEmpty(SdLoadTaskQueueR))
        {
            SdLoadTask_t* Task = *(SdLoadTask_t**)SdQueueFront(SdLoadTaskQueueR);
            SdQueuePop(SdLoadTaskQueueR);

            if (Task->Handle)
            {
                if (ReadFile(Task->Handle, Task->ReadAddr, Task->ReadSize, LPDWORD(Task->cbReaded), NULL))
                {
                    CloseHandle(Task->Handle);
                    Task->Handle = NULL;
                    Task->State = true;
                };
            };
        };
    };

    return 0;
};


bool SdLoadTaskInit(void)
{
    /*
     *	Init sd loader queues
     */
    SdLoadTaskQueueR = SdQueueCreate(COUNT_OF(SdLoadTask), sizeof(SdLoadTask_t*));
    if (!SdLoadTaskQueueR)
        return false;

    SdLoadTaskQueueW = SdQueueCreate(COUNT_OF(SdLoadTask), sizeof(SdLoadTask_t*));
    if (!SdLoadTaskQueueW)
        return false;

    /*
     *	Start sd loader thread
     */
    SdLoadTaskQueueMutex = SdMutexCreate();
    if (!SdLoadTaskQueueMutex)
        return false;
    
    SdLoadThreadHandle = CreateThread(NULL, 0, SdLoadThreadProc, NULL, NULL, NULL);
    if (SdLoadThreadHandle == NULL)
        return false;

    return true;
};


void SdLoadTaskTerm(void)
{
    /*
     *	Stop sd loader thread & cleanup tasks
     */
    if (SdLoadThreadHandle)
    {
        SdLoadThreadLoopFlag = false;

        WaitForSingleObject(SdLoadThreadHandle, INFINITE);
        SdLoadThreadHandle = NULL;
    };

    if (SdLoadTaskQueueMutex)
    {
        SdMutexDestroy(SdLoadTaskQueueMutex);
        SdLoadTaskQueueMutex = nullptr;
    };

    /* load thread will exit BEFORE moving W queue into R queue - so R queue should be empty */
    ASSERT(SdQueueIsEmpty(SdLoadTaskQueueR) == true);

    while (!SdQueueIsEmpty(SdLoadTaskQueueW))
    {
        SdLoadTask_t* Task = (SdLoadTask_t*)SdQueueFront(SdLoadTaskQueueR);
        SdQueuePop(SdLoadTaskQueueR);

        if (Task->Handle)
        {
            CloseHandle(Task->Handle);
            Task->Handle = NULL;
            Task->State = true;
        };
    };

    /*
     *	Terminate sd loader queues
     */
    ASSERT(SdQueueIsEmpty(SdLoadTaskQueueW) == true);
    ASSERT(SdQueueIsEmpty(SdLoadTaskQueueR) == true);

    if (SdLoadTaskQueueW)
    {
        SdQueueDestroy(SdLoadTaskQueueW);
        SdLoadTaskQueueW = nullptr;
    };

    if (SdLoadTaskQueueR)
    {
        SdQueueDestroy(SdLoadTaskQueueR);
        SdLoadTaskQueueR = nullptr;
    };
};


void SdLoadTaskMain(void)
{
    ;
};


bool SdLoadTaskReadSync(const char* _path, void* _buffer, uint32 _size, uint32 _offset)
{
    bool bResult = false;

    if (!_path)
        return bResult;

    if (!_buffer)
        return bResult;

    HANDLE hFile = CreateFileA(_path, (GENERIC_READ | GENERIC_WRITE), FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        uint32 ReadSize = _size;
        uint32 FSizeHigh = 0;
        uint32 FSize = GetFileSize(hFile, LPDWORD(&FSizeHigh));

        if (!_size)
            ReadSize = FSize;

        if (SetFilePointer(hFile, LONG(_offset), 0, FILE_BEGIN) == _offset)
        {
            DWORD Readed = 0;
            if (ReadFile(hFile, _buffer, ReadSize, &Readed, NULL))
                bResult = true;
        }
        else
        {
            ASSERT(false);
        };

        CloseHandle(hFile);
    };

    return bResult;
};


void* SdLoadTaskReadAsync(const char* _path, void* _buffer, uint32 _size, uint32 _offset)
{
    SdLoadTask_t* Result = nullptr;

    if (!_path)
        return Result;

    if (!_buffer)
        return Result;

    HANDLE hFile = CreateFileA(_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        uint32 ReadSize = _size;
        uint32 FSizeHigh= 0;
        uint32 FSize    = GetFileSize(hFile, LPDWORD(&FSizeHigh));

        if (!_size)
            ReadSize = FSize;

        if (!FSizeHigh)
        {
            if (ReadSize > (FSize - _offset))
                ReadSize = (FSize - _offset);
        };

        if (SetFilePointer(hFile, LONG(_offset), 0, FILE_BEGIN) == _offset)
        {
            SdLoadTask_t* Task = &SdLoadTask[SdLoadTaskPos++ % COUNT_OF(SdLoadTask)];
            std::memset(Task, 0, sizeof(*Task));
            Task->Handle = hFile;
            std::strcpy(Task->Filepath, _path);
            Task->ReadAddr = _buffer;
            Task->ReadSize = ReadSize;
            Task->State = false;
            Task->cbReaded = 0;

            SdMutexLock(SdLoadTaskQueueMutex);
            if (!SdQueueIsFull(SdLoadTaskQueueW))
            {
                SdQueuePush(SdLoadTaskQueueW, &Task);
                Result = Task;
            };            
            SdMutexUnlock(SdLoadTaskQueueMutex);
        };
    };

#ifdef _DEBUG    
    if (!Result)
        ASSERT(false, "%u", GetLastError());
#endif /* _DEBUG */
    
    return Result;
};


bool SdLoadTaskIsReadEnd(void* _task)
{
    ASSERT(_task);
    
    return ((SdLoadTask_t*)_task)->State;
};


uint32 SdLoadTaskGetReadBytes(void* _task)
{
    ASSERT(_task);

    return ((SdLoadTask_t*)_task)->cbReaded;
};


void* SdLoadTaskGetReadAddr(void* _task)
{
    ASSERT(_task);

    return ((SdLoadTask_t*)_task)->ReadAddr;
};


void* SdLoadTaskGetUserParam(void* _task)
{
    ASSERT(_task);

    return ((SdLoadTask_t*)_task)->UserData;
};


void SdLoadTaskSetUserParam(void* _task, void* _user)
{
    ASSERT(_task);

    ((SdLoadTask_t*)_task)->UserData = _user;
};