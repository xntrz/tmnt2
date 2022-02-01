#pragma once


class CKeyboard
{
public:
    static int32 GetPort(void);
    static void MapDigital(uint32 uDigital, int32 iDIKey);
    static void MapDigitalFixed(uint32 uDigital, int32 iDIKey);
    static void MapAnalog(int32 analog, int32 iDIKeyX, int32 iDIKeyY);
    static int32 GetDownKey(void);
    static bool IsKeyDown(int32 iDIKey);
    static bool IsKeyValid(int32 iDIKey);
    static const char* GetKeyName(int32 iDIKey);
};