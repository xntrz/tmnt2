#pragma once


class CMemory;
class CGraphicsDevice;
class CInputsDevice;
class CFileManager;
class IClockDevice;
class CProcessDispatcher;


class CFramework
{
public:
    static CFramework* GetConcreteInstance(void);
    
    CFramework(void);
    virtual ~CFramework(void);
    virtual void Startup(void);
    virtual void Cleanup(void);
    virtual bool Initialize(void);
    virtual void Terminate(void);
    virtual void Run(void);
    virtual void Move(void);
    virtual void Render(void);
    virtual void Flip(void);
    virtual bool IsEnded(void) const;

protected:
    CMemory* m_pMemory;
    CGraphicsDevice* m_pGraphicsDevice;
    CInputsDevice* m_pInputsDevice;
    CFileManager* m_pFileManager;
    IClockDevice* m_pClockDevice;
    CProcessDispatcher* m_pProcessDispatcher;
};