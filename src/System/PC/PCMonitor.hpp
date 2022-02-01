#pragma once


class CPCMonitor
{
public:
    static bool Initialize(void);
    static void Terminate(void);
    static int32 FindMonitor(int32 iWidth, int32 iHeight);
    static int32 GetPrimary(void);
    static int32 GetNum(void);
    static int32 GetWidth(int32 iMonitorNo);
    static int32 GetHeight(int32 iMonitorNo);
    static int32 GetOffsetX(int32 iMonitorNo);
    static int32 GetOffsetY(int32 iMonitorNo);
    static int32 GetWorkWidth(int32 iMonitorNo);
    static int32 GetWorkHeight(int32 iMonitorNo);
    static const char* GetName(int32 iMonitorNo);
};