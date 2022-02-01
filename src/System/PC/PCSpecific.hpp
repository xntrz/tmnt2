#pragma once

#include "PCTypedefs.hpp"


class CPCSpecific
{
public:
    static void SetFlipEnable(bool bEnable);
    static bool SetVideomode(int32 iVideomodeIndex);    
    static bool SetVideomode(int32 iWidth, int32 iHeight, int32 iDepth);
    static void GetVideomode(int32 iVideomodeIndex, int32& iWidth, int32& iHeight, int32& iDepth);
    static const char* GetVideoDeviceName(void);
    static int32 GetVideomodeNum(void);

public:
    static HINSTANCE m_hInstance;
    static HWND m_hWnd;
};