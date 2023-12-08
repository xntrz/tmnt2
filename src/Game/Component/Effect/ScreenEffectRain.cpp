#include "ScreenEffectRain.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Camera.hpp"


class CScrEffectRain
{
private:
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
    PARTICLE m_aParticle[30];
    RwIm2DVertex m_aVertex[30 * 2];
    int32 m_nDisplayNum;
    float m_fLength;
    RwRGBA m_Color;
};


class CScrEffectRainBack
{
private:
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
    PARTICLE m_aParticle[120];
    RwIm2DVertex m_aVertex[120 * 6];
    int32 m_nDisplayNum;
    RwV2d m_vVelocityMax;
    float m_fWidth;
    float m_fHeight;
    float m_fRandomW;
    float m_fRandomH;
    RwTexture* m_pTexture;
    RwRGBA m_Color;
};


CScrEffectRain::CScrEffectRain(void)
: m_nDisplayNum(30)
, m_fLength(0.0f)
, m_Color({ 0xEE, 0xEE, 0xEE, 0x28 })
{
    std::memset(m_aParticle, 0x00, sizeof(m_aParticle));
    std::memset(m_aVertex, 0x00, sizeof(m_aVertex));

    float sw = float(CScreen::Width());
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
    for (int32 i = 0; i < m_nDisplayNum; ++i)
    {
        PARTICLE* pParticle = &m_aParticle[i];

        pParticle->m_fTime += CGameProperty::GetElapsedTime();
        if (pParticle->m_vPosition.x <= 0.0f ||
            pParticle->m_vPosition.x >= float(CScreen::Width()) ||
            pParticle->m_vPosition.y >= float(CScreen::Height()) ||
            pParticle->m_fTime >= pParticle->m_fLifespan)
        {
            Generate(pParticle);
        };

        pParticle->m_vPosition.x += pParticle->m_vVelocity.x;
        pParticle->m_vPosition.y += pParticle->m_vVelocity.y;

        RwV2d vBuff = Math::VECTOR2_ZERO;
        Math::Vec2_Normalize(&vBuff, &pParticle->m_vVelocity);

        vBuff.x *= (pParticle->m_fLength * -1.0f);
        vBuff.y *= (pParticle->m_fLength * -1.0f);

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

    float z = RwIm2DGetNearScreenZ();
    float rhw = 1.0f / RwCameraGetNearClipPlane(CCamera::CameraCurrent());

    for (int32 i = 0, j = 0; i < m_nDisplayNum; ++i, j += 2)
    {
        ASSERT(j < COUNT_OF(m_aVertex));

        PARTICLE* pParticle = &m_aParticle[i];
        RwIm2DVertex* pVertex = &m_aVertex[j];

        pVertex[0].x = pParticle->m_vPosition.x;
        pVertex[0].y = pParticle->m_vPosition.y;
        pVertex[0].z = z;
        pVertex[0].u = 0.0f;
        pVertex[0].v = 0.0f;
        pVertex[0].rhw = rhw;
        pVertex[0].emissiveColor = RWRGBALONGEX(pParticle->m_Color);

        pVertex[1].x = pParticle->m_vTailPosition.x;
        pVertex[1].y = pParticle->m_vTailPosition.y;
        pVertex[1].z = z;
        pVertex[1].u = 0.0f;
        pVertex[1].v = 0.0f;
        pVertex[1].rhw = rhw;
        pVertex[1].emissiveColor = RWRGBALONGEX(pParticle->m_Color);
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

    pParticle->m_vPosition.x = float(Math::Rand() % uint32(CScreen::Width()));
    pParticle->m_vPosition.y = float(Math::Rand() % uint32(CScreen::Height()));
    pParticle->m_vPosition.z = float(Math::Rand() % 3) + 1.0f;

    RwV2d vVelocity = Math::VECTOR2_ZERO;
    vVelocity.x = (float(Math::Rand() % 10) - 5.0f) * 0.1f;
    vVelocity.y = (float(Math::Rand() % 20) + 20.0f) * 0.1f;
    
    Math::Vec2_Normalize(&vVelocity, &vVelocity);
    
    vVelocity.x *= 60.0f;
    vVelocity.y *= 60.0f;
    
    pParticle->m_vVelocity.x = vVelocity.x * (1.0f / pParticle->m_vPosition.z);
    pParticle->m_vVelocity.y = vVelocity.y * (1.0f / pParticle->m_vPosition.z);

    pParticle->m_fLength = (1.0f / pParticle->m_vPosition.z) * m_fLength;
    pParticle->m_fTime = 0.0f;
    pParticle->m_fLifespan = (float(Math::Rand() % 10) + 10.0f) * 0.01f;
    pParticle->m_Color = m_Color;

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
: m_nDisplayNum(120)
, m_vVelocityMax(Math::VECTOR2_ZERO)
, m_fWidth(0.0f)
, m_fHeight(0.0f)
, m_fRandomW(0.0f)
, m_fRandomH(0.0f)
, m_pTexture(nullptr)
, m_Color({ 0xFF, 0xFF, 0xFF, 0x23 })
{
    std::memset(m_aParticle, 0x00, sizeof(m_aParticle));
    std::memset(m_aVertex, 0x00, sizeof(m_aVertex));

    float sw = float(CScreen::Width());
    float sh = float(CScreen::Height());
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
        if (pParticle->m_vPosition.x <= 0.0f ||
            pParticle->m_vPosition.x >= float(CScreen::Width()) ||
            pParticle->m_vPosition.y >= float(CScreen::Height()) ||
            pParticle->m_fTime >= pParticle->m_fLifespan)
        {
            Generate(pParticle);
        };

        pParticle->m_vPosition.x += (pParticle->m_vVelocity.x * dt);
        pParticle->m_vPosition.y += (pParticle->m_vVelocity.y * dt);

        pParticle->m_uAlphaBasis =
            uint8(float(m_Color.alpha) * ((pParticle->m_fLifespan - pParticle->m_fTime) / pParticle->m_fLifespan));
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

        PARTICLE* pParticle = &m_aParticle[i];
        RwIm2DVertex* pVertex = &m_aVertex[j];

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

    pParticle->m_vPosition.x = float(Math::Rand() % uint32(CScreen::Width()));
    pParticle->m_vPosition.y = 0.0f;

    float fScale = float(Math::Rand() % 130) * 0.01f + 0.2f;

    pParticle->m_vVelocity.x = (float(Math::Rand() % 150) - 75.0f) * fScale * fScale;
    pParticle->m_vVelocity.y = m_vVelocityMax.y * fScale;

	pParticle->m_vSize.x = (fScale >= 0.3f ? m_fWidth : (m_fWidth * 0.5f));
    pParticle->m_vSize.y = fScale * m_fHeight;

    pParticle->m_uAlphaBasis = m_Color.alpha;
    pParticle->m_fTime = 0.0f;
	pParticle->m_fLifespan = (1.0f / fScale) * 0.2f;
    pParticle->m_bFront = bool((Math::Rand() % 1) > 0);
};


void CScrEffectRainBack::SetVertex(PARTICLE* pParticle, RwIm2DVertex* pVertex)
{
    ASSERT(pParticle);
    ASSERT(pVertex);

    RwCamera* pCamera = CCamera::CameraCurrent();
    ASSERT(pCamera);

    float z = RwIm2DGetNearScreenZ();
    float rhw = 1.0f / RwCameraGetNearClipPlane(CCamera::CameraCurrent());

    RwV2d aPos[2] = { Math::VECTOR2_ZERO };

    aPos[0].x = pParticle->m_vPosition.x + (-1.0f * (pParticle->m_vSize.x * 0.5f));
    aPos[0].y = pParticle->m_vPosition.y + (-1.0f * (pParticle->m_vSize.y * 0.5f));
    
    aPos[1].x = pParticle->m_vPosition.x + (pParticle->m_vSize.x * 0.5f);
    aPos[1].y = pParticle->m_vPosition.y + (pParticle->m_vSize.y * 0.5f);

    RwRGBA color = m_Color;
    color.alpha = pParticle->m_uAlphaBasis;

    pVertex[0].x = aPos[0].x;
    pVertex[0].y = aPos[1].y;
    pVertex[0].z = z;
    pVertex[0].u = (pParticle->m_bFront ? 0.0f : 1.0f);
    pVertex[0].v = 1.0f;
    pVertex[0].rhw = rhw;
    pVertex[0].emissiveColor = RWRGBALONGEX(color);

    pVertex[1].x = aPos[0].x;
    pVertex[1].y = aPos[0].y;
    pVertex[1].z = z;
    pVertex[1].u = (pParticle->m_bFront ? 0.0f : 1.0f);    
    pVertex[1].v = 0.0f;
    pVertex[1].rhw = rhw;
    pVertex[1].emissiveColor = RWRGBALONGEX(color);

    pVertex[2].x = aPos[1].x;
    pVertex[2].y = aPos[1].y;
    pVertex[2].z = z;
    pVertex[2].u = (pParticle->m_bFront ? 1.0f : 0.0f);    
    pVertex[2].v = 1.0f;
    pVertex[2].rhw = rhw;
    pVertex[2].emissiveColor = RWRGBALONGEX(color);

    pVertex[3].x = aPos[1].x;
    pVertex[3].y = aPos[1].y;
    pVertex[3].z = z;
    pVertex[3].u = (pParticle->m_bFront ? 1.0f : 0.0f);    
    pVertex[3].v = 1.0f;
    pVertex[3].rhw = rhw;
    pVertex[3].emissiveColor = RWRGBALONGEX(color);

    pVertex[4].x = aPos[0].x;
    pVertex[4].y = aPos[0].y;
    pVertex[4].z = z;
    pVertex[4].u = (pParticle->m_bFront ? 0.0f : 1.0f);    
    pVertex[4].v = 0.0f;
    pVertex[4].rhw = rhw;
    pVertex[4].emissiveColor = RWRGBALONGEX(color);

    pVertex[5].x = aPos[1].x;
    pVertex[5].y = aPos[0].y;
    pVertex[5].z = z;
    pVertex[5].u = (pParticle->m_bFront ? 1.0f : 0.0f);    
    pVertex[5].v = 0.0f;
    pVertex[5].rhw = rhw;
    pVertex[5].emissiveColor = RWRGBALONGEX(color);
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