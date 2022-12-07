#pragma once

#include "System/Common/Framework.hpp"


class CPCSystem;
class CPCTimer;
class CPCFrameSkipController;
class CPCGraphicsDevice;
class CPCSoundDevice;


class CPCFramework final : public CFramework
{
public:
    static CPCFramework& Instance(void);
    
    CPCFramework(void);
    virtual ~CPCFramework(void);
    virtual bool Initialize(void) override;
    virtual void Terminate(void) override;
    virtual void Run(void) override;
    virtual void Move(void) override;
    virtual void Render(void) override;
    void UpdateSize(int32 iWidth, int32 iHeight);
    CPCGraphicsDevice& GraphicsDevice(void);

private:
    static CPCFramework* m_pInstance;
    CPCSystem* m_pSystem;
    CPCTimer* m_pTimer;
    CPCGraphicsDevice* m_pPCGraphicsDevice;
    CPCSoundDevice* m_pPCSoundDevice;
    bool m_bStarted;
};