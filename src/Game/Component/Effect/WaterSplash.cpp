#include "WaterSplash.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Camera.hpp"


class CWaterSplash
{
private:
    struct WATERSPLASH
    {
        RwV3d m_vPosition;
        uint8 m_uAlphaBasis;
        float m_fBaseScale;
        float m_fScale;
        float m_fTime;
        float m_fLifespan;
    };
    
public:
    CWaterSplash(void);
    ~CWaterSplash(void);
    void Run(void);
    void Draw(void);
    void SetBasisPosition(const RwV3d* pvPosition);
    void SetTexture(RwTexture* pTexture);
    void Generate(WATERSPLASH* pWaterSplash);
    void SetVertex(WATERSPLASH* pWaterSplash, RwMatrix* pMatrix, RwIm3DVertex* pVertex, RwCamera* pCamera);    
    
private:
    WATERSPLASH m_aWaterSplash[100];
    int32 m_nNumWaterSplash;
    RwIm3DVertex m_aVertex[100 * 6];
    RwV3d m_vBasisPosition;
    float m_fRadius;
    RwTexture* m_pTexture;
    RwRGBA m_Color;
    RwV2d m_vBaseSize;
};


CWaterSplash::CWaterSplash(void)
: m_nNumWaterSplash(100)
, m_vBasisPosition(Math::VECTOR3_ZERO)
, m_fRadius(20.0f)
, m_pTexture(nullptr)
, m_Color({ 0xFF, 0xFF,0xFF, 0x78 })
, m_vBaseSize({ 0.2f, 0.2f })
{
    std::memset(m_aWaterSplash, 0x00, sizeof(m_aWaterSplash));
    std::memset(m_aVertex, 0x00, sizeof(m_aVertex));
    
    for (int32 i = 0; i < m_nNumWaterSplash; ++i)
        Generate(&m_aWaterSplash[i]);
};


CWaterSplash::~CWaterSplash(void)
{
    ;
};


void CWaterSplash::Run(void)
{
    for (int32 i = 0; i < m_nNumWaterSplash; ++i)
    {
        WATERSPLASH* pWaterSplash = &m_aWaterSplash[i];

        pWaterSplash->m_fTime += CGameProperty::GetElapsedTime();
        
        if (pWaterSplash->m_fTime >= pWaterSplash->m_fLifespan)
            Generate(pWaterSplash);

        pWaterSplash->m_uAlphaBasis =
            uint8((pWaterSplash->m_fLifespan - pWaterSplash->m_fTime) * 1.0f / pWaterSplash->m_fLifespan * float(m_Color.alpha));

        pWaterSplash->m_fScale =
            pWaterSplash->m_fBaseScale + (pWaterSplash->m_fBaseScale * (pWaterSplash->m_fTime / pWaterSplash->m_fLifespan));
    };
};


void CWaterSplash::Draw(void)
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

    RwMatrix billboardMatrix;
    RwMatrixSetIdentityMacro(&billboardMatrix);

    RwMatrixInvert(&billboardMatrix, RwCameraGetViewMatrixMacro(CCamera::CameraCurrent()));

    for (int32 i = 0, j = 0; i < m_nNumWaterSplash; ++i, j += 6)
    {
        WATERSPLASH* pWaterSplash = &m_aWaterSplash[i];
        RwIm3DVertex* pVertex = &m_aVertex[j];
        
        SetVertex(pWaterSplash, &billboardMatrix, pVertex, CCamera::CameraCurrent());        
    };

    uint32 uFlags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV;

    if (RwIm3DTransform(m_aVertex, m_nNumWaterSplash * 6, nullptr, uFlags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
        RwIm3DEnd();
    };

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};


void CWaterSplash::SetBasisPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    m_vBasisPosition = *pvPosition;
};


void CWaterSplash::SetTexture(RwTexture* pTexture)
{
    ASSERT(pTexture);

    m_pTexture = pTexture;
};


void CWaterSplash::Generate(WATERSPLASH* pWaterSplash)
{
    RwV3d vAppearPosition = m_vBasisPosition;
    float fScale = (float(Math::Rand() % 50) * 0.01f + 0.5f);
    float fLifespan = (float(Math::Rand() % 10) * 0.01f + 0.1f);

    vAppearPosition.x += (float(Math::Rand() % uint32(m_fRadius * 20.0f)) - m_fRadius * 10.0f) * 0.1f;
    vAppearPosition.z += (float(Math::Rand() % uint32(m_fRadius * 20.0f)) - m_fRadius * 10.0f) * 0.1f;
    float fMapHeight = CWorldMap::GetMapHeight(&vAppearPosition);
    vAppearPosition.y = fMapHeight;

    pWaterSplash->m_vPosition = vAppearPosition;
    pWaterSplash->m_fBaseScale = fScale;
    pWaterSplash->m_fScale = fScale;
    pWaterSplash->m_fLifespan = fLifespan;
	pWaterSplash->m_fTime = 0.0f;
    pWaterSplash->m_uAlphaBasis = m_Color.alpha;
};


void CWaterSplash::SetVertex(WATERSPLASH* pWaterSplash, RwMatrix* pMatrix, RwIm3DVertex* pVertex, RwCamera* pCamera)
{
    ASSERT(pWaterSplash);
    ASSERT(pMatrix);
    ASSERT(pVertex);
    ASSERT(pCamera);

    RwV2d vNowSize;
    vNowSize.x = (m_vBaseSize.x * pWaterSplash->m_fScale);
    vNowSize.y = (m_vBaseSize.y * pWaterSplash->m_fScale);
    
    RwRGBA color = m_Color;
    color.alpha = pWaterSplash->m_uAlphaBasis;

    float w = float(pCamera->frameBuffer->width);
    float h = float(pCamera->frameBuffer->height);

    float x = vNowSize.x * 0.5f;
    float y = ((w / h) * vNowSize.y) * 0.5f;

    RwV3d aPosition[4] = { Math::VECTOR3_ZERO };

    aPosition[0].x = x * -1.0f;
    aPosition[0].y = y * -1.0f;
    aPosition[0].z = 0.0f;

    aPosition[1].x = x * -1.0f;
    aPosition[1].y = y;
    aPosition[1].z = 0.0f;

    aPosition[2].x = x;
    aPosition[2].y = y * -1.0f;
    aPosition[2].z = 0.0f;
    
    aPosition[3].x = x;
    aPosition[3].y = y;
    aPosition[3].z = 0.0f;

    RwV3dTransformVectors(aPosition, aPosition, COUNT_OF(aPosition), pMatrix);
    Math::Vec3_Add(&aPosition[0], &aPosition[0], &pWaterSplash->m_vPosition);
    Math::Vec3_Add(&aPosition[1], &aPosition[1], &pWaterSplash->m_vPosition);
    Math::Vec3_Add(&aPosition[2], &aPosition[2], &pWaterSplash->m_vPosition);
    Math::Vec3_Add(&aPosition[3], &aPosition[3], &pWaterSplash->m_vPosition);

    pVertex[0].objVertex = aPosition[1];
    pVertex[0].objNormal = Math::VECTOR3_ZERO;
    pVertex[0].color = RWRGBALONGEX(color);
    pVertex[0].u = 0.0f;
    pVertex[0].v = 1.0f;

    pVertex[1].objVertex = aPosition[0];
    pVertex[1].objNormal = Math::VECTOR3_ZERO;
    pVertex[1].color = RWRGBALONGEX(color);
    pVertex[1].u = 0.0f;
    pVertex[1].v = 0.0f;

    pVertex[2].objVertex = aPosition[3];
    pVertex[2].objNormal = Math::VECTOR3_ZERO;
    pVertex[2].color = RWRGBALONGEX(color);
    pVertex[2].u = 1.0f;
    pVertex[2].v = 1.0f;

    pVertex[3].objVertex = aPosition[3];
    pVertex[3].objNormal = Math::VECTOR3_ZERO;
    pVertex[3].color = RWRGBALONGEX(color);
    pVertex[3].u = 1.0f;
    pVertex[3].v = 1.0f;

    pVertex[4].objVertex = aPosition[0];
    pVertex[4].objNormal = Math::VECTOR3_ZERO;
    pVertex[4].color = RWRGBALONGEX(color);
    pVertex[4].u = 0.0f;
    pVertex[4].v = 0.0f;

    pVertex[5].objVertex = aPosition[2];
    pVertex[5].objNormal = Math::VECTOR3_ZERO;
    pVertex[5].color = RWRGBALONGEX(color);
    pVertex[5].u = 1.0f;
    pVertex[5].v = 0.0f;
};


static CWaterSplash* s_pWaterSplash = nullptr;
static bool s_bActive = false;


static inline CWaterSplash& WaterSplash(void)
{
    ASSERT(s_pWaterSplash);
    return *s_pWaterSplash;
};


/*static*/ void CWaterSplashManager::Initialize(void)
{
    if (!s_pWaterSplash)
    {
        s_pWaterSplash = new CWaterSplash;
        s_bActive = false;
    };
};


/*static*/ void CWaterSplashManager::Terminate(void)
{
    if (s_pWaterSplash)
    {
        delete s_pWaterSplash;
        s_pWaterSplash = nullptr;
    };
};


/*static*/ void CWaterSplashManager::Run(void)
{
    if(s_bActive)
        WaterSplash().Run();
};


/*static*/ void CWaterSplashManager::Draw(void)
{
    if(s_bActive)
        WaterSplash().Draw();
};


/*static*/ void CWaterSplashManager::StartWaterSplash(void)
{
    s_bActive = true;
};


/*static*/ void CWaterSplashManager::StopWaterSplash(void)
{
    s_bActive = false;
};


/*static*/ void CWaterSplashManager::SetBasisPosition(const RwV3d* pvPosition)
{
    WaterSplash().SetBasisPosition(pvPosition);
};


/*static*/ void CWaterSplashManager::SetDefaultTexture(void)
{
    CTextureManager::SetCurrentTextureSet("common_ef");
    RwTexture* pTexture = CTextureManager::GetRwTexture("raindrop");
    if (!pTexture)
    {
        CTextureManager::SetCurrentTextureSet("rain_ef");
        pTexture = CTextureManager::GetRwTexture("raindrop");
    };
    
    ASSERT(pTexture);

    WaterSplash().SetTexture(pTexture);
    StartWaterSplash();
};