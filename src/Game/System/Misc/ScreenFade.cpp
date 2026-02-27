#include "ScreenFade.hpp"

#include "Game/Component/Menu/MessageWindow.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Camera.hpp"


class CScreenFade::CScreenFadeController final
{
public:
    static void Color4B_Lerp(RwRGBA& result, const RwRGBA& c1, const RwRGBA& c2, float t);
    
    CScreenFadeController(void);
    ~CScreenFadeController(void);
    void Period(void);
    void Draw(void) const;
    void Start(float fFadeTime, bool bDraw);

    inline void SetColor(const RwRGBA& start, const RwRGBA& end) { m_colorStart = start;  m_colorEnd = end; };
    inline bool IsFading(void) const { return m_bIsFading; };
    inline bool IsDrawing(void) const { return (m_bIsFading || m_bDrawFlag); };

private:
    RwRGBA m_color;
    RwRGBA m_colorStart;
    RwRGBA m_colorEnd;
    float m_fFadeTime;
    float m_fTime;
    bool m_bIsFading;
    bool m_bDrawFlag;
};


/*static*/ void CScreenFade::CScreenFadeController::Color4B_Lerp(RwRGBA& result, const RwRGBA& c1, const RwRGBA& c2, float t)
{
	result.red      = RwUInt8(Math::Lerp(float(c1.red),     float(c2.red),      t));
	result.green    = RwUInt8(Math::Lerp(float(c1.green),   float(c2.green),    t));
	result.blue     = RwUInt8(Math::Lerp(float(c1.blue),    float(c2.blue),     t));
	result.alpha    = RwUInt8(Math::Lerp(float(c1.alpha),   float(c2.alpha),    t));
};


CScreenFade::CScreenFadeController::CScreenFadeController(void)
: m_color({})
, m_colorStart({})
, m_colorEnd({})
, m_fFadeTime(0.0f)
, m_fTime(0.0f)
, m_bIsFading(false)
, m_bDrawFlag(false)
{
    ;
};


CScreenFade::CScreenFadeController::~CScreenFadeController(void)
{
    ;
};


void CScreenFade::CScreenFadeController::Period(void)
{
    if (!IsFading())
        return;

    m_fTime     += CScreen::TimerStride();
    m_bIsFading = (m_fTime < m_fFadeTime);
    m_fTime     = Clamp(m_fTime, 0.0f, m_fFadeTime);

	if (m_fFadeTime == 0.0f)
		m_color = m_colorEnd;
    else
        Color4B_Lerp(m_color, m_colorStart, m_colorEnd, m_fTime / m_fFadeTime);    
};


void CScreenFade::CScreenFadeController::Draw(void) const
{
    if (!(IsFading() || IsDrawing()))
        return;

    RwCamera* camera = CCamera::CameraCurrent();

    if (camera == NULL)
        return;

    float x = static_cast<float>(CScreen::Width());
    float y = static_cast<float>(CScreen::Height());
	float z = RwIm2DGetNearScreenZ();
    float rhw = 1.0f / RwCameraGetNearClipPlane(camera);
    RwRGBA color = m_color;

    RwIm2DVertex aVertices[4];

    RwIm2DVertexSetScreenX(&aVertices[0], 0.0f);
    RwIm2DVertexSetScreenY(&aVertices[0], y);
    RwIm2DVertexSetScreenZ(&aVertices[0], z);
    RwIm2DVertexSetIntRGBA(&aVertices[0], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&aVertices[0], 0.0f, rhw);
    RwIm2DVertexSetV(&aVertices[0], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&aVertices[0], rhw);

    RwIm2DVertexSetScreenX(&aVertices[1], 0.0f);
    RwIm2DVertexSetScreenY(&aVertices[1], 0.0f);
    RwIm2DVertexSetScreenZ(&aVertices[1], z);
    RwIm2DVertexSetIntRGBA(&aVertices[1], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&aVertices[1], 0.0f, rhw);
    RwIm2DVertexSetV(&aVertices[1], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&aVertices[1], rhw);

    RwIm2DVertexSetScreenX(&aVertices[2], x);
    RwIm2DVertexSetScreenY(&aVertices[2], y);
    RwIm2DVertexSetScreenZ(&aVertices[2], z);
    RwIm2DVertexSetIntRGBA(&aVertices[2], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&aVertices[2], 0.0f, rhw);
    RwIm2DVertexSetV(&aVertices[2], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&aVertices[2], rhw);

    RwIm2DVertexSetScreenX(&aVertices[3], x);
    RwIm2DVertexSetScreenY(&aVertices[3], 0.0f);
    RwIm2DVertexSetScreenZ(&aVertices[3], z);
    RwIm2DVertexSetIntRGBA(&aVertices[3], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&aVertices[3], 0.0f, rhw);
    RwIm2DVertexSetV(&aVertices[3], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&aVertices[3], rhw);

    CSystem2D::PushRenderState();
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, aVertices, COUNT_OF(aVertices));
    CSystem2D::PopRenderState();
};


void CScreenFade::CScreenFadeController::Start(float fFadeTime, bool bDraw)
{
    m_fFadeTime = fFadeTime;
    m_fTime     = 0.0f;
    m_bIsFading = true;
    m_bDrawFlag = bDraw;
    m_color     = (m_fFadeTime == 0.0f) ? m_colorEnd : m_colorStart;
};


/*static*/ float CScreenFade::DEFAULT_FADE_TIME = 0.5f;
/*static*/ CScreenFade::CScreenFadeController* CScreenFade::m_pScreenFade = nullptr;


/*static*/ bool CScreenFade::Initialize(void)
{
    if (!m_pScreenFade)
        m_pScreenFade = new CScreenFadeController;

    return true;
};


/*static*/ void CScreenFade::Terminate(void)
{
    if (m_pScreenFade)
    {
        delete m_pScreenFade;
        m_pScreenFade = nullptr;
    };    
};


/*static*/ void CScreenFade::Period(void)
{
    if (m_pScreenFade)
        m_pScreenFade->Period();
};


/*static*/ void CScreenFade::Draw(void)
{
    if (m_pScreenFade)
        m_pScreenFade->Draw();
};


/*static*/ bool CScreenFade::IsFading(void)
{
    return (m_pScreenFade ? m_pScreenFade->IsFading() : false);
};


/*static*/ bool CScreenFade::IsDrawing(void)
{
    return (m_pScreenFade ? m_pScreenFade->IsDrawing() : false);
};


/*static*/ void CScreenFade::BlackIn(float fTime)
{
    if (m_pScreenFade)
    {
        RwRGBA start= { 0x00, 0x00, 0x00, 0xFF };
        RwRGBA end  = { 0x00, 0x00, 0x00, 0x00 };
        
        m_pScreenFade->SetColor(start, end);
        m_pScreenFade->Start(fTime, false);
    };
};


/*static*/ void CScreenFade::BlackOut(float fTime)
{
    if (m_pScreenFade)
    {
        RwRGBA start= { 0x00, 0x00, 0x00, 0x00 };
        RwRGBA end  = { 0x00, 0x00, 0x00, 0xFF };
        
        m_pScreenFade->SetColor(start, end);
        m_pScreenFade->Start(fTime, true);
    };
};


/*static*/ void CScreenFade::WhiteIn(float fTime)
{
    if (m_pScreenFade)
    {
        RwRGBA start= { 0xFF, 0xFF, 0xFF, 0xFF };
        RwRGBA end  = { 0xFF, 0xFF, 0xFF, 0x00 };
        
        m_pScreenFade->SetColor(start, end);
        m_pScreenFade->Start(fTime, false);
    };
};


/*static*/ void CScreenFade::WhiteOut(float fTime)
{
    if (m_pScreenFade)
    {
        RwRGBA start= { 0xFF, 0xFF, 0xFF, 0x00 };
        RwRGBA end  = { 0xFF, 0xFF, 0xFF, 0xFF };
        
        m_pScreenFade->SetColor(start, end);
        m_pScreenFade->Start(fTime, true);
    };
};


/*static*/ CProcess* CScreenFadeProcess::Instance(void)
{
    return new CScreenFadeProcess;
};


/*static*/ bool CScreenFadeProcess::Initialize(CProcess* pCurrent)
{
    return pCurrent->Mail().Send(PROCLABEL_SCREENFADE, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CScreenFadeProcess::Terminate(CProcess* pCurrent)
{
    pCurrent->Mail().Send(PROCLABEL_SCREENFADE, PROCESSTYPES::MAIL::TYPE_DETACH);
};


CScreenFadeProcess::CScreenFadeProcess(void)
{
    ;
};


CScreenFadeProcess::~CScreenFadeProcess(void)
{
    ;
};


bool CScreenFadeProcess::Attach(void)
{
    if (CScreenFade::Initialize())
        CMessageWindow::Initialize();
    
    return true;
};


void CScreenFadeProcess::Detach(void)
{
    CMessageWindow::Terminate();
    CScreenFade::Terminate();
};


void CScreenFadeProcess::Move(void)
{
    CScreenFade::Period();
    CMessageWindow::Move();
};


void CScreenFadeProcess::Draw(void) const
{
    CMessageWindow::DrawNormal();
    CScreenFade::Draw();
    CMessageWindow::DrawFront();
};