#include "ProcessMemory.hpp"


CProcessMemory::CProcessMemory(void)
{
    ;
};


void* CProcessMemory::Alloc(uint32 size)
{
    return new int8[size];
};


void CProcessMemory::Free(void* mem)
{
    delete[] mem;
};


CProcessMemory::~CProcessMemory(void)
{
    ;
};