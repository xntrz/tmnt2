#pragma once


class CMemPool
{
public:
	struct MEMBLK
	{
		enum STATE
		{
			STATE_NONE = 0,
			STATE_FREE,
			STATE_MOVABLE,
			STATE_FIXED,
			STATE_UPPER,
			STATE_ALLOC,
		};

		void* m_memory;
		uint32 m_memorySize;
		MEMBLK* m_next;
		MEMBLK* m_prev;
		STATE m_state;
		int32 m_iRefCount;
	};

	struct MEMENTRY
	{
		void* m_memory;
		uint32 m_memorySize;
		MEMBLK::STATE m_state;
	};

	struct MEMPOOL
	{
		void* m_pMemory;
		uint32 m_memorySize;
		MEMBLK* m_pPool;
		uint32 m_poolSize;
		MEMBLK* m_pFirstFree;
		MEMBLK m_listMemblkNone;
		MEMBLK m_listMemblkFree;
		MEMBLK m_listMemblkAlloc;
		uint32 m_upperSize;
		uint32 m_align;
	};

public:
	CMemPool(void);
	virtual ~CMemPool(void);
	virtual bool Create(void* memory, uint32 memorySize, uint32 alignment);
	virtual void* Destroy(void);
	virtual bool AttachMemory(void* memory, uint32 memorySize);
	virtual void* DetachMemory(void);
	virtual void* Alloc(uint32 size);
	virtual bool Free(void* memory);
	virtual void* AllocFixed(uint32 size);
	virtual bool FreeFixed(void* memory);
	virtual void* AllocUpper(uint32 size);
	virtual bool FreeUpper(void* memory);
	virtual void* Lock(void* memory);
	virtual void Unlock(void* memory);
	virtual bool Compaction(void);
	virtual bool CollectGarbage(void);
	virtual bool MemWalk(MEMENTRY* mementry);
	virtual void Output(void) const;
	virtual uint32 GetAllocSize(void) const;
	virtual uint32 GetAllocatedSize(void) const;
	virtual uint32 GetAllocatedUpperSize(void) const;
	virtual uint32 GetFreeSize(void) const;
	virtual uint32 GetMemPoolMax(uint32 size) const;
	virtual uint32 GetSize(void* memory) const;
	virtual uint32 GetSizeFixed(void* memory) const;
	virtual uint32 GetTotalSize(void) const;

private:
	virtual void addMemBlk(MEMBLK* dst, MEMBLK* src);
	virtual void removeMemBlk(MEMBLK* blk);
	virtual void setMemBlk(MEMBLK* blk, MEMBLK::STATE state, void* mem, uint32 size);
	virtual MEMBLK* searchMemBlk(void* mem, MEMBLK::STATE state) const;
	virtual MEMBLK* getMemBlk(int32 idx) const;
	virtual void addFreeLinkList(MEMBLK* blk);
	virtual void removeFreeLinkList(MEMBLK* blk);
	virtual uint32 alignmentSize(uint32 size, uint32 align) const;

private:
	MEMPOOL* m_pHeap;
};