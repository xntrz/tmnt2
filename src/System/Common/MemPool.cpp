#include "MemPool.hpp"


/*static*/ size_t
CMemPool::Align(size_t value, size_t align)
{
    return (value + (align - 1)) & ~(align - 1);
};


/*static*/ size_t
CMemPool::GetBlockMaxCount(size_t memSize)
{
    return (memSize - sizeof(MEMPOOL)) / sizeof(MEMBLK);
};


/*static*/ void
CMemPool::CreateAndAttach(CMemPool& pool, void* mem, size_t size,
                          size_t alignment, size_t blockPercent)
{
    uintptr_t memblkStart = reinterpret_cast<uintptr_t>(mem);
    size_t memblkMemSize = (size * blockPercent) / 100;
    size_t memblkCount = GetBlockMaxCount(memblkMemSize);

    pool.Create(reinterpret_cast<void*>(memblkStart), memblkCount, alignment);

    uintptr_t memStart = memblkStart + memblkMemSize;
    memStart = Align(memStart, alignment);

    memblkMemSize = (memStart - memblkStart);

    size_t memSize = (size - memblkMemSize);
    memSize = ((memSize / alignment) * alignment);

    pool.AttachMemory(reinterpret_cast<void*>(memStart), memSize);
};


/*static*/ void*
CMemPool::DetachAndDestroy(CMemPool& pool)
{
    pool.DetachMemory();
    return pool.Destroy();
};


CMemPool::CMemPool(void)
: m_pMemPool(nullptr)
{
	;
};


CMemPool::~CMemPool(void)
{
	;
};


void CMemPool::Create(void* memblkPoolAddr, size_t memblkCount, size_t alignment)
{
    m_pMemPool = reinterpret_cast<MEMPOOL*>(memblkPoolAddr);
	m_pMemPool->align = alignment;
	m_pMemPool->memblkCount = memblkCount;
	m_pMemPool->memblkPool = reinterpret_cast<MEMBLK*>(reinterpret_cast<uintptr_t>(m_pMemPool) + sizeof(MEMPOOL));
	m_pMemPool->addr = nullptr;
	m_pMemPool->size = 0;
	m_pMemPool->sizeUpper = 0;
	m_pMemPool->memblkFirstFreeEnd = nullptr;

    initMemBlkList(&m_pMemPool->listMemblkNone, MEMBLK::STATE_NONE);
    initMemBlkList(&m_pMemPool->listMemblkFree, MEMBLK::STATE_FREE);
    initMemBlkList(&m_pMemPool->listMemblkAlloc, MEMBLK::STATE_ALLOC);

	for (int32 i = (m_pMemPool->memblkCount - 1); i >= 0; --i)
	{
        MEMBLK* memblk = getMemBlk(i);
        
        setMemBlk(memblk, MEMBLK::STATE_NONE, nullptr, 0);
		addMemBlk(&m_pMemPool->listMemblkNone, memblk);
	};
};


void* CMemPool::Destroy(void)
{
	return m_pMemPool;
};


void CMemPool::AttachMemory(void* addr, size_t size)
{
	m_pMemPool->addr = addr;
	m_pMemPool->size = size;

    MEMBLK* memblk = getMemblkListHead(&m_pMemPool->listMemblkNone);
    
    remMemBlk(memblk);
    setMemBlk(memblk, MEMBLK::STATE_FREE, addr, size);
	addFreeLinkList(memblk);
};


void* CMemPool::DetachMemory(void)
{
	Output();

	if (m_pMemPool->listMemblkAlloc.next)
        return nullptr;
    
    void* addr = m_pMemPool->addr;

    m_pMemPool->addr = nullptr;
    m_pMemPool->size = 0;

    return addr;
};


void* CMemPool::Alloc(size_t size)
{
    if (!size)
        return nullptr;
    
    if (isMemblkListEmpty(&m_pMemPool->listMemblkNone))
        return nullptr;

    size = Align(size, m_pMemPool->align);

    MEMBLK* memblkList = getMemblkListHead(&m_pMemPool->listMemblkFree);
    MEMBLK* memblkFree = searchFitMemBlk(memblkList, size, false);

    if (!memblkFree)
        return nullptr;

    MEMBLK* memblkAlloc = getMemblkListHead(&m_pMemPool->listMemblkNone);

    remMemBlk(memblkAlloc);
    addMemBlk(&m_pMemPool->listMemblkAlloc, memblkAlloc);
    setMemBlk(memblkAlloc, MEMBLK::STATE_MOVABLE, memblkFree->addr, size);
    memblkAlloc->refCount = 0;

    if (size == memblkFree->size)
    {
        remFreeLinkList(memblkFree);
        setMemBlk(memblkFree, MEMBLK::STATE_NONE, nullptr, 0);
        addMemBlk(&m_pMemPool->listMemblkNone, memblkFree);
    }
    else
    {
        memblkFree->addr =
            reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(memblkFree->addr) + size);
        
        memblkFree->size -= size;
    };

    return memblkAlloc;
};


bool CMemPool::Free(void* mem)
{
    if (!mem)
        return false;

    MEMBLK* memblk = reinterpret_cast<MEMBLK*>(mem);
    if (memblk->state == MEMBLK::STATE_MOVABLE)
    {
        remMemBlk(memblk);
        memblk->state = MEMBLK::STATE_FREE;
        addFreeLinkList(memblk);

        return true;
    };

    return false;
};


void* CMemPool::AllocFixed(size_t size)
{
	void* mem = Alloc(size);
	if (mem)
        return Lock(mem);
    
    return nullptr;
};


bool CMemPool::FreeFixed(void* mem)
{
    if (!mem)
        return false;
    
    MEMBLK* memblk = searchMemBlk(mem, MEMBLK::STATE_FIXED);
    if (memblk)
    {
        Unlock(memblk);
        return Free(memblk);
    };

	return false;
};


void* CMemPool::AllocUpper(size_t size)
{
    if (!size)
        return nullptr;

    if (isMemblkListEmpty(&m_pMemPool->listMemblkNone))
        return nullptr;

    size = Align(size, m_pMemPool->align);

    MEMBLK* memblkList = m_pMemPool->memblkFirstFreeEnd;
    MEMBLK* memblkFree = searchFitMemBlk(memblkList, size, true);

    if (!memblkFree)
        return nullptr;

    MEMBLK* memblkAlloc = getMemblkListHead(&m_pMemPool->listMemblkNone);

    void* addr =
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(memblkFree->addr) + memblkFree->size - size);

    remMemBlk(memblkAlloc);
    addMemBlk(&m_pMemPool->listMemblkAlloc, memblkAlloc);
    setMemBlk(memblkAlloc, MEMBLK::STATE_UPPER, addr, size);

    if (size == memblkFree->size)
    {
        remFreeLinkList(memblkFree);
        setMemBlk(memblkFree, MEMBLK::STATE_NONE, nullptr, 0);
        addMemBlk(&m_pMemPool->listMemblkNone, memblkFree);
    }
    else
    {
        memblkFree->size -= size;
    };

    m_pMemPool->sizeUpper += size;

    return memblkAlloc->addr;
};


bool CMemPool::FreeUpper(void* mem)
{
    if (!mem)
        return false;
    
    MEMBLK* memblk = searchMemBlk(mem, MEMBLK::STATE_UPPER);
    if (!memblk)
        return false;

    if ((memblk->state == MEMBLK::STATE_FREE) ||
        (memblk->state == MEMBLK::STATE_NONE))
        return false;

    m_pMemPool->sizeUpper -= memblk->size;
    
    remMemBlk(memblk);
    memblk->state = MEMBLK::STATE_FREE;
    addFreeLinkList(memblk);

    return true;
};


void* CMemPool::Lock(void* mem)
{
    if (!mem)
        return nullptr;
    
    MEMBLK* memblk = reinterpret_cast<MEMBLK*>(mem);
    if ((memblk->state == MEMBLK::STATE_MOVABLE) ||
        (memblk->state == MEMBLK::STATE_FIXED))
    {
        memblk->state = MEMBLK::STATE_FIXED;
        ++memblk->refCount;

        return memblk->addr;
    };

	return nullptr;
};


void CMemPool::Unlock(void* mem)
{
	if (mem)
	{
        MEMBLK* memblk = reinterpret_cast<MEMBLK*>(mem);
        if (memblk->state != MEMBLK::STATE_FIXED)
            return;

        if (memblk->refCount > 0)
        {
            if (!--memblk->refCount)
                memblk->state = MEMBLK::STATE_MOVABLE;
        };
    };
};


bool CMemPool::Compaction(void)
{
    MEMBLK* memblk = getMemblkListHead(&m_pMemPool->listMemblkFree);
	while (memblk)
	{
        MEMBLK* memblkNext = memblk->next;

        void* addr =
            reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(memblk->addr) + memblk->size);
        
        if (memblkNext && (addr == memblkNext->addr))
		{
			memblk->size += memblkNext->size;
			remFreeLinkList(memblkNext);
			setMemBlk(memblkNext, MEMBLK::STATE_NONE, nullptr, 0);
			addMemBlk(&m_pMemPool->listMemblkNone, memblkNext);
		}
		else
		{
			memblk = memblk->next;
		};
	};

	return true;
};


bool CMemPool::CollectGarbage(void)
{
    MEMBLK* memblkFree = getMemblkListHead(&m_pMemPool->listMemblkFree);
	while (memblkFree)
	{
        MEMBLK* memblkAlloc = getMemblkListHead(&m_pMemPool->listMemblkAlloc);
		while (memblkAlloc)
		{
            if ((memblkAlloc->state == MEMBLK::STATE_MOVABLE) &&
                (memblkFree->size >= memblkAlloc->size) &&
                (memblkFree->addr < memblkAlloc->addr))
            {
                break;
            };

			memblkAlloc = memblkAlloc->next;
		};

        MEMBLK* memblkTemp = getMemblkListHead(&m_pMemPool->listMemblkNone);
		if (memblkAlloc && memblkTemp)
        {
            std::memmove(memblkFree->addr,
                         memblkAlloc->addr,
                         memblkAlloc->size);

			remMemBlk(memblkTemp);
			setMemBlk(memblkTemp, MEMBLK::STATE_FREE, memblkAlloc->addr, memblkAlloc->size);
            addFreeLinkList(memblkTemp);
            
            memblkAlloc->addr = memblkFree->addr;

            memblkFree->addr =
                reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(memblkFree->addr) + memblkAlloc->size);

            ASSERT(memblkFree->size >= memblkAlloc->size);
            memblkFree->size -= memblkAlloc->size;

            if (!memblkFree->size)
			{
                MEMBLK* memblkNext = memblkFree->next;
                
                remFreeLinkList(memblkFree);
				setMemBlk(memblkFree, MEMBLK::STATE_NONE, nullptr, 0);
                addMemBlk(&m_pMemPool->listMemblkNone, memblkFree);

                memblkFree = memblkNext;
            };
		}
		else
		{
            MEMBLK* memblkNext = memblkFree->next;

            void* addr =
                reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(memblkFree->addr) + memblkFree->size);

            if (memblkNext && (addr == memblkNext->addr))
			{
				memblkFree->size += memblkNext->size;
				remFreeLinkList(memblkNext);
				setMemBlk(memblkNext, MEMBLK::STATE_NONE, nullptr, 0);
				addMemBlk(&m_pMemPool->listMemblkNone, memblkNext);
			}
			else
			{
				memblkFree = memblkFree->next;
			};
		};
	};

	return true;
};


bool CMemPool::MemWalk(MEMENTRY* mementry) const
{
	const MEMBLK* memblk = nullptr;

	if (mementry->addr)
	{
        memblk = getMemblkListHead(&m_pMemPool->listMemblkAlloc);
		while (memblk)
		{
			if (memblk->addr == mementry->addr)
			{
				memblk = memblk->next;
				break;
			};

			memblk = memblk->next;
		};
	}
	else
	{
		memblk = m_pMemPool->listMemblkAlloc.next;
	};

	if (memblk)
	{
		mementry->addr = memblk->addr;
		mementry->size = memblk->size;
        mementry->state = memBlockToMemEntryState(memblk->state);
	};

	return (memblk != nullptr);
};


void CMemPool::Output(void) const
{
	;
};


size_t CMemPool::GetAllocSize(void) const
{
    size_t allocSize = 0;

	MEMBLK* memblk = &m_pMemPool->listMemblkFree;
	while (memblk)
	{
        if (memblk->size >= allocSize)
            allocSize = memblk->size;

		memblk = memblk->next;
	};

    return allocSize;
};


size_t CMemPool::GetAllocatedSize(void) const
{
    size_t allocatedSize = 0;

	MEMBLK* memblk = &m_pMemPool->listMemblkAlloc;
	while (memblk)
	{
        if (memblk->state != MEMBLK::STATE_UPPER)
            allocatedSize += memblk->size;

		memblk = memblk->next;
	};

	Output();

    return allocatedSize;
};


size_t CMemPool::GetAllocatedUpperSize(void) const
{
	return m_pMemPool->sizeUpper;
};


size_t CMemPool::GetFreeSize(void) const
{
    size_t freeSize = 0;

	MEMBLK* memblk = &m_pMemPool->listMemblkFree;
	while (memblk)
	{
		freeSize += memblk->size;
		memblk = memblk->next;
	};

    return freeSize;
};


size_t CMemPool::GetSize(void* mem) const
{
    if (!mem)
        return 0;

    MEMBLK* memblk = reinterpret_cast<MEMBLK*>(mem);

    if ((memblk->state == MEMBLK::STATE_FREE) ||
        (memblk->state == MEMBLK::STATE_NONE))
        return 0;

    return memblk->size;
};


size_t CMemPool::GetSizeFixed(void* mem) const
{
	MEMBLK* memblk = &m_pMemPool->listMemblkAlloc;
	while (memblk)
	{
		if (memblk->addr == mem)
			return memblk->size;

		memblk = memblk->next;
	};

    return 0;
};


size_t CMemPool::GetTotalSize(void) const
{
	return m_pMemPool->size;
};


void CMemPool::initMemBlkList(MEMBLK* list, MEMBLK::STATE state)
{
    list->addr = nullptr;
    list->size = 0;
    list->next = nullptr;
    list->prev = nullptr;
    list->state = state;
    list->refCount = 0;
};


void CMemPool::addMemBlk(MEMBLK* memblk, MEMBLK* memblkAdd)
{
	if (memblk)
	{
		memblkAdd->next = memblk->next;
        memblkAdd->prev = memblk;
        
        if (memblk->next)
            memblk->next->prev = memblkAdd;
        
        memblk->next = memblkAdd;
	}
	else
	{
        memblkAdd->next = nullptr;        
        memblkAdd->prev = nullptr;
    };
};


void CMemPool::remMemBlk(MEMBLK* memblk)
{
	if (memblk->next)
        memblk->next->prev = memblk->prev;
    
    if (memblk->prev)
		memblk->prev->next = memblk->next;

    memblk->next = nullptr;
    memblk->prev = nullptr;
};


void CMemPool::setMemBlk(MEMBLK* memblk, MEMBLK::STATE state, void* mem, size_t size)
{
	memblk->addr = mem;
	memblk->size = size;
	memblk->state = state;
};


CMemPool::MEMBLK* CMemPool::searchMemBlk(void* addr, MEMBLK::STATE state) const
{
	MEMBLK* memblk = nullptr;

	switch (state)
	{
	case MEMBLK::STATE_NONE:
		memblk = &m_pMemPool->listMemblkNone;
		break;

	case MEMBLK::STATE_FREE:
		memblk = &m_pMemPool->listMemblkFree;
		break;

    case MEMBLK::STATE_MOVABLE:
	case MEMBLK::STATE_FIXED:
	case MEMBLK::STATE_UPPER:
    case MEMBLK::STATE_ALLOC:
		memblk = &m_pMemPool->listMemblkAlloc;
        break;

    default:
        ASSERT(false);
        break;
    };

	while (memblk)
	{
        if ((memblk->state == state) &&
            (memblk->addr  == addr))
        {
            return memblk;
        };

		memblk = memblk->next;
	};

	return nullptr;
};


CMemPool::MEMBLK* CMemPool::searchFitMemBlk(MEMBLK* list, size_t size, bool bReverse) const
{
    return searchFirstFitMemBlk(list, size, bReverse);
    //return searchBestFitMemBlk(list, size, bReverse);
    //return searchWorstFitMemBlk(list, size, bReverse);
};


CMemPool::MEMBLK* CMemPool::searchFirstFitMemBlk(MEMBLK* list, size_t size, bool bReverse) const
{
    size_t nextOffset = bReverse ? offsetof(MEMBLK, prev) :
                                   offsetof(MEMBLK, next);

    MEMBLK* memblk = list;
    while (memblk)
    {
        if (memblk->size >= size)
            return memblk;

        memblk = *reinterpret_cast<MEMBLK**>(reinterpret_cast<uintptr_t>(memblk) + nextOffset);
    };

    return nullptr;
};


CMemPool::MEMBLK* CMemPool::searchBestFitMemBlk(MEMBLK* list, size_t size, bool bReverse) const
{
    size_t nextOffset = bReverse ? offsetof(MEMBLK, prev) :
                                   offsetof(MEMBLK, next);

    MEMBLK* memblkBest = nullptr;
    MEMBLK* memblk = list;
    while (memblk)
    {
        if (memblk->size >= size)
        {
            if (!memblkBest || (memblk->size < memblkBest->size))
            {
                memblkBest = memblk;

                if (memblkBest->size == size)
                    break;
            };
        };

        memblk = *reinterpret_cast<MEMBLK**>(reinterpret_cast<uintptr_t>(memblk) + nextOffset);
    };

    return memblkBest;
};


CMemPool::MEMBLK* CMemPool::searchWorstFitMemBlk(MEMBLK* list, size_t size, bool bReverse) const
{
    size_t nextOffset = bReverse ? offsetof(MEMBLK, prev) :
                                   offsetof(MEMBLK, next);

    MEMBLK* memblkWorst = nullptr;
    MEMBLK* memblk = list;
    while (memblk)
    {
        if (memblk->size >= size)
        {
            if (!memblkWorst || (memblk->size > memblkWorst->size))
                memblkWorst = memblk;
        };

        memblk = *reinterpret_cast<MEMBLK**>(reinterpret_cast<uintptr_t>(memblk) + nextOffset);
    };

    return memblkWorst;
};


CMemPool::MEMBLK* CMemPool::getMemBlk(int32 idx) const
{
    return &m_pMemPool->memblkPool[idx];
};


void CMemPool::addFreeLinkList(MEMBLK* memblk)
{
	MEMBLK* memblkFree = &m_pMemPool->listMemblkFree;
	while (memblkFree)
	{
		if (!memblkFree->next)
		{
			addMemBlk(memblkFree, memblk);
			m_pMemPool->memblkFirstFreeEnd = memblk;
			break;
		};

        if ((memblkFree->addr < memblk->addr) &&
            (memblk->addr < memblkFree->next->addr))
        {
			addMemBlk(memblkFree, memblk);
			break;
		};

		memblkFree = memblkFree->next;
	};

	if (memblkFree)
    {
        void* addr =
            reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(memblkFree->addr) + memblkFree->size);

        if (addr == memblk->addr)
        {
			memblkFree->size += memblk->size;
			remFreeLinkList(memblk);
			setMemBlk(memblk, MEMBLK::STATE_NONE, nullptr, 0);
			addMemBlk(&m_pMemPool->listMemblkNone, memblk);
		};
	};
};


void CMemPool::remFreeLinkList(MEMBLK* memblk)
{
	if (!memblk->next)
		m_pMemPool->memblkFirstFreeEnd = memblk->prev;

	remMemBlk(memblk);
};


CMemPool::MEMBLK* CMemPool::getMemblkListHead(MEMBLK* list)
{
    return list->next;
};


const CMemPool::MEMBLK* CMemPool::getMemblkListHead(const MEMBLK* list) const
{
    return list->next;
};


CMemPool::MEMBLK* CMemPool::getMemblkListTail(MEMBLK* list)
{
    return list->prev;
};


bool CMemPool::isMemblkListEmpty(CMemPool::MEMBLK* list) const
{
    return (list->next == nullptr);
};


CMemPool::MEMENTRY::STATE
CMemPool::memBlockToMemEntryState(MEMBLK::STATE state) const
{
    return static_cast<MEMENTRY::STATE>(state);
};