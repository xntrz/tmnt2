#include "Snow.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Screen.hpp"


class CSnow
{
private:
    struct PARTICLE
    {
        RwV3d m_vPosition;
        RwV3d m_vVelocity;
        RwV2d m_vSize;
        float m_fTime;
        float m_fLifespan;
        RwRGBA m_Color;
    };
    
public:
    CSnow(void);
    ~CSnow(void);
    void Run(void);
    void Draw(void);
    void SetCameraInfo(const RwV3d* pvAt, const RwV3d* pvEye);
    void SetBasisPosition(const RwV3d* pvPosition);
    void SetWind(const RwV3d* pvWind);
    void SetTexture(RwTexture* pTexture);
    void SetColor(const RwRGBA& color);
    void SetRadius(float fRadius);
    void SetDisplayNum(int32 nDispNum);
    void SetSpeed(float fSpeed);
    void SetLiveScale(float fScale);
    void SetParticleSize(const RwV2d* pvSize);
    void SetCameraPosition(const RwV3d* pvPosition);
    void SetRideFlag(bool bSet);
    void CalcWind(void);
    void Generate(PARTICLE* pParticle);
    bool SetVertex(PARTICLE* pParticle, RwIm3DVertex* pVertex, RwMatrix* pMatrix, RwCamera* pCamera);
    
private:
    PARTICLE m_aParticle[80];
    RwIm3DVertex m_aVertex[80 * 6];
    int32 m_nDisplayNum;
    float m_fVolume;
    RwV3d m_vBasisPosition;
    RwV2d m_vBasisSize;
    float m_fRadius;
    float m_fHeight;
    RwV3d m_vAppearVector;
    float m_fSpeed;
    float m_fLiveScale;
    RwRGBA m_Color;
    RwTexture* m_pTexture;
    RwV3d m_vNowCameraPosition;
    RwV3d m_vPrevCameraPosition;
    int32 m_nSettingNum;
    bool m_bRideFlag;
};


CSnow::CSnow(void)
: m_nDisplayNum(0)
, m_fVolume(100.0f)
, m_vBasisPosition(Math::VECTOR3_ZERO)
, m_vBasisSize({ 3.0f, 3.0f })
, m_fRadius(5.0f)
, m_fHeight(6.0f)
, m_vAppearVector({ 0.0f, -2.0f, 0.0f })
, m_fSpeed(1.5f)
, m_fLiveScale(0.0f)
, m_Color({ 0xFF, 0xFF, 0xFF, 0x38 })
, m_pTexture(nullptr)
, m_vNowCameraPosition(Math::VECTOR3_ZERO)
, m_vPrevCameraPosition(Math::VECTOR3_ZERO)
, m_nSettingNum(0)
, m_bRideFlag(false)
{
    std::memset(m_aVertex, 0x00, sizeof(m_aVertex));
    std::memset(m_aParticle, 0x00, sizeof(m_aParticle));
    
    Math::Vec3_Normalize(&m_vAppearVector, &m_vAppearVector);

    for (int32 i = 0; i < COUNT_OF(m_aParticle); ++i)
        Generate(&m_aParticle[i]);
};


CSnow::~CSnow(void)
{
    ;
};


void CSnow::Run(void)
{
    RwV3d vFrameVelocity = Math::VECTOR3_ZERO;
    float dt = CGameProperty::GetElapsedTime();
    
    for (int32 i = 0; i < m_nDisplayNum; ++i)
    {
        PARTICLE* pParticle = &m_aParticle[i];

        pParticle->m_fTime += dt;

        RwV2d vBuffer = Math::VECTOR2_ZERO;
        vBuffer.x = pParticle->m_vPosition.x - m_vBasisPosition.x;
        vBuffer.y = pParticle->m_vPosition.z - m_vBasisPosition.z;

        if (pParticle->m_fTime > pParticle->m_fLifespan ||
            Math::Vec2_Length(&vBuffer) > m_fRadius)
            Generate(pParticle);
        
        Math::Vec3_Scale(
            &vFrameVelocity,
            &pParticle->m_vVelocity,
            dt
        );

        Math::Vec3_Add(
            &pParticle->m_vPosition,
            &pParticle->m_vPosition,
            &vFrameVelocity
        );
    };

    if (m_bRideFlag)
        CalcWind();
};


void CSnow::Draw(void)
{
    if (!m_nDisplayNum)
        return;

    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    uint32 uVertexNum = 0;
    RwMatrix billboardMatrix;
    RwMatrixSetIdentityMacro(&billboardMatrix);

    RwMatrixInvert(&billboardMatrix, RwCameraGetViewMatrixMacro(CCamera::CameraCurrent()));

    int32 nDispNum = m_nDisplayNum * int32(m_fVolume / 100.0f);
    
    for (int32 i = 0; i < nDispNum; ++i)
    {
        PARTICLE* pParticle = &m_aParticle[i];
        RwIm3DVertex* pVertex = &m_aVertex[uVertexNum];

        if (SetVertex(pParticle, pVertex, &billboardMatrix, CCamera::CameraCurrent()))
            uVertexNum += 6;        
    };

	if (uVertexNum)
	{
		uint32 uFlags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV;

		if (RwIm3DTransform(m_aVertex, uVertexNum, nullptr, uFlags))
		{
			RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
			RwIm3DEnd();
		};
	};

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};


void CSnow::SetCameraInfo(const RwV3d* pvAt, const RwV3d* pvEye)
{
    RwV3d vBuff = Math::VECTOR3_ZERO;

    Math::Vec3_Sub(&vBuff, pvAt, pvEye);
    Math::Vec3_Scale(&vBuff, &vBuff, 0.33f);
    
    if (Math::Vec3_Length(&vBuff) >= 5.0f)
    {
        Math::Vec3_Normalize(&vBuff, &vBuff);
        Math::Vec3_Scale(&vBuff, &vBuff, 5.0f);
    };
    
    Math::Vec3_Add(&vBuff, &vBuff, pvEye);
    
    SetBasisPosition(&vBuff);
};


void CSnow::SetBasisPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);    
    m_vBasisPosition = *pvPosition;
};


void CSnow::SetWind(const RwV3d* pvWind)
{
    ASSERT(pvWind);
    m_vAppearVector = *pvWind;
};


void CSnow::SetTexture(RwTexture* pTexture)
{
    ASSERT(pTexture);
    m_pTexture = pTexture;
};


void CSnow::SetColor(const RwRGBA& color)
{
    m_Color = color;
};


void CSnow::SetRadius(float fRadius)
{
    m_fRadius = fRadius;
};


void CSnow::SetDisplayNum(int32 nDispNum)
{
    m_nDisplayNum = nDispNum;
    
    ASSERT(m_nDisplayNum <= COUNT_OF(m_aParticle));
};


void CSnow::SetSpeed(float fSpeed)
{
    m_fSpeed = fSpeed;
};


void CSnow::SetLiveScale(float fScale)
{
    m_fLiveScale = fScale;
};


void CSnow::SetParticleSize(const RwV2d* pvSize)
{
    ASSERT(pvSize);
    m_vBasisSize = *pvSize;
};


void CSnow::SetCameraPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);

    m_vPrevCameraPosition = m_vNowCameraPosition;
    m_vNowCameraPosition = *pvPosition;

    if (m_nSettingNum < 3)
        ++m_nSettingNum;
};


void CSnow::SetRideFlag(bool bSet)
{
    m_bRideFlag = bSet;
};


void CSnow::CalcWind(void)
{
	//return;

    if (m_nSettingNum >= 3)
    {
        RwV3d vDirection = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDirection, &m_vPrevCameraPosition, &m_vNowCameraPosition);
        vDirection.y = 0.0f;
        Math::Vec3_Normalize(&vDirection, &vDirection);
        Math::Vec3_Scale(&vDirection, &vDirection, m_fSpeed);
        SetWind(&vDirection);
    };
};


void CSnow::Generate(PARTICLE* pParticle)
{
    ASSERT(pParticle);

    if (m_fRadius >= 0.0f)
    {
        pParticle->m_vPosition.x = m_vBasisPosition.x + ((Math::Rand() % uint32(m_fRadius + m_fRadius)) - m_fRadius);
        pParticle->m_vPosition.y = m_vBasisPosition.y + ((Math::Rand() % uint32(m_fHeight)) - m_fRadius);
        pParticle->m_vPosition.z = m_vBasisPosition.z + ((Math::Rand() % uint32(m_fRadius + m_fRadius)) - m_fRadius);
    };

    float fRandScale = (float(Math::Rand() % 100) * 0.01f) + 0.5f;

    pParticle->m_vVelocity = m_vAppearVector;

    pParticle->m_vVelocity.x += (float(Math::Rand() % 300) - 150.0f) * 0.001f;
    pParticle->m_vVelocity.z += (float(Math::Rand() % 300) - 150.0f) * 0.001f;

    Math::Vec3_Normalize(
        &pParticle->m_vVelocity,
        &pParticle->m_vVelocity
    );

    Math::Vec3_Scale(
        &pParticle->m_vVelocity,
        &pParticle->m_vVelocity,
        (fRandScale * m_fSpeed)
    );

    pParticle->m_vSize.x = m_vBasisSize.x * fRandScale;
	pParticle->m_vSize.y = m_vBasisSize.y * fRandScale;
    
    pParticle->m_fTime = 0.0f;
    pParticle->m_Color = m_Color;
    pParticle->m_fLifespan = (float(Math::Rand() % 30) + 70.0f) * 0.01f * m_fLiveScale;
};


bool CSnow::SetVertex(PARTICLE* pParticle, RwIm3DVertex* pVertex, RwMatrix* pMatrix, RwCamera* pCamera)
{
    ASSERT(pParticle);
    ASSERT(pVertex);
    ASSERT(pMatrix);
    ASSERT(pCamera);
    
    RwV3d vScPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(
        &vScPos,
        &pParticle->m_vPosition,
        RwCameraGetViewMatrixMacro(pCamera)
    );

    if (vScPos.z <= 2.5f)
    {
        Generate(pParticle);
        return false;
    };

    float sw = float(CScreen::Width());
	float sh = float(CScreen::Height());

    float x = pParticle->m_vSize.x * 0.5f;
    float y = ((sw / sh) * pParticle->m_vSize.y) * 0.5f;
    
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

    Math::Vec3_Add(&aPosition[0], &aPosition[0], &pParticle->m_vPosition);
    Math::Vec3_Add(&aPosition[1], &aPosition[1], &pParticle->m_vPosition);
    Math::Vec3_Add(&aPosition[2], &aPosition[2], &pParticle->m_vPosition);
    Math::Vec3_Add(&aPosition[3], &aPosition[3], &pParticle->m_vPosition);

    pVertex[0].objVertex = aPosition[1];
    pVertex[0].objNormal = Math::VECTOR3_ZERO;
    pVertex[0].color = COLOR_TO_INTEGER_RWRGBA(pParticle->m_Color);
    pVertex[0].u = 0.0f;
    pVertex[0].v = 1.0f;

    pVertex[1].objVertex = aPosition[0];
    pVertex[1].objNormal = Math::VECTOR3_ZERO;
    pVertex[1].color = COLOR_TO_INTEGER_RWRGBA(pParticle->m_Color);
    pVertex[1].u = 0.0f;
    pVertex[1].v = 0.0f;

    pVertex[2].objVertex = aPosition[3];
    pVertex[2].objNormal = Math::VECTOR3_ZERO;
    pVertex[2].color = COLOR_TO_INTEGER_RWRGBA(pParticle->m_Color);
    pVertex[2].u = 1.0f;
    pVertex[2].v = 1.0f;

    pVertex[3].objVertex = aPosition[3];
    pVertex[3].objNormal = Math::VECTOR3_ZERO;
    pVertex[3].color = COLOR_TO_INTEGER_RWRGBA(pParticle->m_Color);
    pVertex[3].u = 1.0f;
    pVertex[3].v = 1.0f;

    pVertex[4].objVertex = aPosition[0];
    pVertex[4].objNormal = Math::VECTOR3_ZERO;
    pVertex[4].color = COLOR_TO_INTEGER_RWRGBA(pParticle->m_Color);
    pVertex[4].u = 0.0f;
    pVertex[4].v = 0.0f;

    pVertex[5].objVertex = aPosition[2];
    pVertex[5].objNormal = Math::VECTOR3_ZERO;
    pVertex[5].color = COLOR_TO_INTEGER_RWRGBA(pParticle->m_Color);
    pVertex[5].u = 1.0f;
    pVertex[5].v = 0.0f;

    return true;
};


static CSnow* s_pSnow = nullptr;
static bool s_bActive = false;


static CSnow& Snow(void)
{
    ASSERT(s_pSnow);
    return *s_pSnow;
};


/*static*/ void CSnowManager::Initialize(void)
{
    if (!s_pSnow)
    {
        s_pSnow = new CSnow;
		StopSnow();
    };
};


/*static*/ void CSnowManager::Terminate(void)
{
    if (s_pSnow)
    {
        delete s_pSnow;
        s_pSnow = nullptr;
    };
};


/*static*/ void CSnowManager::Run(void)
{
    if (s_bActive)
        Snow().Run();
};


/*static*/ void CSnowManager::Draw(void)
{
    if (s_bActive)
        Snow().Draw();
};


/*static*/ void CSnowManager::StartSnow(void)
{
    s_bActive = true;
};


/*static*/ void CSnowManager::StopSnow(void)
{
    s_bActive = false;
};


/*static*/ void CSnowManager::SetNormalSnow(void)
{
    CTextureManager::SetCurrentTextureSet("common_ef");
    RwTexture* pTexture = CTextureManager::GetRwTexture("snowstorm2");
    if (!pTexture)
    {
        CTextureManager::SetCurrentTextureSet("snow_ef");
        RwTexture* pTexture = CTextureManager::GetRwTexture("snowstorm1");
    };

    ASSERT(pTexture);
    Snow().SetTexture(pTexture);
    Snow().SetRadius(5.0f);
    Snow().SetDisplayNum(80);
    Snow().SetSpeed(1.5f);
    Snow().SetLiveScale(1.3f);

    RwV3d vWind = { 0.0f, -2.0f, 0.0f };
    Snow().SetWind(&vWind);

    RwV2d vSize = { 3.0f, 3.0f };
    Snow().SetParticleSize(&vSize);
    Snow().SetRideFlag(false);
    
    StartSnow();
};


/*static*/ void CSnowManager::SetRideSnow(void)
{
    CTextureManager::SetCurrentTextureSet("common_ef");
    RwTexture* pTexture = CTextureManager::GetRwTexture("snowstorm1");
    if (!pTexture)
    {
        CTextureManager::SetCurrentTextureSet("snow_ef");
        RwTexture* pTexture = CTextureManager::GetRwTexture("snowstorm1");
    };

    ASSERT(pTexture);
    Snow().SetTexture(pTexture);
    Snow().SetColor({ 0xFF, 0xFF, 0xFF, 0x3C });
    Snow().SetRadius(7.0f);
    Snow().SetDisplayNum(30);
    Snow().SetSpeed(14.0f);
    Snow().SetLiveScale(0.6f);

    RwV3d vWind = { 0.0f, 0.0f, -1.0f };
    Snow().SetWind(&vWind);

    RwV2d vSize = { 6.0f, 6.0f };
    Snow().SetParticleSize(&vSize);
    Snow().SetRideFlag(true);
    
    StartSnow();
};


/*static*/ void CSnowManager::SetCameraPosition(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);
    
    RwV3d vAt = Math::VECTOR3_ZERO;
    RwV3d vPosition = Math::VECTOR3_ZERO;

    RwCamera* pCamera = pMapCamera->GetRwCamera();
    ASSERT(pCamera);

    pMapCamera->GetLookat(&vAt);
    vPosition = RwFrameGetMatrixMacro(RwCameraGetFrameMacro(pCamera))->pos;    

    Snow().SetCameraPosition(&vPosition);
    Snow().SetCameraInfo(&vAt, &vPosition);
};


/*static*/ void CSnowManager::SetBasisPosition(const RwV3d* pvPosition)
{
    Snow().SetBasisPosition(pvPosition);
};