#include "SdLoadTask.hpp"
#include "SdQueue.hpp"
#include "SdDrv.hpp"

#include "System/Web/File/WebFileManager.hpp"

#include "rtfsyst.h"

#include <emscripten.h>


struct SdLoadTask_t
{
    char path[256];
    bool		State;
    void*       ReadAddr;
    uint32		cbReaded;
    uint32      cbReadRequest;
    void*       UserData;
    void*       FileHandle;
    uint32      ReadOffset;
};


static SdLoadTask_t SdLoadTask[64];
static int32        SdLoadTaskPos = 0;


static void SdLoadTaskFinish(SdLoadTask_t* Task, uint32 Bytes)
{
    if (Task->FileHandle)
    {
        RwFclose(Task->FileHandle);
        Task->FileHandle = nullptr;
    };

    Task->cbReaded = Bytes;
    Task->State = true;
};


static void SdLoadTaskFileOpCallback(void* file, RwUInt32 size, RwUInt32 status,
                                     RtFSFileOperation fileOp, void* data)
{
    SdLoadTask_t* Task = (SdLoadTask_t*)data;

    switch (fileOp)
    {
    case RTFS_FILEOP_OPEN:
        {
            if (RtFileSystemGetFileSize(file) == 0)
            {
                SdLoadTaskFinish(Task, size);
                break;
            };

            RwFseek(Task->FileHandle, static_cast<long>(Task->ReadOffset), RTFILE_POS_BEGIN);
            RwFread(Task->ReadAddr, static_cast<size_t>(Task->cbReadRequest), 1, Task->FileHandle);
        }
        break;

    case RTFS_FILEOP_SEEK:
        break;

    case RTFS_FILEOP_READ:
        {
            SdLoadTaskFinish(Task, size);
        }
        break;

    default:
        {
            SdLoadTaskFinish(Task, 0);
        }
        break;
    };
};


bool SdLoadTaskInit(void)
{
    return true;
};


void SdLoadTaskTerm(void)
{
    ;
};


void SdLoadTaskMain(void)
{
    ;
};


bool SdLoadTaskReadSync(const char* _path, void* _buffer, uint32 _size, uint32 _offset)
{
    if (!_path)
        return false;

    if (!_buffer)
        return false;

    void* Task = SdLoadTaskReadAsync(_path, _buffer, _size, _offset);
    if (Task)
    {
        while (!SdLoadTaskIsReadEnd(Task))
            CWebFileManager::Instance().SyncWait();

        return (SdLoadTaskGetReadBytes(Task) > 0);
    };

    return false;
};


void* SdLoadTaskReadAsync(const char* _path, void* _buffer, uint32 _size, uint32 _offset)
{
    if (!_path)
        return nullptr;

    if (!_buffer)
        return nullptr;

    SdLoadTask_t* Task = &SdLoadTask[SdLoadTaskPos++ % COUNT_OF(SdLoadTask)];
    std::memset(Task, 0, sizeof(*Task));
    Task->ReadAddr = _buffer;
    Task->State = false;
    Task->cbReadRequest = _size;
    Task->cbReaded = 0;
    Task->ReadOffset = _offset;
    strcpy(Task->path, CWebFileManager::MakePath(_path));

    Task->FileHandle = RtFSManagerFOpen(CWebFileManager::MakePath(_path), 
                                        RTFILE_ACCESS_OPEN_READ,
                                        SdLoadTaskFileOpCallback, Task);

    if (!Task->FileHandle)
        return nullptr;

    return Task;
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