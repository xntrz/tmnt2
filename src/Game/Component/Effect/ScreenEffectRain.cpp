#include "ScreenEffectRain.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Camera.hpp"


class CScrEffectRain
{
private:
    static const int32 PARTICLE_NUM = 30;

    struct PARTICLE
    {
        RwV3d m_vPosition;
        RwV2d m_vTailPosition;
        RwV2d m_vVelocity;
        float m_fLength;
        float m_fTime;
        float m_fLifespan;
        RwRGBA m_Color;
    };
    
public:
    CScrEffectRain(void);
    ~CScrEffectRain(void);
    void Run(void);
    void Draw(void);
    void Generate(PARTICLE* pParticle);
    
private:
    PARTICLE     m_aParticle[PARTICLE_NUM];
    RwIm2DVertex m_aVertex[PARTICLE_NUM * 2];
    int32        m_nDisplayNum;
    float        m_fLength;
    RwRGBA       m_color;
};


class CScrEffectRainBack
{
private:
    static const int32 PARTICLE_NUM = 120;

    struct PARTICLE
    {
        RwV2d m_vPosition;
        RwV2d m_vVelocity;
        RwV2d m_vSize;
        uint8 m_uAlphaBasis;
        float m_fTime;
        float m_fLifespan;
        bool m_bFront;
    };
    
public:
    CScrEffectRainBack(void);
    ~CScrEffectRainBack(void);
    void Run(void);
    void Draw(void);
    void Generate(PARTICLE* pParticle);
    void SetVertex(PARTICLE* pParticle, RwIm2DVertex* pVertex);
    void SetTexture(RwTexture* pTexture);
    
private:
    PARTICLE     m_aParticle[PARTICLE_NUM];
    RwIm2DVertex m_aVertex[PARTICLE_NUM * 6];
    int32        m_nDisplayNum;
    RwV2d        m_vVelocityMax;
    float        m_fWidth;
    float        m_fHeight;
    float        m_fRandomW;
    float        m_fRandomH;
    RwTexture*   m_pTexture;
    RwRGBA       m_color;
};


CScrEffectRain::CScrEffectRain(void)
: m_aParticle()
, m_aVertex()
, m_nDisplayNum(COUNT_OF(m_aParticle))
, m_fLength(0.0f)
, m_color({ 0xEE, 0xEE, 0xEE, 0x28 })
{
    std::memset(m_aParticle, 0x00, sizeof(m_aParticle));
    std::memset(m_aVertex, 0x00, sizeof(m_aVertex));

    float sw = static_cast<float>(CScreen::Width());
    float vsw = TYPEDEF::VSCR_W;

    m_fLength = (sw / vsw) * 150.0f;

    for (int32 i = 0; i < m_nDisplayNum; ++i)
        Generate(&m_aParticle[i]);
};


CScrEffectRain::~CScrEffectRain(void)
{
    ;
};


void CScrEffectRain::Run(void)
{
    float dt = CGameProperty::GetElapsedTime();

    for (int32 i = 0; i < m_nDisplayNum; ++i)
    {
        PARTICLE* pParticle = &m_aParticle[i];

        pParticle->m_fTime += dt;

        if ((pParticle->m_vPosition.x <= 0.0f) ||
            (pParticle->m_vPosition.x >= static_cast<float>(CScreen::Width())) ||
            (pParticle->m_vPosition.y >= static_cast<float>(CScreen::Height())) ||
            (pParticle->m_fTime >= pParticle->m_fLifespan))
        {
            Generate(pParticle);
        };

        pParticle->m_vPosition.x += pParticle->m_vVelocity.x;
        pParticle->m_vPosition.y += pParticle->m_vVelocity.y;

        RwV2d vBuff = Math::VECTOR2_ZERO;
        Math::Vec2_Normalize(&vBuff, &pParticle->m_vVelocity);
        Math::Vec2_Scale(&vBuff, &vBuff, (pParticle->m_fLength * -1.0f));

        pParticle->m_vTailPosition.x = pParticle->m_vPosition.x + vBuff.x;
        pParticle->m_vTailPosition.y = pParticle->m_vPosition.y + vBuff.y;
    };
};


void CScrEffectRain::Draw(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    RwCamera* camera = CCamera::CameraCurrent();
    float rhw = 1.0f / RwCameraGetNearClipPlane(camera);

    float z = RwIm2DGetNearScreenZ();

    for (int32 i = 0, j = 0; i < m_nDisplayNum; ++i, j += 2)
    {
        ASSERT(j < COUNT_OF(m_aVertex));
        RwIm2DVertex* pVertex = &m_aVertex[j];

        PARTICLE* pParticle = &m_aParticle[i];
        RwRGBA color = pParticle->m_Color;

        RwIm2DVertexSetScreenX(&pVertex[0], pParticle->m_vPosition.x);
        RwIm2DVertexSetScreenY(&pVertex[0], pParticle->m_vPosition.y);
        RwIm2DVertexSetScreenZ(&pVertex[0], z);
        RwIm2DVertexSetIntRGBA(&pVertex[0], color.red, color.green, color.blue, color.alpha);
        RwIm2DVertexSetU(&pVertex[0], 0.0f, rhw);
        RwIm2DVertexSetV(&pVertex[0], 0.0f, rhw);
        RwIm2DVertexSetRecipCameraZ(&pVertex[0], rhw);

        RwIm2DVertexSetScreenX(&pVertex[1], pParticle->m_vTailPosition.x);
        RwIm2DVertexSetScreenY(&pVertex[1], pParticle->m_vTailPosition.y);
        RwIm2DVertexSetScreenZ(&pVertex[1], z);
        RwIm2DVertexSetIntRGBA(&pVertex[1], color.red, color.green, color.blue, color.alpha);
        RwIm2DVertexSetU(&pVertex[1], 0.0f, rhw);
        RwIm2DVertexSetV(&pVertex[1], 0.0f, rhw);
        RwIm2DVertexSetRecipCameraZ(&pVertex[1], rhw);
    };

    RwIm2DRenderPrimitive(rwPRIMTYPELINELIST, m_aVertex, m_nDisplayNum * 2);

    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};


void CScrEffectRain::Generate(PARTICLE* pParticle)
{
    ASSERT(pParticle);

    pParticle->m_vPosition.x =
        static_cast<float>(Math::Rand() % static_cast<uint32>(CScreen::Width()));
    
    pParticle->m_vPosition.y =
        static_cast<float>(Math::Rand() % static_cast<uint32>(CScreen::Height()));
    
    pParticle->m_vPosition.z =
        static_cast<float>(Math::Rand() % 3) + 1.0f;

    RwV2d vVelocity = Math::VECTOR2_ZERO;
    vVelocity.x = (static_cast<float>(Math::Rand() % 10) - 5.0f) * 0.1f;
    vVelocity.y = (static_cast<float>(Math::Rand() % 20) + 20.0f) * 0.1f;    
    Math::Vec2_Normalize(&vVelocity, &vVelocity);
    Math::Vec2_Scale(&vVelocity, &vVelocity, 60.0f);
    
    pParticle->m_vVelocity.x = vVelocity.x * (1.0f / pParticle->m_vPosition.z);
    pParticle->m_vVelocity.y = vVelocity.y * (1.0f / pParticle->m_vPosition.z);

    pParticle->m_fLength = (1.0f / pParticle->m_vPosition.z) * m_fLength;
    pParticle->m_fTime = 0.0f;
    pParticle->m_fLifespan = (static_cast<float>(Math::Rand() % 10) + 10.0f) * 0.01f;
    pParticle->m_Color = m_color;

    if (pParticle->m_vPosition.z >= 3.0f)
    {
        pParticle->m_Color.alpha -= 32;
    }
    else if (pParticle->m_vPosition.z >= 2.0f)
    {
        pParticle->m_Color.alpha -= 16;
    }
    else
    {
        pParticle->m_Color.alpha -= 8;
    };
};


CScrEffectRainBack::CScrEffectRainBack(void)
: m_aParticle()
, m_aVertex()
, m_nDisplayNum(COUNT_OF(m_aParticle))
, m_vVelocityMax(Math::VECTOR2_ZERO)
, m_fWidth(0.0f)
, m_fHeight(0.0f)
, m_fRandomW(0.0f)
, m_fRandomH(0.0f)
, m_pTexture(nullptr)
, m_color({ 0xFF, 0xFF, 0xFF, 0x23 })
{
    std::memset(m_aParticle, 0x00, sizeof(m_aParticle));
    std::memset(m_aVertex, 0x00, sizeof(m_aVertex));

    float sw = static_cast<float>(CScreen::Width());
    float sh = static_cast<float>(CScreen::Height());
    float vsw = TYPEDEF::VSCR_W;
    float vsh = TYPEDEF::VSCR_H;
    float scale_w = (sw / vsw);
    float scale_h = (sh / vsh);

    m_vVelocityMax.x    = scale_w * 0.0f;
    m_vVelocityMax.y    = scale_h * 1800.0f;    
    m_fWidth            = scale_w * 100.0f;
    m_fHeight           = scale_h * 300.0f;    
    m_fRandomW          = scale_w * 25.0f;
    m_fRandomH          = scale_h * 75.0f;

    for (int32 i = 0; i < m_nDisplayNum; ++i)
        Generate(&m_aParticle[i]);
};


CScrEffectRainBack::~CScrEffectRainBack(void)
{
    ;
};


void CScrEffectRainBack::Run(void)
{
    float dt = CGameProperty::GetElapsedTime();
    
    for (int32 i = 0; i < m_nDisplayNum; ++i)
    {
        PARTICLE* pParticle = &m_aParticle[i];

        pParticle->m_fTime += dt;

        if ((pParticle->m_vPosition.x <= 0.0f) ||
            (pParticle->m_vPosition.x >= static_cast<float>(CScreen::Width())) ||
            (pParticle->m_vPosition.y >= static_cast<float>(CScreen::Height())) ||
            (pParticle->m_fTime >= pParticle->m_fLifespan))
        {
            Generate(pParticle);
        };

        pParticle->m_vPosition.x += (pParticle->m_vVelocity.x * dt);
        pParticle->m_vPosition.y += (pParticle->m_vVelocity.y * dt);

        float ratio = ((pParticle->m_fLifespan - pParticle->m_fTime) / pParticle->m_fLifespan);
        float alpha = ratio * static_cast<float>(m_color.alpha);

        pParticle->m_uAlphaBasis = static_cast<uint8>(alpha);
    };
};


void CScrEffectRainBack::Draw(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    
    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
    
    for (int32 i = 0, j = 0; i < m_nDisplayNum; ++i, j += 6)
    {
        ASSERT(j < COUNT_OF(m_aVertex));
        RwIm2DVertex* pVertex = &m_aVertex[j];

        PARTICLE* pParticle = &m_aParticle[i];

        SetVertex(pParticle, pVertex);
    };

    RwIm2DRenderPrimitive(rwPRIMTYPETRILIST, m_aVertex, m_nDisplayNum * 6);

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};


void CScrEffectRainBack::Generate(PARTICLE* pParticle)
{
    ASSERT(pParticle);

    pParticle->m_vPosition.x = static_cast<float>(Math::Rand() % static_cast<uint32>(CScreen::Width()));
    pParticle->m_vPosition.y = 0.0f;

    float fScale = static_cast<float>(Math::Rand() % 130) * 0.01f + 0.2f;

    pParticle->m_vVelocity.x = (static_cast<float>(Math::Rand() % 150) - 75.0f) * fScale * fScale;
    pParticle->m_vVelocity.y = m_vVelocityMax.y * fScale;

	pParticle->m_vSize.x = (fScale >= 0.3f ? m_fWidth : (m_fWidth * 0.5f));
    pParticle->m_vSize.y = fScale * m_fHeight;

    pParticle->m_uAlphaBasis = m_color.alpha;
    pParticle->m_fTime = 0.0f;
	pParticle->m_fLifespan = (1.0f / fScale) * 0.2f;
    pParticle->m_bFront = ((Math::Rand() % 1) > 0);
};


void CScrEffectRainBack::SetVertex(PARTICLE* pParticle, RwIm2DVertex* pVertex)
{
    ASSERT(pParticle);
    ASSERT(pVertex);

    RwCamera* camera = CCamera::CameraCurrent();
    float rhw = 1.0f / RwCameraGetNearClipPlane(camera);

    float z = RwIm2DGetNearScreenZ();

    RwV2d aPos[2] = { Math::VECTOR2_ZERO };

    aPos[0].x = pParticle->m_vPosition.x + (-1.0f * (pParticle->m_vSize.x * 0.5f));
    aPos[0].y = pParticle->m_vPosition.y + (-1.0f * (pParticle->m_vSize.y * 0.5f));
    
    aPos[1].x = pParticle->m_vPosition.x + (pParticle->m_vSize.x * 0.5f);
    aPos[1].y = pParticle->m_vPosition.y + (pParticle->m_vSize.y * 0.5f);

    RwRGBA color = m_color;
    color.alpha = pParticle->m_uAlphaBasis;

    RwIm2DVertexSetScreenX(&pVertex[0], aPos[0].x);
    RwIm2DVertexSetScreenY(&pVertex[0], aPos[1].y);
    RwIm2DVertexSetScreenZ(&pVertex[0], z);
    RwIm2DVertexSetIntRGBA(&pVertex[0], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&pVertex[0], (pParticle->m_bFront ? 0.0f : 1.0f), rhw);
    RwIm2DVertexSetV(&pVertex[0], 1.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&pVertex[0], rhw);

    RwIm2DVertexSetScreenX(&pVertex[1], aPos[0].x);
    RwIm2DVertexSetScreenY(&pVertex[1], aPos[0].y);
    RwIm2DVertexSetScreenZ(&pVertex[1], z);
    RwIm2DVertexSetIntRGBA(&pVertex[1], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&pVertex[1], (pParticle->m_bFront ? 0.0f : 1.0f), rhw);
    RwIm2DVertexSetV(&pVertex[1], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&pVertex[1], rhw);

    RwIm2DVertexSetScreenX(&pVertex[2], aPos[1].x);
    RwIm2DVertexSetScreenY(&pVertex[2], aPos[1].y);
    RwIm2DVertexSetScreenZ(&pVertex[2], z);
    RwIm2DVertexSetIntRGBA(&pVertex[2], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&pVertex[2], (pParticle->m_bFront ? 1.0f : 0.0f), rhw);
    RwIm2DVertexSetV(&pVertex[2], 1.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&pVertex[2], rhw);

    RwIm2DVertexSetScreenX(&pVertex[3], aPos[1].x);
    RwIm2DVertexSetScreenY(&pVertex[3], aPos[1].y);
    RwIm2DVertexSetScreenZ(&pVertex[3], z);
    RwIm2DVertexSetIntRGBA(&pVertex[3], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&pVertex[3], (pParticle->m_bFront ? 1.0f : 0.0f), rhw);
    RwIm2DVertexSetV(&pVertex[3], 1.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&pVertex[3], rhw);

    RwIm2DVertexSetScreenX(&pVertex[4], aPos[0].x);
    RwIm2DVertexSetScreenY(&pVertex[4], aPos[0].y);
    RwIm2DVertexSetScreenZ(&pVertex[4], z);
    RwIm2DVertexSetIntRGBA(&pVertex[4], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&pVertex[4], (pParticle->m_bFront ? 0.0f : 1.0f), rhw);
    RwIm2DVertexSetV(&pVertex[4], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&pVertex[4], rhw);

    RwIm2DVertexSetScreenX(&pVertex[5], aPos[1].x);
    RwIm2DVertexSetScreenY(&pVertex[5], aPos[0].y);
    RwIm2DVertexSetScreenZ(&pVertex[5], z);
    RwIm2DVertexSetIntRGBA(&pVertex[5], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetU(&pVertex[5], (pParticle->m_bFront ? 1.0f : 0.0f), rhw);
    RwIm2DVertexSetV(&pVertex[5], 0.0f, rhw);
    RwIm2DVertexSetRecipCameraZ(&pVertex[5], rhw);
};


void CScrEffectRainBack::SetTexture(RwTexture* pTexture)
{
    ASSERT(pTexture);
    m_pTexture = pTexture;
};


static CScrEffectRain* s_pScrEffectRain = nullptr;
static CScrEffectRainBack* s_pScrEffectRainBack = nullptr;
static bool s_bActive = false;


static inline CScrEffectRain& ScreenEffectRain(void)
{
    ASSERT(s_pScrEffectRain);
    return *s_pScrEffectRain;
};


static inline CScrEffectRainBack& ScreenEffectRainBack(void)
{
    ASSERT(s_pScrEffectRainBack);
    return *s_pScrEffectRainBack;
};


/*static*/ void CScreenEffectRain::Initialize(void)
{
    if (!s_pScrEffectRain)
        s_pScrEffectRain = new CScrEffectRain;
        
    if (!s_pScrEffectRainBack)
        s_pScrEffectRainBack = new CScrEffectRainBack;

    StopRain();
};


/*static*/ void CScreenEffectRain::Terminate(void)
{
    if (s_pScrEffectRainBack)
    {
        delete s_pScrEffectRainBack;
        s_pScrEffectRainBack = nullptr;
    };

    if (s_pScrEffectRain)
    {
        delete s_pScrEffectRain;
        s_pScrEffectRain = nullptr;
    };
};


/*static*/ void CScreenEffectRain::Run(void)
{
    if (s_bActive)
    {
        ScreenEffectRainBack().Run();
        ScreenEffectRain().Run();
    };
};


/*static*/ void CScreenEffectRain::Draw(void)
{
    if (s_bActive)
    {
        ScreenEffectRainBack().Draw();
        ScreenEffectRain().Draw();
    };
};


/*static*/ void CScreenEffectRain::SetRain(void)
{
    CTextureManager::SetCurrentTextureSet("common_ef");
    RwTexture* pTexture = CTextureManager::GetRwTexture("rainline");
    if (!pTexture)
    {
        CTextureManager::SetCurrentTextureSet("rain_ef");
        pTexture = CTextureManager::GetRwTexture("rainline");
    };

    ASSERT(pTexture);
    ScreenEffectRainBack().SetTexture(pTexture);
    StartRain();
};


/*static*/ void CScreenEffectRain::StartRain(void)
{
    s_bActive = true;
};


/*static*/ void CScreenEffectRain::StopRain(void)
{
    s_bActive = false;
};