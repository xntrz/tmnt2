#pragma once

#include "System/Common/Memory.hpp"
#include "System/Common/MemPool.hpp"


class CWebMemory : public CMemory
{
public:
    //static void* calloc(size_t objSize, size_t size, const char* fname, int fline);
    //static void* realloc(void* mem, size_t size, const char* fname, int fline);

    CWebMemory(void) {};
    virtual ~CWebMemory(void) {};
    //virtual void* RepCalloc(size_t objSize, size_t size, const char* fname, int fline) = 0;
    //virtual void* RepRealloc(void* mem, size_t size, const char* fname, int fline) = 0;
};


class CWebMemoryDefault final : public CWebMemory
{
public:
    CWebMemoryDefault(void);
    virtual ~CWebMemoryDefault(void);
    virtual void* RepAlloc(size_t size, const char* fname, int fline) override;
    //virtual void* RepCalloc(size_t objSize, size_t size, const char* fname, int fline) override;
    //virtual void* RepRealloc(void* mem, size_t size, const char* fname, int fline) override;
    virtual void RepFree(void* mem) override;
    virtual void* RepRwAlloc(size_t size, uint32 hint) override;
    virtual void RepRwFree(void* mem) override;
    virtual void* RepRwRealloc(void* mem, size_t size, uint32 hint) override;
    virtual void* RepRwCalloc(size_t objSize, size_t size, uint32 hint) override;
    virtual size_t AllocatedSize(void) override;

private:
    static CWebMemoryDefault m_instance;
    CMemory* m_pPrev;
};


class CWebMemoryPool final : public CWebMemory
{
public:
    CWebMemoryPool(void);
    virtual ~CWebMemoryPool(void);
    virtual void* RepAlloc(size_t size, const char* fname, int fline) override;
    //virtual void* RepCalloc(size_t objSize, size_t size, const char* fname, int fline) override;
    //virtual void* RepRealloc(void* mem, size_t size, const char* fname, int fline) override;
    virtual void RepFree(void* mem) override;
    virtual void* RepRwAlloc(size_t size, uint32 hint) override;
    virtual void RepRwFree(void* mem) override;
    virtual void* RepRwRealloc(void* mem, size_t size, uint32 hint) override;
    virtual void* RepRwCalloc(size_t objSize, size_t size, uint32 hint) override;
    virtual size_t AllocatedSize(void) override;

private:
    CMemPool m_pool;
    CMemory* m_pPrev;
};