#pragma once

#include "PCTypedefs.hpp"

#include "System/Common/GraphicsDevice.hpp"


class CPCFrameTimer;


class CPCGraphicsDevice final : public CGraphicsDevice
{
private:
    struct VIDEOMODE;
    struct DEVICEINFO;
    
public:
    CPCGraphicsDevice(void);
    virtual ~CPCGraphicsDevice(void);
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
    virtual bool EnumerateVideomodes(void);
    virtual void Cleanup(void);
    virtual int32 SearchAndSetVideomode(const PC::VIDEOMODE& vm, bool bSearchInProgress);
    virtual void SetWindowRect(int32 iWidth, int32 iHeight);
    bool SetVideomode(int32 No);
    bool SetVideomode(const PC::VIDEOMODE& vm);
    bool GetVideomode(int32 No, PC::VIDEOMODE& vm) const;
    int32 GetVideomodeCur(void) const;
    int32 GetVideomodeNum(void) const;
    bool IsFullscreen(void) const;

private:
    void OutputInfo(void);
    bool IsFullscreenWindow(void) const;

private:
    CPCFrameTimer* m_pFrameTimer;
    DEVICEINFO* m_pDeviceInfo;
    int32 m_numDevices;
    int32 m_curDevice;
    int32 m_multisamplingLvl;
    bool m_bFullscreen;
    bool m_bHighReso;
};