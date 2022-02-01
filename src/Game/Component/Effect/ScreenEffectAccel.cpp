#include "ScreenEffectAccel.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/RenderState.hpp"


class CScrEffectAccel
{
private:
    struct PARTICLE
    {
        RwV2d m_vPosition;
        RwV2d m_vTailPosition;
        RwV2d m_vVelocity;
        float m_fLength;
        float m_fTime;
        float m_fLifespan;
        RwRGBA m_Color;
    };
    
public:
    CScrEffectAccel(void);
    ~CScrEffectAccel(void);
    void Run(void);
    void Draw(void);
    void SetCenter(const RwV2d* pvCenter);
    void SetCenter(CMapCamera* pMapCamera);
    void SetCameraPosition(const RwV3d* pvPosition);
    void Generate(PARTICLE* pParticle);
    
private:
    PARTICLE m_aParticle[60];
    RwIm2DVertex m_aVertex[60 * 6];
    int32 m_nDisplayNum;
    float m_fLength;
    RwRGBA m_Color;
    RwV2d m_vCenter;
    float m_fRadius;
    float m_fSpeed;
    RwV3d m_vNowCameraPosition;
    RwV3d m_vPrevCameraPosition;
    int32 m_nSetNum;
};


CScrEffectAccel::CScrEffectAccel(void)
: m_nDisplayNum(60)
, m_fLength(150.0f)
, m_Color({ 0xEE, 0xEE, 0xEE, 0x80 })
, m_vCenter({ 320.0f, 100.0f })
, m_fRadius(150.0f)
, m_fSpeed(40.0f)
, m_vNowCameraPosition(Math::VECTOR3_ZERO)
, m_vPrevCameraPosition(Math::VECTOR3_ZERO)
, m_nSetNum(0)
{
    std::memset(m_aParticle, 0x00, sizeof(m_aParticle));
    std::memset(m_aVertex, 0x00, sizeof(m_aVertex));
    
    for (int32 i = 0; i < m_nDisplayNum; ++i)
        Generate(&m_aParticle[i]);        
};


CScrEffectAccel::~CScrEffectAccel(void)
{
    ;
};


void CScrEffectAccel::Run(void)
{
    ASSERT(m_nDisplayNum <= COUNT_OF(m_aParticle));
    
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

        vBuff.x *= pParticle->m_fLength;
        vBuff.y *= pParticle->m_fLength;

        pParticle->m_vTailPosition.x = pParticle->m_vPosition.x + vBuff.x;
        pParticle->m_vTailPosition.y = pParticle->m_vPosition.y + vBuff.y;
    };
};


void CScrEffectAccel::Draw(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    float z = RwIm2DGetNearScreenZ();
    float rhw = 1.0f / RwCameraGetNearClipPlane(CCamera::CameraCurrent());

    for (int32 i = 0, j = 0; i < (m_nDisplayNum * 2); ++i, j += 2)
    {
        ASSERT(j < COUNT_OF(m_aVertex));

        PARTICLE* pParticle = &m_aParticle[i];
        RwIm2DVertex* pVertex = &m_aVertex[j];

        RwRGBA colorHead = pParticle->m_Color;
        RwRGBA colorTail = colorHead;

        colorHead.alpha = 0x00;
        colorTail.alpha = 0xFF;

        pVertex[0].x = pParticle->m_vPosition.x;
        pVertex[0].y = pParticle->m_vPosition.y;
        pVertex[0].z = z;
        pVertex[0].u = 0.0f;
        pVertex[0].v = 0.0f;
        pVertex[0].rhw = rhw;
        pVertex[0].emissiveColor = COLOR_TO_INTEGER_RWRGBA(colorHead);

        pVertex[1].x = pParticle->m_vTailPosition.x;
        pVertex[1].y = pParticle->m_vTailPosition.y;
        pVertex[1].z = z;
        pVertex[1].u = 0.0f;
        pVertex[1].v = 0.0f;
        pVertex[1].rhw = rhw;
        pVertex[1].emissiveColor = COLOR_TO_INTEGER_RWRGBA(colorTail);
    };

    RwIm2DRenderPrimitive(rwPRIMTYPELINELIST, m_aVertex, m_nDisplayNum * 2);

    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};


void CScrEffectAccel::SetCenter(const RwV2d* pvCenter)
{
    ASSERT(pvCenter);
    m_vCenter = *pvCenter;
};


void CScrEffectAccel::SetCenter(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);
    
    if (m_nSetNum < 3)
        return;
    
    RwV3d vScPos = Math::VECTOR3_ZERO;
    RwV3d vAt = Math::VECTOR3_ZERO;
    RwV2d vCenter = Math::VECTOR2_ZERO;
    RwCamera* pCamera = pMapCamera->GetRwCamera();
    ASSERT(pCamera);    

    Math::Vec3_Sub(&vAt, &m_vNowCameraPosition, &m_vPrevCameraPosition);
    Math::Vec3_Normalize(&vAt, &vAt);
    Math::Vec3_Scale(&vAt, &vAt, 1000.0f);
    Math::Vec3_Add(&vAt, &vAt, &m_vNowCameraPosition);
    RwV3dTransformPoint(&vScPos, &vAt, RwCameraGetViewMatrixMacro(pCamera));

    if (vScPos.z > 0.0f)
    {
        vScPos.x *= (1.0f / vScPos.z);
        vScPos.y *= (1.0f / vScPos.z);

        vCenter.x = float(CScreen::Width()) * vScPos.x;
        vCenter.y = float(CScreen::Height()) * vScPos.y;

        SetCenter(&vCenter);
    };
};


void CScrEffectAccel::SetCameraPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    m_vPrevCameraPosition = m_vNowCameraPosition;
    m_vNowCameraPosition = *pvPosition;

    if (m_nSetNum < 3)
        ++m_nSetNum;
};


void CScrEffectAccel::Generate(PARTICLE* pParticle)
{
    ASSERT(pParticle);

    float fRndAngle = Math::ToRadian(float(Math::Rand() % 360));
    float x = Math::Cos(fRndAngle);
    float y = Math::Sin(fRndAngle);

    pParticle->m_vPosition.x = m_vCenter.x + (x * m_fRadius);
    pParticle->m_vPosition.y = m_vCenter.y + (y * m_fRadius);
    pParticle->m_vVelocity.x = m_fSpeed * x;
    pParticle->m_vVelocity.y = m_fSpeed * y;
    pParticle->m_fLength = m_fLength;
    pParticle->m_fTime = 0.0f;
    pParticle->m_fLifespan = 1.0f;
    pParticle->m_Color = m_Color;
    pParticle->m_Color.alpha -= 5;
};


static CScrEffectAccel* s_pScrEffectAccel = nullptr;
static bool s_bActive = false;


static inline CScrEffectAccel& ScreenEffectAccel(void)
{
    ASSERT(s_pScrEffectAccel);
    return *s_pScrEffectAccel;
};


/*static*/ void CScreenEffectAccel::Initialize(void)
{
    if (!s_pScrEffectAccel)
    {
        s_pScrEffectAccel = new CScrEffectAccel;
        StopAccel();
    };
};


/*static*/ void CScreenEffectAccel::Terminate(void)
{
    if (s_pScrEffectAccel)
    {
        delete s_pScrEffectAccel;
        s_pScrEffectAccel = nullptr;
    };
};


/*static*/ void CScreenEffectAccel::Run(void)
{
    if(s_bActive)
        ScreenEffectAccel().Run();
};


/*static*/ void CScreenEffectAccel::Draw(void)
{
    if (s_bActive)
        ScreenEffectAccel().Draw();
};


/*static*/ void CScreenEffectAccel::SetCenter(CMapCamera* pMapCamera)
{
    ASSERT(pMapCamera);

    RwCamera* pCamera = pMapCamera->GetRwCamera();
    ASSERT(pCamera);
    
    RwV3d vPosition = RwFrameGetMatrixMacro(RwCameraGetFrameMacro(pCamera))->pos;

    ScreenEffectAccel().SetCameraPosition(&vPosition);
    ScreenEffectAccel().SetCenter(pMapCamera);
};


/*static*/ void CScreenEffectAccel::StartAccel(void)
{
    s_bActive = true;
};


/*static*/ void CScreenEffectAccel::StopAccel(void)
{
    s_bActive = false;
};