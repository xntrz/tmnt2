#include "Cloth.hpp"

#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "System/Common/RenderState.hpp"


static RwIm3DVertex s_aVertices[100] = { 0 };


CCloth::CCloth(void)
: m_pClothPointList(nullptr)
, m_pSpringList(nullptr)
, m_nDivisionWidth(3)
, m_nDivisionHeight(3)
, m_nRows(4)
, m_nColumns(4)
, m_fWidth(1.0f)
, m_fHeight(1.0f)
, m_fClothMass(100.0f)
, m_fSpringTensionConstant(226.7f)
, m_fSpringShearConstant(276.7f)
, m_fSpringDampingConstant(2.0f)
, m_fSpringLength(100.0f)
, m_fDragCoefficient(0.01f)
, m_nWindFactor(30)
, m_fGravity(-0.98f)
, m_Color({ 0xFF, 0xFF, 0xFF,0xFF })
, m_vWind(Math::VECTOR3_ZERO)
, m_pTexture(nullptr)
, m_fDt(0.0f)
{
    ;
};


CCloth::~CCloth(void)
{
    if (m_pClothPointList)
    {
        delete[] m_pClothPointList;
        m_pClothPointList = nullptr;
    };

    if (m_pSpringList)
    {
        delete[] m_pSpringList;
        m_pSpringList = nullptr;
    };
};


void CCloth::Create(void)
{
	CreateCloth();
};


void CCloth::Update(float dt)
{
    ASSERT(m_pClothPointList);
    ASSERT(m_pSpringList);

    if (!m_pClothPointList)
        return;

    if (!m_pSpringList)
        return;

    SetDeltaTime(dt);
    StepSimulation();
};


void CCloth::Draw(void)
{
    if (!m_pClothPointList)
        return;
    
    if (!m_pSpringList)
        return;

    RenderStatePush();
    
    uint32 uNumVertices = 0;
    uint32 uFlags = rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA;
    if (m_pTexture)
        FLAG_SET(uFlags, rwIM3D_VERTEXUV);

    for (int32 i = 0; i < m_nDivisionWidth; ++i)
    {
        for (int32 j = 0; j < m_nColumns; ++j)
        {
            int32 nIndex = i + j * m_nRows;
            float v = (1.0f / float(m_nDivisionHeight)) * float(j);
            float u0 = (1.0f / float(m_nDivisionWidth)) * float(i);
            float u1 = (1.0f / float(m_nDivisionWidth)) * float(i + 1);

            SetVertex(
                &s_aVertices[uNumVertices],
                &m_pClothPointList[nIndex].m_vPosition,
                u0,
                v
            );

            SetVertex(
                &s_aVertices[uNumVertices + 1],
                &m_pClothPointList[nIndex + 1].m_vPosition,
                u1,
                v
            );

            uNumVertices += 2;
        };

        ASSERT(uNumVertices <= COUNT_OF(s_aVertices));
		uNumVertices = 0;

        if (RwIm3DTransform(s_aVertices, m_nColumns * 2, nullptr, uFlags))
        {
            RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
            RwIm3DEnd();
        };
    };

    RenderStatePop();
};


void CCloth::SetBaseParameter(int32 nDivisionWidth, int32 nDivisionHeight, float w, float h)
{
    m_nDivisionWidth = nDivisionWidth;
    m_nDivisionHeight = nDivisionHeight;
    
    m_nRows = m_nDivisionWidth + 1;
    m_nColumns = m_nDivisionHeight + 1;

    m_fWidth = w;
    m_fHeight = h;
};


void CCloth::SetColor(const RwRGBA& color)
{
    m_Color = color;
};


void CCloth::SetTension(float fTension)
{
    m_fSpringTensionConstant = fTension;
};


void CCloth::SetShear(float fShear)
{
    m_fSpringShearConstant = fShear;
};


void CCloth::SetLength(float fLen)
{
    m_fSpringLength = fLen;
};


void CCloth::SetDrag(float fDrag)
{
    m_fDragCoefficient = fDrag;
};


void CCloth::SetWeight(float fWeight)
{
    m_fClothMass = fWeight;
};


void CCloth::SetWind(const RwV3d* pvWind)
{
    ASSERT(pvWind);
    
    m_vWind = *pvWind;
};


void CCloth::SetGravity(float fGravity)
{
    m_fGravity = fGravity;
};


void CCloth::SetJointPosition(const RwV3d* pvJoint)
{
    ASSERT(pvJoint);
    
    for (int32 i = 0; i < m_nRows; ++i)
        m_pClothPointList[i].m_vPosition = *pvJoint;
};


void CCloth::ResetCloth(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);

    float x = (m_fWidth / float(m_nDivisionWidth));
    float y = -1.0f * (m_fHeight / float(m_nDivisionHeight));
    
    for (int32 i = 0; i < m_nColumns; ++i)
    {
        for (int32 j = 0; j < m_nRows; ++j)
        {
            int32 nIndex = j + i * m_nRows;

            m_pClothPointList[nIndex].m_vPosition.x = pvPosition->x + (x * j);
            m_pClothPointList[nIndex].m_vPosition.y = pvPosition->y + (y * i);
            m_pClothPointList[nIndex].m_vPosition.z = pvPosition->z;
        };
    };
};


void CCloth::CreateCloth(void)
{
    ASSERT(!m_pClothPointList);
    ASSERT(!m_pSpringList);

    if (m_pClothPointList)
    {
        delete[] m_pClothPointList;
        m_pClothPointList = nullptr;
    };

    if (m_pSpringList)
    {
        delete[] m_pSpringList;
        m_pSpringList = nullptr;
    };

    float x = (m_fWidth / float(m_nDivisionWidth));
    float y = -1.0f * (m_fHeight / float(m_nDivisionHeight));
    float fClothBlockMass = (m_fClothMass / float(2 * m_nDivisionWidth * m_nDivisionHeight));
    const int32 nSpringMax = (m_nRows * m_nDivisionHeight) + (m_nColumns * m_nDivisionWidth);
    const int32 nClothPtMax = m_nRows * m_nColumns;

    m_pClothPointList = new CLOTHPOINT[nClothPtMax]();
    m_pSpringList = new SPRING[nSpringMax]();

    for (int32 i = 0; i < m_nColumns; ++i)
    {
        for (int32 j = 0; j < m_nRows; ++j)
        {
            float fMass = 1.0f;

            ASSERT(fClothBlockMass > 0.0f);

            if (
                (i || j) &&
                ((m_nDivisionHeight != i) || (m_nDivisionWidth != j))
                )
            {
                if (
                    (i || (m_nDivisionWidth != j)) &&
                    (j || (m_nDivisionHeight != i))
                    )
                {
                    if (j && (m_nDivisionWidth != j) || !i || (i == m_nDivisionHeight))
                        fMass = 6.0f;
                    else
                        fMass = 3.0f;
                }
                else
                {
                    fMass = 2.0f;
                };
            };

            int32 nIndex = j + i * m_nRows;

            CLOTHPOINT* pClothPoint = &m_pClothPointList[nIndex];

			pClothPoint->m_fMass = (fMass * fClothBlockMass) / 3.0f;
            pClothPoint->m_fInvMass = 1.0f / pClothPoint->m_fMass;
            pClothPoint->m_vPosition.x = j * x;
            pClothPoint->m_vPosition.y = i * y;
            pClothPoint->m_vPosition.z = 0.0f;
            
            pClothPoint->m_vVelocity.x = 0.0f;
            pClothPoint->m_vVelocity.y = 0.0f;
            pClothPoint->m_vVelocity.z = 0.0f;
            
            pClothPoint->m_vAcceleration.x = 0.0f;
            pClothPoint->m_vAcceleration.y = 0.0f;
            pClothPoint->m_vAcceleration.z = 0.0f;
            
            pClothPoint->m_vForces.x = 0.0f;
            pClothPoint->m_vForces.y = 0.0f;
            pClothPoint->m_vForces.z = 0.0f;

            if (i)
                pClothPoint->m_bLock = false;
            else
                pClothPoint->m_bLock = true;
        };
    };

    int32 nSpringIndex = 0;

    for (int32 i = 0; i < m_nColumns; ++i)
    {
        for (int32 j = 0; j < m_nDivisionWidth; ++j)
        {
            SPRING* pSpring = &m_pSpringList[nSpringIndex];

            pSpring->m_nPointIndex1     = j + i * m_nRows;
            pSpring->m_nPointIndex2     = (j + i * m_nRows) + 1;
            pSpring->m_fSpringConstant  = m_fSpringTensionConstant;
            pSpring->m_fDecayCoefficient= m_fSpringDampingConstant;

            CLOTHPOINT* pClothPoint0 = &m_pClothPointList[pSpring->m_nPointIndex1];
            CLOTHPOINT* pClothPoint1 = &m_pClothPointList[pSpring->m_nPointIndex2];

            RwV3d vDistance = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(
                &vDistance,
                &pClothPoint0->m_vPosition,
                &pClothPoint1->m_vPosition
            );

            pSpring->m_fLength = Math::Vec3_Length(&vDistance);
            ++nSpringIndex;
        };
    };

    for (int32 i = 0; i < m_nDivisionHeight; ++i)
    {
        for (int32 j = 0; j < m_nRows; ++j)
        {
            SPRING* pSpring = &m_pSpringList[nSpringIndex];

            pSpring->m_nPointIndex1     = j + i * m_nRows;
            pSpring->m_nPointIndex2     = (j + i * m_nRows) + m_nRows;
            pSpring->m_fSpringConstant  = m_fSpringTensionConstant;
            pSpring->m_fDecayCoefficient= m_fSpringDampingConstant;

            CLOTHPOINT* pClothPoint0 = &m_pClothPointList[pSpring->m_nPointIndex1];
            CLOTHPOINT* pClothPoint1 = &m_pClothPointList[pSpring->m_nPointIndex2];

            RwV3d vDistance = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(
                &vDistance,
                &pClothPoint0->m_vPosition,
                &pClothPoint1->m_vPosition
            );

            pSpring->m_fLength = Math::Vec3_Length(&vDistance);
            ++nSpringIndex;
        };
    };

    ASSERT(nSpringIndex == nSpringMax);
};


void CCloth::CalcForces(void)
{
    const int32 nClothPtMax = (m_nRows * m_nColumns);
    const int32 nSpringMax = (m_nRows * m_nDivisionHeight) + (m_nColumns * m_nDivisionWidth);
    
    for (int32 i = 0; i < nClothPtMax; ++i)
    {
        m_pClothPointList[i].m_vForces = Math::VECTOR3_ZERO;
    };

    for (int32 i = 0; i < nClothPtMax; ++i)
    {
        CLOTHPOINT* pClothPoint = &m_pClothPointList[i];

        if (pClothPoint->m_bLock)
            continue;

        pClothPoint->m_vForces.y = m_fGravity * pClothPoint->m_fMass;

        RwV3d vDragVector = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDragVector, &vDragVector, &pClothPoint->m_vVelocity);
        Math::Vec3_Normalize(&vDragVector, &vDragVector);

        float fLength = Math::Vec3_Length(&pClothPoint->m_vVelocity);
        float fDragScale = fLength * (m_fDragCoefficient * fLength);
        Math::Vec3_Scale(&vDragVector, &vDragVector, fDragScale);
        Math::Vec3_Add(&pClothPoint->m_vForces, &pClothPoint->m_vForces, &vDragVector);

		float fRandScale = (Math::Rand() % 30) * 0.033333335f;
        RwV3d vWind = m_vWind;
        Math::Vec3_Scale(&vWind, &vWind, fRandScale);
        Math::Vec3_Add(&pClothPoint->m_vForces, &pClothPoint->m_vForces, &vWind);
    };

    for (int32 i = 0; i < nSpringMax; ++i)
    {
        RwV3d vDistance = Math::VECTOR3_ZERO;
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        RwV3d vNormalDistance = Math::VECTOR3_ZERO;
        RwV3d vForce0 = Math::VECTOR3_ZERO;
        RwV3d vForce1 = Math::VECTOR3_ZERO;

        SPRING* pSpring = &m_pSpringList[i];

        int32 nIndexPt0 = pSpring->m_nPointIndex1;
        int32 nIndexPt1 = pSpring->m_nPointIndex2;
        float fSpringLen = pSpring->m_fLength;
        float fSpringTension = pSpring->m_fSpringConstant;
        float fSpringDecay = pSpring->m_fDecayCoefficient;
        
        CLOTHPOINT* pClothPt0 = &m_pClothPointList[nIndexPt0];
        CLOTHPOINT* pClothPt1 = &m_pClothPointList[nIndexPt1];

        Math::Vec3_Sub(&vDistance, &pClothPt0->m_vPosition, &pClothPt1->m_vPosition);
        Math::Vec3_Sub(&vVelocity, &pClothPt0->m_vVelocity, &pClothPt1->m_vVelocity);
        Math::Vec3_Normalize(&vNormalDistance, &vDistance);

        float s = -((Math::Vec3_Length(&vDistance) - fSpringLen) * fSpringTension);
        float d = Math::Vec3_Dot(&vVelocity, &vDistance);
        float l = Math::Vec3_Length(&vDistance);

        float fScale = s - ((d / l) * fSpringDecay);

        Math::Vec3_Scale(&vForce0, &vNormalDistance, fScale);
        Math::Vec3_Scale(&vForce1, &vNormalDistance, fScale);
        Math::Vec3_Scale(&vForce1, &vForce1, -1.0f);

        if (!pClothPt0->m_bLock)
        {
            Math::Vec3_Add(
                &pClothPt0->m_vForces,
                &pClothPt0->m_vForces,
                &vForce0
            );
        };

        if (!pClothPt1->m_bLock)
        {
            Math::Vec3_Add(
                &pClothPt1->m_vForces,
                &pClothPt1->m_vForces,
                &vForce1
            );
        };
    };
};


void CCloth::StepSimulation(void)
{
    float fPercent = (m_fSpringLength * 0.01f) + 1.0f;
    const int32 nClothPtMax = (m_nRows * m_nColumns);
    const int32 nSpringMax = (m_nRows * m_nDivisionHeight) + (m_nColumns * m_nDivisionWidth);
    float fLen = 0.0f;
    
    for (int32 i = 0; i < nSpringMax; ++i)
    {
        RwV3d vDistance = Math::VECTOR3_ZERO;
        RwV3d vDistanceVector = Math::VECTOR3_ZERO;

        SPRING* pSpring = &m_pSpringList[i];

        int32 nIndexPt0 = pSpring->m_nPointIndex1;
        int32 nIndexPt1 = pSpring->m_nPointIndex2;
        
        CLOTHPOINT* pClothPt0 = &m_pClothPointList[nIndexPt0];
        CLOTHPOINT* pClothPt1 = &m_pClothPointList[nIndexPt1];

        Math::Vec3_Sub(&vDistance, &pClothPt0->m_vPosition, &pClothPt1->m_vPosition);

        const int32 nIndexMax = 9;
        int32 nIndex = 0;

        if (Math::Vec3_Length(&vDistance) > (fPercent * pSpring->m_fLength))
        {
            do
            {
                Math::Vec3_Sub(&vDistance, &pClothPt0->m_vPosition, &pClothPt1->m_vPosition);
                fLen = Math::Vec3_Length(&vDistance);
                Math::Vec3_Normalize(&vDistance, &vDistance);

                float fScale = fLen - pSpring->m_fLength;
                Math::Vec3_Scale(&vDistance, &vDistance, fScale);

                if (pClothPt1->m_bLock)
                    break;

                Math::Vec3_Scale(&vDistanceVector, &vDistance, (1.0f / 3.0f));
                Math::Vec3_Add(&pClothPt1->m_vPosition, &pClothPt1->m_vPosition, &vDistanceVector);

                if (nIndex > nIndexMax)
                    break;

                ++nIndex;
            } while (fLen > (fPercent * pSpring->m_fLength));
        };
    };

    CalcForces();

    for (int32 i = 1; i < m_nColumns - 1; ++i)
    {
        for (int32 j = 1; j < m_nRows - 1; ++j)
        {
            CheckCollision(
                &m_pClothPointList[j + i * m_nRows]
            );
        };
    };

    for (int32 i = 0; i < nClothPtMax; ++i)
    {
        Integration(&m_pClothPointList[i], m_fDt);
    };
};


void CCloth::Integration(CLOTHPOINT* pClothPoint, float dt)
{
    ASSERT(pClothPoint);
    
    RwV3d vAccel = pClothPoint->m_vAcceleration;
    RwV3d vVel = pClothPoint->m_vVelocity;
    RwV3d vPos = pClothPoint->m_vPosition;
    RwV3d vForces = pClothPoint->m_vForces;

    vAccel.x = vForces.x * pClothPoint->m_fInvMass;
    vAccel.y = vForces.y * pClothPoint->m_fInvMass;
    vAccel.z = vForces.z * pClothPoint->m_fInvMass;

    vVel.x += vAccel.x * dt;
    vVel.y += vAccel.y * dt;
    vVel.z += vAccel.z * dt;

    vPos.x += vVel.x * dt;
    vPos.y += vVel.y * dt;
    vPos.z += vVel.z * dt;

    pClothPoint->m_vAcceleration = vAccel;
    pClothPoint->m_vVelocity = vVel;
    pClothPoint->m_vPosition = vPos;
};


void CCloth::CheckCollision(CLOTHPOINT* pClothPoint)
{
    CBodyHitData BodyHitData;
    BodyHitData.InitData(&pClothPoint->m_vPosition, 0.01f);

    RwV3d vResult = Math::VECTOR3_ZERO;
	
    if (CBodyHitManager::CheckHit(&BodyHitData, &pClothPoint->m_vVelocity, &vResult))
    {
        pClothPoint->m_vVelocity = vResult;

        if (Math::Vec3_Length(&pClothPoint->m_vVelocity) > 8.0f)
        {
            Math::Vec3_Normalize(&pClothPoint->m_vVelocity, &pClothPoint->m_vVelocity);
            Math::Vec3_Scale(&pClothPoint->m_vVelocity, &pClothPoint->m_vVelocity, 8.0f);            
        };
    };
};


void CCloth::SetVertex(RwIm3DVertex* pVertex, RwV3d* pPos, float u, float v)
{
    ASSERT(pVertex);

    pVertex->objVertex.x = pPos->x;
    pVertex->objVertex.y = pPos->y;
    pVertex->objVertex.z = pPos->z;
    pVertex->objNormal.x = 0.0f;
    pVertex->objNormal.y = 0.0f;
    pVertex->objNormal.z = 0.0f;
    pVertex->color = RWRGBALONGEX(m_Color);
    pVertex->u = u;
    pVertex->v = v;
};


void CCloth::SetDeltaTime(float dt)
{
    m_fDt = dt;
};


void CCloth::RenderStatePush(void)
{
    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, nullptr);

    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
};


void CCloth::RenderStatePop(void)
{
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
};