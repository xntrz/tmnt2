#pragma once


struct SdQueue_t;


SdQueue_t* SdQueueCreate(int32 _capacity, int32 _objectSize);
void SdQueueDestroy(SdQueue_t* _q);
void SdQueueClear(SdQueue_t* _q);
bool SdQueueIsFull(SdQueue_t* _q);
bool SdQueueIsEmpty(SdQueue_t* _q);
void SdQueuePush(SdQueue_t* _q, void* _object);
void SdQueuePop(SdQueue_t* _q);
void* SdQueueFront(SdQueue_t* _q);
void SdQueueClear(SdQueue_t* _q);
bool SdQueueMove(SdQueue_t* _from, SdQueue_t* _to);