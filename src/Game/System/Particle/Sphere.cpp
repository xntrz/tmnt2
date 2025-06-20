#include "Sphere.hpp"

#include "System/Common/RenderState.hpp"


CSphere::CSphere(float fRadius,
                 int32 nDivisionVer,
                 int32 nDivisionHor,
                 int32 nRepetationU,
                 int32 nRepetationV,
                 const RwRGBA& color,
                 bool bRandom /*= false*/)
: m_vPosition(Math::VECTOR3_ZERO)
, m_fRadius(fRadius)
, m_nDivisionVer(nDivisionVer)
, m_nDivisionHor(nDivisionHor)
, m_Color(color)
, m_pTexture(nullptr)
, m_nRepetationU(nRepetationU)
, m_nRepetationV(nRepetationV)
, m_u0(0.0f)
, m_v0(0.0f)
, m_u1(1.0f)
, m_v1(1.0f)
, m_bRandom(bRandom)
, m_bUVAnim(true)
, m_pVertex(nullptr)
, m_nNumVertex(0)
, m_nNumBodyVertex(0)
, m_nNumTopvertex(0)
, m_nNumUnderVertex(0)
{
    m_nNumBodyVertex = 2 * m_nDivisionVer * (m_nDivisionHor + 1);
	m_pVertex = new RwIm3DVertex[m_nNumBodyVertex];

    Reset();
};


/*virtual*/ CSphere::~CSphere(void)
{
    if (m_pVertex)
    {
        delete[] m_pVertex;
        m_pVertex = nullptr;
    };
};


void CSphere::Run(float dt)
{
    if (m_bUVAnim)
    {
        m_u0 += dt;
        m_v0 += dt;
        
        m_u1 += dt;
        m_v1 += dt;

        SetUV(m_u0, m_v0, m_u1, m_v1);
    };
};


void CSphere::Draw(void)
{
    RenderStatePush();

    uint32 flags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ | (m_pTexture ? rwIM3D_VERTEXUV : 0);

    m_matrix.pos = m_vPosition;
    
    for (int32 i = 0; i < m_nDivisionVer; ++i)
    {
		int32 nIndex = 2 * (m_nDivisionHor + 1);
        
        if (RwIm3DTransform(&m_pVertex[i * nIndex], nIndex, &m_matrix, flags))
        {
            RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
            RwIm3DEnd();
        };
    };

    RenderStatePop();
};


void CSphere::SetPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);

    m_vPosition = *pvPosition;
};


void CSphere::SetTexture(RwTexture* pTexture)
{
    ASSERT(pTexture);
    
    m_pTexture = pTexture;
};


void CSphere::SetUV(float u0, float v0, float u1, float v1)
{
    m_nNumVertex = 0;

    for (int32 i = 0; i < m_nDivisionVer; ++i)
    {
        for (int32 j = 0; j < m_nDivisionHor + 1; ++j)
        {
            m_pVertex[m_nNumVertex].u = u0 + (j * (u1 / static_cast<float>(m_nRepetationU)));
            m_pVertex[m_nNumVertex].v = v0 + (i * (v1 / static_cast<float>(m_nRepetationV)));

            m_pVertex[m_nNumVertex + 1].u = u0 + (j * (u1 / static_cast<float>(m_nRepetationU)));
            m_pVertex[m_nNumVertex + 1].v = v0 + ((i + 1) * (v1 / static_cast<float>(m_nRepetationV)));

            m_nNumVertex += 2;
        };
    };
};


void CSphere::SetScale(const RwV3d* pvScaling)
{
    ASSERT(pvScaling);
    
    Math::Matrix_Scale(&m_matrix, pvScaling->x, pvScaling->y, pvScaling->z);
};


void CSphere::Reset(void)
{
    RwMatrixSetIdentityMacro(&m_matrix);

    if (m_bRandom)
        SetVertexRandom();
    else
        SetVertex();
};


void CSphere::ResetUV(void)
{
    m_u0 = 0.0f;
    m_v0 = 0.0f;

    m_u1 = 0.0f;
    m_v1 = 0.0f;
};


void CSphere::SetVertex(void)
{
    float fDeltaHorizonAngle = MATH_PI2 / static_cast<float>(m_nDivisionHor);
    float fDeltaVerticalAngle = MATH_PI / static_cast<float>(m_nDivisionVer);
    
    for (int32 i = 0; i < m_nDivisionVer; ++i)
    {
        for (int32 j = 0; j < m_nDivisionHor + 1; ++j)
        {
            float tu = (1.0f / static_cast<float>(m_nRepetationU));
            float tv = (1.0f / static_cast<float>(m_nRepetationV));

            RwIm3DVertex* pVertex = &m_pVertex[m_nNumVertex];

            /* vert 0 */
            pVertex[0].objVertex.x =
                std::sin(static_cast<float>(j) * fDeltaHorizonAngle) *
                std::sin(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

            pVertex[0].objVertex.y =
                std::cos(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;
            
            pVertex[0].objVertex.z =
                std::cos(static_cast<float>(j) * fDeltaHorizonAngle) *
                std::sin(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

            pVertex[0].objNormal = Math::VECTOR3_ZERO;            

            pVertex[0].color = RWRGBALONG(m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);

            pVertex[0].u = tu * j;
            pVertex[0].u = tv * i;

            /* vert 1 */
            pVertex[1].objVertex.x =
                std::sin(static_cast<float>(j) * fDeltaHorizonAngle) *
                std::sin(static_cast<float>(i + 1) * fDeltaVerticalAngle) * m_fRadius;

            pVertex[1].objVertex.y =
                std::cos(static_cast<float>(i + 1) * fDeltaVerticalAngle) * m_fRadius;

            pVertex[1].objVertex.z =
                std::cos(static_cast<float>(j) * fDeltaHorizonAngle) *
                std::sin(static_cast<float>(i + 1) * fDeltaVerticalAngle) * m_fRadius;

            pVertex[1].objNormal = Math::VECTOR3_ZERO;

            pVertex[1].color = RWRGBALONG(m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);

            pVertex[1].u = tu * j;
            pVertex[1].u = tv * (i + 1);

            m_nNumVertex += 2;
        };
    };
};


void CSphere::SetVertexRandom(void)
{
    m_nNumVertex = 0;

    float fDeltaHorizonAngle = MATH_PI2 / static_cast<float>(m_nDivisionHor);
    float fDeltaVerticalAngle = MATH_PI / static_cast<float>(m_nDivisionVer);

    RwIm3DVertex* pVertexList = new RwIm3DVertex[(m_nDivisionVer + 1) * (m_nDivisionHor + 1)];

    for (int32 i = 0; i < m_nDivisionVer + 1; ++i)
    {
        for (int32 j = 0; j < m_nDivisionHor + 1; ++j)
        {
            int32 index = j + i * (m_nDivisionHor + 1);

			if ((i == 0) || (i == m_nDivisionVer))
			{
                pVertexList[index].objVertex.x =
                    m_fRadius * (std::sin(static_cast<float>(j) * fDeltaHorizonAngle) * std::sin(static_cast<float>(i) * fDeltaVerticalAngle));

				pVertexList[index].objVertex.y =            
                    m_fRadius * (std::cos(static_cast<float>(i) * fDeltaVerticalAngle));

				pVertexList[index].objVertex.z =                
                    m_fRadius * (std::cos(static_cast<float>(j) * fDeltaHorizonAngle) * std::sin(static_cast<float>(i) * fDeltaVerticalAngle));
            }
			else
			{
				float x = ((static_cast<float>(Math::Rand() % 20) * 0.01f) - 0.1f) * m_fRadius;
				float y = ((static_cast<float>(Math::Rand() % 20) * 0.01f) - 0.1f) * m_fRadius;
				float z = ((static_cast<float>(Math::Rand() % 20) * 0.01f) - 0.1f) * m_fRadius;

				pVertexList[index].objVertex.x =                
                    x + (m_fRadius * (std::sin(static_cast<float>(j) * fDeltaHorizonAngle) * std::sin(static_cast<float>(i) * fDeltaVerticalAngle)));

				pVertexList[index].objVertex.y =            
                    y + (m_fRadius * (std::cos(static_cast<float>(i) * fDeltaVerticalAngle)));

				pVertexList[index].objVertex.z =                
                    z + (m_fRadius * (std::cos(static_cast<float>(j) * fDeltaHorizonAngle) * std::sin(static_cast<float>(i) * fDeltaVerticalAngle)));
            };
        };

        int32 indexTo = ((m_nDivisionHor + 1) * i) + m_nDivisionHor;
        int32 indexFrom = (m_nDivisionHor + 1) * i;

        pVertexList[indexTo] = pVertexList[indexFrom];
    };

    for (int32 i = 0; i < m_nDivisionVer; ++i)
    {
		for (int32 j = 0; j < m_nDivisionHor + 1; ++j)
		{
			float tu = (1.0f / static_cast<float>(m_nRepetationU));
			float tv = (1.0f / static_cast<float>(m_nRepetationV));

			RwIm3DVertex* pVertex = &m_pVertex[m_nNumVertex];
			RwIm3DVertex* pVertexFromList = &pVertexList[j + i * (m_nDivisionHor + 1)];
			RwV3d vPosition = pVertexFromList[0].objVertex;
			RwV3d vNormal = vPosition;
			Math::Vec3_Normalize(&vNormal, &vNormal);

			pVertex[0].objVertex = vPosition;
			pVertex[0].objNormal = vNormal;
            pVertex[0].color = RWRGBALONG(m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);
			pVertex[0].u = tu * j;
			pVertex[0].v = tv * i;

			pVertexFromList = &pVertexList[j + ((i + 1) * (m_nDivisionHor + 1))];
			vPosition = pVertexFromList[0].objVertex;
			vNormal = vPosition;
			Math::Vec3_Normalize(&vNormal, &vNormal);

			pVertex[1].objVertex = vPosition;
			pVertex[1].objNormal = vNormal;
            pVertex[1].color = RWRGBALONG(m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);
			pVertex[1].u = tu * j;
			pVertex[1].v = tv * (i + 1);

			m_nNumVertex += 2;
		};
    };

    if (pVertexList)
    {
        delete[] pVertexList;
        pVertexList = nullptr;
    };
};


void CSphere::RenderStatePush(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);    
};


void CSphere::RenderStatePop(void)
{
    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);    
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};