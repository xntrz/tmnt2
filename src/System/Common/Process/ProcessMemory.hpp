#pragma once


class CProcessMemory
{
public:
    CProcessMemory(void);
    virtual void* Alloc(uint32 size);
    virtual void Free(void* mem);
    virtual ~CProcessMemory(void);
};