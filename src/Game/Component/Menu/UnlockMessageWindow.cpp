#include "UnlockMessageWindow.hpp"

#include "Game/Component/GameMain/SecretInfo.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Screen.hpp"


static const float PASSWORD_SPRITE_X = 192.0f;
static const float PASSWORD_SPRITE_SIZE = 64.0f;
static const float s_fWindowFontSize = 17.0f;
static const RwRGBA s_WindowFontColor = { 0xFF, 0xFF, 0XFF, 0xFF };
static const Rt2dBBox s_WindowRect = { 0.0f, 0.0f, 640.0f, 224.0f };


/*static*/ CUnlockMessage::SPRITETEXTURE CUnlockMessage::m_aPasswordTextureTable []
{
    { "pass_icon_l", nullptr },
    { "pass_icon_r", nullptr },
    { "pass_icon_m", nullptr },
    { "pass_icon_d", nullptr },
    { "pass_icon_s", nullptr },
};


/*static*/ void CUnlockMessage::SetTexture(void)
{
    CTextureManager::SetCurrentTextureSet("password");

    for (int32 i = 0; i < COUNT_OF(m_aPasswordTextureTable); ++i)
    {
        RwTexture* pRwTexture = CTextureManager::GetRwTexture(m_aPasswordTextureTable[i].m_pszName);
        ASSERT(pRwTexture);

        m_aPasswordTextureTable[i].m_pTexture = pRwTexture;
    };
};


CUnlockMessage::CUnlockMessage(void)
: CMessageWindow(COLOR_UNLOCK)
, m_fCursorRotation(0.0f)
, m_bPasswordDraw(false)
{    
    Set(s_WindowRect.x, s_WindowRect.y, s_WindowRect.w, s_WindowRect.h);
    SetOpenAction(true);
    
    CTextureManager::SetCurrentTextureSet("MessageWindow");
    m_cursorSprite.SetTexture(CTextureManager::GetRwTexture("win_icon"));

    const wchar* pwszText = CGameText::GetText(GAMETEXT_OK);
    m_cursorText.SetText(pwszText, s_fWindowFontSize, s_WindowFontColor);
};


CUnlockMessage::~CUnlockMessage(void)
{
    ;
};


void CUnlockMessage::SetSprite(float x, float y, float w, float h)
{
    CMessageWindow::SetSprite(x, y, w, h);

    for (int32 i = 0; i < COUNT_OF(m_aSpritePassword); ++i)
    {
        m_aSpritePassword[i].Move(i * PASSWORD_SPRITE_SIZE + x - PASSWORD_SPRITE_X, 15.0f + y);
        m_aSpritePassword[i].Resize(PASSWORD_SPRITE_SIZE, PASSWORD_SPRITE_SIZE);
    };

    m_cursorSprite.Move(x - 220.0f, y + 70.0f);
    m_cursorSprite.ResizeStrict(28.0f, 28.0f);
};


void CUnlockMessage::DrawInWindow(const Rt2dBBox& bbox) const
{
    if (m_textTitle.Text())
    {
        Rt2dBBox bboxTitle;        
        bboxTitle.x = bbox.x - bbox.w * 0.45f;
        bboxTitle.y = bbox.h * 0.2f - bbox.y;
        bboxTitle.w = bbox.w * 0.9f;
        bboxTitle.h = bbox.h * 0.2f;

        Rt2dBBox bboxMsg;
        bboxMsg.x = bbox.x - bbox.w * 0.45f;
        bboxMsg.y = bbox.h * -0.2f - bbox.y;
        bboxMsg.w = bbox.w * 0.9f;
        bboxMsg.h = bbox.h * 0.4f;

        m_textTitle.Draw(bboxTitle);
        m_textMsg.Draw(bboxMsg);
    }
    else
    {
        Rt2dBBox bboxMsg;
        bboxMsg.x = bbox.x - bbox.w * 0.4f;
        bboxMsg.y = bbox.h * -0.2f - bbox.y;
        bboxMsg.w = bbox.w * 0.8f;
        bboxMsg.h = bbox.h * 0.57f;

        m_textMsg.Draw(bboxMsg);
    };

    if (m_bPasswordDraw)
    {
        for (int32 i = 0; i < COUNT_OF(m_aSpritePassword); ++i)
            m_aSpritePassword[i].Draw();
    };

    m_cursorSprite.DrawRotate();

    Rt2dBBox bboxCursor;
    bboxCursor.x = bbox.x - bbox.w * 0.3f;
    bboxCursor.y = bbox.h * -0.78f - bbox.y;
    bboxCursor.w = bbox.w * 0.9f;
    bboxCursor.h = bbox.h * 0.55f;

    m_cursorText.Draw(bboxCursor);
};


void CUnlockMessage::Input(void)
{
    m_fCursorRotation += (CScreen::TimerStride() * 4.0f);
    if (m_fCursorRotation >= 360.0f)
        m_fCursorRotation -= 360.0f;
    
    m_cursorSprite.SetRotate(m_fCursorRotation);

    int32 iController = CGameData::Attribute().GetVirtualPad();
    if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
        DoneInput();
};


void CUnlockMessage::SetMessage(SECRETID::VALUE idSecret)
{
    GAMETEXT secretTextId = CSecretInfo::GetGameText(idSecret);
    const wchar* pwszText = CGameText::GetText(secretTextId);

    SetText(pwszText, s_fWindowFontSize, s_WindowFontColor);
    SetSizeFromID(idSecret);

    if ((idSecret >= SECRETID::PASSWORDSTART) &&
        (idSecret <  SECRETID::PASSWORDMAX))
        SetPassword(CSecretInfo::GetPassword(idSecret));
    else
        SetPasswordDraw(false);
};


void CUnlockMessage::SetPassword(const char* pszPassword)
{
    int32 len = static_cast<int32>(std::strlen(pszPassword));
    ASSERT(len == COUNT_OF(m_aSpritePassword));
    
    for (int32 i = 0; i < len; ++i)
    {
        switch (pszPassword[i])
        {
        case 'L': m_aSpritePassword[i].SetTexture(m_aPasswordTextureTable[0].m_pTexture); break;
        case 'R': m_aSpritePassword[i].SetTexture(m_aPasswordTextureTable[1].m_pTexture); break;
        case 'M': m_aSpritePassword[i].SetTexture(m_aPasswordTextureTable[2].m_pTexture); break;
        case 'D': m_aSpritePassword[i].SetTexture(m_aPasswordTextureTable[3].m_pTexture); break;
        case 'S': m_aSpritePassword[i].SetTexture(m_aPasswordTextureTable[4].m_pTexture); break;
        default: ASSERT(false); break;
        };

        m_aSpritePassword[i].Move(i * PASSWORD_SPRITE_SIZE - PASSWORD_SPRITE_X, 15.0f);
        m_aSpritePassword[i].Resize(PASSWORD_SPRITE_SIZE, PASSWORD_SPRITE_SIZE);        
    };

    m_bPasswordDraw = true;
};


void CUnlockMessage::SetPasswordDraw(bool bSet)
{
    m_bPasswordDraw = bSet;
};


void CUnlockMessage::SetSizeFromID(SECRETID::VALUE idSecret)
{
    ;
};

