#include "Memory.hpp"


/*static*/ CMemory* CMemory::m_pThis = nullptr;


/*static*/ CMemory& CMemory::Instance(void)
{
    return *m_pThis;
};


/*static*/ void* CMemory::_new(uint32 size, const char* fname, int32 fline)
{
    return Instance().RepAlloc(size, fname, fline);
};


/*static*/ void CMemory::_delete(void* mem)
{
    Instance().RepFree(mem);
};


/*static*/ void* CMemory::malloc(uint32 size)
{
    return Instance().RepAlloc(size);
};


/*static*/ void CMemory::free(void* mem)
{
    Instance().RepFree(mem);
};


/*static*/ void* CMemory::rwmalloc(uint32 size, uint32 hint)
{
    return Instance().RepRwAlloc(size, hint);
};


/*static*/ void CMemory::rwfree(void* mem)
{
	Instance().RepRwFree(mem);
};


/*static*/ void* CMemory::rwrealloc(void* mem, uint32 size, uint32 hint)
{
    return Instance().RepRwRealloc(mem, size, hint);
};


/*static*/ void* CMemory::rwcalloc(uint32 objSize, uint32 size, uint32 hint)
{
    return Instance().RepRwCalloc(objSize, size, hint);
};


CMemory::CMemory(void)
{
    ;
};


CMemory::~CMemory(void)
{
    ;
};