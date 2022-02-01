#include "ScreenEffectFlash.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/RenderState.hpp"


class CScrEffectFlash
{
public:
    CScrEffectFlash(void);
    ~CScrEffectFlash(void);
    void Run(void);
    void Draw(void);
    void Play(void);
    void SetColor(const RwRGBA& color);
    
private:
    RwIm2DVertex m_aVertex[4];
    RwTexture* m_pTexture;
    RwRGBA m_Color;
    uint8 m_uAlphaBasis;
    float m_fTime;
    float m_fDisplayTime;
    bool m_bPlay;
};


CScrEffectFlash::CScrEffectFlash(void)
: m_pTexture(nullptr)
, m_Color({ 0xFF,0xFF,0xFF,0x80 })
, m_uAlphaBasis(0xFF)
, m_fTime(0.0f)
, m_fDisplayTime(0.3f)
, m_bPlay(false)
{
    float w = float(CScreen::Width());
    float h = float(CScreen::Height());
    float z = RwIm2DGetNearScreenZ();
    float rhw = 1.0f;
    
    m_aVertex[0].x = 0.0f;
    m_aVertex[0].y = h;
    m_aVertex[0].z = z;
    m_aVertex[0].u = 0.0f;
    m_aVertex[0].v = 1.0f;
    m_aVertex[0].rhw = rhw;

    m_aVertex[1].x = w;
    m_aVertex[1].y = h;
    m_aVertex[1].z = z;
    m_aVertex[1].u = 1.0f;
    m_aVertex[1].v = 1.0f;
    m_aVertex[1].rhw = rhw;

    m_aVertex[2].x = 0.0f;
    m_aVertex[2].y = 0.0f;
    m_aVertex[2].z = z;
    m_aVertex[2].u = 0.0f;
    m_aVertex[2].v = 0.0f;
    m_aVertex[2].rhw = rhw;

    m_aVertex[3].x = w;
    m_aVertex[3].y = 0.0f;
    m_aVertex[3].z = z;
    m_aVertex[3].u = 1.0f;
    m_aVertex[3].v = 0.0f;
    m_aVertex[3].rhw = rhw;
};


CScrEffectFlash::~CScrEffectFlash(void)
{
    ;
};


void CScrEffectFlash::Run(void)
{
    if (m_bPlay)
    {
        m_fTime += CGameProperty::GetElapsedTime();
        m_uAlphaBasis = uint8(((m_fDisplayTime - m_fTime) / m_fDisplayTime) * float(m_uAlphaBasis));

        if (m_fTime >= m_fDisplayTime)
        {
            m_uAlphaBasis = 0;
            m_fTime = 0.0f;
            m_bPlay = false;
        };
    };
};


void CScrEffectFlash::Draw(void)
{
    if (!m_bPlay)
        return;

    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);

    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    for (int32 i = 0; i < COUNT_OF(m_aVertex); ++i)
    {
        RwRGBA color = m_Color;
        color.alpha = m_uAlphaBasis;
        
        m_aVertex[i].emissiveColor = COLOR_TO_INTEGER_RWRGBA(color);
    };

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_aVertex, COUNT_OF(m_aVertex));

    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};


void CScrEffectFlash::Play(void)
{
    m_uAlphaBasis = m_Color.alpha;
    m_fTime = 0.0f;
    m_bPlay = true;
};


void CScrEffectFlash::SetColor(const RwRGBA& color)
{
    m_Color = color;
};


static CScrEffectFlash* s_pScrEffectFlash = nullptr;


static CScrEffectFlash& ScreenEffectFlash(void)
{
    ASSERT(s_pScrEffectFlash);
    return *s_pScrEffectFlash;
};


/*static*/ void CScreenEffectFlash::Initialize(void)
{
    if (!s_pScrEffectFlash)
    {
        s_pScrEffectFlash = new CScrEffectFlash;
    };
};


/*static*/ void CScreenEffectFlash::Terminate(void)
{
    if (s_pScrEffectFlash)
    {
        delete s_pScrEffectFlash;
        s_pScrEffectFlash = nullptr;
    };
};


/*static*/ void CScreenEffectFlash::Run(void)
{
    ScreenEffectFlash().Run();
};


/*static*/ void CScreenEffectFlash::Draw(void)
{
    ScreenEffectFlash().Draw();
};


/*static*/ void CScreenEffectFlash::Flash(void)
{
    ScreenEffectFlash().Play();
};


/*static*/ void CScreenEffectFlash::SetColor(const RwRGBA& color)
{
    ScreenEffectFlash().SetColor(color);
};