#include "ScreenFade.hpp"

#include "Game/Component/Menu/MessageWindow.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Process/ProcessList.hpp"


class CScreenFade::CHandler final
{
public:
    static const RwRGBA DEFAULT_COLOR;
    
public:
    CHandler(void);
    ~CHandler(void);
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


/*static*/ const RwRGBA CScreenFade::CHandler::DEFAULT_COLOR = { 0x00, 0x00, 0x00, 0xFF };


CScreenFade::CHandler::CHandler(void)
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


CScreenFade::CHandler::~CHandler(void)
{
    ;
};


void CScreenFade::CHandler::Move(void)
{
    if (!m_bIsFading)
        return;

    m_fTime += CScreen::TimerStride();
    m_bIsFading = (m_fTime < m_fFadeTime);
    m_fTime = Math::Clamp(m_fTime, 0.0f, m_fFadeTime);
    
    updateColor();
};


void CScreenFade::CHandler::Draw(void) const
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
    uint32 color = COLOR_TO_INTEGER(m_color.red, m_color.green, m_color.blue, m_AlphaBasis);

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


void CScreenFade::CHandler::Start(float fFadeTime, bool bOut)
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


void CScreenFade::CHandler::SetColor(uint8 r, uint8 g, uint8 b)
{
    m_color = { r, g, b, 0 };
};


bool CScreenFade::CHandler::IsFading(void)
{
    return m_bIsFading;
};


void CScreenFade::CHandler::updateColor(void)
{
	if (Math::FEqual(m_fFadeTime, 0.0f))
		m_AlphaBasis = m_AlphaBasisEnd;
	else
		m_AlphaBasis = uint8(Math::Lerp(float(m_AlphaBasisStart), float(m_AlphaBasisEnd), m_fTime / m_fFadeTime));
};


/*static*/ float CScreenFade::DEFAULT_FADE_TIME = 0.5f;


/*static*/ CProcess* CScreenFade::Instance(void)
{
    return new CScreenFade;
};


/*static*/ void CScreenFade::Initialize(void)
{
    CProcessDispatcher::AttachProcess(PROCESSTYPES::LABEL_SCREENFADE);
};


/*static*/ void CScreenFade::Terminate(void)
{
    CProcessDispatcher::DetachProcess(PROCESSTYPES::LABEL_SCREENFADE);
};


/*static*/ void CScreenFade::StartOut(float fFadeTime)
{
    Handler().Start(fFadeTime, false);
};


/*static*/ void CScreenFade::StartIn(float fFadeTime)
{
    Handler().Start(fFadeTime, true);
};


/*static*/ void CScreenFade::SetColor(uint8 r, uint8 g, uint8 b)
{
    Handler().SetColor(r, g, b);
};


/*static*/ void CScreenFade::ResetColor(void)
{
    Handler().SetColor(
        CScreenFade::CHandler::DEFAULT_COLOR.red,
        CScreenFade::CHandler::DEFAULT_COLOR.green,
        CScreenFade::CHandler::DEFAULT_COLOR.blue
    );
};


/*static*/ bool CScreenFade::IsFading(void)
{
    return Handler().IsFading();
};


/*static*/ CScreenFade* CScreenFade::m_pScreenFade = nullptr;


CScreenFade::CScreenFade(void)
: m_pHandler(nullptr)
{
    ASSERT(!m_pScreenFade);
    
    m_pScreenFade = this;
};


CScreenFade::~CScreenFade(void)
{
    ASSERT(!m_pHandler);
    ASSERT(m_pScreenFade);
    ASSERT(m_pScreenFade == this);
    
    m_pScreenFade = nullptr;
};


bool CScreenFade::Attach(void)
{
    m_pHandler = new CScreenFade::CHandler;
    return true;
};


void CScreenFade::Detach(void)
{
    if (m_pHandler)
    {
        delete m_pHandler;
        m_pHandler = nullptr;
    };
};


void CScreenFade::Move(void)
{
    ASSERT(m_pHandler);

    m_pHandler->Move();
    CMessageWindow::Move();
};


void CScreenFade::Draw(void) const
{
    ASSERT(m_pHandler);

    CMessageWindow::DrawNormal();
    m_pHandler->Draw();
    CMessageWindow::DrawFront();
};


/*static*/ CScreenFade::CHandler& CScreenFade::Handler(void)
{
    ASSERT(m_pScreenFade);
    ASSERT(m_pScreenFade->m_pHandler);

    return *m_pScreenFade->m_pHandler;
};