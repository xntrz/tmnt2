#include "WebMemory.hpp"

#include <malloc.h>
#include <emscripten/emmalloc.h>


///*static*/ void* CWebMemory::calloc(size_t objSize, size_t size, const char* fname, int fline)
//{
//    return static_cast<CWebMemory*>(m_pThis)->RepCalloc(objSize, size, fname, fline);
//};
//
//
///*static*/ void* CWebMemory::realloc(void* mem, size_t size, const char* fname, int fline)
//{
//    return static_cast<CWebMemory*>(m_pThis)->RepRealloc(mem, size, fname, fline);
//};


//
// *********************************************************************************
//


/*static*/ CWebMemoryDefault CWebMemoryDefault::m_instance;


CWebMemoryDefault::CWebMemoryDefault(void)
: m_pPrev(nullptr)
{
    m_pPrev = m_pThis;
    m_pThis = this;
};


CWebMemoryDefault::~CWebMemoryDefault(void)
{
    m_pThis = m_pPrev;
    m_pPrev = nullptr;
};


void* CWebMemoryDefault::RepAlloc(size_t size, const char* fname, int fline)
{
    void* mem = std::malloc(size);

    return mem;
};


//void* CWebMemoryDefault::RepCalloc(size_t objSize, size_t size, const char* fname, int fline)
//{
//    return std::calloc(objSize, size);
//};
//
//
//void* CWebMemoryDefault::RepRealloc(void* mem, size_t size, const char* fname, int fline)
//{
//    return std::realloc(mem, size);
//};


void CWebMemoryDefault::RepFree(void* mem)
{
    std::free(mem);
};


void* CWebMemoryDefault::RepRwAlloc(size_t size, uint32 hint)
{
    void* mem = std::malloc(size);

    return mem;
};


void CWebMemoryDefault::RepRwFree(void* mem)
{    
    std::free(mem);
};


void* CWebMemoryDefault::RepRwRealloc(void* mem, size_t size, uint32 hint)
{
    mem = std::realloc(mem, size);

    return mem;
};


void* CWebMemoryDefault::RepRwCalloc(size_t objSize, size_t size, uint32 hint)
{
    void* mem = std::calloc(objSize, size);

    return mem;
};


size_t CWebMemoryDefault::AllocatedSize(void)
{
    struct mallinfo mi = mallinfo();
    
    return mi.uordblks;
};


//
// *********************************************************************************
//


CWebMemoryPool::CWebMemoryPool(void)
: m_pool()
, m_pPrev(nullptr)
{
    m_pPrev = m_pThis;
    m_pThis = this;

    const size_t size = (1024 * 1024) * 48;
    const size_t alignment = 16;
    const size_t blockPercent = 5;

    void* mem = std::malloc(size);
    std::memset(mem, 0, size);

    CMemPool::CreateAndAttach(m_pool, mem, size, alignment, blockPercent);
};


CWebMemoryPool::~CWebMemoryPool(void)
{
    void* mem = CMemPool::DetachAndDestroy(m_pool);
    std::free(mem);

    m_pThis = m_pPrev;
    m_pPrev = nullptr;
};


void* CWebMemoryPool::RepAlloc(size_t size, const char* fname, int fline)
{
    return m_pool.AllocFixed(size);
};


//void* CWebMemoryPool::RepCalloc(size_t objSize, size_t size, const char* fname, int fline)
//{
//    void* mem = RepAlloc(objSize * size, fname, fline);
//    if (mem)
//        std::memset(mem, 0, objSize * size);
//
//    return mem;
//};
//
//
//void* CWebMemoryPool::RepRealloc(void* mem, size_t size, const char* fname, int fline)
//{
//    if (mem)
//    {
//        if (size)
//        {
//            void* memNew = m_pool.AllocFixed(size);
//            if (memNew)
//            {
//                if (m_pool.GetSizeFixed(mem) < size)
//                    size = m_pool.GetSizeFixed(mem);
//
//                std::memcpy(memNew, mem, size);
//                m_pool.FreeFixed(mem);
//
//                mem = memNew;
//            };
//        }
//        else
//        {
//            m_pool.FreeFixed(mem);
//            mem = nullptr;
//        };
//    }
//    else
//    {
//        mem = m_pool.AllocFixed(size);
//    };
//
//    return mem;
//};


void CWebMemoryPool::RepFree(void* mem)
{
    m_pool.FreeFixed(mem);
};


void* CWebMemoryPool::RepRwAlloc(size_t size, uint32 hint)
{
    return m_pool.AllocUpper(size);
};


void CWebMemoryPool::RepRwFree(void* mem)
{
    m_pool.FreeUpper(mem);
};


void* CWebMemoryPool::RepRwRealloc(void* mem, size_t size, uint32 hint)
{
    if (mem)
    {
        if (size)
        {
            void* memNew = m_pool.AllocUpper(size);
            if (memNew)
            {
                size_t sizeOld = m_pool.GetSizeFixed(mem);
                if (sizeOld < size)
                    size = sizeOld;

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


void* CWebMemoryPool::RepRwCalloc(size_t objSize, size_t size, uint32 hint)
{
    void* mem = RepRwAlloc(objSize * size, hint);
    
    if (mem)
        std::memset(mem, 0, objSize * size);

    return mem;
};


size_t CWebMemoryPool::AllocatedSize(void)
{
    struct mallinfo mi = mallinfo();

    return mi.uordblks + m_pool.GetAllocatedSize() + m_pool.GetAllocatedUpperSize();
};