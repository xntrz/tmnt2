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
            STATE_ALLOC, // TODO: unused (?)

            STATE_FORCEDWORD = ((int32)((~((uint32)0)) >> 1)),
        };

		void*   addr;
		size_t  size;
		MEMBLK* next;
		MEMBLK* prev;
		STATE   state;
		int32   refCount;
	};

	struct MEMENTRY
    {
        enum STATE
        {
            STATE_NONE = 0,
            STATE_FREE,
            STATE_MOVABLE,
            STATE_FIXED,
            STATE_UPPER,
            STATE_ALLOC,

            STATE_FORCEDWORD = ((int32)((~((uint32)0)) >> 1)),
        };

        void*   addr;
        size_t  size;
        STATE   state;
	};

	struct MEMPOOL
	{
		void*   addr;
		size_t  size;
		MEMBLK* memblkPool;
		size_t  memblkCount;
		MEMBLK  listMemblkNone;
		MEMBLK  listMemblkFree;
		MEMBLK  listMemblkAlloc;
		MEMBLK* memblkFirstFreeEnd;
		size_t  sizeUpper;
		size_t  align;
	};

public:
    static size_t Align(size_t value, size_t align);
    static size_t GetBlockMaxCount(size_t memSize);
    static void CreateAndAttach(CMemPool& pool, void* mem, size_t size, size_t alignment, size_t blockPercent);
    static void* DetachAndDestroy(CMemPool& pool);

    CMemPool(void);
    ~CMemPool(void);
	void Create(void* memblkPoolAddr, size_t memblkCount, size_t alignment);
	void* Destroy(void);
	void AttachMemory(void* addr, size_t size);
	void* DetachMemory(void);
	void* Alloc(size_t size);
	bool Free(void* mem);
	void* AllocFixed(size_t size);
	bool FreeFixed(void* mem);
	void* AllocUpper(size_t size);
	bool FreeUpper(void* mem);
	void* Lock(void* mem);
	void Unlock(void* mem);
	bool Compaction(void);
	bool CollectGarbage(void);
    bool MemWalk(MEMENTRY* mementry) const;
	void Output(void) const;
	size_t GetAllocSize(void) const;
	size_t GetAllocatedSize(void) const;
	size_t GetAllocatedUpperSize(void) const;
	size_t GetFreeSize(void) const;
	size_t GetSize(void* mem) const;
	size_t GetSizeFixed(void* mem) const;
	size_t GetTotalSize(void) const;

private:
    void initMemBlkList(MEMBLK* list, MEMBLK::STATE state);
    void addMemBlk(MEMBLK* memblk, MEMBLK* memblkAdd);
	void remMemBlk(MEMBLK* blk);
	void setMemBlk(MEMBLK* blk, MEMBLK::STATE state, void* mem, size_t size);
    MEMBLK* searchMemBlk(void* addr, MEMBLK::STATE state) const;
    MEMBLK* searchFitMemBlk(MEMBLK* list, size_t size, bool bReverse) const;
    MEMBLK* searchFirstFitMemBlk(MEMBLK* list, size_t size, bool bReverse) const;
    MEMBLK* searchBestFitMemBlk(MEMBLK* list, size_t size, bool bReverse) const;
    MEMBLK* searchWorstFitMemBlk(MEMBLK* list, size_t size, bool bReverse) const;
    MEMBLK* getMemBlk(int32 idx) const;
	void addFreeLinkList(MEMBLK* blk);
    void remFreeLinkList(MEMBLK* blk);
    MEMBLK* getMemblkListHead(MEMBLK* list);
    const MEMBLK* getMemblkListHead(const MEMBLK* list) const;
    MEMBLK* getMemblkListTail(MEMBLK* list);
    bool isMemblkListEmpty(MEMBLK* list) const;
    MEMENTRY::STATE memBlockToMemEntryState(MEMBLK::STATE state) const;

private:
	MEMPOOL* m_pMemPool;
};