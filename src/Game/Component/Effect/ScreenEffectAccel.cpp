#include "ScreenEffectAccel.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/RenderState.hpp"


class CScrEffectAccel
{
private:
    static const int32 PARTICLE_NUM = 60;

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
    PARTICLE     m_aParticle[PARTICLE_NUM];
    RwIm2DVertex m_aVertex[PARTICLE_NUM * 6];
    int32        m_nDisplayNum;
    float        m_fLength;
    RwRGBA       m_Color;
    RwV2d        m_vCenter;
    float        m_fRadius;
    float        m_fSpeed;
    RwV3d        m_vNowCameraPosition;
    RwV3d        m_vPrevCameraPosition;
    int32        m_nSetNum;
};


CScrEffectAccel::CScrEffectAccel(void)
: m_nDisplayNum(COUNT_OF(m_aParticle))
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

        Math::Vec2_Add(&pParticle->m_vPosition,
                       &pParticle->m_vPosition, 
                       &pParticle->m_vVelocity);

        RwV2d vBuff = Math::VECTOR2_ZERO;
        Math::Vec2_Normalize(&vBuff, &pParticle->m_vVelocity);
        Math::Vec2_Scale(&vBuff, &vBuff, pParticle->m_fLength);

        Math::Vec2_Add(&pParticle->m_vTailPosition,
                       &pParticle->m_vPosition,
                       &vBuff);
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

    RwCamera* camera = CCamera::CameraCurrent();
    float rhw = 1.0f / RwCameraGetNearClipPlane(camera);

    float z = RwIm2DGetNearScreenZ();

    for (int32 i = 0, j = 0; i < (m_nDisplayNum * 2); ++i, j += 2)
    {
        ASSERT(j < COUNT_OF(m_aVertex));

        PARTICLE* pParticle = &m_aParticle[i];
        RwIm2DVertex* pVertex = &m_aVertex[j];

        RwRGBA colorHead = pParticle->m_Color;
        RwRGBA colorTail = colorHead;

        colorHead.alpha = 0x00;

        RwIm2DVertexSetScreenX(&pVertex[0], pParticle->m_vPosition.x);
        RwIm2DVertexSetScreenY(&pVertex[0], pParticle->m_vPosition.y);
        RwIm2DVertexSetScreenZ(&pVertex[0], z);
        RwIm2DVertexSetIntRGBA(&pVertex[0], colorHead.red, colorHead.green, colorHead.blue, colorHead.alpha);
        RwIm2DVertexSetRecipCameraZ(&pVertex[0], rhw);
        RwIm2DVertexSetU(&pVertex[0], 0.0f, rhw);
        RwIm2DVertexSetV(&pVertex[0], 0.0f, rhw);

        RwIm2DVertexSetScreenX(&pVertex[1], pParticle->m_vTailPosition.x);
        RwIm2DVertexSetScreenY(&pVertex[1], pParticle->m_vTailPosition.y);
        RwIm2DVertexSetScreenZ(&pVertex[1], z);
        RwIm2DVertexSetIntRGBA(&pVertex[1], colorTail.red, colorTail.green, colorTail.blue, colorTail.alpha);
        RwIm2DVertexSetRecipCameraZ(&pVertex[1], rhw);
        RwIm2DVertexSetU(&pVertex[1], 0.0f, rhw);
        RwIm2DVertexSetV(&pVertex[1], 0.0f, rhw);
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
    
    RwCamera* pCamera = pMapCamera->GetRwCamera();
    ASSERT(pCamera);    

    RwV3d vAt = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vAt, &m_vNowCameraPosition, &m_vPrevCameraPosition);
    Math::Vec3_Normalize(&vAt, &vAt);
    Math::Vec3_Scale(&vAt, &vAt, 1000.0f);
    Math::Vec3_Add(&vAt, &vAt, &m_vNowCameraPosition);

    RwV3d vScPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vScPos, &vAt, RwCameraGetViewMatrix(pCamera));

    if (vScPos.z > 0.0f)
    {
        vScPos.x *= (1.0f / vScPos.z);
        vScPos.y *= (1.0f / vScPos.z);

        RwV2d vCenter = Math::VECTOR2_ZERO;
        vCenter.x = static_cast<float>(CScreen::Width()) * vScPos.x;
        vCenter.y = static_cast<float>(CScreen::Height()) * vScPos.y;

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

    float fRndAngle = MATH_DEG2RAD(static_cast<float>(Math::Rand() % 360));
    
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

    RwFrame* pFrame = RwCameraGetFrame(pCamera);
    RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);
    RwV3d vPosition = *RwMatrixGetPos(pMatrix);

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