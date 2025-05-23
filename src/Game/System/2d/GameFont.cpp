#include "GameFont.hpp"

#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Font.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/TextData.hpp"


static inline float GetFontHeight(Rt2dFont* pFont)
{
#ifdef TMNT2_BUILD_EU
    return 8.5f;
#else /* TMNT2_BUILD_EU */
    float fHeight = Rt2dFontGetHeight(pFont);
    float fScrH = static_cast<float>(CScreen::Height());
    return ((fHeight * fScrH) / TYPEDEF::VSCR_H);
#endif /* TMNT2_BUILD_EU */
};


/*static*/ CUnicodeFont* CGameFont::m_pFont = nullptr;
/*static*/ float CGameFont::m_fHeight = 0.0f;


/*static*/ bool CGameFont::Initialize(void)
{
    return CSystem2D::Initialize();
};


/*static*/ void CGameFont::Terminate(void)
{
    Close();
    CSystem2D::Terminate();
};


/*static*/ bool CGameFont::Open(const char* pszName, const char* pszFilePath, const char* pszTextureDict)
{
    ASSERT(pszFilePath);
    ASSERT(pszTextureDict);

    if (!CGameFont::SetTexDictionary(pszTextureDict))
        return false;
    
    Rt2dFontSetPath("");

    Rt2dFont* pRt2dFont = Rt2dFontRead("Common/Fonts/MainFont");
    ASSERT(pRt2dFont);

    m_pFont   = new CUnicodeFont(pRt2dFont);
    m_fHeight = GetFontHeight(m_pFont->GetFontObj());

    ChangeCharacterRect();

    return true;
};


/*static*/ void CGameFont::Close(void)
{
    if (m_pFont)
    {
        delete m_pFont;
        m_pFont = nullptr;
    };
};


/*static*/ bool CGameFont::SetTexDictionary(const char* pszName)
{
    RwTexDictionary* pRwTexDictionary = CTextureManager::GetRwTextureDictionary(pszName);    

    if (!pRwTexDictionary)
        return false;
    
    Rt2dFontTexDictionarySet(pRwTexDictionary);        
    return true;
};


/*static*/ bool CGameFont::ChangeCharacterRect(void)
{
    return true;
};


/*static*/ int32 CGameFont::GetDisplayLineString(wchar* dst, const wchar* src, float fWidth, int32 count)
{
    return m_pFont->GetDisplayLineString(dst, src, m_fHeight, fWidth, count);
};


/*static*/ int32 CGameFont::CountFlowLine(const wchar* pwszString, float fWidth)
{
    return m_pFont->CountFlowLine(pwszString, m_fHeight, fWidth);
};


/*static*/ void CGameFont::ConvertToUnicode(wchar* dst, const char* src)
{
    CUnicodeFont::ConvertToUnicode(dst, src);
};


/*static*/ void CGameFont::ConvertToMultibyte(char* dst, const wchar* src)
{
    char szTempDst[1024];
    szTempDst[0] = '\0';

    CTextData::ToMultibyte(szTempDst, sizeof(szTempDst), src);

    std::strcpy(dst, szTempDst);
};


/*static*/ void CGameFont::Show(const wchar* pwszString, float x, float y)
{
    m_pFont->Show(pwszString, m_fHeight, x, y);
};


/*static*/ void CGameFont::Show(const wchar* pwszString, RwV2d* pvPos)
{
    m_pFont->Show(pwszString, m_fHeight, pvPos);
};


/*static*/ void CGameFont::Flow(const wchar* pwszString, Rt2dBBox* pBBox, Rt2dJustificationType format)
{
    m_pFont->Flow(pwszString, m_fHeight, pBBox, format);
};


/*static*/ void CGameFont::FlowEx(const wchar* pwszString, int32 numLinePad, const Rt2dBBox* pBBox, Rt2dJustificationType format)
{
    m_pFont->FlowEx(pwszString, m_fHeight, numLinePad, pBBox, format);
};


/*static*/ void CGameFont::Show(const char* pszString, float x, float y)
{
    m_pFont->Show(pszString, m_fHeight, x, y);
};


/*static*/ void CGameFont::Show(const char* pszString, const RwV2d* pvPos)
{
    m_pFont->Show(pszString, m_fHeight, pvPos);
};


/*static*/ void CGameFont::SetRGBA(const RwRGBA& rColor)
{
    m_pFont->SetRGBA(rColor);
};


/*static*/ void CGameFont::SetRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
    SetRGBA({ r, g, b, a });
};


/*static*/ void CGameFont::SetHeight(float fHeight)
{
    m_fHeight = fHeight;
};


/*static*/ void CGameFont::SetHeightScaled(float fScale)
{
    SetHeight( GetFontHeight(m_pFont->GetFontObj()) * fScale );
};


/*static*/ float CGameFont::GetHeight(void)
{
    return m_fHeight;
};


/*static*/ float CGameFont::GetHeightScaled(void)
{
    return GetFontHeight(m_pFont->GetFontObj());
};


/*static*/ float CGameFont::GetStringWidth(const wchar* pwszString)
{
    return Rt2dFontGetStringWidth(m_pFont->GetFontObj(), reinterpret_cast<const RwChar*>(pwszString), m_fHeight);
};


/*static*/ float CGameFont::GetStringWidth(const wchar* pwszString, float fHeight)
{
    return Rt2dFontGetStringWidth(m_pFont->GetFontObj(), reinterpret_cast<const RwChar*>(pwszString), fHeight);
};


/*static*/ float CGameFont::GetStringWidth(const char* pszString)
{
    static wchar s_awszBuff[1024];
    s_awszBuff[0] = UTEXT('\0');

    CUnicodeFont::ConvertToUnicode(s_awszBuff, pszString);

    return GetStringWidth(s_awszBuff);
};


/*static*/ float CGameFont::GetStringWidth(const char* pszString, float fHeight)
{
    static wchar s_awszBuff[1024];
    s_awszBuff[0] = UTEXT('\0');

    CUnicodeFont::ConvertToUnicode(s_awszBuff, pszString);

    return GetStringWidth(s_awszBuff, fHeight);
};


/*static*/ Rt2dFont* CGameFont::GetFontObj(void)
{
    return m_pFont->GetFontObj();
};


/*static*/ void CGameFont::SetIntergapSpacing(float fSpacing)
{
    m_pFont->SetIntergapSpacing(fSpacing);
};