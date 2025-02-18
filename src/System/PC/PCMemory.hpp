#pragma once

#include "System/Common/Memory.hpp"


class CPCMemoryDefault final : public CMemory
{
public:
    CPCMemoryDefault(void);
    virtual ~CPCMemoryDefault(void);
    virtual void* RepAlloc(size_t size, const char* fname, int fline) override;
    virtual void RepFree(void* mem) override;
    virtual void* RepRwAlloc(size_t size, uint32 hint) override;
    virtual void RepRwFree(void* mem) override;
    virtual void* RepRwRealloc(void* mem, size_t size, uint32 hint) override;
    virtual void* RepRwCalloc(size_t objSize, size_t size, uint32 hint) override;
    virtual size_t AllocatedSize(void) override;

private:
    static CPCMemoryDefault m_instance;
};


class CPCMemory final : public CMemory
{
public:
    CPCMemory(void);
    virtual ~CPCMemory(void);
    virtual void* RepAlloc(size_t size, const char* fname, int fline) override;
    virtual void RepFree(void* mem) override;
    virtual void* RepRwAlloc(size_t size, uint32 hint) override;
    virtual void RepRwFree(void* mem) override;
    virtual void* RepRwRealloc(void* mem, size_t size, uint32 hint) override;
    virtual void* RepRwCalloc(size_t objSize, size_t size, uint32 hint) override;
    virtual size_t AllocatedSize(void) override;
    
private:
    int32 m_iMemBlock;
    int32 m_iRwMemBlock;
    void* m_hHeap;
    void* m_hRwHeap;
    CMemory* m_pPrev;
};
