#include "Font.hpp"
#include "System2D.hpp"
#include "TextData.hpp"
#include "Sprite.hpp"
#include "Screen.hpp"


/*static*/ bool CStringParser::IsSeparator(const wchar* pwszString)
{
    return (*pwszString == UTEXT(' '));
};


/*static*/ bool CStringParser::IsNull(const wchar* pwszString)
{
    return (*pwszString == UTEXT('\0'));
};


/*static*/ bool CStringParser::IsNewLine(const wchar* pwszString)
{
    return  (*pwszString == UTEXT('\r')) ||
            (*pwszString == UTEXT('\n'));
};


/*static*/ int32 CStringParser::ExtractSeparator(wchar* pDst, const wchar* pSrc, int32 count)
{
    int32 iSize = 0;
    
    for (iSize = 0; iSize < (count - 1); ++iSize)
    {
        if (!CStringParser::IsSeparator(&pSrc[iSize]))
            break;

        pDst[iSize] = pSrc[iSize];
    };

    pDst[iSize] = '\0';

    return iSize;
};


/*static*/ int32 CStringParser::ExtractToken(wchar* pDst, const wchar* pSrc, int32 count)
{
    int32 iSize = 0;

    for (iSize = 0; iSize < (count - 1); ++iSize)
    {
        if (CStringParser::IsSeparator(&pSrc[iSize]))
            break;
        
        if (CStringParser::IsNewLine(&pSrc[iSize]))
            break;
        
        if (CStringParser::IsNull(&pSrc[iSize]))
            break;        

        pDst[iSize] = pSrc[iSize];
    };

    pDst[iSize] = '\0';

    return iSize;
};


/*static*/ int32 CStringParser::ExtractNewLine(wchar* pDst, const wchar* pSrc)
{
    int32 iSize = 0;
    
    if (pSrc[0] == UTEXT('\n'))
    {
        pDst[0] = UTEXT('\n');
        pDst[1] = UTEXT('\0');
        iSize = 1;
    }
    else if (pSrc[0] == UTEXT('\r'))
    {
        pDst[0] = UTEXT('\r');
        iSize = 1;

        if (pSrc[1] == UTEXT('\n'))
        {
            pDst[1] = UTEXT('\n');
            iSize = 2;
        };

        pDst[iSize] = UTEXT('\0');
    }
    else
    {
        pDst[0] = UTEXT('\0');
    };

    return iSize;
};


CFont::CFont(Rt2dFont* pFont)
: m_pFont(pFont)
, m_pBrush(nullptr)
{
    ASSERT(m_pFont);

    CSystem2D::Initialize();

    m_pBrush = Rt2dBrushCreate();
    ASSERT(m_pBrush);

    if (m_pBrush)
    {
        RwRGBA ColorDefault = { 0xFF,0xFF,0xFF,0xFF };
        Rt2dBrushSetRGBA(
            m_pBrush,
            &ColorDefault,
            &ColorDefault,
            &ColorDefault,
            &ColorDefault
        );
        Rt2dBrushSetTexture(m_pBrush, nullptr);
    };
};


CFont::~CFont(void)
{
    if (m_pBrush)
    {
        Rt2dBrushDestroy(m_pBrush);
        m_pBrush = nullptr;
    };

    if (m_pFont)
    {
        Rt2dFontDestroy(m_pFont);
        m_pFont = nullptr;
    };

    CSystem2D::Terminate();
};


void CFont::SetRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
    RwRGBA BrushColor = { r, g, b, a };
    Rt2dBrushSetRGBA(m_pBrush, &BrushColor, &BrushColor, &BrushColor, &BrushColor);
};


void CFont::SetRGBA(const RwRGBA& rgba)
{
    SetRGBA(rgba.red, rgba.green, rgba.blue, rgba.alpha);
};


Rt2dFont* CFont::GetFontObj(void) const
{
    return m_pFont;
};


/*static*/ int32 CUnicodeFont::ConvertToUnicode(wchar* dst, const char* src)
{
    int32 nConvertedChars = 0;

    while (*src)
    {
        *dst++ = *src++;
        ++nConvertedChars;
    };

    *dst = UTEXT('\0');

    return nConvertedChars;
};


CUnicodeFont::CUnicodeFont(Rt2dFont* pFont)
: CFont(pFont)
{
    ;
};


CUnicodeFont::~CUnicodeFont(void)
{
    ;
};


int32 CUnicodeFont::GetDisplayLineString(wchar* dst, const wchar* src, float fHeight, float fWidth, int32 count) const
{
    wchar TmpBuff[128];
    TmpBuff[0] = UTEXT('\0');

    wchar TextBuffer[128];
    TextBuffer[0] = UTEXT('\0');

    dst[0] = UTEXT('\0');
    
    float fStringWidthCache = 0.0f;
    const wchar* ptr = &src[CStringParser::ExtractSeparator(TmpBuff, src, COUNT_OF(TmpBuff))];
    
    for (int32 DstCnt = (count - 1); DstCnt > 0; )
    {
        int32 CntSep = CStringParser::ExtractSeparator(TmpBuff, ptr, COUNT_OF(TmpBuff));
        CTextData::StrCpy(TextBuffer, TmpBuff);

        int32 CntTok = CStringParser::ExtractToken(TmpBuff, &ptr[CntSep], COUNT_OF(TmpBuff));
        CTextData::StrCat(TextBuffer, TmpBuff);

        if (CntTok > 0)
        {
			float fStringWidth = Rt2dFontGetStringWidth(m_pFont, (const RwChar*)TextBuffer, fHeight) + fStringWidthCache;
            if (fStringWidth >= fWidth)
                break;
            
            fStringWidthCache = fStringWidth;

            //
            //  FIXME implementation is identically retail game version but dst buffer overflow is possible here
            //
            CTextData::StrCat(dst, TextBuffer);
            DstCnt -= (CntTok + CntSep);
            ptr += (CntTok + CntSep);
        }
        else
        {
            ptr += CntSep;
        };

        if (CStringParser::IsNewLine(ptr))
        {
			ptr += CStringParser::ExtractNewLine(TmpBuff, ptr);
            break;
        };

        if (CStringParser::IsNull(ptr))
            break;
    };
    
    return (ptr - src);
};


int32 CUnicodeFont::CountFlowLine(const wchar* pwszString, float fHeight, float fWidth)
{
    int32 LineNum = 0;
    wchar Buff[128];
    Buff[0] = UTEXT('\0');

    for (int32 Cnt = GetDisplayLineString(Buff, pwszString, fHeight, fWidth, COUNT_OF(Buff));
        Cnt;
        Cnt = GetDisplayLineString(Buff, pwszString, fHeight, fWidth, COUNT_OF(Buff)))
    {
        pwszString += Cnt;
        ++LineNum;
    };

    return LineNum;
};


void CUnicodeFont::Show(const char* pszString, float fHeight, float x, float y)
{
    static wchar s_wszTempBuffer[256 * 256];

    s_wszTempBuffer[0] = UTEXT('\0');

    ASSERT(std::strlen(pszString) < COUNT_OF(s_wszTempBuffer));

    if (ConvertToUnicode(s_wszTempBuffer, pszString))
        Show(s_wszTempBuffer, fHeight, x, y);
};


void CUnicodeFont::Show(const wchar* pwszString, float fHeight, RwV2d* pPosition)
{
    ASSERT(m_pFont);
    ASSERT(pwszString);
    ASSERT(pPosition);

    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    Rt2dDeviceGetMetric(&x, &y, &w, &h);

    pPosition->y = y + y + h - pPosition->y;
    if (!Rt2dFontShow(m_pFont, (RwChar*)pwszString, fHeight, pPosition, m_pBrush))
        ASSERT(false);
    pPosition->y = y + y + h - pPosition->y;
};


void CUnicodeFont::Show(const wchar* pwszString, float fHeight, float fPosX, float fPosY)
{
    ASSERT(m_pFont);
    ASSERT(pwszString);

    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    Rt2dDeviceGetMetric(&x, &y, &w, &h);

    RwV2d Anchor;
    Anchor.x = fPosX;
    Anchor.y = y + y + h - fPosY;

    if (!Rt2dFontShow(m_pFont, (const RwChar*)pwszString, fHeight, &Anchor, m_pBrush))
        ASSERT(false);
};


void CUnicodeFont::Flow(const wchar* pwszString, float fHeight, Rt2dBBox* pBBox, Rt2dJustificationType format)
{
    ASSERT(m_pFont);
    ASSERT(pwszString);

    static wchar s_wszTempBuffer[256 * 256];
    s_wszTempBuffer[0] = UTEXT('\0');

    ASSERT(CTextData::StrLen(pwszString) < sizeof(s_wszTempBuffer));

    CTextData::StrCpy(s_wszTempBuffer, pwszString);

    Rt2dFont* pResult = Rt2dFontFlow(m_pFont, (RwChar*)pwszString, fHeight, pBBox, format, m_pBrush);
    ASSERT(pResult);
};


void CUnicodeFont::FlowEx(const wchar* pwszString, float fHeight, int32 numLinePad, const Rt2dBBox* pBBox, Rt2dJustificationType format)
{
    wchar wszBuff[CStringParser::BUFF_SIZE] = { UTEXT('\0') };
    Rt2dBBox BBox = { 0.0f };

    BBox.x = pBBox->x;
    BBox.y = pBBox->y;
    BBox.w = pBBox->w;
    BBox.h = pBBox->h;

    while (BBox.h >= fHeight)
    {
        int32 iLen = GetDisplayLineString(wszBuff, pwszString, fHeight, BBox.w, sizeof(wszBuff));
        if (!iLen)
            break;
		pwszString += iLen;
        if (numLinePad-- <= 0)
        {
            if (!CStringParser::IsNewLine(wszBuff))
                Flow(wszBuff, fHeight, &BBox, format);

            BBox.h -= fHeight;
        };
    };
};


CAnsiFont::CAnsiFont(Rt2dFont* pFont)
: CFont(pFont)
{
    ;
};


CAnsiFont::~CAnsiFont(void)
{
    ;
};


void CAnsiFont::Show(const char* pszString, float fHeight, float fX, float fY)
{
    ASSERT(m_pFont);
    ASSERT(pszString);

    RwV2d Anchor;
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    Rt2dDeviceGetMetric(&x, &y, &w, &h);

    Anchor.x = fX;
    Anchor.y = y + y + h - fY;

    if (!Rt2dFontShow(m_pFont, (const RwChar*)pszString, fHeight, &Anchor, m_pBrush))
        ASSERT(false);
};


void CAnsiFont::Show(const char* pszString, float fHeight, RwV2d* pvPos)
{
    ASSERT(m_pFont);
    ASSERT(pszString);
    ASSERT(pvPos);

    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    Rt2dDeviceGetMetric(&x, &y, &w, &h);

    pvPos->y = y + y + h - pvPos->y;
    if (!Rt2dFontShow(m_pFont, (RwChar*)pszString, fHeight, pvPos, m_pBrush))
        ASSERT(false);
    pvPos->y = y + y + h - pvPos->y;
};

