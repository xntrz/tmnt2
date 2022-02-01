#pragma once

#include "System/Common/Font.hpp"


class CGameFont
{
public:
    static bool Initialize(void);
    static void Terminate(void);
    static void Open(const char* pszName, const char* pszFilePath, const char* pszTextureDict);
    static void Close(void);
    static bool SetTxDictionary(const char* pszName);
    static bool ChangeCharacterRect(void);
    static float GetScreenSize(void);
    static float GetStringWidth(const wchar* pwszString);
    static float GetStringWidth(const wchar* pwszString, float fHeight);
    static void Show(const wchar* pwszString, float x, float y);
    static void Show(const wchar* pwszString, const RwV2d* pvPos);
    static void Show(const wchar* pwszString, const RwV2d& rvPos);
    static void Show(const char* pszString, float x, float y);
    static void Show(const char* pszString, const RwV2d* pvPos);
    static void Show(const char* pszString, const RwV2d& rvPos);
    static void SetRGBA(const RwRGBA& rColor);
    static void SetRGBA(uint8 r, uint8 g, uint8 b, uint8 a);

public:
    static CUnicodeFont* m_pFont;
    static float m_fHeight;
};