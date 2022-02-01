#pragma once


void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void* operator new(std::size_t size, const char* fname, int32 fline);
void* operator new[](std::size_t size, const char* fname, int32 fline);

void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, const char* fname, int32 fline);
void operator delete[](void* ptr, const char* fname, int32 fline);

void* operator new(std::size_t size, const std::nothrow_t& nth);
void* operator new[](std::size_t size, const std::nothrow_t& nth);


#ifdef _DEBUG
#define new new(__FILE__, __LINE__)
#endif