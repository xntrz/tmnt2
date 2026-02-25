#include "Accumulate.hpp"

#include "Game/System/Model/Model.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Camera.hpp"


CAccumulate::CAccumulate(CModel* pModel, int32 nBoneID, const RwV3d* pvTop, const RwV3d* pvBottom)
: m_pModel(pModel)
, m_nBoneID(nBoneID)
, m_pTexture(nullptr)
, m_vTop(*pvTop)
, m_vBottom(*pvBottom)
, m_vSize({ 0.15f, 0.15f })
, m_Color({ 0xFF, 0xFF, 0xFF, 0xFF })
, m_pVertex(nullptr)
, m_nVertexNum(0)
, m_nParticleMax(0)
{
    ASSERT(m_pModel);

    CTextureManager::SetCurrentTextureSet("common_ef");
    m_pTexture = CTextureManager::GetRwTexture("maru02");
    ASSERT(m_pTexture);

    RwV3d vBuff = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vBuff, &m_vTop, &m_vBottom);
    
    m_nParticleMax = int32(Math::Vec3_Length(&vBuff) / 0.05f);
    ASSERT(m_nParticleMax > 0);
    
    m_pVertex = new RwIm3DVertex[m_nParticleMax * 6];
    ASSERT(m_pVertex);
};


CAccumulate::~CAccumulate(void)
{
    if (m_pVertex)
    {
        delete[] m_pVertex;
        m_pVertex = nullptr;
        
        m_nParticleMax = 0;
    };
};


void CAccumulate::Draw(void)
{
    if (!m_pModel || !m_Color.alpha)
        return;

    m_nVertexNum = 0;
    SetVertexAll();

    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    if (m_pTexture)
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(m_pTexture));
    else
        RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    uint32 uFlags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV;

    if (RwIm3DTransform(m_pVertex, m_nVertexNum, nullptr, uFlags))
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


void CAccumulate::SetModel(CModel* pModel)
{
    m_pModel = pModel;
};


void CAccumulate::SetBoneID(int32 nBoneID)
{
    m_nBoneID = nBoneID;
};


void CAccumulate::SetLine(const RwV3d* pvTop, const RwV3d* pvBottom)
{
    ASSERT(pvTop);
    ASSERT(pvBottom);
    
    m_vTop = *pvTop;
    m_vBottom = *pvBottom;
};


void CAccumulate::SetTextureSize(float w, float h)
{
    m_vSize = { w, h };
};


void CAccumulate::SetColor(const RwRGBA& color)
{
    m_Color = color;
};


void CAccumulate::SetVertexAll(void)
{
    RwCamera* pCamera = CCamera::CameraCurrent();
    ASSERT(pCamera);
    
    RwMatrix* pMatrix = m_pModel->GetBoneMatrixFromID(m_nBoneID);
    ASSERT(pMatrix);

    RwV3d aPosition[2] = { Math::VECTOR3_ZERO };
    aPosition[0] = m_vTop;
    aPosition[1] = m_vBottom;
    RwV3dTransformPoints(aPosition, aPosition, COUNT_OF(aPosition), pMatrix);

    RwV3d vUnit = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vUnit, &aPosition[0], &aPosition[1]);
    float fLenght = Math::Vec3_Length(&vUnit);
    Math::Vec3_Scale(&vUnit, &vUnit, (0.05f / fLenght));

    RwMatrix matBillboard;
    RwMatrixSetIdentity(&matBillboard);
    Math::Matrix_Invert(&matBillboard, RwCameraGetViewMatrix(pCamera));

    RwV3d vMoveFront = Math::VECTOR3_ZERO;    
    RwV3d vCameraPosition = RwFrameGetMatrix(RwCameraGetFrame(pCamera))->pos;
    Math::Vec3_Sub(&vMoveFront, &aPosition[1], &vCameraPosition);
    Math::Vec3_Normalize(&vMoveFront, &vMoveFront);
    Math::Vec3_Scale(&vMoveFront, &vMoveFront, -0.05f);

    RwV3d aSummit[4] = { Math::VECTOR3_ZERO };
    float fRasterW = float(pCamera->frameBuffer->width);
    float fRasterH = float(pCamera->frameBuffer->height);
    float w = m_vSize.x;
    float h = m_vSize.y * (fRasterW / fRasterH);
    float x = w * 0.5f;
    float y = h * 0.5f;
    
    aSummit[0].x = x * -1.0f;
    aSummit[0].y = y * -1.0f;
    aSummit[0].z = 0.0f;

    aSummit[1].x = x * -1.0f;
    aSummit[1].y = y;
    aSummit[1].z = 0.0f;

    aSummit[2].x = x;
    aSummit[2].y = y * -1.0f;
    aSummit[2].z = 0.0f;
    
    aSummit[3].x = x;
    aSummit[3].y = y;
    aSummit[3].z = 0.0f;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vPosition, &aPosition[1], &vMoveFront);
    for (int32 i = 0; i < m_nParticleMax; ++i)
    {
        Math::Vec3_Add(&vPosition, &vPosition, &vUnit);
        SetVertex(&vPosition, aSummit, &matBillboard);
    };
};


void CAccumulate::SetVertex(RwV3d* pvPosition, RwV3d* aSummit, RwMatrix* pMatrix)
{
    ASSERT(pvPosition);
    ASSERT(aSummit);
    ASSERT(pMatrix);
    
    RwV3d aPosition[4] = { Math::VECTOR3_ZERO };

    RwV3dTransformVectors(aPosition, aSummit, COUNT_OF(aPosition), pMatrix);
    
    Math::Vec3_Add(&aPosition[0], &aPosition[0], pvPosition);
    Math::Vec3_Add(&aPosition[1], &aPosition[1], pvPosition);
    Math::Vec3_Add(&aPosition[2], &aPosition[2], pvPosition);
    Math::Vec3_Add(&aPosition[3], &aPosition[3], pvPosition);

    ASSERT(m_nVertexNum < (m_nParticleMax * 6));

    RwIm3DVertex* pVertex = &m_pVertex[m_nVertexNum];

    RwIm3DVertexSetPos(&pVertex[0], aPosition[1].x, aPosition[1].y, aPosition[1].z);
    RwIm3DVertexSetNormal(&pVertex[0], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[0], m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);
    RwIm3DVertexSetU(&pVertex[0], 0.0f);
    RwIm3DVertexSetV(&pVertex[0], 1.0f);

    RwIm3DVertexSetPos(&pVertex[1], aPosition[0].x, aPosition[0].y, aPosition[0].z);
    RwIm3DVertexSetNormal(&pVertex[1], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[1], m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);
    RwIm3DVertexSetU(&pVertex[1], 0.0f);
    RwIm3DVertexSetV(&pVertex[1], 0.0f);

    RwIm3DVertexSetPos(&pVertex[2], aPosition[3].x, aPosition[3].y, aPosition[3].z);
    RwIm3DVertexSetNormal(&pVertex[2], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[2], m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);
    RwIm3DVertexSetU(&pVertex[2], 1.0f);
    RwIm3DVertexSetV(&pVertex[2], 1.0f);

    RwIm3DVertexSetPos(&pVertex[3], aPosition[3].x, aPosition[3].y, aPosition[3].z);
    RwIm3DVertexSetNormal(&pVertex[3], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[3], m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);
    RwIm3DVertexSetU(&pVertex[3], 1.0f);
    RwIm3DVertexSetV(&pVertex[3], 1.0f);

    RwIm3DVertexSetPos(&pVertex[4], aPosition[0].x, aPosition[0].y, aPosition[0].z);
    RwIm3DVertexSetNormal(&pVertex[4], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[4], m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);
    RwIm3DVertexSetU(&pVertex[4], 0.0f);
    RwIm3DVertexSetV(&pVertex[4], 0.0f);

    RwIm3DVertexSetPos(&pVertex[5], aPosition[2].x, aPosition[2].y, aPosition[2].z);
    RwIm3DVertexSetNormal(&pVertex[5], 0.0f, 0.0f, 0.0f);
    RwIm3DVertexSetRGBA(&pVertex[5], m_Color.red, m_Color.green, m_Color.blue, m_Color.alpha);
    RwIm3DVertexSetU(&pVertex[5], 1.0f);
    RwIm3DVertexSetV(&pVertex[5], 0.0f);

    m_nVertexNum += 6;
};