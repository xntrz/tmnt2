#include "Memory.hpp"


/*static*/ CMemory* CMemory::m_pThis = nullptr;


/*static*/ CMemory& CMemory::Instance(void)
{
    return *m_pThis;
};


/*static*/ void* CMemory::malloc(size_t size, const char* fname, int fline)
{
    void* mem = Instance().RepAlloc(size, fname, fline);
#ifdef _DEBUG
    if (mem == nullptr)
    {
        FATAL("Memory allocation request failed!\n\n"
              "bytes: %" PRIuPTR "\n"
              "file: %s\n"
              "line: %" PRIi32 "\n",
              size,
              fname,
              fline);
    };
#endif
    return mem;
};


/*static*/ void CMemory::free(void* mem)
{
    Instance().RepFree(mem);
};


/*static*/ void* CMemory::rwmalloc(size_t size, uint32 hint)
{
    return Instance().RepRwAlloc(size, hint);
};


/*static*/ void CMemory::rwfree(void* mem)
{
	Instance().RepRwFree(mem);
};


/*static*/ void* CMemory::rwrealloc(void* mem, size_t size, uint32 hint)
{
    return Instance().RepRwRealloc(mem, size, hint);
};


/*static*/ void* CMemory::rwcalloc(size_t objSize, size_t size, uint32 hint)
{
    return Instance().RepRwCalloc(objSize, size, hint);
};