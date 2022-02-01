#include "MemoryStd.hpp"
#include "Memory.hpp"

#include <new>


#ifdef new
#undef new
#endif


void* operator new(std::size_t size)
{
    return CMemory::_new(size, "unknown source file", 0);
};


void* operator new[](std::size_t size)
{
    return CMemory::_new(size, "unknown source file", 0);
};


void* operator new(std::size_t size, const char* fname, int32 fline)
{
    return CMemory::_new(size, fname, fline);
};


void* operator new[](std::size_t size, const char* fname, int32 fline)
{
    return CMemory::_new(size, fname, fline);
};


void operator delete(void* ptr)
{
    CMemory::_delete(ptr);
};


void operator delete [](void* ptr)
{
    CMemory::_delete(ptr);
};


//
//	warning C4291
//
void operator delete(void* ptr, const char* fname, int32 fline)
{
    CMemory::_delete(ptr);
};


//
//	warning C4291
//
void operator delete[](void* ptr, const char* fname, int32 fline)
{
    CMemory::_delete(ptr);
};


void* operator new(std::size_t size, const std::nothrow_t& nth)
{
    return CMemory::_new(size);
};


void* operator new [](std::size_t size, const std::nothrow_t& nth)
{
    return CMemory::_new(size);
};