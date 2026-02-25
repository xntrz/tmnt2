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


CSphere::~CSphere(void)
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

    uint32 flags = rwIM3D_VERTEXRGBA
                 | rwIM3D_VERTEXXYZ
                 | rwIM3D_VERTEXUV;

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
            float _u0 = u0 + (j * (u1 / static_cast<float>(m_nRepetationU)));
            float _v0 = v0 + (i * (v1 / static_cast<float>(m_nRepetationV)));

            float _u1 = u0 + (j * (u1 / static_cast<float>(m_nRepetationU)));
            float _v1 = v0 + ((i + 1) * (v1 / static_cast<float>(m_nRepetationV)));

            RwIm3DVertexSetU(&m_pVertex[m_nNumVertex + 0], _u0);
            RwIm3DVertexSetV(&m_pVertex[m_nNumVertex + 0], _v0);

            RwIm3DVertexSetU(&m_pVertex[m_nNumVertex + 1], _u1);
            RwIm3DVertexSetV(&m_pVertex[m_nNumVertex + 1], _v1);

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
    RwMatrixSetIdentity(&m_matrix);

    if (m_bRandom)
        SetVertexRandom();
    else
        SetVertex();
};


void CSphere::ResetUV(void)
{
    m_u0 = 0.0f;
    m_v0 = 0.0f;
    m_u1 = 1.0f;
    m_v1 = 1.0f;
};


void CSphere::SetVertex(void)
{
    float fDeltaHorizonAngle = MATH_PI2 / static_cast<float>(m_nDivisionHor);
    float fDeltaVerticalAngle = MATH_PI / static_cast<float>(m_nDivisionVer);
    
    for (int32 i = 0; i < m_nDivisionVer; ++i)
    {
        for (int32 j = 0; j < m_nDivisionHor + 1; ++j)
        {
            RwIm3DVertex* pVertex = &m_pVertex[m_nNumVertex];

            RwRGBA color = m_Color;

            float tu = (1.0f / static_cast<float>(m_nRepetationU));
            float tv = (1.0f / static_cast<float>(m_nRepetationV));

            float x0 =
                std::sin(static_cast<float>(j) * fDeltaHorizonAngle) *
                std::sin(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

            float y0 =
                std::cos(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

            float z0 =
                std::cos(static_cast<float>(j) * fDeltaHorizonAngle) *
                std::sin(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

            RwIm3DVertexSetPos(&pVertex[0], x0, y0, z0);
            RwIm3DVertexSetNormal(&pVertex[0], 0.0f, 0.0f, 0.0f);
            RwIm3DVertexSetRGBA(&pVertex[0], color.red, color.green, color.blue, color.alpha);
            RwIm3DVertexSetU(&pVertex[0], tu * static_cast<float>(j));
            RwIm3DVertexSetV(&pVertex[0], tv * static_cast<float>(i));

            float x1 =
                std::sin(static_cast<float>(j) * fDeltaHorizonAngle) *
                std::sin(static_cast<float>(i + 1) * fDeltaVerticalAngle) * m_fRadius;

            float y1 =
                std::cos(static_cast<float>(i + 1) * fDeltaVerticalAngle) * m_fRadius;

            float z1 =
                std::cos(static_cast<float>(j) * fDeltaHorizonAngle) *
                std::sin(static_cast<float>(i + 1) * fDeltaVerticalAngle) * m_fRadius;

            RwV3d vPos = { x1, y1, z1 };
            RwV3d vNormal = Math::VECTOR3_ZERO;
            Math::Vec3_Normalize(&vNormal, &vPos);

            RwIm3DVertexSetPos(&pVertex[1], x1, y1, z1);
            RwIm3DVertexSetNormal(&pVertex[1], vNormal.x, vNormal.y, vNormal.z);
            RwIm3DVertexSetRGBA(&pVertex[1], color.red, color.green, color.blue, color.alpha);
            RwIm3DVertexSetU(&pVertex[1], tu * static_cast<float>(j));
            RwIm3DVertexSetV(&pVertex[1], tv * static_cast<float>(i + 1));

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
                float x =
                    std::sin(static_cast<float>(j) * fDeltaHorizonAngle) *
                    std::sin(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

                float y =
                    std::cos(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

                float z =
                    std::cos(static_cast<float>(j) * fDeltaHorizonAngle) *
                    std::sin(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

                RwIm3DVertexSetPos(&pVertexList[index], x, y, z);
            }
			else
            {            
                float xx = ((static_cast<float>(Math::Rand() % 20) * 0.01f) - 0.1f) * m_fRadius;
				float yy = ((static_cast<float>(Math::Rand() % 20) * 0.01f) - 0.1f) * m_fRadius;
				float zz = ((static_cast<float>(Math::Rand() % 20) * 0.01f) - 0.1f) * m_fRadius;

				float x =
                    std::sin(static_cast<float>(j) * fDeltaHorizonAngle) *
                    std::sin(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

				float y =
                    std::cos(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

				float z =
                    std::cos(static_cast<float>(j) * fDeltaHorizonAngle) *
                    std::sin(static_cast<float>(i) * fDeltaVerticalAngle) * m_fRadius;

                x += xx;
                y += yy;
                z += zz;

                RwIm3DVertexSetPos(&pVertexList[index], x, y, z);
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

            RwRGBA color = m_Color;

            RwIm3DVertex* pVertex = &m_pVertex[m_nNumVertex];
            RwIm3DVertex* pVertexFromList0 = &pVertexList[j + i * (m_nDivisionHor + 1)];
            RwIm3DVertex* pVertexFromList1 = &pVertexList[j + ((i + 1) * (m_nDivisionHor + 1))];

            RwV3d vPos0 = *RwIm3DVertexGetPos(pVertexFromList0);
            RwV3d vPos1 = *RwIm3DVertexGetPos(pVertexFromList1);

            RwV3d vNormal0 = Math::VECTOR3_ZERO;
			Math::Vec3_Normalize(&vNormal0, &vPos0);

            RwV3d vNormal1 = Math::VECTOR3_ZERO;
            Math::Vec3_Normalize(&vNormal1, &vPos1);

            RwIm3DVertexSetPos(&pVertex[0], vPos0.x, vPos0.y, vPos0.z);
            RwIm3DVertexSetNormal(&pVertex[0], vNormal0.x, vNormal0.y, vNormal0.z);
            RwIm3DVertexSetRGBA(&pVertex[0], color.red, color.green, color.blue, color.alpha);
            RwIm3DVertexSetU(&pVertex[0], tu * j);
            RwIm3DVertexSetV(&pVertex[0], tv * i);

            RwIm3DVertexSetPos(&pVertex[1], vPos1.x, vPos1.y, vPos1.z);
            RwIm3DVertexSetNormal(&pVertex[1], vNormal1.x, vNormal1.y, vNormal1.z);
            RwIm3DVertexSetRGBA(&pVertex[1], color.red, color.green, color.blue, color.alpha);
            RwIm3DVertexSetU(&pVertex[1], tu * j);
            RwIm3DVertexSetV(&pVertex[1], tv * (i + 1));

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