#include "SdQueue.hpp"
#include "SdMem.hpp"


struct SdQueue_t
{
    void* ObjectArray;
    int32 PosCur;
    int32 PosCap;
    int32 Capacity;
    int32 Stride;
    int32 Size;
};


SdQueue_t* SdQueueCreate(int32 Capacity, int32 ObjectSize)
{
    ASSERT(Capacity > 0);
    ASSERT(ObjectSize > 0);

    SdQueue_t* Queue = (SdQueue_t*)SdMemAlloc(sizeof(SdQueue_t));
    if (!Queue)
        return 0;
    
	std::memset(Queue, 0, sizeof(*Queue));

    Queue->ObjectArray = SdMemAlloc(ObjectSize * Capacity);
    if (Queue->ObjectArray)
    {
        std::memset(Queue->ObjectArray, 0, Capacity * ObjectSize);
        
        Queue->PosCur = 0;
        Queue->PosCap = 0;
        Queue->Capacity = Capacity;
        Queue->Stride = ObjectSize;
        Queue->Size;
    }
    else
    {
        SdMemFree(Queue);
        Queue = nullptr;
    };
    
    return Queue;
};


void SdQueueDestroy(SdQueue_t* q)
{
    SdQueue_t* Queue = (SdQueue_t*)q;

    if (Queue->ObjectArray)
    {
        SdMemFree(Queue->ObjectArray);
        Queue->ObjectArray = nullptr;
    };

    SdMemFree(Queue);
};


bool SdQueueIsFull(SdQueue_t* q)
{
    SdQueue_t* Queue = (SdQueue_t*)q;

    return (Queue->Size == Queue->Capacity);
};


bool SdQueueIsEmpty(SdQueue_t* q)
{
    SdQueue_t* Queue = (SdQueue_t*)q;

    return (Queue->Size == 0);
};


void SdQueuePush(SdQueue_t* q, void* Object)
{
    SdQueue_t* Queue = (SdQueue_t*)q;

    ASSERT(!SdQueueIsFull(Queue));

    std::memcpy(
        &((char*)Queue->ObjectArray)[Queue->PosCap * Queue->Stride],
        Object,
        Queue->Stride
    );

    Queue->PosCap = (++Queue->PosCap % Queue->Capacity);
    ++Queue->Size;
};


void SdQueuePop(SdQueue_t* q)
{
    SdQueue_t* Queue = (SdQueue_t*)q;

    ASSERT(!SdQueueIsEmpty(Queue));

    Queue->PosCur = (++Queue->PosCur % Queue->Capacity);

    ASSERT(Queue->Size > 0);
    --Queue->Size;
};


void* SdQueueFront(SdQueue_t* q)
{
    SdQueue_t* Queue = (SdQueue_t*)q;

    return &((char*)Queue->ObjectArray)[Queue->PosCur * Queue->Stride];
};


void SdQueueClear(SdQueue_t* q)
{
    SdQueue_t* Queue = (SdQueue_t*)q;

    while (!SdQueueIsEmpty(Queue))
        SdQueuePop(Queue);
};


bool SdQueueMove(SdQueue_t* from, SdQueue_t* to)
{
    SdQueue_t* QueueSrc = (SdQueue_t*)from;
    SdQueue_t* QueueDst = (SdQueue_t*)to;

    ASSERT(QueueDst->Stride == QueueSrc->Stride);
    ASSERT(QueueDst->Capacity == QueueSrc->Capacity);

    if (QueueDst->Stride != QueueSrc->Stride)
        return false;

    if (QueueDst->Capacity != QueueSrc->Capacity)
        return false;

    while (!SdQueueIsEmpty(QueueSrc) && !SdQueueIsFull(QueueDst))
    {
        SdQueuePush(QueueDst, SdQueueFront(QueueSrc));
        SdQueuePop(QueueSrc);
    };

    return true;
};