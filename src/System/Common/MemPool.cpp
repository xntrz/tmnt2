#include "MemPool.hpp"


CMemPool::CMemPool(void)
: m_pHeap(nullptr)
{
	;
};


CMemPool::~CMemPool(void)
{
	;
};


bool CMemPool::Create(void* memory, uint32 memorySize, uint32 alignment)
{
	m_pHeap = (MEMPOOL*)memory;
	m_pHeap->m_align = alignment;
	m_pHeap->m_poolSize = memorySize;
	m_pHeap->m_pPool = (MEMBLK*)((int8*)m_pHeap + sizeof(MEMPOOL));

	m_pHeap->m_pMemory = nullptr;
	m_pHeap->m_memorySize = 0;

	m_pHeap->m_upperSize = 0;
	m_pHeap->m_pFirstFree = nullptr;

	m_pHeap->m_listMemblkNone.m_memory = nullptr;
	m_pHeap->m_listMemblkNone.m_memorySize = 0;
	m_pHeap->m_listMemblkNone.m_next = m_pHeap->m_listMemblkNone.m_prev = nullptr;
	m_pHeap->m_listMemblkNone.m_state = MEMBLK::STATE_NONE;
	m_pHeap->m_listMemblkNone.m_iRefCount = 0;

	m_pHeap->m_listMemblkFree.m_memory = nullptr;
	m_pHeap->m_listMemblkFree.m_memorySize = 0;
	m_pHeap->m_listMemblkFree.m_next = m_pHeap->m_listMemblkFree.m_prev = nullptr;
	m_pHeap->m_listMemblkFree.m_state = MEMBLK::STATE_FREE;
	m_pHeap->m_listMemblkFree.m_iRefCount = 0;

	m_pHeap->m_listMemblkAlloc.m_memory = nullptr;
	m_pHeap->m_listMemblkAlloc.m_memorySize = 0;
	m_pHeap->m_listMemblkAlloc.m_next = m_pHeap->m_listMemblkAlloc.m_prev = nullptr;
	m_pHeap->m_listMemblkAlloc.m_state = MEMBLK::STATE_ALLOC;
	m_pHeap->m_listMemblkAlloc.m_iRefCount = 0;

	for (int32 i = m_pHeap->m_poolSize - 1; i >= 0; --i)
	{
		MEMBLK* t = &m_pHeap->m_pPool[i];
		MEMBLK* memblk = getMemBlk(i);
		setMemBlk(memblk, MEMBLK::STATE_NONE, nullptr, 0);
		addMemBlk(&m_pHeap->m_listMemblkNone, memblk);
	};

	return true;
};


void* CMemPool::Destroy(void)
{
	return m_pHeap;
};


bool CMemPool::AttachMemory(void* memory, uint32 memorySize)
{
	m_pHeap->m_pMemory = memory;
	m_pHeap->m_memorySize = memorySize;

	MEMBLK* memblk = m_pHeap->m_listMemblkNone.m_next;
	setMemBlk(memblk, MEMBLK::STATE_FREE, memory, memorySize);
	removeMemBlk(memblk);
	addFreeLinkList(memblk);

	return true;
};


void* CMemPool::DetachMemory(void)
{
	Output();

	if (m_pHeap->m_listMemblkAlloc.m_next)
	{
		return nullptr;
	}
	else
	{
		void* memory = m_pHeap->m_pMemory;
		m_pHeap->m_pMemory = nullptr;
		m_pHeap->m_memorySize = 0;
		return memory;
	};
};


void* CMemPool::Alloc(uint32 size)
{
	size = alignmentSize(size, m_pHeap->m_align);
	MEMBLK* memblkEmpty = m_pHeap->m_listMemblkNone.m_next;
	if (size && memblkEmpty)
	{
		MEMBLK* memblk = nullptr;
		for (memblk = m_pHeap->m_listMemblkFree.m_next; memblk; memblk = memblk->m_next)
		{
			if (memblk->m_memorySize >= size)
				break;
		};

		if (memblk)
		{
			removeMemBlk(memblkEmpty);
			addMemBlk(&m_pHeap->m_listMemblkAlloc, memblkEmpty);
			setMemBlk(memblkEmpty, MEMBLK::STATE_ALLOC, memblk->m_memory, size);
			memblkEmpty->m_iRefCount = 0;
			if (size == memblk->m_memorySize)
			{
				removeFreeLinkList(memblk);
				setMemBlk(memblk, MEMBLK::STATE_NONE, nullptr, 0);
				addMemBlk(&m_pHeap->m_listMemblkNone, memblk);
			}
			else
			{
				memblk->m_memory = (int8*)memblk->m_memory + size;
				memblk->m_memorySize -= size;
			};

			return memblkEmpty;
		};
	};

	return nullptr;
};


bool CMemPool::Free(void* memory)
{
	if (memory)
	{
		MEMBLK* memblk = (MEMBLK*)memory;
		if(memblk->m_state == MEMBLK::STATE_ALLOC)
		{
			removeMemBlk(memblk);
			memblk->m_state = MEMBLK::STATE_FREE;
			addFreeLinkList(memblk);

			return true;
		};
	};

	return false;
};


void* CMemPool::AllocFixed(uint32 size)
{
	void* pResult = Alloc(size);
	if (pResult)
		return Lock(pResult);
	else
		return nullptr;
};


bool CMemPool::FreeFixed(void* memory)
{
	if (memory)
	{
		MEMBLK* memblk = searchMemBlk(memory, MEMBLK::STATE_FIXED);
		if (memblk)
		{
			Unlock(memblk);
			Free(memblk);

			return true;
		};
	};

	return false;
};


void* CMemPool::AllocUpper(uint32 size)
{
	size = alignmentSize(size, m_pHeap->m_align);
	MEMBLK* memblkEmpty = m_pHeap->m_listMemblkNone.m_next;
	if (size && memblkEmpty)
	{
		MEMBLK* memblk = nullptr;
		for (memblk = m_pHeap->m_pFirstFree; memblk; memblk = memblk->m_prev)
		{
			if (memblk->m_memorySize >= size)
				break;
		};

		if (memblk)
		{
			removeMemBlk(memblkEmpty);
			addMemBlk(&m_pHeap->m_listMemblkAlloc, memblkEmpty);
			setMemBlk(
				memblkEmpty, 
				MEMBLK::STATE_UPPER, 
				(void*)((int8*)memblk->m_memory + memblk->m_memorySize - size),
				size
			);
			if (size == memblk->m_memorySize)
			{
				removeFreeLinkList(memblk);
				setMemBlk(memblk, MEMBLK::STATE_NONE, nullptr, 0);
				addMemBlk(&m_pHeap->m_listMemblkNone, memblk);
			}
			else
			{
				memblk->m_memorySize -= size;
			};

			m_pHeap->m_upperSize += size;
			return memblkEmpty->m_memory;
		};
	};

	return nullptr;
};


bool CMemPool::FreeUpper(void* memory)
{
	if (memory)
	{
		MEMBLK* memblk = searchMemBlk(memory, MEMBLK::STATE_UPPER);
		if (memblk && memblk->m_state > MEMBLK::STATE_FREE)
		{
			m_pHeap->m_upperSize -= memblk->m_memorySize;
			removeMemBlk(memblk);
			memblk->m_state = MEMBLK::STATE_FREE;
			addFreeLinkList(memblk);

			return true;
		};
	};

	return false;
};


void* CMemPool::Lock(void* memory)
{
	if (memory)
	{
		MEMBLK* memblk = (MEMBLK*)memory;
		if (memblk->m_state == MEMBLK::STATE_ALLOC ||
			memblk->m_state == MEMBLK::STATE_FIXED)
		{
			memblk->m_state = MEMBLK::STATE_FIXED;
			++memblk->m_iRefCount;

			return memblk->m_memory;
		};
	};

	return nullptr;
};


void CMemPool::Unlock(void* memory)
{
	if (memory)
	{
		MEMBLK* memblk = (MEMBLK*)memory;
		if (memblk->m_state > MEMBLK::STATE_ALLOC && !--memblk->m_iRefCount)
			memblk->m_state = MEMBLK::STATE_ALLOC;		
	};
};


bool CMemPool::Compaction(void)
{
	MEMBLK* memblk = &m_pHeap->m_listMemblkFree;
	while (memblk)
	{
		MEMBLK* memblkNext = memblk->m_next;
		if (memblkNext && (void*)((int8*)memblk->m_memory + memblk->m_memorySize) == memblkNext->m_memory)
		{
			memblk->m_memorySize += memblkNext->m_memorySize;
			removeFreeLinkList(memblkNext);
			setMemBlk(memblkNext, MEMBLK::STATE_NONE, nullptr, 0);
			addMemBlk(&m_pHeap->m_listMemblkNone, memblkNext);
		}
		else
		{
			memblk = memblk->m_next;
		};
	};

	return true;
};


bool CMemPool::CollectGarbage(void)
{
	MEMBLK* memblkFree = &m_pHeap->m_listMemblkFree;
	while (memblkFree)
	{
		MEMBLK* memblkUsing = &m_pHeap->m_listMemblkAlloc;
		while (memblkUsing)
		{
			if (memblkUsing->m_state == MEMBLK::STATE_ALLOC				&&
				memblkFree->m_memorySize >= memblkUsing->m_memorySize	&&
				memblkFree->m_memory < memblkUsing->m_memory)
				break;

			memblkUsing = memblkUsing->m_next;
		};

		MEMBLK* memblkEmpty = m_pHeap->m_listMemblkNone.m_next;
		if (memblkUsing && memblkEmpty)
		{
			for (uint32 i = 0; i < memblkUsing->m_memorySize; i++)
				((int8*)memblkFree->m_memory)[i] = ((int8*)memblkUsing->m_memory)[i];

			removeMemBlk(memblkEmpty);
			setMemBlk(memblkEmpty, MEMBLK::STATE_FREE, memblkUsing->m_memory, memblkUsing->m_memorySize);
			addFreeLinkList(memblkEmpty);
			memblkUsing->m_memory = memblkFree->m_memory;
			memblkFree->m_memory = ((int8*)memblkFree->m_memory + memblkUsing->m_memorySize);
			memblkFree->m_memorySize -= memblkUsing->m_memorySize;

			if (!memblkFree->m_memorySize)
			{
				memblkFree = memblkFree->m_next;

				removeFreeLinkList(memblkFree);
				setMemBlk(memblkFree, MEMBLK::STATE_NONE, nullptr, 0);
				addMemBlk(&m_pHeap->m_listMemblkNone, memblkFree);
			};
		}
		else
		{
			MEMBLK* memblkNext = memblkFree->m_next;
			if (memblkNext && (void*)((int8*)memblkFree->m_memory + memblkFree->m_memorySize) == memblkNext->m_memory)
			{
				memblkFree->m_memorySize += memblkNext->m_memorySize;
				removeFreeLinkList(memblkNext);
				setMemBlk(memblkNext, MEMBLK::STATE_NONE, nullptr, 0);
				addMemBlk(&m_pHeap->m_listMemblkNone, memblkNext);
			}
			else
			{
				memblkFree = memblkFree->m_next;
			};
		};
	};

	return true;
};


bool CMemPool::MemWalk(MEMENTRY* mementry)
{
	MEMBLK* memblk = nullptr;

	if (mementry->m_memory)
	{
		memblk = &m_pHeap->m_listMemblkAlloc;
		while (memblk)
		{
			if (memblk->m_memory == mementry->m_memory)
			{
				memblk = memblk->m_next;
				break;
			};

			memblk = memblk->m_next;
		};
	}
	else
	{
		memblk = m_pHeap->m_listMemblkAlloc.m_next;
	};

	if (memblk)
	{
		mementry->m_memory = memblk->m_memory;
		mementry->m_memorySize = memblk->m_memorySize;
		mementry->m_state = memblk->m_state;
	};

	return (!!memblk);
};


void CMemPool::Output(void) const
{
	;
};


uint32 CMemPool::GetAllocSize(void) const
{
	uint32 result = 0;

	MEMBLK* memblk = &m_pHeap->m_listMemblkFree;
	while (memblk)
	{
		if (memblk->m_memorySize >= result)
			result = memblk->m_memorySize;

		memblk = memblk->m_next;
	};

	return result;
};


uint32 CMemPool::GetAllocatedSize(void) const
{
	uint32 result = 0;

	MEMBLK* memblk = &m_pHeap->m_listMemblkAlloc;
	while (memblk)
	{
		if(memblk->m_state != MEMBLK::STATE_UPPER)
			result += memblk->m_memorySize;

		memblk = memblk->m_next;
	};

	Output();

	return result;
};


uint32 CMemPool::GetAllocatedUpperSize(void) const
{
	return m_pHeap->m_upperSize;
};


uint32 CMemPool::GetFreeSize(void) const
{
	uint32 result = 0;

	MEMBLK* memblk = &m_pHeap->m_listMemblkFree;
	while (memblk)
	{
		result += memblk->m_memorySize;
		memblk = memblk->m_next;
	};

	return result;
};


uint32 CMemPool::GetMemPoolMax(uint32 size) const
{
	return (size - sizeof(MEMPOOL)) / sizeof(MEMBLK);
};


uint32 CMemPool::GetSize(void* memory) const
{
	if (memory)
	{
		MEMBLK* memblk = (MEMBLK*)memory;		
		if (memblk->m_state > MEMBLK::STATE_FREE)
			return memblk->m_memorySize;		
	};

	return -1;
};


uint32 CMemPool::GetSizeFixed(void* memory) const
{
	MEMBLK* memblk = &m_pHeap->m_listMemblkAlloc;
	while (memblk)
	{
		if (memblk->m_memory == memblk)
			return memblk->m_memorySize;

		memblk = memblk->m_next;
	};

	return -1;
};


uint32 CMemPool::GetTotalSize(void) const
{
	return m_pHeap->m_memorySize;
};


void CMemPool::addMemBlk(MEMBLK* dst, MEMBLK* src)
{
	if (dst)
	{
		src->m_next = dst->m_next;
		src->m_prev = dst;
		if (dst->m_next)
			dst->m_next->m_prev = src;
		dst->m_next = src;
	}
	else
	{
		src->m_next = src->m_prev = nullptr;
	};
};


void CMemPool::removeMemBlk(MEMBLK* memblk)
{
	if (memblk->m_next)
		memblk->m_next->m_prev = memblk->m_prev;
	if (memblk->m_prev)
		memblk->m_prev->m_next = memblk->m_next;

	memblk->m_next = memblk->m_prev = nullptr;
};


void CMemPool::setMemBlk(MEMBLK* memblk, MEMBLK::STATE state, void* mem, uint32 size)
{
	memblk->m_memory = mem;
	memblk->m_memorySize = size;
	memblk->m_state = state;
};


CMemPool::MEMBLK* CMemPool::searchMemBlk(void* mem, MEMBLK::STATE state) const
{
	MEMBLK* memblk = nullptr;

	switch (state)
	{
	case MEMBLK::STATE_NONE:
		memblk = &m_pHeap->m_listMemblkNone;
		break;

	case MEMBLK::STATE_FREE:
		memblk = &m_pHeap->m_listMemblkFree;
		break;

	case MEMBLK::STATE_ALLOC:
	case MEMBLK::STATE_FIXED:
	case MEMBLK::STATE_UPPER:
		memblk = &m_pHeap->m_listMemblkAlloc;
		break;
	};

	while (memblk)
	{
		if (memblk->m_state == state && memblk->m_memory == mem)
			return memblk;

		memblk = memblk->m_next;
	};

	return nullptr;
};


CMemPool::MEMBLK* CMemPool::getMemBlk(int32 idx) const
{
	return &((MEMBLK*)m_pHeap->m_pPool)[idx];
};


void CMemPool::addFreeLinkList(MEMBLK* memblk)
{
	MEMBLK* memblkFree = &m_pHeap->m_listMemblkFree;
	while (memblkFree)
	{
		if (!memblkFree->m_next)
		{
			addMemBlk(memblkFree, memblk);
			m_pHeap->m_pFirstFree = memblk;
			break;
		};

		if (memblkFree->m_memory < memblk->m_memory && memblk->m_memory < memblkFree->m_next->m_memory)
		{
			addMemBlk(memblkFree, memblk);
			break;
		};

		memblkFree = memblkFree->m_next;
	};

	if (memblkFree)
	{
		if ((int8*)memblkFree->m_memory + memblkFree->m_memorySize == memblk->m_memory)
		{
			memblkFree->m_memorySize += memblk->m_memorySize;
			removeFreeLinkList(memblk);
			setMemBlk(memblk, MEMBLK::STATE_NONE, nullptr, 0);
			addMemBlk(&m_pHeap->m_listMemblkNone, memblk);
		};
	};
};


void CMemPool::removeFreeLinkList(MEMBLK* memblk)
{
	if (!memblk->m_next)
		m_pHeap->m_pFirstFree = memblk->m_prev;

	removeMemBlk(memblk);
};


uint32 CMemPool::alignmentSize(uint32 size, uint32 align) const
{
	return (size + (align - 1)) & ~(align - 1);
};