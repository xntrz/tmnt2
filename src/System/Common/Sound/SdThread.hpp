#pragma once


typedef void(*SdThreadCallback_t)(void* _param);

typedef struct SdMutex_t SdMutex_t;


bool SdThreadInitialize(SdThreadCallback_t _cb, void* _param);
void SdThreadTerminate(void);
void SdThreadSetName(const char* _name);

SdMutex_t* SdMutexCreate(void);
void SdMutexDestroy(SdMutex_t* _mutex);
void SdMutexLock(SdMutex_t* _mutex);
void SdMutexUnlock(SdMutex_t* _mutex);