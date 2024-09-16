#include "MemoryStd.hpp"
#include "Memory.hpp"

#include <new>


#ifdef new
#undef new
#endif

#define ALLOC_FNAME_INIT (nullptr)
#define ALLOC_FLINE_INIT (0)


static const char* s_lastAllocFname = ALLOC_FNAME_INIT;
static int s_lastAllocFline = ALLOC_FLINE_INIT;


/*extern*/ void memory_std_set_alloc_source(const char* fname, int fline)
{
	s_lastAllocFname = fname;
	s_lastAllocFline = fline;
};


struct LAST_ALLOC_CLEANUP
{
	inline LAST_ALLOC_CLEANUP() {};
	inline ~LAST_ALLOC_CLEANUP() { s_lastAllocFname = ALLOC_FNAME_INIT; s_lastAllocFline = ALLOC_FLINE_INIT; };
};


void* operator new(std::size_t size)
{
	LAST_ALLOC_CLEANUP obj;
	return CMemory::_new(size, s_lastAllocFname, s_lastAllocFline);
};


void* operator new[](std::size_t size)
{
	LAST_ALLOC_CLEANUP obj;
	return CMemory::_new(size, s_lastAllocFname, s_lastAllocFline);
};


void operator delete(void* ptr)
{
    CMemory::_delete(ptr);
};


void operator delete[](void* ptr)
{
    CMemory::_delete(ptr);
};


void* operator new(std::size_t size, const std::nothrow_t& nth) noexcept
{
	LAST_ALLOC_CLEANUP obj;
	return CMemory::_new(size, s_lastAllocFname, s_lastAllocFline);
};


void* operator new[](std::size_t size, const std::nothrow_t& nth) noexcept
{
	LAST_ALLOC_CLEANUP obj;
	return CMemory::_new(size, s_lastAllocFname, s_lastAllocFline);
};