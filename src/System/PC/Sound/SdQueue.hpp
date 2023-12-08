#pragma once


struct SdQueue_t;


SdQueue_t* SdQueueCreate(int32 Capacity, int32 ObjectSize);
void SdQueueDestroy(SdQueue_t* q);
void SdQueueClear(SdQueue_t* q);
bool SdQueueIsFull(SdQueue_t* q);
bool SdQueueIsEmpty(SdQueue_t* q);
void SdQueuePush(SdQueue_t* q, void* Object);
void SdQueuePop(SdQueue_t* q);
void* SdQueueFront(SdQueue_t* q);
void SdQueueClear(SdQueue_t* q);
bool SdQueueMove(SdQueue_t* from, SdQueue_t* to);