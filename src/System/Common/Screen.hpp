#pragma once


class CGraphicsDevice;


class CScreen
{
public:
    static void AttachDevice(CGraphicsDevice* pDevice);
    static void DetachDevice(void);
    static void DeviceChanged(void);
    static void SetFlipEnable(bool bEnable);
    static void SetFlipInterval(int32 nVsyncCount);
    static void SetPalMode(bool bEnable);
    static void SetClearColor(const RwRGBA* pColor);
    static void SetClearColor(const RwRGBA& rColor);
    static void ResetClearColor(void);
    static void OnFlip(void);
    static float Framerate(void);
    static float TimerStride(void);
    static float TimeElapsed(void);
    static int32 Width(void);
    static int32 Height(void);
    static CGraphicsDevice& Device(void);
};