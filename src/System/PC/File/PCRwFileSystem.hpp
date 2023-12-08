#pragma once


class CPCRwFileSystem
{
public:
    inline CPCRwFileSystem(void) {};
    virtual ~CPCRwFileSystem(void) {};
    bool Initialize(void);
    void Terminate(void);
};