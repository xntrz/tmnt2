#include "Light.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/RenderState.hpp"


class CEffectLight
{
private:
    struct LIGHTWORK
    {
        RwV3d        vPosition;
        RwRGBA       color;
        float        fRadius;
        RwIm3DVertex aVertices[4];        
        bool         bVisible;
        bool         bUse;
    };

public:
    CEffectLight(void);
    ~CEffectLight(void);
    void Run(void);
    void Draw(void);
    uint32 Regist(const RwV3d* pvPosition, float fRadius, const RwRGBA& color);
    void Remove(uint32 hLight);
    void SetColor(uint32 hLight, const RwRGBA& color);
    void SetPosition(uint32 hLight, const RwV3d* pvPosition);
    void SetRadius(uint32 hLight, float fRadius);
    LIGHTWORK* WorkAlloc(void);
    void WorkFree(LIGHTWORK* pWork);
    LIGHTWORK* WorkFromHandle(uint32 hLight) const;
    void SetVertex(LIGHTWORK* pWork, float fRadius, const RwRGBA& color) const;

private:
    LIGHTWORK   m_aLightWork[120];
    int32       m_nNumActiveLights;
    RwTexture*  m_pTexture;
};


CEffectLight::CEffectLight(void)
: m_aLightWork()
, m_nNumActiveLights(0)
, m_pTexture(nullptr)
{
    std::memset(m_aLightWork, 0, sizeof(m_aLightWork));    
};


CEffectLight::~CEffectLight(void)
{
    ;
};


void CEffectLight::Run(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aLightWork); ++i)
    {
        LIGHTWORK* pWork = &m_aLightWork[i];

        if (pWork->bUse)
        {
            pWork->bVisible = CGimmickUtils::IsPositionVisible(&pWork->vPosition, pWork->fRadius, true);
            ++m_nNumActiveLights;
        };
    };
};


void CEffectLight::Draw(void)
{
    if (!m_nNumActiveLights)
        return;

    m_nNumActiveLights = 0;

    RwMatrix matrixView;
    RwMatrixSetIdentity(&matrixView);
    CGameProperty::GetCameraViewMatrix(&matrixView);

    RwMatrix matrixBillboard;
    RwMatrixSetIdentity(&matrixBillboard);
    Math::Matrix_Invert(&matrixBillboard, &matrixView);

    RwV3d vEyePos = matrixBillboard.pos;

    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);

    for (int32 i = 0; i < COUNT_OF(m_aLightWork); ++i)
    {
        LIGHTWORK* pWork = &m_aLightWork[i];

        if (!pWork->bUse)
            continue;

        if (pWork->bVisible)
        {
            RwV3d vTmp = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vTmp, &vEyePos, &pWork->vPosition);
            Math::Vec3_Normalize(&vTmp, &vTmp);
            Math::Vec3_Scale(&vTmp, &vTmp, 1.0f);
            Math::Vec3_Add(&matrixBillboard.pos, &pWork->vPosition, &vTmp);
            
            SetVertex(pWork, pWork->fRadius, pWork->color);
            
            uint32 flags  = rwIM3D_VERTEXRGBA
                          | rwIM3D_VERTEXXYZ
                          | rwIM3D_VERTEXUV;

            if (RwIm3DTransform(pWork->aVertices, COUNT_OF(pWork->aVertices), &matrixBillboard, flags))
            {
                RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
                RwIm3DEnd();
            };
        };
    };

    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
};


uint32 CEffectLight::Regist(const RwV3d* pvPosition, float fRadius, const RwRGBA& color)
{
    LIGHTWORK* pWork = WorkAlloc();
    ASSERT(pWork);

    if (pWork)
    {
        pWork->vPosition  = *pvPosition;
        pWork->fRadius    = fRadius;
        pWork->color      = color;
        pWork->bVisible   = true;
    };

    if (!m_pTexture)
    {
        CTextureManager::SetCurrentTextureSet("mapeffect");

        m_pTexture = CTextureManager::GetRwTexture("light");
        ASSERT(m_pTexture);
    };

    return reinterpret_cast<uint32>(pWork);
};


void CEffectLight::Remove(uint32 hLight)
{
    LIGHTWORK* pWork = WorkFromHandle(hLight);
    ASSERT(pWork);

    if (pWork)
        WorkFree(pWork);
};


void CEffectLight::SetColor(uint32 hLight, const RwRGBA& color)
{
    LIGHTWORK* pWork = WorkFromHandle(hLight);
    ASSERT(pWork);

    pWork->color = color;
};


void CEffectLight::SetPosition(uint32 hLight, const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    LIGHTWORK* pWork = WorkFromHandle(hLight);
    ASSERT(pWork);    

    pWork->vPosition = *pvPosition;
};


void CEffectLight::SetRadius(uint32 hLight, float fRadius)
{
    LIGHTWORK* pWork = WorkFromHandle(hLight);
    ASSERT(pWork);

    pWork->fRadius = fRadius;
};


CEffectLight::LIGHTWORK* CEffectLight::WorkAlloc(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aLightWork); ++i)
    {
        if (!m_aLightWork[i].bUse)
        {
            m_aLightWork[i].bUse = true;
            return &m_aLightWork[i];
        };
    };

    return nullptr;
};


void CEffectLight::WorkFree(LIGHTWORK* pWork)
{
    ASSERT(pWork);

    pWork->bUse = false;
};


CEffectLight::LIGHTWORK* CEffectLight::WorkFromHandle(uint32 hLight) const
{
    LIGHTWORK* pWork = reinterpret_cast<LIGHTWORK*>(hLight);

    if (!pWork)
        return nullptr;

    if (!pWork->bUse)
        return nullptr;

    return pWork;
};


void CEffectLight::SetVertex(LIGHTWORK* pWork, float fRadius, const RwRGBA& color) const
{
    ASSERT(pWork);
    
    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    ASSERT(pMapCamera);

    float fViewRatio = pMapCamera->GetViewRatio();
    float fWidth  = static_cast<float>(CScreen::Width());
    float fHeight = static_cast<float>(CScreen::Height());    
    float x = fViewRatio * fRadius;
    float y = (fWidth / fHeight) * fViewRatio * fRadius;

    RwIm3DVertex* pVertex = pWork->aVertices;

    RwIm3DVertexSetPos(&pVertex[0], x, y, 0.0f);
    RwIm3DVertexSetNormal(&pVertex[0], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[0], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[0], 0.0f);
    RwIm3DVertexSetV(&pVertex[0], 1.0f);

    RwIm3DVertexSetPos(&pVertex[1], x, (y * -1.0f), 0.0f);
    RwIm3DVertexSetNormal(&pVertex[1], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[1], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[1], 0.0f);
    RwIm3DVertexSetV(&pVertex[1], 0.0f);

    RwIm3DVertexSetPos(&pVertex[2], (x * -1.0f), y, 0.0f);
    RwIm3DVertexSetNormal(&pVertex[2], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[2], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[2], 1.0f);
    RwIm3DVertexSetV(&pVertex[2], 1.0f);

    RwIm3DVertexSetPos(&pVertex[3], (x * -1.0f), (y * -1.0f), 0.0f);
    RwIm3DVertexSetNormal(&pVertex[3], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[3], color.red, color.green, color.blue, color.alpha);
    RwIm3DVertexSetU(&pVertex[3], 1.0f);
    RwIm3DVertexSetV(&pVertex[3], 0.0f);
};


static CEffectLight* s_pEffectLight = nullptr;


static inline CEffectLight& EffectLight(void)
{
    ASSERT(s_pEffectLight);
    return *s_pEffectLight;
};


/*static*/ void CEffectLightManager::Initialize(void)
{
    if (!s_pEffectLight)
        s_pEffectLight = new CEffectLight;
};


/*static*/ void CEffectLightManager::Terminate(void)
{
    if (s_pEffectLight)
    {
        delete s_pEffectLight;
        s_pEffectLight = nullptr;
    };
};


/*static*/ void CEffectLightManager::Run(void)
{
    EffectLight().Run();
};


/*static*/ void CEffectLightManager::Draw(void)
{
    EffectLight().Draw();
};


/*static*/ uint32 CEffectLightManager::Regist(const RwV3d* pvPosition, float fRadius, const RwRGBA& color)
{
    return EffectLight().Regist(pvPosition, fRadius, color);
};


/*static*/ void CEffectLightManager::Remove(uint32 hLight)
{
    EffectLight().Remove(hLight);
};


/*static*/ void CEffectLightManager::SetPosition(uint32 hLight, const RwV3d* pvPosition)
{
    EffectLight().SetPosition(hLight, pvPosition);
};


/*static*/ void CEffectLightManager::SetColor(uint32 hLight, const RwRGBA& color)
{
    EffectLight().SetColor(hLight, color);
};


/*static*/ void CEffectLightManager::SetRadius(uint32 hLight, float fRadius)
{
    EffectLight().SetRadius(hLight, fRadius);
};