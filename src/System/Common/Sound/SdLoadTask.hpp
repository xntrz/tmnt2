#pragma once


bool SdLoadTaskInit(void);
void SdLoadTaskTerm(void);
void SdLoadTaskMain(void);
bool SdLoadTaskReadSync(const char* _path, void* _buffer, uint32 _size, uint32 _offset);
void* SdLoadTaskReadAsync(const char* _path, void* _buffer, uint32 _size, uint32 _offset);
bool SdLoadTaskIsReadEnd(void* _task);
uint32 SdLoadTaskGetReadBytes(void* _task);
void* SdLoadTaskGetReadAddr(void* _task);
void* SdLoadTaskGetUserParam(void* _task);
void SdLoadTaskSetUserParam(void* _task, void* _user);