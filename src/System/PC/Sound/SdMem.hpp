#pragma once


void SdMemInitialize(void);
void SdMemTerminate(void);
void* SdMemAlloc(uint32 Size);
void SdMemFree(void* Ptr);