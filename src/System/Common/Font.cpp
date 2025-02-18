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


//
// *********************************************************************************
//


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
        RwRGBA defaultColor = { 0xFF, 0xFF, 0xFF, 0xFF };
        Rt2dBrushSetRGBA(m_pBrush,
                         &defaultColor,
                         &defaultColor,
                         &defaultColor,
                         &defaultColor);
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


//
// *********************************************************************************
//


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


int32 CUnicodeFont::GetDisplayLineString(wchar* dst,
                                         const wchar* src,
                                         float fHeight,
                                         float fWidth,
                                         int32 count) const
{
    dst[0] = UTEXT('\0');

    wchar wszTmpBuff[128];
    wszTmpBuff[0] = UTEXT('\0');
    
    float fStringWidthBuff = 0.0f;
    const wchar* ptr = &src[CStringParser::ExtractSeparator(wszTmpBuff, src, COUNT_OF(wszTmpBuff))];
    
    for (int32 dstCnt = (count - 1); dstCnt > 0; )
    {
        wchar wszTextBuffer[128];
        wszTextBuffer[0] = UTEXT('\0');

        int32 cntSep = CStringParser::ExtractSeparator(wszTmpBuff, ptr, COUNT_OF(wszTmpBuff));
        CTextData::StrCpy(wszTextBuffer, wszTmpBuff);

        int32 cntTok = CStringParser::ExtractToken(wszTmpBuff, &ptr[cntSep], COUNT_OF(wszTmpBuff));
        CTextData::StrCat(wszTextBuffer, wszTmpBuff);

        if (cntTok > 0)
        {
            float fStringWidth = Rt2dFontGetStringWidth(m_pFont,
                                                        reinterpret_cast<const RwChar*>(wszTextBuffer),
                                                        fHeight);
            fStringWidth += fStringWidthBuff;

            if (fStringWidth >= fWidth)
                break;
            
            fStringWidthBuff = fStringWidth;

            CTextData::StrCat(dst, wszTextBuffer);
            
            dstCnt  -= (cntTok + cntSep);
            ptr     += (cntTok + cntSep);
        }
        else
        {
            ptr += cntSep;
        };

        if (CStringParser::IsNewLine(ptr))
        {
			ptr += CStringParser::ExtractNewLine(wszTmpBuff, ptr);
            break;
        };

        if (CStringParser::IsNull(ptr))
            break;
    };
    
    return (ptr - src);
};


int32 CUnicodeFont::CountFlowLine(const wchar* pwszString, float fHeight, float fWidth)
{
    wchar wszBuff[128];
    wszBuff[0] = UTEXT('\0');

    int32 lineNum = 0;

    int32 len = GetDisplayLineString(wszBuff, pwszString, fHeight, fWidth, COUNT_OF(wszBuff));
    while (len)
    {
        pwszString += len;
        ++lineNum;

        len = GetDisplayLineString(wszBuff, pwszString, fHeight, fWidth, COUNT_OF(wszBuff));
    };

    return lineNum;
};


void CUnicodeFont::SetIntergapSpacing(float fSpacing)
{
    ASSERT(m_pFont);

    Rt2dFontSetIntergapSpacing(m_pFont, fSpacing);
};


void CUnicodeFont::Show(const char* pszString, float fHeight, float x, float y)
{
    static wchar s_wszTempBuffer[256 * 256];
    s_wszTempBuffer[0] = UTEXT('\0');

    ASSERT(std::strlen(pszString) < COUNT_OF(s_wszTempBuffer));

    if (ConvertToUnicode(s_wszTempBuffer, pszString))
        Show(s_wszTempBuffer, fHeight, x, y);
};


void CUnicodeFont::Show(const char* pszString, float fHeight, const RwV2d* pvPos)
{
    Show(pszString, fHeight, pvPos->x, pvPos->y);
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

    if (!Rt2dFontShow(m_pFont, reinterpret_cast<const RwChar*>(pwszString), fHeight, pPosition, m_pBrush))
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

    if (!Rt2dFontShow(m_pFont, reinterpret_cast<const RwChar*>(pwszString), fHeight, &Anchor, m_pBrush))
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

    Rt2dFont* pResult = Rt2dFontFlow(m_pFont, reinterpret_cast<RwChar*>(s_wszTempBuffer), fHeight, pBBox, format, m_pBrush);
    ASSERT(pResult);
};


void CUnicodeFont::FlowEx(const wchar* pwszString,
                          float fHeight,
                          int32 numLinePad,
                          const Rt2dBBox* pBBox,
                          Rt2dJustificationType format)
{
    wchar wszBuff[CStringParser::BUFF_SIZE] = { UTEXT('\0') };
    Rt2dBBox BBox = { 0.0f };

    BBox.x = pBBox->x;
    BBox.y = pBBox->y;
    BBox.w = pBBox->w;
    BBox.h = pBBox->h;

    while (BBox.h >= fHeight)
    {
        int32 len = GetDisplayLineString(wszBuff, pwszString, fHeight, BBox.w, COUNT_OF(wszBuff));
        if (!len)
            break;
        
        pwszString += len;

        if (numLinePad-- <= 0)
        {
            if (!CStringParser::IsNewLine(wszBuff))
                Flow(wszBuff, fHeight, &BBox, format);

            BBox.h -= fHeight;
        };
    };
};