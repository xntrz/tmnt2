#pragma once


bool SdLoadTaskInit(void);
void SdLoadTaskTerm(void);
bool SdLoadTaskSync(const char* Path, void* Buffer, uint32 BufferSize, uint32 Offset);
void* SdLoadTaskAsync(const char* Path, void* Buffer, uint32 BufferSize, uint32 Offset);
bool SdLoadTaskIsEnd(void* hTask);
uint32 SdLoadTaskReaded(void* hTask);
void* SdLoadTaskAddress(void* hTask);
void* SdLoadTaskGetUser(void* hTask);
void SdLoadTaskSetUser(void* hTask, void* UserData);