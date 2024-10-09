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


SdQueue_t* SdQueueCreate(int32 _capacity, int32 _objectSize)
{
    ASSERT(_capacity > 0);
    ASSERT(_objectSize > 0);

    SdQueue_t* Queue = (SdQueue_t*)SdMemAlloc(sizeof(SdQueue_t));
    if (!Queue)
        return 0;
    
    std::memset(Queue, 0, sizeof(*Queue));

    Queue->ObjectArray = SdMemAlloc(_objectSize * _capacity);
    if (Queue->ObjectArray)
    {
        std::memset(Queue->ObjectArray, 0, _capacity * _objectSize);
        
        Queue->PosCur = 0;
        Queue->PosCap = 0;
        Queue->Capacity = _capacity;
        Queue->Stride = _objectSize;
        Queue->Size;
    }
    else
    {
        SdMemFree(Queue);
        Queue = nullptr;
    };
    
    return Queue;
};


void SdQueueDestroy(SdQueue_t* _q)
{
    if (_q->ObjectArray)
    {
        SdMemFree(_q->ObjectArray);
        _q->ObjectArray = nullptr;
    };

    SdMemFree(_q);
};


bool SdQueueIsFull(SdQueue_t* _q)
{
    return (_q->Size == _q->Capacity);
};


bool SdQueueIsEmpty(SdQueue_t* _q)
{
    return (_q->Size == 0);
};


void SdQueuePush(SdQueue_t* _q, void* _object)
{
    ASSERT(!SdQueueIsFull(_q));

    std::memcpy(
        &((char*)_q->ObjectArray)[_q->PosCap * _q->Stride],
        _object,
        _q->Stride
    );

    _q->PosCap = (++_q->PosCap % _q->Capacity);
    ++_q->Size;
};


void SdQueuePop(SdQueue_t* _q)
{
    ASSERT(!SdQueueIsEmpty(_q));

    _q->PosCur = (++_q->PosCur % _q->Capacity);

    ASSERT(_q->Size > 0);
    --_q->Size;
};


void* SdQueueFront(SdQueue_t* _q)
{
    return &((char*)_q->ObjectArray)[_q->PosCur * _q->Stride];
};


void SdQueueClear(SdQueue_t* _q)
{
    while (!SdQueueIsEmpty(_q))
        SdQueuePop(_q);
};


bool SdQueueMove(SdQueue_t* _from, SdQueue_t* _to)
{
    ASSERT(_to->Stride == _from->Stride);
    ASSERT(_to->Capacity == _from->Capacity);

    if (_to->Stride != _from->Stride)
        return false;

    if (_to->Capacity != _from->Capacity)
        return false;

    while (!SdQueueIsEmpty(_from) && !SdQueueIsFull(_to))
    {
        SdQueuePush(_to, SdQueueFront(_from));
        SdQueuePop(_from);
    };

    return true;
};