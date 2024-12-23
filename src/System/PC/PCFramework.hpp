#pragma once

#include "PCTypedefs.hpp"

#include "System/Common/Framework.hpp"


class CPCSystem;
class CPCTimer;
class CPCGraphicsDevice;
class CPCSoundDevice;


class CPCFramework final : public CFramework
{
private:
    class CFrameSkipController;
    
public:
    static CPCFramework& Instance(void);
    
    CPCFramework(void);
    virtual ~CPCFramework(void);
    virtual bool Initialize(void) override;
    virtual void Terminate(void) override;
    virtual void Run(void) override;
    virtual void Move(void) override;
    virtual void Render(void) override;
    virtual void FlipNoSync(void);  // makes gfx flip without subsystems sync
    CPCGraphicsDevice& GraphicsDevice(void);
    bool SetVideomode(int32 No);
    bool GetVideomode(int32 No, PC::VIDEOMODE& vm);
    int32 GetVideomodeCur(void);
    int32 GetVideomodeNum(void);
    void SetSkipEnable(bool bState);

private:
    static CPCFramework*    m_pInstance;
    CPCSystem*              m_pSystem;
    CPCTimer*               m_pTimer;
    CPCGraphicsDevice*      m_pPCGraphicsDevice;
    CPCSoundDevice*         m_pPCSoundDevice;
    CFrameSkipController*   m_pFrameSkipController;
};