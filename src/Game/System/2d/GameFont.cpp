#include "GameFont.hpp"

#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Screen.hpp"


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


/*static*/ void CGameFont::Open(const char* pszName, const char* pszFilePath, const char* pszTextureDict)
{
    ASSERT(pszFilePath);

    if (CGameFont::SetTxDictionary(pszTextureDict))
    {
        Rt2dFontSetPath("");
        Rt2dFont* pRt2dFont = Rt2dFontRead("Common/Fonts/MainFont");
        ASSERT(pRt2dFont);
		
        m_pFont = new CUnicodeFont(pRt2dFont);
        ASSERT(m_pFont);
		m_fHeight = GetScreenSize();
		return;
    };

    ASSERT(false);
};


/*static*/ void CGameFont::Close(void)
{
    if (m_pFont)
    {
        delete m_pFont;
        m_pFont = nullptr;
    };
};


/*static*/ bool CGameFont::SetTxDictionary(const char* pszName)
{
    ASSERT(pszName);
    
    RwTexDictionary* pRwTexDictionary = CTextureManager::GetRwTextureDictionary(pszName);    
    ASSERT(pRwTexDictionary);
    
    Rt2dFontTexDictionarySet(pRwTexDictionary);
    
    return true;
};


/*static*/ bool CGameFont::ChangeCharacterRect(void)
{
    return true;
};


/*static*/ float CGameFont::GetScreenSize(void)
{
    return (Rt2dFontGetHeight(m_pFont->GetFontObj()) * CScreen::Height());
};


/*static*/ float CGameFont::GetStringWidth(const wchar* pwszString)
{
    return Rt2dFontGetStringWidth(m_pFont->GetFontObj(), (const RwChar*)pwszString, m_fHeight);
};


/*static*/ float CGameFont::GetStringWidth(const wchar* pwszString, float fHeight)
{
    return Rt2dFontGetStringWidth(m_pFont->GetFontObj(), (const RwChar*)pwszString, fHeight);
};


/*static*/ void CGameFont::Show(const wchar* pwszString, float x, float y)
{
    m_pFont->Show(pwszString, m_fHeight, x, y);
};


/*static*/ void CGameFont::Show(const wchar* pwszString, const RwV2d* pvPos)
{
    m_pFont->Show(pwszString, m_fHeight, pvPos->x, pvPos->y);
};


/*static*/ void CGameFont::Show(const wchar* pwszString, const RwV2d& rvPos)
{
    m_pFont->Show(pwszString, m_fHeight, rvPos.x, rvPos.y);
};


/*static*/ void CGameFont::Show(const char* pszString, float x, float y)
{
    m_pFont->Show(pszString, m_fHeight, x, y);
};


/*static*/ void CGameFont::Show(const char* pszString, const RwV2d* pvPos)
{
    m_pFont->Show(pszString, m_fHeight, pvPos->x, pvPos->y);
};


/*static*/ void CGameFont::Show(const char* pszString, const RwV2d& rvPos)
{
    m_pFont->Show(pszString, m_fHeight, rvPos.x, rvPos.y);
};


/*static*/ void CGameFont::SetRGBA(const RwRGBA& rColor)
{
    m_pFont->SetRGBA(rColor);
};


/*static*/ void CGameFont::SetRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
    m_pFont->SetRGBA(r, g, b, a);
};