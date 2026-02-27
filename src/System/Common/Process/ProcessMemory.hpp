#pragma once


class CProcessMemory
{
public:
    CProcessMemory(void);
    virtual void* Alloc(size_t size);
    virtual void Free(void* mem);
    virtual ~CProcessMemory(void);
};