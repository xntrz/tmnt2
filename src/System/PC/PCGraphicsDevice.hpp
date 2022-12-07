#pragma once

#include "System/Common/GraphicsDevice.hpp"


class CPCFrameTimer;


class CPCGraphicsDevice final : public CGraphicsDevice
{
public:
    enum DISPLAYMODE
    {
        DISPLAYMODE_FULLSCREEN,
        DISPLAYMODE_FULLWINDOW,
        DISPLAYMODE_WINDOW,
    };
    
private:
    struct VIDEOMODE;
    struct DEVICE;
    
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
    virtual bool IsExclusive(void) const override;    
    const char* GetDeviceName(void) const;
    void SetDisplaymode(DISPLAYMODE mode);
    bool SetVideomode(int32 iVideomodeIndex);
    bool SetVideomode(int32 iWidth, int32 iHeight, int32 iDepth);
    DISPLAYMODE GetDisplaymode(void) const;
    void GetVideomode(int32 iVideomodeIndex, int32& riWidth, int32& riHeight, int32& riDepth);
    int32 GetVideomodeNum(void) const;
    int32 GetVideomodeCur(void) const;
    bool IsVideomodeWindow(int32 iVideomodeNo) const;

private:
    int32 GetDeviceNoByMonitorNo(int32 iMonitorNo);
    DEVICE& Device(void);
    const DEVICE& Device(void) const;
    void ConfigureUpdate(int32 iWidth, int32 iHeight);
    void DeviceChanged(void);
    bool DeviceStartup(void);
    void DeviceCleanup(void);
    int32 DeviceSyncPrimary(void);
    bool DeviceInitMonitor(int32 iDevice);
    bool EvalVideomode(const VIDEOMODE* pVideomode) const;
    static int32 VideomodeSortCallback(const void* a, const void* b);

private:
    CPCFrameTimer* m_pFrameTimer;
    DEVICE* m_paDevice;
    int32 m_numDevice;
    int32 m_curDevice;
    DISPLAYMODE m_displaymode;
};