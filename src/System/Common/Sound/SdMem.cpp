#include "SdMem.hpp"


static int32 SdMemAllocCnt = 0;


void SdMemInitialize(void)
{

};


void SdMemTerminate(void)
{
    ASSERT(SdMemAllocCnt == 0);
};


void* SdMemAlloc(size_t _size)
{
    void* ret = std::malloc(_size);
    if (ret)
        ++SdMemAllocCnt;

    return ret;
};


void SdMemFree(void* _mem)
{
    ASSERT(SdMemAllocCnt > 0);
    --SdMemAllocCnt;
    
    std::free(_mem);
};