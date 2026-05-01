#pragma once


void SdMemInitialize(void);
void SdMemTerminate(void);
void* SdMemAlloc(size_t _size);
void SdMemFree(void* _mem);