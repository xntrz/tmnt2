#pragma once

class CSoundDevice
{
public:
    CSoundDevice(void);
    virtual ~CSoundDevice(void);
    virtual bool Initialize(void) = 0;
    virtual void Terminate(void) = 0;
    virtual bool StartupFramework(void) = 0;
    virtual void ShutdownFramework(void) = 0;
    uint32 GetFrameworkOptFlags(void) const;
};