#include "ScreenFade.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Process/ProcessList.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "Game/Component/Menu/MessageWindow.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/ProcessList.hpp"


class CScreenFadeCtrl
{
public:
    static const RwRGBA DEFAULT_COLOR;
    
public:
    CScreenFadeCtrl(void);
    ~CScreenFadeCtrl(void);
    void Move(void);
    void Draw(void) const;
    void Start(float fFadeTime, bool bOut);
    void SetColor(uint8 r, uint8 g, uint8 b);
    bool IsFading(void);

private:
    void updateColor(void);

private:
    RwRGBA m_color;
    float m_fFadeTime;
    float m_fTime;
    uint8 m_AlphaBasis;
    uint8 m_AlphaBasisStart;
    uint8 m_AlphaBasisEnd;
    bool m_bIsFading;
};


/*static*/ const RwRGBA CScreenFadeCtrl::DEFAULT_COLOR = { 0x00, 0x00, 0x00, 0xFF };


CScreenFadeCtrl::CScreenFadeCtrl(void)
: m_color(DEFAULT_COLOR)
, m_fFadeTime(0.0f)
, m_fTime(0.0f)
, m_AlphaBasis(0)
, m_AlphaBasisStart(0)
, m_AlphaBasisEnd(0)
, m_bIsFading(false)
{
    ;
};


CScreenFadeCtrl::~CScreenFadeCtrl(void)
{
    ;
};


void CScreenFadeCtrl::Move(void)
{
    if (!m_bIsFading)
        return;

    m_fTime += CScreen::TimerStride();
    m_bIsFading = (m_fTime < m_fFadeTime);
    m_fTime = Math::Clamp(m_fTime, 0.0f, m_fFadeTime);
    
    updateColor();
};


void CScreenFadeCtrl::Draw(void) const
{
	if (!m_AlphaBasis)
		return;

    if (!CCamera::CameraCurrent())
        return;

    RwIm2DVertex aVertices[4];
    float w = float(CScreen::Width());
    float h = float(CScreen::Height());
	float z = RwIm2DGetNearScreenZ();
	float rhw = 1.0f / RwCameraGetNearClipPlane(CCamera::CameraCurrent());
    uint32 color = RWRGBALONG(m_color.red, m_color.green, m_color.blue, m_AlphaBasis);
    
    aVertices[0].x = 0.0f;
    aVertices[0].y = h;
    aVertices[0].z = z;
    aVertices[0].rhw = rhw;
    aVertices[0].emissiveColor = color;
	aVertices[0].u = 0.0f;
	aVertices[0].v = 0.0f;

    aVertices[1].x = 0.0f;
    aVertices[1].y = 0.0f;
    aVertices[1].z = z;
    aVertices[1].rhw = rhw;
    aVertices[1].emissiveColor = color;
	aVertices[1].u = 0.0f;
	aVertices[1].v = 0.0f;

    aVertices[2].x = w;
    aVertices[2].y = h;
    aVertices[2].z = z;
    aVertices[2].rhw = rhw;
    aVertices[2].emissiveColor = color;
	aVertices[2].u = 0.0f;
	aVertices[2].v = 0.0f;

    aVertices[3].x = w;
    aVertices[3].y = 0.0f;
    aVertices[3].z = z;
    aVertices[3].rhw = rhw;
    aVertices[3].emissiveColor = color;
	aVertices[3].u = 0.0f;
	aVertices[3].v = 0.0f;

    CSystem2D::PushRenderState();
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, aVertices, COUNT_OF(aVertices));
    CSystem2D::PopRenderState();
};


void CScreenFadeCtrl::Start(float fFadeTime, bool bOut)
{
    if (bOut)
    {
        m_AlphaBasisStart   = 0;
        m_AlphaBasisEnd     = 255;
    }
    else
    {
        m_AlphaBasisStart   = 255;
        m_AlphaBasisEnd     = 0;
    };
    
    m_bIsFading = true;
    m_AlphaBasis = 0;
    m_fFadeTime = fFadeTime;
    m_fTime = 0.0f;

    updateColor();
};


void CScreenFadeCtrl::SetColor(uint8 r, uint8 g, uint8 b)
{
    m_color = { r, g, b, 0 };
};


bool CScreenFadeCtrl::IsFading(void)
{
    return m_bIsFading;
};


void CScreenFadeCtrl::updateColor(void)
{
	if (Math::FEqual(m_fFadeTime, 0.0f))
		m_AlphaBasis = m_AlphaBasisEnd;
	else
		m_AlphaBasis = uint8(Math::Lerp(float(m_AlphaBasisStart), float(m_AlphaBasisEnd), m_fTime / m_fFadeTime));
};


static CScreenFadeCtrl* s_pScreenFadeCtrl = nullptr;


/*static*/ float CScreenFade::DEFAULT_FADE_TIME = 0.5f;


/*static*/ CProcess* CScreenFade::Instance(void)
{
    return new CScreenFade;
};


/*static*/ bool CScreenFade::Initialize(CProcess* pCurrent)
{
    return pCurrent->Mail().Send(PROCESSTYPES::LABEL_SCREENFADE, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CScreenFade::Terminate(CProcess* pCurrent)
{
    pCurrent->Mail().Send(PROCESSTYPES::LABEL_SCREENFADE, PROCESSTYPES::MAIL::TYPE_DETACH);
};


/*static*/ void CScreenFade::StartOut(float fFadeTime)
{
	if (s_pScreenFadeCtrl)
		s_pScreenFadeCtrl->Start(fFadeTime, false);
};


/*static*/ void CScreenFade::StartIn(float fFadeTime)
{
	if (s_pScreenFadeCtrl)
		s_pScreenFadeCtrl->Start(fFadeTime, true);
};


/*static*/ void CScreenFade::SetColor(uint8 r, uint8 g, uint8 b)
{
	if (s_pScreenFadeCtrl)
		s_pScreenFadeCtrl->SetColor(r, g, b);
};


/*static*/ void CScreenFade::ResetColor(void)
{
	if (s_pScreenFadeCtrl)
		s_pScreenFadeCtrl->SetColor(
			CScreenFadeCtrl::DEFAULT_COLOR.red,
			CScreenFadeCtrl::DEFAULT_COLOR.green,
			CScreenFadeCtrl::DEFAULT_COLOR.blue
		);
};


/*static*/ bool CScreenFade::IsFading(void)
{
	if (s_pScreenFadeCtrl)
		return s_pScreenFadeCtrl->IsFading();
	else
		return false;
};


CScreenFade::CScreenFade(void)
{
    ;
};


CScreenFade::~CScreenFade(void)
{
    ;
};


bool CScreenFade::Attach(void)
{
    s_pScreenFadeCtrl = new CScreenFadeCtrl;
    CMessageWindow::Initialize();
    return true;
};


void CScreenFade::Detach(void)
{
    CMessageWindow::Terminate();
    if (s_pScreenFadeCtrl)
    {
        delete s_pScreenFadeCtrl;
        s_pScreenFadeCtrl = nullptr;
    };
};


void CScreenFade::Move(void)
{
    s_pScreenFadeCtrl->Move();
    CMessageWindow::Move();
};


void CScreenFade::Draw(void) const
{
    CMessageWindow::DrawNormal();
    s_pScreenFadeCtrl->Draw();
    CMessageWindow::DrawFront();
};