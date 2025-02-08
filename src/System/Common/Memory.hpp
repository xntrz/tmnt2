#pragma once


class CMemory
{
private:    
	static CMemory& Instance(void);
    
public:
	static void* malloc(size_t size, const char* fname, int fline);
    static void free(void* mem);
	static void* rwmalloc(size_t size, uint32 hint);
    static void rwfree(void* mem);
    static void* rwrealloc(void* mem, size_t size, uint32 hint);
	static void* rwcalloc(size_t objSize, size_t size, uint32 hint);
	
	inline CMemory(void) {};
	inline virtual ~CMemory(void) {};
    virtual void* RepAlloc(size_t size, const char* fname, int fline) = 0;
    virtual void RepFree(void* mem) = 0;
    virtual void* RepRwAlloc(size_t size, uint32 hint) = 0;
    virtual void RepRwFree(void* mem) = 0;
    virtual void* RepRwRealloc(void* mem, size_t size, uint32 hint) = 0;
    virtual void* RepRwCalloc(size_t objSize, size_t size, uint32 hint) = 0;
    virtual size_t AllocatedSize(void) = 0;

protected:
    static CMemory* m_pThis;
};