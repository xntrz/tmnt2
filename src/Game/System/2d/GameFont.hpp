#pragma once


class CUnicodeFont;


class CGameFont
{
public:
    static bool Initialize(void);
    static void Terminate(void);
    static bool Open(const char* pszName, const char* pszFilePath, const char* pszTextureDict);
    static void Close(void);
    static bool SetTexDictionary(const char* pszName);
    static bool ChangeCharacterRect(void);
    static int32 GetDisplayLineString(wchar* dst, const wchar* src, float fWidth, int32 count);
    static int32 CountFlowLine(const wchar* pwszString, float fWidth);
    static void ConvertToUnicode(wchar* dst, const char* src);
    static void Show(const wchar* pwszString, float x, float y);
    static void Show(const wchar* pwszString, RwV2d* pvPos);
    static void Flow(const wchar* pwszString, Rt2dBBox* pBBox, Rt2dJustificationType format = rt2dJUSTIFYLEFT);
    static void FlowEx(const wchar* pwszString, int32 numLinePad, const Rt2dBBox* pBBox, Rt2dJustificationType format = rt2dJUSTIFYLEFT);
    static void Show(const char* pszString, float x, float y);
    static void Show(const char* pszString, const RwV2d* pvPos);    
    static void SetRGBA(const RwRGBA& rColor);
    static void SetRGBA(uint8 r, uint8 g, uint8 b, uint8 a);
    static void SetHeight(float h);
    static float GetHeight(void);
    static float GetScreenHeight(void);
    static float GetScreenHeightEx(float fScreenH);
    static float GetStringWidth(const wchar* pwszString);
    static float GetStringWidth(const wchar* pwszString, float fHeight);
    static Rt2dFont* GetFontObj(void);
    
private:
    static CUnicodeFont* m_pFont;
    static float m_fHeight;
};