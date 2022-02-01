#pragma once


class CMemory
{
private:    
    static CMemory& Instance(void);
    
public:
    static void* _new(uint32 size, const char* fname = nullptr, int32 fline = 0);
    static void _delete(void* mem);
    static void* malloc(uint32 size);
    static void free(void* mem);
    static void* rwmalloc(uint32 size, uint32 hint);
    static void rwfree(void* mem);
    static void* rwrealloc(void* mem, uint32 size, uint32 hint);
    static void* rwcalloc(uint32 objSize, uint32 size, uint32 hint);
    
    CMemory(void);
    virtual ~CMemory(void);
    virtual void* RepAlloc(uint32 size, const char* fname = nullptr, int32 fline = 0) = 0;
    virtual void RepFree(void* mem) = 0;
    virtual void* RepRwAlloc(uint32 size, uint32 hint) = 0;
    virtual void RepRwFree(void* mem) = 0;
    virtual void* RepRwRealloc(void* mem, uint32 size, uint32 hint) = 0;
    virtual void* RepRwCalloc(uint32 objSize, uint32 size, uint32 hint) = 0;
    virtual uint32 AllocatedSize(void) = 0;

protected:
    static CMemory* m_pThis;
};