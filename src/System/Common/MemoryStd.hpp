#pragma once


void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void* operator new(std::size_t size, const std::nothrow_t& nth) noexcept;
void* operator new[](std::size_t size, const std::nothrow_t& nth) noexcept;


#ifdef _DEBUG
extern void memory_std_set_alloc_source(const char* fname, int fline);
#define new (memory_std_set_alloc_source(__FILENAME__, __LINE__), 0) ? NULL : new
#endif /* _DEBUG */
