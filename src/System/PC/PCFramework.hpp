#pragma once

#include "System/Common/Framework.hpp"


class CPCSystem;
class CPCTimer;
class CPCFrameSkipController;
class CPCGraphicsDevice;


class CPCFramework final : public CFramework
{
public:
    static CPCFramework& Instance(void);
    
    CPCFramework(void);
    virtual ~CPCFramework(void);
    virtual void Startup(void) override;
    virtual void Cleanup(void) override;
    virtual bool Initialize(void) override;
    virtual void Terminate(void) override;
    virtual void Run(void) override;
    virtual void Move(void) override;
    virtual void Render(void) override;
    void UpdateSize(int32 iWidth, int32 iHeight);
    void SetFlipEnable(bool bEnable);
    bool SetVideomode(int32 iVideomodeIndex);
    bool SetVideomode(int32 iWidth, int32 iHeight, int32 iDepth);
    void GetVideomode(int32 iVideomodeIndex, int32& iWidth, int32& iHeight, int32& iDepth);
    const char* GetVideoDeviceName(void);
    int32 GetVideomodeNum(void);
    
private:
    static CPCFramework* m_pInstance;
    CPCSystem* m_pSystem;
    CPCTimer* m_pTimer;
    CPCGraphicsDevice* m_pPCGraphicsDevice;
    bool m_bStarted;
};