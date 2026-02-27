#include "ProcessMemory.hpp"

#include "System/Common/Memory.hpp"


CProcessMemory::CProcessMemory(void)
{
    ;
};


void* CProcessMemory::Alloc(size_t size)
{
    return CMemory::malloc(size, __FILE__, __LINE__);
};


void CProcessMemory::Free(void* mem)
{
    CMemory::free(mem);
};


CProcessMemory::~CProcessMemory(void)
{
    ;
};