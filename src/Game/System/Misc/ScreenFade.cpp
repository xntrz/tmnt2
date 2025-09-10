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
: m_color({ 0 })
, m_colorStart({ 0 })
, m_colorEnd({ 0 })
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

    if (!CCamera::CameraCurrent())
        return;

    RwIm2DVertex aVertices[4];
    float w = float(CScreen::Width());
    float h = float(CScreen::Height());
	float z = RwIm2DGetNearScreenZ();
	float rhw = 1.0f / RwCameraGetNearClipPlane(CCamera::CameraCurrent());
    uint32 color = RWRGBALONG(m_color.red, m_color.green, m_color.blue, m_color.alpha);
    
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
    ASSERT(!m_pScreenFade);
    
    if (!m_pScreenFade)
        m_pScreenFade = new CScreenFadeController;

	return true;
};


/*static*/ void CScreenFade::Terminate(void)
{
    ASSERT(m_pScreenFade);

    if (m_pScreenFade)
    {
        delete m_pScreenFade;
        m_pScreenFade = nullptr;
    };    
};


/*static*/ void CScreenFade::Period(void)
{
    (m_pScreenFade ? m_pScreenFade->Period() : (void)0);
};


/*static*/ void CScreenFade::Draw(void)
{
    (m_pScreenFade ? m_pScreenFade->Draw() : (void)0);
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