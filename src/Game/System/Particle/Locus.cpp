#include "Locus.hpp"

#include "System/Common/RenderState.hpp"


static RwIm3DVertex s_aVertex[CLocus::POINTSET_MAX * 4] = { 0 };


CLocus::CLocus(int32 nNumPoint, const RwRGBA& color)
: m_pvPositionHigh(nullptr)
, m_pvPositionLow(nullptr)
, m_nNumPoint(0)
, m_nNumPointMax(nNumPoint)
, m_nRepetitionNum(1)
, m_Color(color)
, m_pTexture(nullptr)
, m_uAlphaBasis(0xFF)
, m_bEnableDraw(false)
, m_bCompletion(true)
{
    ASSERT(m_nNumPointMax < POINTSET_MAX);

	m_pvPositionHigh = new RwV3d[m_nNumPointMax]();
	m_pvPositionLow = new RwV3d[m_nNumPointMax]();

    ASSERT(m_pvPositionHigh);
    ASSERT(m_pvPositionLow);
};


CLocus::~CLocus(void)
{
    if (m_pvPositionHigh)
    {
        delete[] m_pvPositionHigh;
        m_pvPositionHigh = nullptr;
    };
    
    if (m_pvPositionLow)
    {
        delete[] m_pvPositionLow;
        m_pvPositionLow = nullptr;
    };
};


void CLocus::Draw(void)
{
    if (!m_bEnableDraw)
        return;
    
    if (m_nNumPoint < 2)
        return;

    int32 nNumVertices = 0;
    RwV3d aCompletionListHigh[POINTSET_MAX * 2] = { 0 };
    RwV3d aCompletionListLow[POINTSET_MAX * 2] = { 0 };

    if (!m_bCompletion)
    {
        uint8 nowAlpha = m_uAlphaBasis;
        float tu = 0.0f;
        int32 numPt = m_nNumPoint;
        float tuDelta = float((1.0f / float(numPt - 1)) * m_nRepetitionNum);
        uint8 alphaDelta = uint8((1.0f / float(numPt)) * m_uAlphaBasis);

        for (int32 i = 0; i < m_nNumPoint; ++i)
        {
            SetVertex(
                &s_aVertex[nNumVertices],
                &m_pvPositionHigh[i],
                m_Color,
                nowAlpha,
                tu,
                0.0f
            );

            SetVertex(
                &s_aVertex[nNumVertices + 1],
                &m_pvPositionLow[i],
                m_Color,
                nowAlpha,
                tu,
                1.0f
            );

            nowAlpha -= alphaDelta;
            tu += tuDelta;
            nNumVertices += 2;
        };
    }
    else
    {
        GetCompletionListHigh(aCompletionListHigh);
        GetCompletionListLow(aCompletionListLow);

        int32 nowAlpha = m_uAlphaBasis;
        float tu = 0.0f;        
        int32 numPt = m_nNumPoint * 2;
        float tuDelta = float((1.0f / float(numPt - 3)) * m_nRepetitionNum);
        int32 alphaDelta = int32((1.0f / float(numPt - 2)) * m_uAlphaBasis);

        for (int32 i = 0; i < m_nNumPoint; ++i)
        {
            SetVertex(
                &s_aVertex[nNumVertices],
                &m_pvPositionHigh[i],
                m_Color,
                uint8(nowAlpha),
                tu,
                0.0f
            );

            SetVertex(
                &s_aVertex[nNumVertices + 1],
                &m_pvPositionLow[i],
                m_Color,
                uint8(nowAlpha),
                tu,
                1.0f
            );

            nowAlpha -= alphaDelta;
            if (nowAlpha < 0)
                nowAlpha = 0;
            
            tu += tuDelta;
            nNumVertices += 2;

            if (i < (m_nNumPoint - 1))
            {
                SetVertex(
                    &s_aVertex[nNumVertices],
                    &aCompletionListHigh[i],
                    m_Color,
                    uint8(nowAlpha),
                    tu,
                    0.0f
                );

                SetVertex(
                    &s_aVertex[nNumVertices + 1],
                    &aCompletionListLow[i],
                    m_Color,
                    uint8(nowAlpha),
                    tu,
                    1.0f
                );

                nowAlpha -= alphaDelta;
                if (nowAlpha < 0)
                    nowAlpha = 0;
                
                tu += tuDelta;
                nNumVertices += 2;
            };
        };
    };

    ASSERT(nNumVertices <= COUNT_OF(s_aVertex));

    {
        RenderStateSet();

        uint32 uFlags = rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA;
		if (m_pTexture)
			FLAG_SET(uFlags, rwIM3D_VERTEXUV);

        if (RwIm3DTransform(s_aVertex, nNumVertices, nullptr, uFlags))
        {
            RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
            RwIm3DEnd();
        };

        RenderStateReset();
    }
};


void CLocus::Reset(void)
{
    m_nNumPoint = 0;
};


void CLocus::SetPosition(const RwV3d* pvPositionHigh, const RwV3d* pvPositionLow)
{
    if (m_nNumPoint >= m_nNumPointMax)
    {
        for (int32 i = m_nNumPointMax - 1; i > 0; --i)
        {
            m_pvPositionHigh[i] = m_pvPositionHigh[i - 1];
            m_pvPositionLow[i] = m_pvPositionLow[i - 1];
        };

        m_pvPositionHigh[0] = *pvPositionHigh;
        m_pvPositionLow[0] = *pvPositionLow;
    }
    else
    {
        for (int32 i = m_nNumPoint; i > 0; --i)
        {
            m_pvPositionHigh[i] = m_pvPositionHigh[i - 1];
            m_pvPositionLow[i] = m_pvPositionLow[i - 1];
        };

        m_pvPositionHigh[0] = *pvPositionHigh;
        m_pvPositionLow[0] = *pvPositionLow;

		++m_nNumPoint;
    };
};


void CLocus::DeletePair(void)
{
    if (m_nNumPoint > 0)
        --m_nNumPoint;
};


void CLocus::SetColor(const RwRGBA& color)
{
    m_Color = color;
};


void CLocus::SetAlphaBasis(uint8 uAlphaBasis)
{
    m_uAlphaBasis = uAlphaBasis;
};


void CLocus::SetTexture(RwTexture* pTexture)
{
    ASSERT(pTexture);
    
    m_pTexture = pTexture;
};


void CLocus::SetDrawEnable(bool bEnable)
{
    m_bEnableDraw = bEnable;
};


void CLocus::RenderStateSet(void)
{
    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, nullptr);

    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
};


void CLocus::RenderStateReset(void)
{
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
};


void CLocus::GetCompletionListHigh(RwV3d* aCompletionList)
{
    ASSERT(aCompletionList);

    if (m_nNumPoint < 2)
        return;

    for (int32 i = 0; i < m_nNumPoint - 1; ++i)
    {
        if ((i > 0) && (i == (m_nNumPoint - 2)))
        {
            Vec3CatmullRom(
                &aCompletionList[i],
                &m_pvPositionHigh[i - 1],
                &m_pvPositionHigh[i],
                &m_pvPositionHigh[i + 1],
                &m_pvPositionHigh[i + 1],
                0.5f
            );
        }
        else if (i > 0)
        {
           Vec3CatmullRom(
                &aCompletionList[i],
                &m_pvPositionHigh[i - 1],
                &m_pvPositionHigh[i],
                &m_pvPositionHigh[i + 1],
                &m_pvPositionHigh[i + 2],
                0.5f
            );
        }
        else
        {
            Vec3CatmullRom(
                aCompletionList,
                &m_pvPositionHigh[0],
                &m_pvPositionHigh[0],
                &m_pvPositionHigh[1],
                &m_pvPositionHigh[1],
                0.5f
            );
        };
    };
};


void CLocus::GetCompletionListLow(RwV3d* aCompletionList)
{
    ASSERT(aCompletionList);

    if (m_nNumPoint < 2)
        return;

    for (int32 i = 0; i < m_nNumPoint - 1; ++i)
    {
        if ((i > 0) && (i == (m_nNumPoint - 2)))
        {
            Vec3CatmullRom(
                &aCompletionList[i],
                &m_pvPositionLow[i - 1],
                &m_pvPositionLow[i],
                &m_pvPositionLow[i + 1],
                &m_pvPositionLow[i + 1],
                0.5f
            );
        }
        else if (i > 0)
        {
           Vec3CatmullRom(
                &aCompletionList[i],
                &m_pvPositionLow[i - 1],
                &m_pvPositionLow[i],
                &m_pvPositionLow[i + 1],
                &m_pvPositionLow[i + 2],
                0.5f
            );
        }
        else
        {
            Vec3CatmullRom(
                aCompletionList,
                &m_pvPositionLow[0],
                &m_pvPositionLow[0],
                &m_pvPositionLow[1],
                &m_pvPositionLow[1],
                0.5f
            );
        };
    };
};


void CLocus::SetVertex(RwIm3DVertex* pVertex, RwV3d* pVector, RwRGBA& color, uint8 alpha, float u, float v)
{
    color.alpha = alpha;
    
    pVertex->color = RWRGBALONGEX(color);
    pVertex->objVertex.x = pVector->x;
    pVertex->objVertex.y = pVector->y;
    pVertex->objVertex.z = pVector->z;
    pVertex->u = u;
    pVertex->v = v;
};


void CLocus::Vec3CatmullRom(RwV3d* pOut, const RwV3d* pV1, const RwV3d* pV2, const RwV3d* pV3, const RwV3d* pV4, float fS)
{
    RwV3d vTS = Math::VECTOR3_ZERO;
    RwV3d vTE = Math::VECTOR3_ZERO;

    vTS.x = (pV3->x - pV1->x) * 0.5f;
    vTS.y = (pV3->y - pV1->y) * 0.5f;
    vTS.z = (pV3->z - pV1->z) * 0.5f;

    vTE.x = (pV4->x - pV2->x) * 0.5f;
    vTE.y = (pV4->y - pV2->y) * 0.5f;
    vTE.z = (pV4->z - pV2->z) * 0.5f;

    Vec3Hermite(pOut, pV2, &vTS, pV3, &vTE, fS);
};


void CLocus::Vec3Hermite(RwV3d* pOut, const RwV3d* pV1, const RwV3d* pT1, const RwV3d* pV2, const RwV3d* pT2, float fS)
{
    float afParam[4] = { 0.0f };

    CalcSpline3DParam(afParam, fS);

    pOut->x = afParam[3] * pT2->x + (afParam[2] * pT1->x + (afParam[1] * pV2->x + afParam[0] * pV1->x));
    pOut->y = afParam[3] * pT2->y + (afParam[2] * pT1->y + (afParam[1] * pV2->y + afParam[0] * pV1->y));
    pOut->z = afParam[3] * pT2->z + (afParam[2] * pT1->z + (afParam[1] * pV2->z + afParam[0] * pV1->z));
};


void CLocus::CalcSpline3DParam(float* pfParamArray, float fS)
{
    pfParamArray[0] = ((fS - 1.0f) * (fS - 1.0f)) * ((2.0f * fS) + 1.0f);
    pfParamArray[1] = (fS * fS) * ((-2.0f * fS) + 3.0f);
    pfParamArray[2] = ((fS - 1.0f) * (fS - 1.0f)) * fS;
    pfParamArray[3] = (fS * fS) * (fS - 1.0f);
};