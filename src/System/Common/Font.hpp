#pragma once


class CStringParser
{
public:
    static const int32 BUFF_SIZE = 128;

public:
    static bool IsSeparator(const wchar* pwszString);
    static bool IsNull(const wchar* pwszString);
    static bool IsNewLine(const wchar* pwszString);
    static int32 ExtractSeparator(wchar* pDst, const wchar* pSrc, int32 count);
    static int32 ExtractToken(wchar* pDst, const wchar* pSrc, int32 count);
    static int32 ExtractNewLine(wchar* pDst, const wchar* pSrc);
};


class CFont
{
public:
    CFont(Rt2dFont* pFont);
    virtual ~CFont(void);
    void SetRGBA(uint8 r, uint8 g, uint8 b, uint8 a);
    void SetRGBA(const RwRGBA& rgba);
    Rt2dFont* GetFontObj(void) const;

protected:
    Rt2dFont* m_pFont;
    Rt2dBrush* m_pBrush;
};


class CUnicodeFont : public CFont
{
public:
    static int32 ConvertToUnicode(wchar* dst, const char* src);
        
    CUnicodeFont(Rt2dFont* pFont);
    virtual ~CUnicodeFont(void);
    int32 GetDisplayLineString(wchar* dst, const wchar* src, float fHeight, float fWidth, int32 count) const;
    int32 CountFlowLine(const wchar* pwszString, float fHeight, float fWidth);
    void SetIntergapSpacing(float fSpacing);
    void Show(const char* pszString, float fHeight, float x, float y);
    void Show(const char* pszString, float fHeight, const RwV2d* pvPos);
    void Show(const wchar* pwszString, float fHeight, RwV2d* pPosition);
    void Show(const wchar* pwszString, float fHeight, float fPosX, float fPosY);
    void Flow(const wchar* pwszString, float fHeight, Rt2dBBox* pBBox, Rt2dJustificationType format = rt2dJUSTIFYLEFT);
    void FlowEx(const wchar* pwszString, float fHeight, int32 numLinePad, const Rt2dBBox* pBBox, Rt2dJustificationType format = rt2dJUSTIFYLEFT);
};