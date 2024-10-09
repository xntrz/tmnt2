#include "SdMem.hpp"


static int32 SdMemAllocCnt = 0;


void SdMemInitialize(void)
{
    SdMemAllocCnt = 0;
};


void SdMemTerminate(void)
{
    ASSERT(SdMemAllocCnt == 0);
};


void* SdMemAlloc(size_t _size)
{
    void* ret = _aligned_malloc(_size, 4096);
    if (ret)
        ++SdMemAllocCnt;

    return ret;
};


void SdMemFree(void* _mem)
{
    ASSERT(SdMemAllocCnt > 0);
    --SdMemAllocCnt;
    
    _aligned_free(_mem);
};