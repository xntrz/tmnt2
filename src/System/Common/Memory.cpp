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
        FATAL("Memory alloc request failed!\n\n"
              "bytes: %" PRIuPTR "\n"
              "file: %s\n"
              "line: %" PRIi32 "\n",
              size,
              fname,
              fline);
    };
#endif /* _DEBUG */
    return mem;
};


/*static*/ void CMemory::free(void* mem)
{
    Instance().RepFree(mem);
};


/*static*/ void* CMemory::rwmalloc(size_t size, uint32 hint)
{
    void* mem = Instance().RepRwAlloc(size, hint);
#ifdef _DEBUG
    if (mem == nullptr)
    {
        FATAL("Memory RW alloc request failed!\n\n"
              "bytes: %" PRIuPTR "\n"
              "hint: 0x%" PRIx32 "\n",
              size,
              hint);
    };
#endif /* _DEBUG */
    return mem;
};


/*static*/ void CMemory::rwfree(void* mem)
{
    Instance().RepRwFree(mem);
};


/*static*/ void* CMemory::rwrealloc(void* mem, size_t size, uint32 hint)
{
    mem = Instance().RepRwRealloc(mem, size, hint);
#ifdef _DEBUG
    if (mem == nullptr)
    {
        FATAL("Memory RW realloc request failed!\n\n"
              "bytes: %" PRIuPTR "\n"
              "hint: 0x%" PRIx32 "\n",
              size,
              hint);
    };
#endif /* _DEBUG */
    return mem;
};


/*static*/ void* CMemory::rwcalloc(size_t objSize, size_t size, uint32 hint)
{
    void* mem = Instance().RepRwCalloc(objSize, size, hint);
#ifdef _DEBUG
    if (mem == nullptr)
    {
        FATAL("Memory RW calloc request failed!\n\n"
              "bytes: %" PRIuPTR "\n"
              "hint: 0x%" PRIx32 "\n",
              size,
              hint);
    };
#endif /* _DEBUG */
    return mem;
};