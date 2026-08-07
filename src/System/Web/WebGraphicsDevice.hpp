#pragma once

#include "System/Common/GraphicsDevice.hpp"


class CWebGraphicsDevice final : public CGraphicsDevice
{
public:
    CWebGraphicsDevice(void);
    virtual ~CWebGraphicsDevice(void);
    virtual bool Initialize(void) override;
    virtual void Terminate(void) override;
    virtual bool Start(void) override;
    virtual void Flip(void) override;
    virtual int32 ScreenWidth(void) override;
    virtual int32 ScreenHeight(void) override;
    virtual int32 ScreenDepth(void) override;
    virtual void* Configure(void) override;
    virtual int32 Subsystem(void) override;
    virtual int32 Videomode(void) override;
    virtual bool CreateFrameBuffer(void) override;
    virtual void DestroyFrameBuffer(void) override;
};