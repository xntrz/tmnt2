#include "SdLoadTask.hpp"
#include "SdQueue.hpp"
#include "SdDrv.hpp"

#include "System/Web/File/WebFile.hpp"


struct SdLoadTask_t
{    
    bool		State;
    void*       ReadAddr;
    uint32		cbReaded;
    uint32      cbReadRequest;
    void*       UserData;
};


static SdLoadTask_t SdLoadTask[64];
static int32        SdLoadTaskPos = 0;


static void SdLoadTaskReadCallback(const uint8* data, size_t size, void* param)
{
    SdLoadTask_t* Task = (SdLoadTask_t*)param;

    ASSERT(Task->ReadAddr != 0);
    ASSERT(size <= Task->cbReadRequest,
          "requested %d, readed %d",
          Task->cbReadRequest, size);

    if (data && size)
    {
        std::memcpy(Task->ReadAddr, data, size);
        Task->cbReaded = size;
    };
    
    Task->State = true;
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

    char Filepath[256];
    std::strcpy(Filepath, _path);
    CWebFile::MakePath(Filepath);

    int32 readed = CWebFile::ReadSync(Filepath, _buffer, static_cast<size_t>(_size), static_cast<size_t>(_offset));

    return (readed > 0);
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

    char Filepath[256];
    std::strcpy(Filepath, _path);
    CWebFile::MakePath(Filepath);

    bool Result = CWebFile::ReadAsync(Filepath,
                                      static_cast<size_t>(_size),
                                      static_cast<size_t>(_offset),
                                      &SdLoadTaskReadCallback,
                                      Task);

    if (!Result)
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