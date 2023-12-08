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


void* SdMemAlloc(uint32 Size)
{
    void* ret = _aligned_malloc(Size, 4096);
    if (ret)
        ++SdMemAllocCnt;

    return ret;
};


void SdMemFree(void* Ptr)
{
    ASSERT(SdMemAllocCnt > 0);
    --SdMemAllocCnt;
    
    _aligned_free(Ptr);
};