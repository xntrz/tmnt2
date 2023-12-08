#pragma once

#include "PCTypedefs.hpp"

#include "System/Common/Controller.hpp"


class CPCSpecific
{
public:
    static HINSTANCE m_hInstance;
    static HWND m_hWnd;

public:    
    static int32 GetKeyboradPort(void);
    static void MapDigital(uint32 btn, int32 iDIKey);
    static void MapDigitalFixed(uint32 btn, int32 iDIKey);
    static void MapAnalog(CController::ANALOG analog, int32 iDIKeyX, int32 iDIKeyY);
    static int32 GetDownKey(void);
    static bool IsKeyDown(int32 iDIKey);
    static bool IsKeyValid(int32 iDIKey);
    static const char* GetKeyName(int32 iDIKey);
    static bool SetVideomode(int32 No);
    static bool GetVideomode(int32 No, PC::VIDEOMODE& vm);
    static int32 GetVideomodeNum(void);
    static int32 GetVideomodeCur(void);
    static void FrameSkipEnable(bool bEnable);
    static void DisplayCursor(bool bDispFlag);
    static void PathCorrection(std::string& str);
};