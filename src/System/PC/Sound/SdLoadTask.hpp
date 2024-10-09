#pragma once


bool SdLoadTaskInit(void);
void SdLoadTaskTerm(void);
bool SdLoadTaskSync(const char* _path, void* _buffer, uint32 _size, uint32 _offset);
void* SdLoadTaskAsync(const char* _path, void* _buffer, uint32 _size, uint32 _offset);
bool SdLoadTaskIsEnd(void* _task);
uint32 SdLoadTaskReaded(void* _task);
void* SdLoadTaskAddress(void* _task);
void* SdLoadTaskGetUser(void* _task);
void SdLoadTaskSetUser(void* _task, void* _user);