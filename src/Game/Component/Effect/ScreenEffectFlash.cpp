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
    RwTexture*   m_pTexture;
    RwRGBA       m_color;
    uint8        m_uAlphaBasis;
    float        m_fTime;
    float        m_fDisplayTime;
    bool         m_bPlay;
};


CScrEffectFlash::CScrEffectFlash(void)
: m_pTexture(nullptr)
, m_color({ 0xFF, 0xFF, 0xFF, 0x80 })
, m_uAlphaBasis(0xFF)
, m_fTime(0.0f)
, m_fDisplayTime(0.3f)
, m_bPlay(false)
{
    float w = static_cast<float>(CScreen::Width());
    float h = static_cast<float>(CScreen::Height());
    float z = RwIm2DGetNearScreenZ();
    float rhw = 1.0f;

    RwRGBA color = m_color;
    color.alpha = m_uAlphaBasis;

    RwIm2DVertexSetScreenX(&m_aVertex[0], 0.0f);
    RwIm2DVertexSetScreenY(&m_aVertex[0], h);
    RwIm2DVertexSetScreenZ(&m_aVertex[0], z);
    RwIm2DVertexSetIntRGBA(&m_aVertex[0], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&m_aVertex[0], 0.0f, rhw);
    RwIm2DVertexSetV(&m_aVertex[0], 1.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&m_aVertex[0], rhw);

    RwIm2DVertexSetScreenX(&m_aVertex[1], w);
    RwIm2DVertexSetScreenY(&m_aVertex[1], h);
    RwIm2DVertexSetScreenZ(&m_aVertex[1], z);
    RwIm2DVertexSetIntRGBA(&m_aVertex[1], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&m_aVertex[1], 1.0f, rhw);
    RwIm2DVertexSetV(&m_aVertex[1], 1.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&m_aVertex[1], rhw);

    RwIm2DVertexSetScreenX(&m_aVertex[2], 0.0f);
    RwIm2DVertexSetScreenY(&m_aVertex[2], 0.0f);
    RwIm2DVertexSetScreenZ(&m_aVertex[2], z);
    RwIm2DVertexSetIntRGBA(&m_aVertex[2], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&m_aVertex[2], 0.0f, rhw);
    RwIm2DVertexSetV(&m_aVertex[2], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&m_aVertex[2], rhw);

    RwIm2DVertexSetScreenX(&m_aVertex[3], w);
    RwIm2DVertexSetScreenY(&m_aVertex[3], 0.0f);
    RwIm2DVertexSetScreenZ(&m_aVertex[3], z);
    RwIm2DVertexSetIntRGBA(&m_aVertex[3], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&m_aVertex[3], 1.0f, rhw);
    RwIm2DVertexSetV(&m_aVertex[3], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&m_aVertex[3], rhw);
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

        float ratio = ((m_fDisplayTime - m_fTime) / m_fDisplayTime);
        float alpha = ratio * static_cast<float>(m_uAlphaBasis);

        m_uAlphaBasis = static_cast<uint8>(alpha);

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
        RwRGBA color = m_color;
        color.alpha = m_uAlphaBasis;

        RwIm2DVertexSetIntRGBA(&m_aVertex[i], color.red, color.green, color.blue, color.alpha);
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
    m_uAlphaBasis = m_color.alpha;
    m_fTime = 0.0f;
    m_bPlay = true;
};


void CScrEffectFlash::SetColor(const RwRGBA& color)
{
    m_color = color;
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
        s_pScrEffectFlash = new CScrEffectFlash;
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