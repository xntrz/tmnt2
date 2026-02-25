#include "WaterSplash.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Camera.hpp"


class CWaterSplash
{
private:
    static const int32 PARTICLE_NUM = 100;

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
    WATERSPLASH  m_aWaterSplash[PARTICLE_NUM];
    int32        m_nNumWaterSplash;
    RwIm3DVertex m_aVertex[PARTICLE_NUM * 6];
    RwV3d        m_vBasisPosition;
    float        m_fRadius;
    RwTexture*   m_pTexture;
    RwRGBA       m_color;
    RwV2d        m_vBaseSize;
};


CWaterSplash::CWaterSplash(void)
: m_nNumWaterSplash(COUNT_OF(m_aWaterSplash))
, m_vBasisPosition(Math::VECTOR3_ZERO)
, m_fRadius(20.0f)
, m_pTexture(nullptr)
, m_color({ 0xFF, 0xFF, 0xFF, 0x78 })
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
    float dt = CGameProperty::GetElapsedTime();

    for (int32 i = 0; i < m_nNumWaterSplash; ++i)
    {
        WATERSPLASH* pWaterSplash = &m_aWaterSplash[i];

        pWaterSplash->m_fTime += dt;
        
        if (pWaterSplash->m_fTime >= pWaterSplash->m_fLifespan)
            Generate(pWaterSplash);

        float ratioAlpha = (pWaterSplash->m_fLifespan - pWaterSplash->m_fTime) * (1.0f / pWaterSplash->m_fLifespan);
        float alpha = ratioAlpha * static_cast<float>(m_color.alpha);

        pWaterSplash->m_uAlphaBasis = static_cast<uint8>(alpha);

        float ratioScale = (pWaterSplash->m_fTime / pWaterSplash->m_fLifespan);
        float scale = pWaterSplash->m_fBaseScale + (pWaterSplash->m_fBaseScale * ratioScale);

        pWaterSplash->m_fScale = scale;
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

    RwCamera* camera = CCamera::CameraCurrent();
    RwMatrix* viewMat = RwCameraGetViewMatrix(camera);

    RwMatrix billboardMatrix;
    RwMatrixSetIdentity(&billboardMatrix);
    RwMatrixInvert(&billboardMatrix, viewMat);

    for (int32 i = 0, j = 0; i < m_nNumWaterSplash; ++i, j += 6)
    {
        WATERSPLASH* pWaterSplash = &m_aWaterSplash[i];
        RwIm3DVertex* pVertex = &m_aVertex[j];
        
        SetVertex(pWaterSplash, &billboardMatrix, pVertex, camera);
    };

    uint32 flags = rwIM3D_VERTEXRGBA
                 | rwIM3D_VERTEXXYZ
                 | rwIM3D_VERTEXUV;

    if (RwIm3DTransform(m_aVertex, m_nNumWaterSplash * 6, nullptr, flags))
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

    vAppearPosition.x +=
        (static_cast<float>(Math::Rand() % static_cast<uint32>(m_fRadius * 20.0f)) - m_fRadius * 10.0f) * 0.1f;

    vAppearPosition.z +=
        (static_cast<float>(Math::Rand() % static_cast<uint32>(m_fRadius * 20.0f)) - m_fRadius * 10.0f) * 0.1f;

    float fMapHeight = CWorldMap::GetMapHeight(&vAppearPosition);
    vAppearPosition.y = fMapHeight;

    float fScale    = (static_cast<float>(Math::Rand() % 50) * 0.01f + 0.5f);
    float fLifespan = (static_cast<float>(Math::Rand() % 10) * 0.01f + 0.1f);

    pWaterSplash->m_vPosition = vAppearPosition;
    pWaterSplash->m_fBaseScale = fScale;
    pWaterSplash->m_fScale = fScale;
    pWaterSplash->m_fLifespan = fLifespan;
	pWaterSplash->m_fTime = 0.0f;
    pWaterSplash->m_uAlphaBasis = m_color.alpha;
};


void CWaterSplash::SetVertex(WATERSPLASH* pWaterSplash, RwMatrix* pMatrix, RwIm3DVertex* pVertex, RwCamera* pCamera)
{
    ASSERT(pWaterSplash);
    ASSERT(pMatrix);
    ASSERT(pVertex);
    ASSERT(pCamera);
    
    RwV2d vNowSize = { (m_vBaseSize.x * pWaterSplash->m_fScale),
                       (m_vBaseSize.y * pWaterSplash->m_fScale) };

    RwRaster* framebuffer = RwCameraGetRaster(pCamera);
    float w = static_cast<float>(RwRasterGetWidth(framebuffer));
    float h = static_cast<float>(RwRasterGetHeight(framebuffer));

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

    RwRGBA color = m_color;
    color.alpha = pWaterSplash->m_uAlphaBasis;

    RwIm3DVertexSetPos(&pVertex[0], aPosition[1].x, aPosition[1].y, aPosition[1].z);
    RwIm3DVertexSetNormal(&pVertex[0], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[0], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[0], 0.0f);
    RwIm3DVertexSetV(&pVertex[0], 1.0f);

    RwIm3DVertexSetPos(&pVertex[1], aPosition[0].x, aPosition[0].y, aPosition[0].z);
    RwIm3DVertexSetNormal(&pVertex[1], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[1], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[1], 0.0f);
    RwIm3DVertexSetV(&pVertex[1], 0.0f);

    RwIm3DVertexSetPos(&pVertex[2], aPosition[3].x, aPosition[3].y, aPosition[3].z);
    RwIm3DVertexSetNormal(&pVertex[2], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[2], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[2], 1.0f);
    RwIm3DVertexSetV(&pVertex[2], 1.0f);

    RwIm3DVertexSetPos(&pVertex[3], aPosition[3].x, aPosition[3].y, aPosition[3].z);
    RwIm3DVertexSetNormal(&pVertex[3], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[3], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[3], 1.0f);
    RwIm3DVertexSetV(&pVertex[3], 1.0f);

    RwIm3DVertexSetPos(&pVertex[4], aPosition[0].x, aPosition[0].y, aPosition[0].z);
    RwIm3DVertexSetNormal(&pVertex[4], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[4], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[4], 0.0f);
    RwIm3DVertexSetV(&pVertex[4], 0.0f);

    RwIm3DVertexSetPos(&pVertex[5], aPosition[2].x, aPosition[2].y, aPosition[2].z);
    RwIm3DVertexSetNormal(&pVertex[5], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[5], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[5], 1.0f);
    RwIm3DVertexSetV(&pVertex[5], 0.0f);
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
   
    WaterSplash().SetTexture(pTexture);
    StartWaterSplash();
};