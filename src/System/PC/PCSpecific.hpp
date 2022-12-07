#pragma once

#include "PCTypedefs.hpp"


class CPCSpecific
{
public:
    static HINSTANCE m_hInstance;
    static HWND m_hWnd;
    
    static void Initialize(void);
    static void Terminate(void);
    static void SetFlipEnable(bool bEnable);
    static bool SetVideomode(int32 iWidth, int32 iHeight, int32 iDepth);
    static bool SetVideomode(int32 iVideomodeNo);
    static int32 GetVideomodeNum(void);
    static int32 GetVideomodeCur(void);
    static void GetVideomode(int32 iVideomodeNo, int32& w, int32& h, int32& d);
    static bool IsVideomodeWindow(int32 iVideomodeNo);
    static void GetModulePath(char* pszModulePathBuff, int32 buffsize);
    static void DisplayCursor(bool bDispFlag);
};