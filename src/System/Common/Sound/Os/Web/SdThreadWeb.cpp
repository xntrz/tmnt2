#include "SdThread.hpp"
#include "SdMem.hpp"


typedef struct SdMutex_t
{
    void* dummy;
} SdMutex_t;


bool SdThreadInitialize(SdThreadCallback_t _cb, void* _param)
{
    return true;
};


void SdThreadTerminate(void)
{
    ;
};


void SdThreadSetName(const char* _name)
{
    ;
};


SdMutex_t* SdMutexCreate(void)
{
    SdMutex_t* SdMutex = (SdMutex_t*)SdMemAlloc(sizeof(SdMutex_t));
    return SdMutex;
};


void SdMutexDestroy(SdMutex_t* _mutex)
{
    SdMemFree(_mutex);
};


void SdMutexLock(SdMutex_t* _mutex)
{
    ;
};


void SdMutexUnlock(SdMutex_t* _mutex)
{
    ;
};