#include "WindowObject.hpp"

#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/TextData.hpp"


CWindowObject::CMessageText::CMessageText(void)
: m_pwszText(nullptr)
, m_fHeight(CGameFont::GetHeightScaled() * 1.0f)
, m_Color({0xFF, 0xFF, 0xFF, 0xFF})
{
    ;
};


CWindowObject::CMessageText::~CMessageText(void)
{
    if (m_pwszText)
    {
        delete[] m_pwszText;
        m_pwszText = nullptr;
    };
};


void CWindowObject::CMessageText::SetText(const wchar* pwszText, float fHeight, const RwRGBA& rColor)
{
    SetText(pwszText);
    m_fHeight = fHeight;
    m_Color = rColor;
};


void CWindowObject::CMessageText::SetText(const wchar* pwszText)
{
    if (m_pwszText)
    {
        delete[] m_pwszText;
        m_pwszText = nullptr;
    };

    if (pwszText)
    {
        int32 len = CTextData::StrLen(pwszText);

        m_pwszText = new wchar[len + 1];

        CTextData::StrCpy(m_pwszText, pwszText);
    };
};


void CWindowObject::CMessageText::Draw(const Rt2dBBox& bbox) const
{
    if (m_pwszText)
    {
        Rt2dBBox bboxDraw = bbox;

        CGameFont::SetHeight(m_fHeight);
        CGameFont::SetRGBA(m_Color);
        CGameFont::Flow(m_pwszText, &bboxDraw);
    };
};


const wchar* CWindowObject::CMessageText::Text(void) const
{
    return m_pwszText;
};


//
// *********************************************************************************
//


/*static*/ const float CWindowObject::OPENTIME = 0.25f;


/*static*/ CWindowObject::SPRITETEXTURE CWindowObject::m_aSpriteTextureTable[SPRITENUM]
{
    { "win_top",    nullptr },
    { "win_middle", nullptr },
    { "win_bottom", nullptr },
    { "win_icon",   nullptr },
};


/*static*/ void CWindowObject::Load(void)
{
	CTextureManager::SetCurrentTextureSet("MessageWindow");

    for (int32 i = 0; i < COUNT_OF(m_aSpriteTextureTable); ++i)
    {
        RwTexture* pRwTexture = CTextureManager::GetRwTexture(m_aSpriteTextureTable[i].m_pszName);
        ASSERT(pRwTexture);
        
        m_aSpriteTextureTable[i].m_pTexture = pRwTexture;
    };
};


CWindowObject::CWindowObject(void)
: m_aSprite()
, m_textMsg()
, m_textTitle()
, m_bboxDraw({ 0.0f })
, m_bbox({ 0.0f })
, m_phase(PHASE_NONE)
, m_fTime(0.0f)
, m_bOpenAction(true)
{
    SetTexture();
};


CWindowObject::~CWindowObject(void)
{
    ;
};


void CWindowObject::SetSprite(float x, float y, float w, float h)
{
    m_aSprite[0].Move(x, y - ((h - 16.0f) * 0.5f) + 2.0f);
    m_aSprite[0].Resize(w, 16.0f);
    m_aSprite[0].SetUV(0.0f, 0.1f, 1.0f, 0.9f);

    m_aSprite[1].Move(x, y);
    m_aSprite[1].Resize(w, ((h - 16.0f) - 16.0f));
    m_aSprite[1].SetUV(0.0f, 0.1f, 1.0f, 0.9f);
    
    m_aSprite[2].Move(x, ((h - 16.0f) * 0.5f) + y - 2.0f);
    m_aSprite[2].Resize(w, 16.0f);
    m_aSprite[2].SetUV(0.0f, 0.1f, 1.0f, 0.9f);
};


void CWindowObject::DoneInput(void)
{
    m_phase = PHASE_ACTION;
};


void CWindowObject::DrawInWindow(const Rt2dBBox& bbox) const
{
    if (m_textTitle.Text())
    {
        Rt2dBBox bboxTitle;
        bboxTitle.x = bbox.x - bbox.w * 0.45f;
        bboxTitle.y = (bbox.h * 0.2f) - bbox.y;
        bboxTitle.w = bbox.w * 0.9f;
        bboxTitle.h = bbox.h * 0.2f;

        Rt2dBBox bboxMsg;
        bboxMsg.x = bbox.x - bbox.w * 0.45f;
        bboxMsg.y = bbox.h * -0.4f - bbox.y;
        bboxMsg.w = bbox.w * 0.9f;
        bboxMsg.h = bbox.h * 0.6f;

        m_textTitle.Draw(bboxTitle);
        m_textMsg.Draw(bboxMsg);
    }
    else if(m_textMsg.Text())
    {
        Rt2dBBox bboxMsg;
        bboxMsg.x = bbox.x - bbox.w * 0.45f;
        bboxMsg.y = bbox.h * -0.4f - bbox.y;
        bboxMsg.w = bbox.w * 0.9f;
        bboxMsg.h = bbox.h * 0.8f;

        m_textMsg.Draw(bboxMsg);
    };
};


void CWindowObject::Input(void)
{
    ;
};


void CWindowObject::InputAction(void)
{
    Close();
};


void CWindowObject::Period(void)
{
    switch (m_phase)
    {
    case PHASE_NONE:
        break;
        
    case PHASE_OPEN:
        Opening();        
        break;
        
    case PHASE_ACTIVE:
        Input();
        break;
        
    case PHASE_ACTION:
        InputAction();
        break;
        
    case PHASE_CLOSE:
        Closing();
        break;
        
    case PHASE_END:
        m_phase = PHASE_NONE;
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CWindowObject::Draw(void) const
{
    switch (m_phase)
    {    
    case PHASE_OPEN:
    case PHASE_ACTIVE:
    case PHASE_ACTION:
    case PHASE_CLOSE:
    case PHASE_END:
        CSystem2D::PushRenderState();
        m_aSprite[1].Draw();
        m_aSprite[0].Draw();
        m_aSprite[2].Draw();
        DrawInWindow(m_bboxDraw);
        CSystem2D::PopRenderState();
        break;

    default:
        break;
    };
};


void CWindowObject::Open(void)
{
    if (m_bOpenAction)
    {
        m_fTime = 0.0f;
        m_phase = PHASE_OPEN;
        SetPositionOnOpening();
    }
    else
    {
        m_phase = PHASE_ACTIVE;
        SetPosition(m_bbox);
    };
};


void CWindowObject::Close(void)
{
    if (m_bOpenAction)
    {
        m_fTime = 0.0f;
        m_phase = PHASE_CLOSE;
    }
    else
    {
        m_phase = PHASE_END;
    };
};


void CWindowObject::Opening(void)
{
    if (m_fTime >= OPENTIME)
    {
        SetPosition(m_bbox);
        m_phase = PHASE_ACTIVE;
    }
    else
    {
        SetPositionOnOpening();
        m_fTime += CScreen::TimerStride();
    };
};


void CWindowObject::Closing(void)
{
    if (m_fTime >= OPENTIME)
    {
        SetPosition(m_bbox);
		m_phase = PHASE_NONE;
    }
    else
    {
        SetPositionOnClosing();
        m_fTime += CScreen::TimerStride();
    };
};


void CWindowObject::SetPositionOnOpening(void)
{
    float t = ((OPENTIME - m_fTime) * 4.0f);
    
    m_bboxDraw.x = m_bbox.x;
    m_bboxDraw.y = m_bbox.y + (t * ((m_bbox.h + 240.0f - m_bbox.y) * t));
    m_bboxDraw.w = m_bbox.w;
    m_bboxDraw.h = m_bbox.h;

    SetSprite(m_bboxDraw.x,
              m_bboxDraw.y,
              m_bboxDraw.w,
              m_bboxDraw.h);
};


void CWindowObject::SetPositionOnClosing(void)
{
    float t = (m_fTime * 4.0f);

    m_bboxDraw.x = m_bbox.x;
    m_bboxDraw.y = m_bbox.y + (t * ((-240.0f - m_bbox.h - m_bbox.y) * t));
    m_bboxDraw.w = m_bbox.w;
    m_bboxDraw.h = m_bbox.h;

    SetSprite(m_bboxDraw.x,
              m_bboxDraw.y,
              m_bboxDraw.w,
              m_bboxDraw.h);
};


void CWindowObject::SetPosition(Rt2dBBox& bbox)
{
    SetPosition(bbox.x,
                bbox.y,
                bbox.w,
                bbox.h);
};


void CWindowObject::SetPosition(float x, float y, float w, float h)
{
    m_bboxDraw = { x, y, w, h };
    SetSprite(x, y, w, h);
};


void CWindowObject::Set(float x, float y, float w, float h)
{
    m_bbox = { x, y, w, h };    
};


void CWindowObject::SetTexture(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aSpriteTextureTable); ++i)
    {
        ASSERT(m_aSpriteTextureTable[i].m_pTexture);
        m_aSprite[i].SetTexture(m_aSpriteTextureTable[i].m_pTexture);
    };
};


void CWindowObject::SetTitle(const wchar* pwszText, float fHeight, const RwRGBA& rColor)
{
    m_textTitle.SetText(pwszText, fHeight, rColor);
};


void CWindowObject::SetTitle(const wchar* pwszText)
{
    m_textTitle.SetText(pwszText);
};


void CWindowObject::SetText(const wchar* pwszText, float fHeight, const RwRGBA& rColor)
{
    m_textMsg.SetText(pwszText, fHeight, rColor);
};


void CWindowObject::SetText(const wchar* pwszText)
{
    m_textMsg.SetText(pwszText);
};


void CWindowObject::SetOpenAction(bool bSet)
{
    m_bOpenAction = bSet;
};


bool CWindowObject::IsOpen(void) const
{
    return (m_phase >  PHASE_NONE) &&
           (m_phase <= PHASE_CLOSE);
};


bool CWindowObject::IsActive(void) const
{
    return (m_phase == PHASE_ACTIVE);
};

