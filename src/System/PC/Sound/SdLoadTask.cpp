#include "SdLoadTask.hpp"
#include "SdQueue.hpp"
#include "SdDrv.hpp"

#include "../PCTypedefs.hpp"


struct SdLoadTask_t
{
    HANDLE		Handle;
    void*		ReadAddr;
    uint32		ReadSize;
    const char*	Filename;
    bool		State;
    uint32		cbReaded;
    void*       UserData;
};


static bool             SdLoadThreadLoopFlag = false;
static HANDLE           SdLoadThreadHandle = NULL;
static uint32           SdLoadLastError = 0;
static SdLoadTask_t     SdLoadTask[256];
static int32            SdLoadTaskIn = 0;
static int32            SdLoadTaskOut = 0;


static DWORD WINAPI SdLoadThreadProc(void* pParam)
{
    (void)pParam;

#ifdef _DEBUG    
    SdDrvThreadSetName("SoundLoadThread");
#endif
    
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    SdLoadThreadLoopFlag = true;

    while (SdLoadThreadLoopFlag)
    {
        Sleep(10);

        while ((SdLoadTaskOut != SdLoadTaskIn) && SdLoadThreadLoopFlag)
        {
            SdLoadTask_t* Task = &SdLoadTask[SdLoadTaskOut++];
            
            if (SdLoadTaskOut >= COUNT_OF(SdLoadTask))
                SdLoadTaskOut = 0;
            
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
    SdLoadThreadHandle = CreateThread(NULL, 0, SdLoadThreadProc, NULL, NULL, NULL);
    if (SdLoadThreadHandle == NULL)
        return false;

    return true;
};


void SdLoadTaskTerm(void)
{
    if (SdLoadThreadHandle)
    {
        SdLoadThreadLoopFlag = false;

        WaitForSingleObject(SdLoadThreadHandle, INFINITE);
        SdLoadThreadHandle = NULL;
    };

    for (int32 i = 0; i < COUNT_OF(SdLoadTask); ++i)
    {
        SdLoadTask_t* Task = &SdLoadTask[i];

        if (Task->Handle)
        {
            CloseHandle(Task->Handle);
            Task->Handle = NULL;
            Task->State = true;
        };
    };
};


bool SdLoadTaskSync(const char* Path, void* Buffer, uint32 BufferSize, uint32 Offset)
{
    bool bResult = false;

    if (!Path)
        return bResult;

    if (!Buffer)
        return bResult;

    HANDLE hFile = CreateFileA(Path, (GENERIC_READ | GENERIC_WRITE), FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        uint32 ReadSize = BufferSize;

        uint32 FSizeHigh = 0;
        uint32 FSize = GetFileSize(hFile, LPDWORD(&FSizeHigh));

        if (!BufferSize)
            ReadSize = FSize;

        if (SetFilePointer(hFile, LONG(Offset), 0, FILE_BEGIN) == Offset)
        {
            DWORD Readed = 0;
            if (ReadFile(hFile, Buffer, ReadSize, &Readed, NULL))
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


void* SdLoadTaskAsync(const char* Path, void* Buffer, uint32 BufferSize, uint32 Offset)
{
    SdLoadTask_t* Result = nullptr;

    if (!Path)
        return Result;

    if (!Buffer)
        return Result;

    if ((SdLoadTaskIn + 1) == SdLoadTaskOut)
        return Result;

    HANDLE hFile = CreateFileA(Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        uint32 ReadSize = BufferSize;
        uint32 FSizeHigh= 0;
        uint32 FSize    = GetFileSize(hFile, LPDWORD(&FSizeHigh));

        if (!BufferSize)
            ReadSize = FSize;

        if (!FSizeHigh)
        {
            if (ReadSize > (FSize - Offset))
                ReadSize = (FSize - Offset);
        };

        if (SetFilePointer(hFile, LONG(Offset), 0, FILE_BEGIN) == Offset)
        {
            SdLoadTask_t* Task = &SdLoadTask[SdLoadTaskIn];

            std::memset(Task, 0x00, sizeof(*Task));
            Task->Handle = hFile;
            Task->Filename = Path;
            Task->ReadAddr = Buffer;
            Task->ReadSize = ReadSize;
            Task->State = false;
            Task->cbReaded = 0;

            SdLoadTaskIn = (SdLoadTaskIn + 1 >= COUNT_OF(SdLoadTask) ? 0 : SdLoadTaskIn + 1);

            Result = Task;
        };
    };

#ifdef _DEBUG    
    if (!Result)
		ASSERT(false, "%u", GetLastError());
#endif
    
    return Result;
};


bool SdLoadTaskIsEnd(void* hTask)
{
    ASSERT(hTask);
    
    return ((SdLoadTask_t*)hTask)->State;
};


uint32 SdLoadTaskReaded(void* hTask)
{
    ASSERT(hTask);

    return ((SdLoadTask_t*)hTask)->cbReaded;
};


void* SdLoadTaskAddress(void* hTask)
{
    ASSERT(hTask);

    return ((SdLoadTask_t*)hTask)->ReadAddr;
};


void* SdLoadTaskGetUser(void* hTask)
{
    ASSERT(hTask);

    return ((SdLoadTask_t*)hTask)->UserData;
};


void SdLoadTaskSetUser(void* hTask, void* UserData)
{
    ASSERT(hTask);

    ((SdLoadTask_t*)hTask)->UserData = UserData;
};