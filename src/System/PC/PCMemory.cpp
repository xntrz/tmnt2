#include "PCMemory.hpp"
#include "PCTypedefs.hpp"


/*static*/ CPCMemoryDefault CPCMemoryDefault::m_instance;


CPCMemoryDefault::CPCMemoryDefault(void)
{
    m_pThis = this;
};


CPCMemoryDefault::~CPCMemoryDefault(void)
{
    m_pThis = nullptr;
};


void* CPCMemoryDefault::RepAlloc(uint32 size, const char* fname, int32 fline)
{
    return std::malloc(size);
};


void CPCMemoryDefault::RepFree(void* mem)
{
    std::free(mem);
};


void* CPCMemoryDefault::RepRwAlloc(uint32 size, uint32 hint)
{
    return std::malloc(size);
};


void CPCMemoryDefault::RepRwFree(void* mem)
{
    std::free(mem);
};


void* CPCMemoryDefault::RepRwRealloc(void* mem, uint32 size, uint32 hint)
{
    return std::realloc(mem, size);
};


void* CPCMemoryDefault::RepRwCalloc(uint32 objSize, uint32 size, uint32 hint)
{
    return std::calloc(objSize, size);
};


uint32 CPCMemoryDefault::AllocatedSize(void)
{
    return 0;
};


CPCMemory::CPCMemory(void)
: m_iMemBlock(0)
, m_iRwMemBlock(0)
, m_hHeap(NULL)
, m_hRwHeap(NULL)
, m_pPrev(nullptr)
{
    m_hHeap = HeapCreate(0, 0x800000, 0);
    m_hRwHeap = HeapCreate(0, 0x800000, 0);
    ASSERT(m_hHeap);
    ASSERT(m_hRwHeap);

    m_pPrev = m_pThis;
    m_pThis = this;
};


CPCMemory::~CPCMemory(void)
{
    m_pThis = m_pPrev;
    m_pPrev = nullptr;
    
    ASSERT(!m_iMemBlock);
    ASSERT(!m_iRwMemBlock);
    
    if (m_hHeap)
    {
        HeapDestroy(m_hHeap);
        m_hHeap = NULL;
    };

    if (m_hRwHeap)
    {
        HeapDestroy(m_hRwHeap);
        m_hRwHeap = NULL;
    };
};


void* CPCMemory::RepAlloc(uint32 size, const char* fname, int32 fline)
{
    void* pResult = HeapAlloc(m_hHeap, 0, size);
    if(pResult)
        ++m_iMemBlock;
    
    return pResult;
};


void CPCMemory::RepFree(void* mem)
{
    if(mem)
        --m_iMemBlock;
    
    HeapFree(m_hHeap, 0, mem);
};


void* CPCMemory::RepRwAlloc(uint32 size, uint32 hint)
{
    ASSERT(m_hRwHeap);
    
    void* pResult = HeapAlloc(m_hRwHeap, 0, size);
    if(pResult)
        ++m_iRwMemBlock;
    
    return pResult;
};


void CPCMemory::RepRwFree(void* mem)
{
    if(mem)
        --m_iRwMemBlock;
    
    HeapFree(m_hRwHeap, 0, mem);
};


void* CPCMemory::RepRwRealloc(void* mem, uint32 size, uint32 hint)
{
    ASSERT(m_hRwHeap);
    
    void* pResult = nullptr;

    if (mem)
    {
        if (size)
        {
            pResult = HeapReAlloc(m_hRwHeap, 0, mem, size);
        }
        else
        {
            RepRwFree(mem);
        };
    }
    else
    {
        ASSERT(!mem);
        ASSERT(size);

        pResult = RepRwAlloc(size, hint);
    };

    return pResult;
};


void* CPCMemory::RepRwCalloc(uint32 objSize, uint32 size, uint32 hint)
{
    return RepRwAlloc(objSize * size, hint);
};


uint32 CPCMemory::AllocatedSize(void)
{
    return 0;
};