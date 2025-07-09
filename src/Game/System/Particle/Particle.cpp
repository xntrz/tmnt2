#include "Particle.hpp"

#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "System/Common/RenderState.hpp"


static RwIm3DVertex s_aVertexForCylinder[CParticle::VERTEXNUM] = { 0 };
static RwIm3DVertex s_aVertex[300] = { 0 };


CParticle::CParticle(void)
{
    m_szName[0] = '\0';    
    m_paTransition = nullptr;
    m_nTransitionNum = 0;
    m_nTransitionMax = 0;
    m_nNowUseNum = 0;
    m_fCounter = 0.0f;
    m_bRun = false;
    m_bDraw = false;
    m_bLoop = false;
    m_bSimple = false;
    m_bFinish = false;
    m_szTextureName[0] = '\0';
    m_pMyTexture = nullptr;
    m_u0 = 0.0f;
    m_v0 = 0.0f;
    m_u1 = 1.0f;
    m_v1 = 1.0f;
    m_nRepetitionNum = 0;
    m_fColorDecayR = 0.0f;
    m_fColorDecayG = 0.0f;
    m_fColorDecayB = 0.0f;
    m_fColorDecayA = 0.0f;
    m_fColorDecayBuffer[4] = { 0.0f };
    m_fSizeDecayWidth = 0.0f;
    m_fSizeDecayHeight = 0.0f;
    m_fSizeDecayBuffer[2] = { 0.0f };
    m_vScaleDecay = Math::VECTOR3_ZERO;
    m_vScaleDecayBuffer = Math::VECTOR3_ZERO;
    m_nVertexNum = 0;
    m_fTopRadius = 0.0f;
    m_fBottomRadius = 0.0f;
    m_fHeight = 0.0f;
    m_nPointNum = 0;
    m_vAxisX = Math::VECTOR3_ZERO;
    m_vAxisY = Math::VECTOR3_ZERO;
    m_vAxisZ = Math::VECTOR3_ZERO;    
    std::memset(&m_InitTransition, 0x00, sizeof(m_InitTransition));
    RwMatrixSetIdentityMacro(&m_InitTransition.m_matrix);
    m_InitTransition.data = &m_InitTransition;
    m_type = PEFINFO::PARTICLETYPE_BILLBOARD;
    m_blend = PEFINFO::BLENDTYPE_ADD;
    m_kind = PEFINFO::PARTICLEKIND_NORMAL;
    m_uPatternFlag = 0;
    m_uLinearFlag = 0;
    m_fAppearStart = 0.0f;
    m_fAppearEnd = 0.0f;
    m_fAppearTiming = 0.0f;
    m_fEndTime = 0.0f;
    m_vBasisPositionForAll = Math::VECTOR3_ZERO;
    m_vOriginPosition = Math::VECTOR3_ZERO;
    m_vOriginVecFirst = Math::VECTOR3_ZERO;
    m_vOriginAcceleration = Math::VECTOR3_ZERO;
    m_uRandCheckFlag = 0;
    m_vRandPosition = Math::VECTOR3_ZERO;
    m_vRandSize = Math::VECTOR2_ZERO;
    m_vRandFirstVec = Math::VECTOR3_ZERO;
    m_fRandSpeedFirst = 0.0f;
    m_vRandAcceleration = Math::VECTOR3_ZERO;
    m_fRandAccel = 0.0f;
    m_fRandLiveCount = 0.0f;
    m_fRandOccurTiming = 0.0f;
    m_fRandScaleMin = 0.0f;
    m_fRandScaleMax = 0.0f;
    m_vRandScale3D = Math::VECTOR3_ZERO;
    m_fRandAngle = 0.0f;
    m_fCircleRadius = 0.0f;
    m_fCircleSpeed = 0.0f;
    m_fCircleBaseRotation = 0.0f;
    m_fAccelSizeDecayWidth = 0.0f;
    m_fAccelSizeDecayHeight = 0.0f;
    m_fAccelRotationBillboard = 0.0f;
    m_vAccelRotation = Math::VECTOR3_ZERO;
    m_fAccelSizeDecayWidthBuffer = 0.0f;
    m_fAccelSizeDecayHeightBuffer = 0.0f;
    m_fAccelRotationBillboardBuffer = 0.0f;
    m_vAccelRotationBuffer = Math::VECTOR3_ZERO;
    m_fTimer = 0.0f;
    m_fGravity = 0.0f;
    m_clippingtype = PEFINFO::CLIPPINGTYPE_NOCLIPPING;
    m_fClippingParameter = 0.0f;
};


CParticle::~CParticle(void)
{
    if (m_paTransition)
    {
        delete[] m_paTransition;
        m_paTransition = nullptr;
    };
};


void CParticle::Copy(const CParticle* pParticle)
{
    std::memcpy(m_szName, pParticle->m_szName, sizeof(m_szName));
    std::memcpy(m_szTextureName, pParticle->m_szTextureName, sizeof(m_szTextureName));
    std::memcpy(&m_InitTransition, &pParticle->m_InitTransition, sizeof(m_InitTransition));
    m_InitTransition.data = &m_InitTransition;

    m_pMyTexture = pParticle->m_pMyTexture;
    m_fCounter = pParticle->m_fCounter;
    m_bRun = pParticle->m_bRun;
    m_bDraw = pParticle->m_bDraw;
    m_bLoop = pParticle->m_bLoop;
    m_bSimple = pParticle->m_bSimple;
    m_u0 = pParticle->m_u0;
    m_v0 = pParticle->m_v0;
    m_u1 = pParticle->m_u1;
    m_v1 = pParticle->m_v1;
    m_nRepetitionNum = pParticle->m_nRepetitionNum;
    m_fColorDecayR = pParticle->m_fColorDecayR;
    m_fColorDecayG = pParticle->m_fColorDecayG;
    m_fColorDecayB = pParticle->m_fColorDecayB;
    m_fColorDecayA = pParticle->m_fColorDecayA;
    m_fColorDecayBuffer[0] = pParticle->m_fColorDecayBuffer[0];
    m_fColorDecayBuffer[1] = pParticle->m_fColorDecayBuffer[1];
    m_fColorDecayBuffer[2] = pParticle->m_fColorDecayBuffer[2];
    m_fColorDecayBuffer[3] = pParticle->m_fColorDecayBuffer[3];
    m_fSizeDecayWidth = pParticle->m_fSizeDecayWidth;
    m_fSizeDecayHeight = pParticle->m_fSizeDecayHeight;
    m_fSizeDecayBuffer[0] = pParticle->m_fSizeDecayBuffer[0];
    m_fSizeDecayBuffer[1] = pParticle->m_fSizeDecayBuffer[1];
    m_vScaleDecay = pParticle->m_vScaleDecay;
    m_vScaleDecayBuffer = pParticle->m_vScaleDecayBuffer;
    m_type = pParticle->m_type;
    m_blend = pParticle->m_blend;
    m_kind = pParticle->m_kind;
    m_uPatternFlag = pParticle->m_uPatternFlag;
    m_uLinearFlag = pParticle->m_uLinearFlag;
    m_fAppearStart = pParticle->m_fAppearStart;
    m_fAppearEnd = pParticle->m_fAppearEnd;
    m_fAppearTiming = pParticle->m_fAppearTiming;
    m_fEndTime = pParticle->m_fEndTime;
    m_vBasisPositionForAll = pParticle->m_vBasisPositionForAll;
    m_uRandCheckFlag = pParticle->m_uRandCheckFlag;
    m_vRandPosition = pParticle->m_vRandPosition;
    m_vRandSize = pParticle->m_vRandSize;
    m_vRandFirstVec = pParticle->m_vRandFirstVec;
    m_vRandAcceleration = pParticle->m_vRandAcceleration;
    m_fRandAccel = pParticle->m_fRandAccel;
    m_fRandLiveCount = pParticle->m_fRandLiveCount;
    m_fRandOccurTiming = pParticle->m_fRandOccurTiming;
    m_fRandScaleMin = pParticle->m_fRandScaleMin;
    m_fRandScaleMax = pParticle->m_fRandScaleMax;
    m_vRandScale3D = pParticle->m_vRandScale3D;
    m_fRandAngle = pParticle->m_fRandAngle;
    m_fCircleRadius = pParticle->m_fCircleRadius;
    m_fCircleSpeed = pParticle->m_fCircleSpeed;
    m_fCircleBaseRotation = pParticle->m_fCircleBaseRotation;
    m_fAccelSizeDecayWidth = pParticle->m_fAccelSizeDecayWidth;
    m_fAccelSizeDecayHeight = pParticle->m_fAccelSizeDecayHeight;
    m_fAccelRotationBillboard = pParticle->m_fAccelRotationBillboard;
    m_vAccelRotation = pParticle->m_vAccelRotation;
    m_fAccelSizeDecayWidthBuffer = pParticle->m_fAccelSizeDecayWidthBuffer;
    m_fAccelSizeDecayHeightBuffer = pParticle->m_fAccelSizeDecayHeightBuffer;
    m_fAccelRotationBillboardBuffer = pParticle->m_fAccelRotationBillboardBuffer;
    m_vAccelRotationBuffer = pParticle->m_vAccelRotationBuffer;
    m_nVertexNum = pParticle->m_nVertexNum;
    m_fTopRadius = pParticle->m_fTopRadius;
    m_fBottomRadius = pParticle->m_fBottomRadius;
    m_fHeight = pParticle->m_fHeight;
    m_nPointNum = pParticle->m_nPointNum;
    m_clippingtype = pParticle->m_clippingtype;
    m_fClippingParameter = pParticle->m_fClippingParameter;
    m_vOriginPosition = pParticle->m_vOriginPosition;
    m_vOriginVecFirst = pParticle->m_vOriginVecFirst;
    m_vOriginAcceleration = pParticle->m_vOriginAcceleration;

    ASSERT(m_nVertexNum <= VERTEXNUM);

    if (m_type == PEFINFO::PARTICLETYPE_CYLINDER)
        CreateCylinder(m_fTopRadius, m_fBottomRadius, m_nPointNum, m_fHeight);

    InitializeVector();
};


void CParticle::ConvertEndian(PEFINFO::PARTICLEINFO* pParticleInfo)
{
    ;
};


void CParticle::ReadParticleData(PEFINFO::PARTICLEINFO* pParticleInfo)
{
    ConvertEndian(pParticleInfo);
    
    std::memcpy(m_szName, pParticleInfo->m_szParticleName, sizeof(m_szName));
    m_type = PEFINFO::PARTICLETYPE(pParticleInfo->m_nType);
    m_uLinearFlag = pParticleInfo->m_uLinearFlag;
    m_blend = PEFINFO::BLENDTYPE(pParticleInfo->m_nBlend);
    m_kind = PEFINFO::PARTICLEKIND(pParticleInfo->m_nKind);
    m_InitTransition.m_vPosition = pParticleInfo->m_vPosition;
    m_InitTransition.m_Color = pParticleInfo->m_Color;
    m_InitTransition.m_vSize = pParticleInfo->m_vSize;
    m_InitTransition.m_vScale = pParticleInfo->m_vScale;
    m_InitTransition.m_vDirection = pParticleInfo->m_vDirection;
    m_InitTransition.m_vVecFirst = pParticleInfo->m_vVecFirst;
    m_InitTransition.m_fSpeedFirst = pParticleInfo->m_fSpeedFirst;
    m_InitTransition.m_vAcceleration = pParticleInfo->m_vAcceleration;
    m_InitTransition.m_fAccelSpeed = pParticleInfo->m_fAccelSpeed;
    m_InitTransition.m_fLiveCount = pParticleInfo->m_fLiveCount;
    m_InitTransition.m_fOccurTiming = pParticleInfo->m_fOccurTiming;
    m_InitTransition.m_nOccurNum = pParticleInfo->m_nOccurNum;
    m_InitTransition.m_fRotSpeedBillboard = pParticleInfo->m_fRotSpeedBillboard;
    m_InitTransition.m_fRotationBillboard = pParticleInfo->m_fRotationBillboard;
    m_InitTransition.m_vRotSpeed = pParticleInfo->m_vRotationSpeed;
    m_InitTransition.m_vRotation = pParticleInfo->m_vRotation;
    m_uPatternFlag = pParticleInfo->m_uPatternFlag;
    m_fAppearStart = pParticleInfo->m_fAppearStart;
    m_fAppearEnd = pParticleInfo->m_fAppearEnd;
    m_fAppearTiming = pParticleInfo->m_fAppearTiming;
    m_fEndTime = pParticleInfo->m_fEndTime;
    std::memcpy(m_szTextureName, pParticleInfo->m_szTextureName, sizeof(m_szTextureName));
    m_u0 = pParticleInfo->m_u0;
    m_v0 = pParticleInfo->m_v0;
    m_u1 = pParticleInfo->m_u1;
    m_v1 = pParticleInfo->m_v1;
    m_nRepetitionNum = pParticleInfo->m_nRepetitionNum;
    m_fColorDecayR = pParticleInfo->m_fColorDecayR;
    m_fColorDecayG = pParticleInfo->m_fColorDecayG;
    m_fColorDecayB = pParticleInfo->m_fColorDecayB;
    m_fColorDecayA = pParticleInfo->m_fColorDecayA;
    m_fSizeDecayWidth = pParticleInfo->m_fSizeDecayWidth;
    m_fSizeDecayHeight = pParticleInfo->m_fSizeDecayHeight;
    m_vScaleDecay = pParticleInfo->m_vScaleDecay;
    m_vScaleDecayBuffer = pParticleInfo->m_vScaleDecayBuffer;
    m_fTopRadius = pParticleInfo->m_fTopRadius;
    m_fBottomRadius = pParticleInfo->m_fBottomRadius;
    m_fHeight = pParticleInfo->m_fHeight;
    m_nPointNum = pParticleInfo->m_nPointNum;
    m_uRandCheckFlag = pParticleInfo->m_bRandCheckFlag;
    m_vRandPosition = pParticleInfo->m_vRandPosition;
    m_vRandSize = pParticleInfo->m_vRandSize;
    m_vRandFirstVec = pParticleInfo->m_vRandVecFirst;
    m_fRandSpeedFirst = pParticleInfo->m_fRandSpeedFirst;
    m_vRandAcceleration = pParticleInfo->m_vRandAcceleration;
    m_fRandAccel = pParticleInfo->m_fRandAccel;
    m_fRandLiveCount = pParticleInfo->m_fRandLiveCount;
    m_fRandOccurTiming = pParticleInfo->m_fRandOccurTiming;
    m_fRandScaleMin = pParticleInfo->m_fRandScaleMin;
    m_fRandScaleMax = pParticleInfo->m_fRandScaleMax;
    m_vRandScale3D = pParticleInfo->m_vRandScale3D;
    m_fRandAngle = pParticleInfo->m_fRandAngle;
    m_fCircleRadius = pParticleInfo->m_fCircleRadius;
    m_fCircleSpeed = pParticleInfo->m_fCircleSpeed;
    m_fAccelSizeDecayWidth = pParticleInfo->m_fAccelSizeDecayWidth;
    m_fAccelSizeDecayHeight = pParticleInfo->m_fAccelSizeDecayHeight;
    m_fAccelRotationBillboard = pParticleInfo->m_fAccelRotationBillboard;
    m_vAccelRotation = pParticleInfo->m_vAccelRotation;
    m_vOriginPosition = m_InitTransition.m_vPosition;
    m_vOriginVecFirst = m_InitTransition.m_vVecFirst;
    m_vOriginAcceleration = m_InitTransition.m_vAcceleration;

    if (m_type == PEFINFO::PARTICLETYPE_CYLINDER)
        CreateCylinder(m_fTopRadius, m_fBottomRadius, m_nPointNum, m_fHeight);

    InitializeVector();
};


void CParticle::Initialize(void)
{
    if (m_paTransition)
    {
        delete[] m_paTransition;
        m_paTransition = nullptr;
    };

    m_nTransitionMax = TRANSITIONNUM;
    m_paTransition = new TRANSITION[m_nTransitionMax]();

    if (m_paTransition)
    {
        for (int32 i = 0; i < m_nTransitionMax; ++i)
            m_listTransitionPool.push_back(&m_paTransition[i]);
    };
};


void CParticle::InitializeVector(void)
{
    SetVector(&Math::VECTOR3_AXIS_X,
              &Math::VECTOR3_AXIS_Y,
              &Math::VECTOR3_AXIS_Z);
};


void CParticle::StartParticle(void)
{
    m_fTimer = 0.0f;
    m_fCounter = 0.0f;
    m_bRun = true;
    m_bDraw = true;
    m_bSimple = false;
    m_bFinish = false;
    m_InitTransition.m_nDisplayNum = 0;

    DeleteElement();
};


void CParticle::Run(float dt)
{
    m_fTimer += dt;
    AddElapsedTime(dt);

    if (m_bRun)
    {
        if ((m_fAppearStart <= m_fCounter) &&
            (m_fAppearEnd >= m_fCounter) &&
            (m_InitTransition.m_fOccurTiming != 0.0f))
        {
            int32 nAppearNum = static_cast<int32>((m_fCounter - m_fAppearStart) / m_InitTransition.m_fOccurTiming);

            while (m_InitTransition.m_nDisplayNum <= nAppearNum)
            {
                for (int32 i = 0; i < m_InitTransition.m_nOccurNum; ++i)
                    CreateParticle(&m_InitTransition);

                ++m_InitTransition.m_nDisplayNum;
            };
            
            m_bSimple = true;
        }
        else if ((m_fAppearStart == m_fAppearEnd) && (!m_bSimple) && (m_fAppearEnd < m_fCounter))
        {
            for (int32 i = 0; i < m_InitTransition.m_nOccurNum; ++i)
                CreateParticle(&m_InitTransition);
            
            m_bSimple = true;
        };
    };

    auto it = m_listTransitionAlloc.begin();
    auto itEnd = m_listTransitionAlloc.end();
    while (it != itEnd)
    {
        TRANSITION* pTransition = &(*it);

        UpdateTransition(pTransition);
        TransitionColor(pTransition);
        TransitionSize(pTransition);
        TransitionAcceleration(pTransition);

        pTransition->m_vRotation.x = (pTransition->m_fCounterNow * (pTransition->m_vRotSpeed.x + m_vAccelRotationBuffer.x));
        pTransition->m_vRotation.y = (pTransition->m_fCounterNow * (pTransition->m_vRotSpeed.y + m_vAccelRotationBuffer.y));
        pTransition->m_vRotation.z = (pTransition->m_fCounterNow * (pTransition->m_vRotSpeed.z + m_vAccelRotationBuffer.z));

        pTransition->m_fRotationBillboard =
            (pTransition->m_fCounterNow * (pTransition->m_fRotSpeedBillboard + m_fAccelRotationBillboardBuffer));

        if (pTransition->m_fLiveCount < pTransition->m_fCounterNow)
        {
            it = m_listTransitionAlloc.erase(it);
            m_listTransitionPool.push_back(pTransition);

            --m_nTransitionNum;
        }
        else
        {
            ++it;
        };
    };

    if (m_fCounter >= m_fEndTime)
    {
        if (m_bLoop)
        {
            m_fTimer = 0.0f;
            m_fCounter = 0.0f;
            m_bSimple = false;
            m_InitTransition.m_nDisplayNum = 0;
        }
        else
        {
            m_bRun = false;
            m_bFinish = true;
        };
    }
    else
    {
        m_fCounter = m_fTimer;
    };
};


void CParticle::Draw(RwCamera* pCamera)
{
    if (!m_bDraw)
        return;

    RenderStatePush();
    SortZ(pCamera);

    RwMatrix* pMatModeling = RwFrameGetMatrixMacro(RwCameraGetFrameMacro(pCamera));
    ASSERT(pMatModeling);

    RwMatrix* pMatView = RwCameraGetViewMatrixMacro(pCamera);
    ASSERT(pMatView);

    RwMatrix matBillboard;
    RwMatrixSetIdentityMacro(&matBillboard);
    Math::Matrix_Invert(&matBillboard, pMatView);
    
    for (TRANSITION& it : m_listTransitionAlloc)
    {
        TRANSITION* pTransition = &it;

        switch (m_type)
        {
        case PEFINFO::PARTICLETYPE_BILLBOARD:
            {			
                std::memcpy(&pTransition->m_matrix, &matBillboard, sizeof(pTransition->m_matrix));
                TransitionRotation(pTransition, &pMatModeling->pos);
                pTransition->m_matrix.pos = pTransition->m_vPositionNow;
            }
            break;

        case PEFINFO::PARTICLETYPE_NORMALBOARD:
            {
                SetMatrixDirection(&pTransition->m_matrix, &pTransition->m_vDirection);
                TransitionRotation(pTransition, &pMatModeling->pos);
                TransitionScale(pTransition);

                RwMatrix matrix;
                RwMatrixSetIdentityMacro(&matrix);
                Math::Matrix_Update(&matrix, &m_vAxisX, &m_vAxisY, &m_vAxisZ, &Math::VECTOR3_ZERO);
                RwMatrixTransform(&pTransition->m_matrix, &matrix, rwCOMBINEPOSTCONCAT);

                pTransition->m_matrix.pos = pTransition->m_vPositionNow;
            }
            break;

        case PEFINFO::PARTICLETYPE_CYLINDER:
            {
                SetMatrixDirection(&pTransition->m_matrix, &pTransition->m_vDirection);
                TransitionRotation(pTransition, &pMatModeling->pos);
                TransitionScale(pTransition);

                RwMatrix matrix;
                RwMatrixSetIdentityMacro(&matrix);
                Math::Matrix_Update(&matrix, &m_vAxisX, &m_vAxisY, &m_vAxisZ, &Math::VECTOR3_ZERO);
                RwMatrixTransform(&pTransition->m_matrix, &matrix, rwCOMBINEPOSTCONCAT);

                pTransition->m_matrix.pos = pTransition->m_vPositionNow;

                CreateCylinder(m_fTopRadius, m_fBottomRadius, m_nPointNum, m_fHeight);
                SetColorToVertex(pTransition->m_ColorNow);

                DrawModel(&pTransition->m_matrix);
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    if ((m_type == PEFINFO::PARTICLETYPE_BILLBOARD) ||
        (m_type == PEFINFO::PARTICLETYPE_NORMALBOARD))
        DrawVertex(pCamera);

    RenderStatePop();
};


void CParticle::SetGravity(float fGravity)
{
    m_fGravity = fGravity;
};


void CParticle::SetUpdate(bool bUpdate)
{
    m_bRun = bUpdate;
};


void CParticle::SetLoop(bool bLoop)
{
    m_bLoop = bLoop;
};


void CParticle::SetBasisPositionForAll(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);

    m_vBasisPositionForAll = *pvPosition;
};


void CParticle::SetBasisPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    for (TRANSITION& it : m_listTransitionAlloc)
        it.m_vPositionBasis = *pvPosition;
};


void CParticle::SetClippingInfo(PEFINFO::CLIPPINGTYPE clippingtype, float fParameter)
{
    m_clippingtype = clippingtype;
    m_fClippingParameter = fParameter;
};


void CParticle::SetScale(float fScale)
{
    m_InitTransition.m_vPosition.x *= fScale;
    m_InitTransition.m_vPosition.y *= fScale;
    m_InitTransition.m_vPosition.z *= fScale;

    if ((m_type == PEFINFO::PARTICLETYPE_BILLBOARD) ||
        (m_type == PEFINFO::PARTICLETYPE_NORMALBOARD))
    {
        m_InitTransition.m_vSize.x *= fScale;
        m_InitTransition.m_vSize.y *= fScale;
    }
    else if (m_type == PEFINFO::PARTICLETYPE_CYLINDER)
    {
        m_InitTransition.m_vScale.x *= fScale;
        m_InitTransition.m_vScale.y *= fScale;
        m_InitTransition.m_vScale.z *= fScale;
    };
    
    m_InitTransition.m_fSpeedFirst *= fScale;
    m_InitTransition.m_fAccelSpeed *= fScale;
    
    m_vRandPosition.x *= fScale;
    m_vRandPosition.y *= fScale;
    m_vRandPosition.z *= fScale;
    
    m_vRandSize.x *= fScale;
    m_vRandSize.y *= fScale;
    
    m_fRandSpeedFirst *= fScale;
    m_fRandAccel *= fScale;
    
    m_vRandScale3D.x *= fScale;
    m_vRandScale3D.y *= fScale;
    m_vRandScale3D.z *= fScale;
    
    m_fCircleRadius *= fScale;
};


void CParticle::SetVector(const RwV3d* pvAxisX, const RwV3d* pvAxisY, const RwV3d* pvAxisZ)
{
    ASSERT(pvAxisX);
    ASSERT(pvAxisY);
    ASSERT(pvAxisZ);
    
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    
    RwV3d vBuff = Math::VECTOR3_ZERO;
    Math::Matrix_Update(&matrix, pvAxisX, pvAxisY, pvAxisZ, &vBuff);

    m_vAxisX = *pvAxisX;
    m_vAxisY = *pvAxisY;
    m_vAxisZ = *pvAxisZ;

    RwV3dTransformVector(&vBuff, &m_vOriginPosition, &matrix);
    m_InitTransition.m_vPosition = vBuff;

    RwV3dTransformVector(&vBuff, &m_vOriginVecFirst, &matrix);
    m_InitTransition.m_vVecFirst = vBuff;

    RwV3dTransformVector(&vBuff, &m_vOriginAcceleration, &matrix);
    m_InitTransition.m_vAcceleration = vBuff;

    m_fCircleBaseRotation = Math::ATan2(matrix.at.x, matrix.at.z);
};


bool CParticle::IsStringParticle(void) const
{
    return (m_kind == PEFINFO::PARTICLEKIND_STRING);
};


bool CParticle::IsEndParticle(void) const
{
    bool bResult = true;

    if (m_bRun || !m_bFinish || m_nTransitionNum)
        bResult = false;

    return bResult;
};


bool CParticle::IsDrawingParticle(void)
{
    return (m_nTransitionNum != 0);
};


void CParticle::AddElapsedTime(float dt)
{
    for (TRANSITION& it : m_listTransitionAlloc)
        it.m_fCounterNow += dt;
};


void CParticle::RenderStatePush(void)
{
    switch (m_blend)
    {
    case PEFINFO::BLENDTYPE_ADD:
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND,  rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
        break;

    case PEFINFO::BLENDTYPE_SUB:
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND,  rwBLENDZERO);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        break;

    case PEFINFO::BLENDTYPE_ALPHA:
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND,  rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        break;
        
    default:
        ASSERT(false);
        break;
    };
    
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    if (m_pMyTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRasterMacro(m_pMyTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
};


void CParticle::RenderStatePop(void)
{
    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};


void CParticle::SetMatrixDirection(RwMatrix* pMatrix, const RwV3d* pvLookVec)
{
    ASSERT(pMatrix);
    ASSERT(pvLookVec);
    
    RwV3d vMatPos = Math::VECTOR3_ZERO;
    vMatPos = pMatrix->pos;
    
    RwV2d vXZ = Math::VECTOR2_ZERO;
    vXZ.x = pvLookVec->x;
    vXZ.y = pvLookVec->z;

    const float fAngle = 90.0f;

    RwV2d vRot = Math::VECTOR2_ZERO;
    vRot.y = MATH_RAD2DEG(Math::ATan2(vXZ.x, vXZ.y));
    vRot.x = MATH_RAD2DEG(Math::ATan2(Math::Vec2_Length(&vXZ), pvLookVec->y)) - fAngle;

    while (vRot.x < -fAngle)
        vRot.x += fAngle;

    while (vRot.x > fAngle)
        vRot.x -= fAngle;

    RwMatrixRotate(pMatrix, &Math::VECTOR3_AXIS_Y, vRot.y, rwCOMBINEREPLACE);
    RwMatrixRotate(pMatrix, &pMatrix->right, vRot.x, rwCOMBINEPOSTCONCAT);
    RwMatrixTranslate(pMatrix, &vMatPos, rwCOMBINEPOSTCONCAT);
};


void CParticle::DeleteElement(void)
{
    if (!m_listTransitionAlloc.empty())
    {
        auto it = m_listTransitionAlloc.begin();
        auto itEnd = m_listTransitionAlloc.end();
        while (it != itEnd)
        {
            TRANSITION* pTransition = &(*it);
            it = m_listTransitionAlloc.erase(it);
            m_listTransitionPool.push_back(pTransition);
        };
    };

    m_nTransitionNum = 0;
};


void CParticle::SetColorToVertex(const RwRGBA& color)
{
    if (m_nPointNum == -1)
        return;
    
    for (int32 i = 0, j = 0; i < m_nPointNum + 1; ++i, j += 2)
    {
        ASSERT(j <= m_nVertexNum);
        s_aVertexForCylinder[j + 0].color = RWRGBALONGEX(color);
        s_aVertexForCylinder[j + 1].color = RWRGBALONGEX(color);
    };
};


bool CParticle::GetMyTexture(void)
{
    m_pMyTexture = CTextureManager::GetRwTexture(m_szTextureName);
    
    return (!!m_pMyTexture);
};


void CParticle::SetMyTexture(RwTexture* pTexture)
{
    ASSERT(pTexture);
    
    m_pMyTexture = pTexture;
};


void CParticle::SetVertexInfo(RwIm3DVertex* aVertexList, RwCamera* pCamera, TRANSITION* pTransition, uint32 uStartPos)
{
    if (!pTransition->m_ColorNow.alpha)
        return;

    ASSERT(pCamera);
    ASSERT(pTransition);
    ASSERT(aVertexList);

    switch (m_clippingtype)
    {
    case PEFINFO::CLIPPINGTYPE_CAMERADISTANCE:
        {
            RwMatrix* pMatModeling = RwFrameGetMatrixMacro(RwCameraGetFrameMacro(pCamera));            
            ASSERT(pMatModeling);

            RwV3d vCameraPos = pMatModeling->pos;

            RwV3d vDist = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vDist, &vCameraPos, &pTransition->m_vPositionNow);

            float fDist = Math::Vec3_Length(&vDist);
            if (fDist > m_fClippingParameter)
                return;
        }
        break;
        
    case PEFINFO::CLIPPINGTYPE_INCLUDEDCAMERA:
        {
            float fFar = RwCameraGetFarClipPlaneMacro(pCamera);
            float fNear = RwCameraGetNearClipPlaneMacro(pCamera);
            
            RwV3d vScreenPos = Math::VECTOR3_ZERO;
            RwV3dTransformPoint(&vScreenPos, &pTransition->m_vPositionNow, RwCameraGetViewMatrixMacro(pCamera));
            
            if (vScreenPos.z > 0.0f)
            {          
                vScreenPos.x *= (1.0f / vScreenPos.z);
                vScreenPos.y *= (1.0f / vScreenPos.z);

                if ((vScreenPos.z < fNear) ||
                    (vScreenPos.z > fFar) ||
                    (vScreenPos.x < 0.0f) ||
                    (vScreenPos.x > 1.0f) ||
                    (vScreenPos.y < 0.0f) ||
                    (vScreenPos.y > 1.0f))
                {
                    return;
                };
            }
            else
            {
                return;
            };
        }
        break;

    default:
        break;
    };

    float w = 0.0f;
    float h = 0.0f;

    if (m_type == PEFINFO::PARTICLETYPE_NORMALBOARD)
    {
        w = pTransition->m_vSizeNow.x;
        h = pTransition->m_vSizeNow.y;
    }
    else
    {
        RwRaster* pFrameBuffer = RwCameraGetRasterMacro(pCamera);
        ASSERT(pFrameBuffer);

        float fWidth  = static_cast<float>(pFrameBuffer->width);
        float fHeight = static_cast<float>(pFrameBuffer->height);
        float fViewRatio = CMapCamera::m_fDefaultViewSize / RwCameraGetViewWindowMacro(pCamera)->x;
        
        w = fViewRatio * pTransition->m_vSizeNow.x;
        h = (fWidth / fHeight) * (fViewRatio * pTransition->m_vSizeNow.y);
    };

    RwV3d aPos[4] = { Math::VECTOR3_ZERO };

    aPos[0].x = (w * 0.5f) * -1.0f;
    aPos[0].y = (h * 0.5f) * -1.0f;
    aPos[0].z = 0.0f;
    
    aPos[1].x = (w * 0.5f) * -1.0f;
    aPos[1].y = (h * 0.5f);
    aPos[1].z = 0.0f;
    
    aPos[2].x = (w * 0.5f);
    aPos[2].y = (h * 0.5f) * -1.0f;
    aPos[2].z = 0.0f;

    aPos[3].x = (w * 0.5f);
    aPos[3].y = (h * 0.5f);
    aPos[3].z = 0.0f;

    RwV3dTransformVectors(aPos, aPos, COUNT_OF(aPos), &pTransition->m_matrix);

    Math::Vec3_Add(&aPos[0], &aPos[0], &pTransition->m_vPositionNow);
    Math::Vec3_Add(&aPos[1], &aPos[1], &pTransition->m_vPositionNow);
    Math::Vec3_Add(&aPos[2], &aPos[2], &pTransition->m_vPositionNow);
    Math::Vec3_Add(&aPos[3], &aPos[3], &pTransition->m_vPositionNow);

    RwIm3DVertex* pVertex = &aVertexList[uStartPos];

    pVertex[0].objVertex.x = aPos[1].x;
    pVertex[0].objVertex.y = aPos[1].y;
    pVertex[0].objVertex.z = aPos[1].z;
    pVertex[0].u = m_u0;
    pVertex[0].v = m_v1;
    pVertex[0].color =  RWRGBALONGEX(pTransition->m_ColorNow);

    pVertex[1].objVertex.x = aPos[0].x;
    pVertex[1].objVertex.y = aPos[0].y;
    pVertex[1].objVertex.z = aPos[0].z;
    pVertex[1].u = m_u0;
    pVertex[1].v = m_v0;
    pVertex[1].color = RWRGBALONGEX(pTransition->m_ColorNow);

    pVertex[2].objVertex.x = aPos[3].x;
    pVertex[2].objVertex.y = aPos[3].y;
    pVertex[2].objVertex.z = aPos[3].z;
    pVertex[2].u = m_u1;
    pVertex[2].v = m_v1;
    pVertex[2].color = RWRGBALONGEX(pTransition->m_ColorNow);

    pVertex[3].objVertex.x = aPos[3].x;
    pVertex[3].objVertex.y = aPos[3].y;
    pVertex[3].objVertex.z = aPos[3].z;
    pVertex[3].u = m_u1;
    pVertex[3].v = m_v1;
    pVertex[3].color = RWRGBALONGEX(pTransition->m_ColorNow);

    pVertex[4].objVertex.x = aPos[0].x;
    pVertex[4].objVertex.y = aPos[0].y;
    pVertex[4].objVertex.z = aPos[0].z;
    pVertex[4].u = m_u0;
    pVertex[4].v = m_v0;
    pVertex[4].color = RWRGBALONGEX(pTransition->m_ColorNow);

    pVertex[5].objVertex.x = aPos[2].x;
    pVertex[5].objVertex.y = aPos[2].y;
    pVertex[5].objVertex.z = aPos[2].z;
    pVertex[5].u = m_u1;
    pVertex[5].v = m_v0;
    pVertex[5].color = RWRGBALONGEX(pTransition->m_ColorNow);

    m_nNowUseNum += 6;
};


void CParticle::DrawVertex(RwCamera* pCamera)
{
    ASSERT(pCamera);

    for (TRANSITION& it : m_listTransitionAlloc)
        SetVertexInfo(s_aVertex, pCamera, &it, m_nNowUseNum);

    if (m_nNowUseNum >= 6)
    {
        const uint32 uFlags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV;

        if (RwIm3DTransform(s_aVertex, m_nNowUseNum, nullptr, uFlags))
        {
            RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
            RwIm3DEnd();
        };

        m_nNowUseNum = 0;
    };
};


void CParticle::CreateCylinder(float fTopRadius, float fBottomRadius, int32 nPointNum, float fHeight)
{
    m_nPointNum = nPointNum;
    if (m_nPointNum == -1)
        return;
    
    m_fTopRadius = fTopRadius;
    m_fBottomRadius = fBottomRadius;
    m_fHeight = fHeight;
    m_nVertexNum = 2 * m_nPointNum + 2;
    ASSERT(m_nVertexNum <= VERTEXNUM);

    float fDeltaSegAngle = MATH_PI2 / float(m_nPointNum);
    
    for (int32 i = 0, j = 0; i < (m_nPointNum + 1); ++i, j += 2)
    {
        float tu = (1.0f / float(m_nPointNum) * float(m_nRepetitionNum) * i);

        RwIm3DVertex* pVertex = &s_aVertexForCylinder[j];

        pVertex[0].objVertex.x = Math::Sin(float(i) * fDeltaSegAngle) * m_fTopRadius;
        pVertex[0].objVertex.y = m_fHeight;
        pVertex[0].objVertex.z = Math::Cos(float(i) * fDeltaSegAngle) * m_fTopRadius;
        pVertex[0].objNormal.x = 0.0f;
        pVertex[0].objNormal.y = 0.0f;
        pVertex[0].objNormal.z = 0.0f;
        pVertex[0].u = tu;
        pVertex[0].v = 0.0f;
        pVertex[0].color = RWRGBALONGEX(m_InitTransition.m_Color);

        pVertex[1].objVertex.x = Math::Sin(float(i) * fDeltaSegAngle) * m_fBottomRadius;
        pVertex[1].objVertex.y = 0.0f;
        pVertex[1].objVertex.z = Math::Cos(float(i) * fDeltaSegAngle) * m_fBottomRadius;
        pVertex[1].objNormal.x = 0.0f;
        pVertex[1].objNormal.y = 0.0f;
        pVertex[1].objNormal.z = 0.0f;
        pVertex[1].u = tu;
        pVertex[1].v = 1.0f;
        pVertex[1].color = RWRGBALONGEX(m_InitTransition.m_Color);
    };
};


void CParticle::RecreateCylinder(float fTopRadius, float fBottomRadius, int32 nPointNum, float fHeight)
{
    CreateCylinder(fTopRadius, fBottomRadius, nPointNum, fHeight);
};


void CParticle::DrawModel(RwMatrix* pMatrix)
{
    ASSERT(pMatrix);
    
    uint32 uFlags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV;
    
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLFRONT);
    if (RwIm3DTransform(s_aVertexForCylinder, m_nVertexNum, pMatrix, uFlags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        RwIm3DEnd();
    };
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLBACK);
    if (RwIm3DTransform(s_aVertexForCylinder, m_nVertexNum, pMatrix, uFlags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        RwIm3DEnd();
    };
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
};


void CParticle::UpdateTransition(TRANSITION* pTransition)
{
    pTransition->m_vPositionNow = pTransition->m_vPositionBasis;

    RwV3d vSpeed = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vSpeed, &pTransition->m_vVecFirst, pTransition->m_fSpeedFirst);
    
    RwV3d vAccel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vAccel, &pTransition->m_vAcceleration, (pTransition->m_fAccelSpeed * pTransition->m_fCounterNow));

    TransitionNatureFall(pTransition, &vAccel);
    TransitionNatureRise(pTransition, &vAccel);
    
    RwV3d vSpeedNow = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vSpeedNow, &vAccel, &vSpeed);
    Math::Vec3_Scale(&vSpeedNow, &vSpeedNow, pTransition->m_fCounterNow);

    Math::Vec3_Add(&pTransition->m_vPositionNow,
                   &pTransition->m_vPositionNow,
                   &pTransition->m_vPositionRand);

    Math::Vec3_Add(&pTransition->m_vPositionNow,
                   &pTransition->m_vPositionNow,
                   &pTransition->m_vPositionCircle);

    Math::Vec3_Add(&pTransition->m_vPositionNow,
                   &pTransition->m_vPositionNow,
                   &vSpeedNow);

    Math::Vec3_Add(&pTransition->m_vPositionNow,
                   &pTransition->m_vPositionNow,
                   &pTransition->m_vPosition);
};


void CParticle::CreateParticle(TRANSITION* pInitTransition)
{
    TRANSITION transition;
    std::memcpy(&transition, pInitTransition, sizeof(transition));

    transition.m_fCounter = 0.0f;
    transition.m_vPositionBasis = m_vBasisPositionForAll;
    
    m_fColorDecayBuffer[0] = 0.01f * (static_cast<float>(m_InitTransition.m_Color.red)   * m_fColorDecayR);
    m_fColorDecayBuffer[1] = 0.01f * (static_cast<float>(m_InitTransition.m_Color.green) * m_fColorDecayG);
    m_fColorDecayBuffer[2] = 0.01f * (static_cast<float>(m_InitTransition.m_Color.blue)  * m_fColorDecayB);
    m_fColorDecayBuffer[3] = 0.01f * (static_cast<float>(m_InitTransition.m_Color.alpha) * m_fColorDecayA);
    
    m_fSizeDecayBuffer[0] = 0.01f * (m_InitTransition.m_vSize.x * m_fSizeDecayWidth);
    m_fSizeDecayBuffer[1] = 0.01f * (m_InitTransition.m_vSize.y * m_fSizeDecayHeight);

    m_vScaleDecayBuffer = m_vScaleDecay;
    
    CircleMove(&transition.m_vPositionCircle, m_fCircleRadius, m_fCircleSpeed, m_fCounter, m_uLinearFlag);
    AddRandomStartPos(&transition.m_vPositionRand, &m_vRandPosition, m_uRandCheckFlag);
    AddRandomStartSize(&transition.m_vSize, &m_vRandSize, m_uRandCheckFlag);
    AddRandomFirstVec(&transition.m_vVecFirst, &m_vRandFirstVec, m_uRandCheckFlag);
    AddRandomAccelVec(&transition.m_vAcceleration, &m_vRandAcceleration, m_uRandCheckFlag);    
    RandomScale(&transition.m_vSize, m_fRandScaleMin, m_fRandScaleMax, m_uRandCheckFlag);
    RandomScale3D(&transition.m_vScale, &m_vRandScale3D, m_uRandCheckFlag);    
    transition.m_fLiveCount = AddRandomStartLive(transition.m_fLiveCount, m_fRandLiveCount, m_uRandCheckFlag);
    transition.m_fSpeedFirst = AddRandomSpeed(transition.m_fSpeedFirst, m_fRandSpeedFirst, m_uRandCheckFlag);
    transition.m_fAccelSpeed = AddRandomAccel(transition.m_fAccelSpeed, m_fRandAccel, m_uRandCheckFlag);
    transition.m_fInitRotationBillboard = RandomAngle(m_fRandAngle, m_uRandCheckFlag);

    ASSERT(m_nTransitionNum < m_nTransitionMax);
    if (m_nTransitionNum < m_nTransitionMax)
    {
        ASSERT(!m_listTransitionPool.empty());

        TRANSITION* pNode = m_listTransitionPool.front();
        m_listTransitionPool.erase(pNode);
        m_listTransitionAlloc.push_back(pNode);

        CopyTransition(pNode, &transition);

        ++m_nTransitionNum;
    };
};


void CParticle::TransitionNatureFall(TRANSITION* pTransition, RwV3d* pvVec)
{
    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_FALL)
        pvVec->y += (m_fGravity * pTransition->m_fCounterNow);
};


void CParticle::TransitionNatureRise(TRANSITION* pTransition, RwV3d* pvVec)
{
    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_RISE)
        pvVec->y += ((m_fGravity * pTransition->m_fCounterNow) * -1.0f);
};


void CParticle::TransitionColor(TRANSITION* pTransition)
{
    int32 r = static_cast<int32>(pTransition->m_Color.red);
    int32 g = static_cast<int32>(pTransition->m_Color.green);
    int32 b = static_cast<int32>(pTransition->m_Color.blue);
    int32 a = static_cast<int32>(pTransition->m_Color.alpha);
   
    r += static_cast<int32>(m_fColorDecayBuffer[0] * pTransition->m_fCounterNow);
    g += static_cast<int32>(m_fColorDecayBuffer[1] * pTransition->m_fCounterNow);
    b += static_cast<int32>(m_fColorDecayBuffer[2] * pTransition->m_fCounterNow);
    a += static_cast<int32>(m_fColorDecayBuffer[3] * pTransition->m_fCounterNow);

    r = Clamp(r, 0, 255);
    g = Clamp(g, 0, 255);
    b = Clamp(b, 0, 255);
    a = Clamp(a, 0, 255);

    pTransition->m_ColorNow.red   = static_cast<RwUInt8>(r);
    pTransition->m_ColorNow.green = static_cast<RwUInt8>(g);
    pTransition->m_ColorNow.blue  = static_cast<RwUInt8>(b);
    pTransition->m_ColorNow.alpha = static_cast<RwUInt8>(a);
};


void CParticle::TransitionSize(TRANSITION* pTransition)
{
    float fBuffX =
        pTransition->m_vSize.x + (pTransition->m_fCounterNow * (m_fAccelSizeDecayWidthBuffer + m_fSizeDecayBuffer[0]));
    
    float fBuffY =
        pTransition->m_vSize.y + (pTransition->m_fCounterNow * (m_fAccelSizeDecayHeightBuffer + m_fSizeDecayBuffer[1]));

    if (fBuffX < 0.0f)
        fBuffX = 0.0f;

    if (fBuffY < 0.0f)
        fBuffY = 0.0f;

    pTransition->m_vSizeNow.x = fBuffX;
    pTransition->m_vSizeNow.y = fBuffY;
};


void CParticle::TransitionScale(TRANSITION* pTransition)
{
    float fBuffScaleX = pTransition->m_vScale.x + (m_vScaleDecayBuffer.x * pTransition->m_fCounterNow);
    float fBuffScaleY = pTransition->m_vScale.y + (m_vScaleDecayBuffer.y * pTransition->m_fCounterNow);
    float fBuffScaleZ = pTransition->m_vScale.z + (m_vScaleDecayBuffer.z * pTransition->m_fCounterNow);

    if (fBuffScaleX < 0.0f)
        fBuffScaleX = 0.0f;

    if (fBuffScaleY < 0.0f)
        fBuffScaleY = 0.0f;

    if (fBuffScaleZ < 0.0f)
        fBuffScaleZ = 0.0f;

    pTransition->m_vScaleNow.x = fBuffScaleX;
    pTransition->m_vScaleNow.y = fBuffScaleY;
    pTransition->m_vScaleNow.z = fBuffScaleZ;

    RwMatrixScale(&pTransition->m_matrix, &pTransition->m_vScaleNow, rwCOMBINEPOSTCONCAT);
};


void CParticle::TransitionAcceleration(TRANSITION* pTransition)
{
    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ACCELSIZE2D_X)
    {
        m_fAccelSizeDecayWidthBuffer =
            pTransition->m_fCounterNow * (0.01f * (m_InitTransition.m_vSize.x * m_fAccelSizeDecayWidth));
    };

    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ACCELSIZE2D_Y)
    {
        m_fAccelSizeDecayHeightBuffer =
            pTransition->m_fCounterNow * (0.01f * (m_InitTransition.m_vSize.y * m_fAccelSizeDecayHeight));
    };

    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ACCELROT3D_X)
        m_vAccelRotationBuffer.x = m_vAccelRotation.x * pTransition->m_fCounterNow;

    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ACCELROT3D_Y)
        m_vAccelRotationBuffer.y = m_vAccelRotation.y * pTransition->m_fCounterNow;

    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ACCELROT3D_Z)
        m_vAccelRotationBuffer.z = m_vAccelRotation.z * pTransition->m_fCounterNow;

    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ACCELROT2D)
        m_fAccelRotationBillboardBuffer = m_fAccelRotationBillboard * pTransition->m_fCounterNow;
};


void CParticle::TransitionRotation(TRANSITION* pTransition, const RwV3d* pCameraPos)
{
    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ROT_X)
    {
        RwMatrixRotate(&pTransition->m_matrix,
                       &Math::VECTOR3_AXIS_X,
                       pTransition->m_vRotation.x,
                       rwCOMBINEPOSTCONCAT);
    };

    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ROT_Y)
    {
        RwMatrixRotate(&pTransition->m_matrix,
                       &Math::VECTOR3_AXIS_Y,
                       pTransition->m_vRotation.y,
                       rwCOMBINEPOSTCONCAT);
    };

    if (m_uPatternFlag & PEFINFO::PATTERNFLAG_ROT_Z)
    {
        RwMatrixRotate(&pTransition->m_matrix,
                       &Math::VECTOR3_AXIS_Z,
                       pTransition->m_vRotation.z,
                       rwCOMBINEPOSTCONCAT);
    };

    if ((m_uPatternFlag & PEFINFO::PATTERNFLAG_BILLBOARD) ||
        (m_uRandCheckFlag & PEFINFO::RANDFLAG_RANDANGLE))
    {
        float fAngle = pTransition->m_fInitRotationBillboard + pTransition->m_fRotationBillboard;
        
        RwV3d vAxis = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vAxis, &pTransition->m_vPositionNow, pCameraPos);
        Math::Vec3_Normalize(&vAxis, &vAxis);

        const float EPSILON = 0.000001f;

        if (Math::Vec3_Length(&vAxis) > EPSILON)
            RwMatrixRotate(&pTransition->m_matrix, &vAxis, fAngle, rwCOMBINEPOSTCONCAT);
    };
};


float CParticle::GetSortZ(RwCamera* pCamera, RwV3d* pvPosition)
{
    ASSERT(pCamera);
    ASSERT(pvPosition);

    RwV3d vScreenPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vScreenPos, pvPosition, RwCameraGetViewMatrixMacro(pCamera));

    return vScreenPos.z;
};


void CParticle::SortZ(RwCamera* pCamera)
{
    ASSERT(pCamera);

    if (m_blend != PEFINFO::BLENDTYPE_ALPHA)
        return;

    for (TRANSITION& it : m_listTransitionAlloc)
        it.m_fSortZ = GetSortZ(pCamera, &it.m_vPositionNow);
    
    auto itEnd = m_listTransitionAlloc.end();
    auto itA = m_listTransitionAlloc.begin();
    while (itA != itEnd)
    {
        TRANSITION* pTransition = &*itA;
        TRANSITION* pExchangeTransition = nullptr;
        float fMaxScreenZ = pTransition->m_fSortZ;

        auto itB = ++itA;
        while (itB != itEnd)
        {
            if (fMaxScreenZ < (*itB).m_fSortZ)
            {
                fMaxScreenZ = (*itB).m_fSortZ;
                pExchangeTransition = &*itB;
            };

            ++itB;
        };

        if (pExchangeTransition)
        {
            TRANSITION bufferTransition;

            CopyTransition(&bufferTransition, pTransition);
            CopyTransition(pTransition, pExchangeTransition);
            CopyTransition(pExchangeTransition, &bufferTransition);

            pExchangeTransition = nullptr;
        };
    };
};


void CParticle::AddRandomStartPos(RwV3d* pvStartPos, RwV3d* pvRandElement, uint32 uRandomFlag)
{
    if (uRandomFlag & PEFINFO::RANDFLAG_STARTPOS_X)
    {
        float fRand = (Randomize() * pvRandElement->x);

        pvStartPos->x += (m_vAxisX.x * fRand);
        pvStartPos->y += (m_vAxisX.y * fRand);
        pvStartPos->z += (m_vAxisX.z * fRand);
    };

    if (uRandomFlag & PEFINFO::RANDFLAG_STARTPOS_Y)
    {
        float fRand = (Randomize() * pvRandElement->y);

        pvStartPos->x += (m_vAxisY.x * fRand);
        pvStartPos->y += (m_vAxisY.y * fRand);
        pvStartPos->z += (m_vAxisY.z * fRand);
    };

    if (uRandomFlag & PEFINFO::RANDFLAG_STARTPOS_Z)
    {
        float fRand = (Randomize() * pvRandElement->z);

        pvStartPos->x += (m_vAxisZ.x * fRand);
        pvStartPos->y += (m_vAxisZ.y * fRand);
        pvStartPos->z += (m_vAxisZ.z * fRand);
    };
};


void CParticle::AddRandomStartSize(RwV2d* pvStartSize, RwV2d* pvRandElement, uint32 uRandomFlag)
{
    if (uRandomFlag & PEFINFO::RANDFLAG_STARTSIZE_X)
        pvStartSize->x += (Randomize() * pvRandElement->x);

    if (uRandomFlag & PEFINFO::RANDFLAG_STARTSIZE_Y)
        pvStartSize->y += (Randomize() * pvRandElement->y);
};


float CParticle::AddRandomStartLive(float fFirstLive, float fRandElement, uint32 uRandomFlag)
{
    float fRet = fFirstLive;

    if (uRandomFlag & PEFINFO::RANDFLAG_LIVECOUNT)
        fRet += (Randomize() * fRandElement);

    return fRet;
};


void CParticle::AddRandomFirstVec(RwV3d* pvFirstVec, RwV3d* pvRandomElement, uint32 uRandomFlag)
{
    if (uRandomFlag & PEFINFO::RANDFLAG_FIRSTVEC_X)
    {
        float fRand = (Randomize() * pvRandomElement->x);

        pvFirstVec->x += (m_vAxisX.x * fRand);
        pvFirstVec->y += (m_vAxisX.y * fRand);
        pvFirstVec->z += (m_vAxisX.z * fRand);
    };

    if (uRandomFlag & PEFINFO::RANDFLAG_FIRSTVEC_Y)
    {
        float fRand = (Randomize() * pvRandomElement->y);

        pvFirstVec->x += (m_vAxisY.x * fRand);
        pvFirstVec->y += (m_vAxisY.y * fRand);
        pvFirstVec->z += (m_vAxisY.z * fRand);
    };

    if (uRandomFlag & PEFINFO::RANDFLAG_FIRSTVEC_Z)
    {
        float fRand = (Randomize() * pvRandomElement->z);

        pvFirstVec->x += (m_vAxisZ.x * fRand);
        pvFirstVec->y += (m_vAxisZ.y * fRand);
        pvFirstVec->z += (m_vAxisZ.z * fRand);
    };

    if ((pvFirstVec->x != 0.0f) &&
        (pvFirstVec->y != 0.0f) &&
        (pvFirstVec->z != 0.0f))
    {
        Math::Vec3_Normalize(pvFirstVec, pvFirstVec);
    };
};


float CParticle::AddRandomSpeed(float fFirstSpeed, float fRandElement, uint32 uRandomFlag)
{
    float fRet = fFirstSpeed;

    if (uRandomFlag & PEFINFO::RANDFLAG_FIRSTSPEED)
        fRet += (Randomize() * fRandElement);

    return fRet;
};


void CParticle::AddRandomAccelVec(RwV3d* pvAccelVec, RwV3d* pvRandomElement, uint32 uRandomFlag)
{
    if (uRandomFlag & PEFINFO::RANDFLAG_ACCELVEC_X)
    {
        float fRand = (Randomize() * pvRandomElement->x);

        pvAccelVec->x += (m_vAxisX.x * fRand);
        pvAccelVec->y += (m_vAxisX.y * fRand);
        pvAccelVec->z += (m_vAxisX.z * fRand);
    };

    if (uRandomFlag & PEFINFO::RANDFLAG_ACCELVEC_Y)
    {
        float fRand = (Randomize() * pvRandomElement->y);

        pvAccelVec->x += (m_vAxisY.x * fRand);
        pvAccelVec->y += (m_vAxisY.y * fRand);
        pvAccelVec->z += (m_vAxisY.z * fRand);
    };

    if (uRandomFlag & PEFINFO::RANDFLAG_ACCELVEC_Z)
    {
        float fRand = (Randomize() * pvRandomElement->z);

        pvAccelVec->x += (m_vAxisZ.x * fRand);
        pvAccelVec->y += (m_vAxisZ.y * fRand);
        pvAccelVec->z += (m_vAxisZ.z * fRand);
    };

    if ((pvAccelVec->x != 0.0f) &&
        (pvAccelVec->y != 0.0f) &&
        (pvAccelVec->z != 0.0f))
    {
        Math::Vec3_Normalize(pvAccelVec, pvAccelVec);
    };
};


float CParticle::AddRandomAccel(float fAccel, float fRandElement, uint32 uRandomFlag)
{    
    float fRet = fAccel;
    
    if (uRandomFlag & PEFINFO::RANDFLAG_ACCELVEC_Z)
        fRet += (Randomize() * fRandElement);

    return fRet;
};


void CParticle::RandomScale(RwV2d* pvStartSize, float fScaleMin, float fScaleMax, uint32 uRandomFlag)
{
    if (uRandomFlag & PEFINFO::RANDFLAG_SCALESIZE2D)
    {
        float fValue = (Randomize() * (fScaleMax - fScaleMin) + fScaleMin) * 0.01f;

        pvStartSize->x *= fValue;
        pvStartSize->y *= fValue;
    };
};


void CParticle::RandomScale3D(RwV3d* pvStartSize, RwV3d* pvRandomElement, uint32 uRandomFlag)
{
    if (uRandomFlag & PEFINFO::RANDFLAG_SCALESIZE3D_X)
        pvStartSize->x += (Randomize() * pvRandomElement->x);
    
    if (uRandomFlag & PEFINFO::RANDFLAG_SCALESIZE3D_Y)
        pvStartSize->y += (Randomize() * pvRandomElement->y);
    
    if (uRandomFlag & PEFINFO::RANDFLAG_SCALESIZE3D_Z)
        pvStartSize->z += (Randomize() * pvRandomElement->z);
};


float CParticle::RandomAngle(float fRandElement, uint32 uRandomFlag)
{
    float fRet = 0.0f;
    
    if (uRandomFlag & PEFINFO::RANDFLAG_RANDANGLE)
        fRet = (Randomize() * fRandElement);

    return fRet;
};


void CParticle::CircleMove(RwV3d* pvDest, float fRadius, float fSpeed, float fTime, uint32 uFlag)
{
    if (uFlag & PEFINFO::MOVEFLAG_CIRCLE)
    {
        float fBasis = fSpeed * (1.0f / 360.0f) * fTime * MATH_PI2 + m_fCircleBaseRotation;

        pvDest->x = Math::Sin(fBasis) * fRadius;
        pvDest->y = 0.0f;
        pvDest->z = Math::Cos(fBasis) * fRadius;
    };
};


void CParticle::CopyTransition(TRANSITION* pDst, TRANSITION* pOrg)
{
    ASSERT(pDst);
    ASSERT(pOrg);
    ASSERT(pDst != pOrg);

    pDst->m_vPositionBasis          = pOrg->m_vPositionBasis;
    pDst->m_vPositionNow            = pOrg->m_vPositionNow;
    pDst->m_vPositionRand           = pOrg->m_vPositionRand;
    pDst->m_vPositionCircle         = pOrg->m_vPositionCircle;
    pDst->m_vPosition               = pOrg->m_vPosition;
    pDst->m_Color                   = pOrg->m_Color;
    pDst->m_ColorNow                = pOrg->m_ColorNow;
    pDst->m_matrix                  = pOrg->m_matrix;
    pDst->m_vSize                   = pOrg->m_vSize;
    pDst->m_vSizeNow                = pOrg->m_vSizeNow;
    pDst->m_vScale                  = pOrg->m_vScale;
    pDst->m_vScaleNow               = pOrg->m_vScaleNow;
    pDst->m_vDirection              = pOrg->m_vDirection;
    pDst->m_vVecFirst               = pOrg->m_vVecFirst;
    pDst->m_fSpeedFirst             = pOrg->m_fSpeedFirst;
    pDst->m_vAcceleration           = pOrg->m_vAcceleration;
    pDst->m_fAccelSpeed             = pOrg->m_fAccelSpeed;
    pDst->m_fCounter                = pOrg->m_fCounter;
    pDst->m_fCounterNow             = pOrg->m_fCounterNow;
    pDst->m_fLiveCount              = pOrg->m_fLiveCount;
    pDst->m_nOccurNum               = pOrg->m_nOccurNum;
    pDst->m_fOccurTiming            = pOrg->m_fOccurTiming;
    pDst->m_fRotSpeedBillboard      = pOrg->m_fRotSpeedBillboard;
    pDst->m_fRotationBillboard      = pOrg->m_fRotationBillboard;
    pDst->m_fInitRotationBillboard  = pOrg->m_fInitRotationBillboard;
    pDst->m_vRotSpeed               = pOrg->m_vRotSpeed;
    pDst->m_vRotation               = pOrg->m_vRotation;
    pDst->m_fSortZ                  = pOrg->m_fSortZ;
};


float CParticle::Randomize(void) const
{
    return (static_cast<float>(Math::Rand() % RANDSEED) / static_cast<float>(RANDSEED));
};