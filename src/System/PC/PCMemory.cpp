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


void* CPCMemoryDefault::RepAlloc(size_t size, const char* fname, int fline)
{
    return std::malloc(size);
};


void CPCMemoryDefault::RepFree(void* mem)
{
    std::free(mem);
};


void* CPCMemoryDefault::RepRwAlloc(size_t size, uint32 hint)
{
    return std::malloc(size);
};


void CPCMemoryDefault::RepRwFree(void* mem)
{
    std::free(mem);
};


void* CPCMemoryDefault::RepRwRealloc(void* mem, size_t size, uint32 hint)
{
    return std::realloc(mem, size);
};


void* CPCMemoryDefault::RepRwCalloc(size_t objSize, size_t size, uint32 hint)
{
    return std::calloc(objSize, size);
};


size_t CPCMemoryDefault::AllocatedSize(void)
{
    return 0;
};


//
// *********************************************************************************
//


CPCMemory::CPCMemory(void)
: m_iMemBlock(0)
, m_iRwMemBlock(0)
, m_hHeap(NULL)
, m_hRwHeap(NULL)
, m_pPrev(nullptr)
{
    const SIZE_T memSizeInit = (1024 * 1024) * 8; // 8 MB
    const SIZE_T memSizeMax = 0;
    const DWORD flags = HEAP_NO_SERIALIZE;
    
    m_hHeap = HeapCreate(flags, memSizeInit, memSizeMax);
    ASSERT(m_hHeap != NULL);

    m_hRwHeap = HeapCreate(flags, memSizeInit, memSizeMax);
    ASSERT(m_hRwHeap != NULL);

    m_pPrev = m_pThis;
    m_pThis = this;
};


CPCMemory::~CPCMemory(void)
{
    ASSERT(m_iMemBlock == 0);
    ASSERT(m_iRwMemBlock == 0);

    m_pThis = m_pPrev;
    m_pPrev = nullptr;

    if (m_hRwHeap)
    {
        HeapDestroy(m_hRwHeap);
        m_hRwHeap = NULL;
    };

    if (m_hHeap)
    {
        HeapDestroy(m_hHeap);
        m_hHeap = NULL;
    };
};


void* CPCMemory::RepAlloc(size_t size, const char* fname, int32 fline)
{
    void* pResult = HeapAlloc(m_hHeap, 0, size);
    if (pResult)
    {
        ++m_iMemBlock;
    };
    
    return pResult;
};


void CPCMemory::RepFree(void* mem)
{
    if (mem)
    {
        --m_iMemBlock;
    };
    
    HeapFree(m_hHeap, 0, mem);
};


void* CPCMemory::RepRwAlloc(size_t size, uint32 hint)
{
    ASSERT(m_hRwHeap);
    
    void* pResult = HeapAlloc(m_hRwHeap, 0, size);
    if (pResult)
    {
        ++m_iRwMemBlock;
    };
    
    return pResult;
};


void CPCMemory::RepRwFree(void* mem)
{
    if (mem)
    {
        --m_iRwMemBlock;
    };
    
    HeapFree(m_hRwHeap, 0, mem);
};


void* CPCMemory::RepRwRealloc(void* mem, size_t size, uint32 hint)
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


void* CPCMemory::RepRwCalloc(size_t objSize, size_t size, uint32 hint)
{
    void* mem = RepRwAlloc(objSize * size, hint);
    if (mem)
    {
        std::memset(mem, 0, objSize * size);
    };

    return mem;
};


size_t CPCMemory::AllocatedSize(void)
{
    return  0;
};


//
// *********************************************************************************
//


CPCMemoryPool::CPCMemoryPool(void)
: m_pool()
, m_pPrev(nullptr)
{
    m_pPrev = m_pThis;
    m_pThis = this;

    const size_t size = (1024 * 1024) * 64;
    void* mem = std::malloc(size);

    CMemPool::CreateAndAttach(m_pool, mem, size, 8, 5);
};


CPCMemoryPool::~CPCMemoryPool(void)
{
    void* mem = CMemPool::DetachAndDestroy(m_pool);
    std::free(mem);

    m_pThis = m_pPrev;
    m_pPrev = nullptr;
};


void* CPCMemoryPool::RepAlloc(size_t size, const char* fname, int fline)
{
    return m_pool.AllocFixed(size);
};


void CPCMemoryPool::RepFree(void* mem)
{
    m_pool.FreeFixed(mem);
};


void* CPCMemoryPool::RepRwAlloc(size_t size, uint32 hint)
{
    return m_pool.AllocUpper(size);
};


void CPCMemoryPool::RepRwFree(void* mem)
{
    m_pool.FreeUpper(mem);
};


void* CPCMemoryPool::RepRwRealloc(void* mem, size_t size, uint32 hint)
{
    if (mem)
    {
        if (size)
        {
            void* memNew = m_pool.AllocUpper(size);
            if (memNew)
            {
                if (m_pool.GetSizeFixed(mem) < size)
                    size = m_pool.GetSizeFixed(mem);

                std::memcpy(memNew, mem, size);
                m_pool.FreeUpper(mem);

                mem = memNew;
            };
        }
        else
        {
            m_pool.FreeUpper(mem);
            mem = nullptr;
        };
    }
    else
    {
        mem = m_pool.AllocUpper(size);
    };

    return mem;
};


void* CPCMemoryPool::RepRwCalloc(size_t objSize, size_t size, uint32 hint)
{
    void* mem = RepRwAlloc(objSize * size, hint);
    if (mem)
        std::memset(mem, 0, objSize * size);

    return mem;
};


size_t CPCMemoryPool::AllocatedSize(void)
{
    return m_pool.GetAllocatedSize() + m_pool.GetAllocatedUpperSize();
};

