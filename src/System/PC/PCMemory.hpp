#pragma once

#include "System/Common/Memory.hpp"


class CPCMemoryDefault final : public CMemory
{
public:
    CPCMemoryDefault(void);
    virtual ~CPCMemoryDefault(void);
    virtual void* RepAlloc(uint32 size, const char* fname = nullptr, int32 fline = 0) override;
    virtual void RepFree(void* mem) override;
    virtual void* RepRwAlloc(uint32 size, uint32 hint) override;
    virtual void RepRwFree(void* mem) override;
    virtual void* RepRwRealloc(void* mem, uint32 size, uint32 hint) override;
    virtual void* RepRwCalloc(uint32 objSize, uint32 size, uint32 hint) override;
    virtual uint32 AllocatedSize(void) override;

private:
    static CPCMemoryDefault m_instance;
};


class CPCMemory final : public CMemory
{
public:
    CPCMemory(void);
    virtual ~CPCMemory(void);
    virtual void* RepAlloc(uint32 size, const char* fname = nullptr, int32 fline = 0) override;
    virtual void RepFree(void* mem) override;
    virtual void* RepRwAlloc(uint32 size, uint32 hint) override;
    virtual void RepRwFree(void* mem) override;
    virtual void* RepRwRealloc(void* mem, uint32 size, uint32 hint) override;
    virtual void* RepRwCalloc(uint32 objSize, uint32 size, uint32 hint) override;
    virtual uint32 AllocatedSize(void) override;
    
private:
    int32 m_iMemBlock;
    int32 m_iRwMemBlock;
    void* m_hHeap;
    void* m_hRwHeap;
    CMemory* m_pPrev;
};
