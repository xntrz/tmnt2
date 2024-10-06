#include "DebugShape.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Sprite.hpp"


class CDebugShapeContainer
{
private:
    static const int32 SHAPEMAX     = 512;
    static const int32 TRINUM       = 256 * 256;
    static const int32 VERTEXNUM    = TRINUM * 4;
    static const int32 INDEXNUM     = TRINUM * 6;
    static const int32 SPHERE_DIV_X = 16;
	static const int32 SPHERE_DIV_Y = 16;

    struct SHAPE_SPHERE
    {
        RwV3d m_vPosition;
        float m_fRadius;
        RwRGBA m_Color;
    };

    struct SHAPE_LINE
    {
        RwV3d m_vStart;
        RwV3d m_vEnd;
        RwRGBA m_ColorStart;
        RwRGBA m_ColorEnd;
    };

    struct SHAPE_PLANE
    {
        RwV3d m_vPoint[4];
        RwRGBA m_Color;
    };

    struct SHAPE_BOX
    {
        RwV3d m_vPoint[8];
        RwRGBA m_Color;
    };

    struct LABEL
    {
        RwV3d m_vPosition;
        char m_szLabel[128];
        RwRGBA m_Color;
        float m_fHeight;
    };

    struct SHAPE : public CListNode<SHAPE>
    {
        enum TYPE
        {
            TYPE_SPHERE = 0,
            TYPE_LINE,
            TYPE_PLANE,
            TYPE_BOX,
            TYPE_LABEL,
        };

        TYPE m_type;
        float m_fTime;
        float m_fTimeEnd;
        union SHAPEDATA
        {
            SHAPE_SPHERE    sphere;
            SHAPE_LINE      line;
            SHAPE_PLANE     plane;
            SHAPE_BOX       box;
            LABEL           label;
        } m_data;        
    };

public:
    CDebugShapeContainer(void);
    ~CDebugShapeContainer(void);
    void Run(float dt);
    void FrameBegin(void);
    void FrameEnd(void);
    void ClearPerFrameData(void);
    void Draw3D(void);
    void Draw2D(void);
    void ShowSphere(const RwSphere* pSphere, const RwRGBA& rColor);
    void ShowLine(const RwLine* pLine, const RwRGBA& rColorStart, const RwRGBA& rColorEnd);
    void ShowPlane(const RwV3d aPoint[8], const RwRGBA& rColor);
    void ShowBox(const RwV3d aPoint[8], const RwRGBA& rColor);
    void ShowLabel(const RwV3d* pvPosition, const char* pszLabel, const RwRGBA& rColor, float fHeight);
    SHAPE* ShapeAlloc(SHAPE::TYPE type);
    void CleanupShapeList(CList<SHAPE>& list);
    void DrawSphere(SHAPE_SPHERE* pShapeSPhere);
    void DrawLine(SHAPE_LINE* pShapeLine);
    void DrawPlane(SHAPE_PLANE* pShapePlane);
    void DrawBox(SHAPE_BOX* pShapeBox);
    void DrawLabel(LABEL* pLabel);
    void Accumulate(RwIm3DVertex* pVertex, int32 nVertexNum, RwImVertexIndex* pIndex, int32 nIndexNum);
    void Flush(void);
    void RSPush3D(void);
    void RSPop3D(void);
    void RSPush2D(void);
    void RSPop2D(void);
    bool IsShape2D(SHAPE::TYPE type) const;
    bool IsAnyPointOnScreen(const RwV3d* aPt, int32 nNumPt, RwV3d* pvScreenPosRet = nullptr) const;

private:
    SHAPE m_aShape[SHAPEMAX];
    CList<SHAPE> m_listShapePool;
    CList<SHAPE> m_listShapeAlloc2D;
    CList<SHAPE> m_listShapeAlloc3D;
    RwIm3DVertex* m_pVertexBuffer;
    RwImVertexIndex* m_pIndexBuffer;
    int32 m_nVertexNum;
    int32 m_nVertexMax;
    int32 m_nIndexNum;
    int32 m_nIndexMax;
	int32 m_nIndexOffset;
	int32 m_nIndexAccum;
    int32 m_nShapeNum;
};


CDebugShapeContainer::CDebugShapeContainer(void)
: m_pVertexBuffer(nullptr)
, m_pIndexBuffer(nullptr)
, m_nVertexNum(0)
, m_nIndexNum(0)
, m_nIndexOffset(0)
, m_nIndexAccum(0)
, m_nShapeNum(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aShape); ++i)
        m_listShapePool.push_back(&m_aShape[i]);

    m_pVertexBuffer = new RwIm3DVertex[VERTEXNUM];
    m_pIndexBuffer = new RwImVertexIndex[INDEXNUM];
    ASSERT(m_pVertexBuffer);
    ASSERT(m_pIndexBuffer);

    m_nVertexMax = VERTEXNUM;
    m_nIndexMax = INDEXNUM;
};


CDebugShapeContainer::~CDebugShapeContainer(void)
{
    if (m_pVertexBuffer)
    {
        delete[] m_pVertexBuffer;
        m_pVertexBuffer = nullptr;
        m_nVertexNum = 0;
        m_nVertexMax = 0;
    };

    if (m_pIndexBuffer)
    {
        delete[] m_pIndexBuffer;
        m_pIndexBuffer = nullptr;
        m_nIndexNum = 0;
        m_nIndexMax = 0;
    };
};


void CDebugShapeContainer::Run(float dt)
{
    for (SHAPE& it : m_listShapeAlloc3D)
        it.m_fTime += dt;

	for (SHAPE& it : m_listShapeAlloc2D)
		it.m_fTime += dt;
};


void CDebugShapeContainer::FrameBegin(void)
{
	;
};


void CDebugShapeContainer::FrameEnd(void)
{
    ClearPerFrameData();
};


void CDebugShapeContainer::ClearPerFrameData(void)
{
    CleanupShapeList(m_listShapeAlloc2D);
    CleanupShapeList(m_listShapeAlloc3D);

    m_nIndexOffset = 0;
    m_nIndexAccum = 0;
    m_nIndexNum = 0;
    
    m_nVertexNum = 0;
};


void CDebugShapeContainer::Draw3D(void)
{
    for (SHAPE& it : m_listShapeAlloc3D)
    {
        switch (it.m_type)
        {
        case SHAPE::TYPE_SPHERE:
            DrawSphere(&it.m_data.sphere);
            break;

        case SHAPE::TYPE_LINE:
            break;

        case SHAPE::TYPE_PLANE:
            DrawPlane(&it.m_data.plane);
            break;

        case SHAPE::TYPE_BOX:
			DrawBox(&it.m_data.box);
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    RSPush3D();
    Flush();
    RSPop3D();
};


void CDebugShapeContainer::Draw2D(void)
{
    RSPush2D();
    for (SHAPE& it : m_listShapeAlloc2D)
    {
        switch (it.m_type)
        {
        case SHAPE::TYPE_LABEL:
            DrawLabel(&it.m_data.label);
            break;

        default:
            ASSERT(false);
            break;
        };
    };
    RSPop2D();
};


void CDebugShapeContainer::ShowSphere(const RwSphere* pSphere, const RwRGBA& rColor)
{
    ASSERT(pSphere);
    
    SHAPE* pShape = ShapeAlloc(SHAPE::TYPE_SPHERE);
    ASSERT(pShape);

    pShape->m_data.sphere.m_Color       = rColor;
    pShape->m_data.sphere.m_fRadius     = pSphere->radius;
    pShape->m_data.sphere.m_vPosition   = pSphere->center;
};


void CDebugShapeContainer::ShowLine(const RwLine* pLine, const RwRGBA& rColorStart, const RwRGBA& rColorEnd)
{
    ASSERT(pLine);
    
    SHAPE* pShape = ShapeAlloc(SHAPE::TYPE_LINE);
    ASSERT(pShape);

    pShape->m_data.line.m_vStart        = pLine->start;
    pShape->m_data.line.m_vEnd          = pLine->end;
    pShape->m_data.line.m_ColorStart    = rColorStart;
    pShape->m_data.line.m_ColorEnd      = rColorEnd;
};


void CDebugShapeContainer::ShowPlane(const RwV3d aPoint[4], const RwRGBA& rColor)
{
    ASSERT(aPoint);
    
    SHAPE* pShape = ShapeAlloc(SHAPE::TYPE_PLANE);
    ASSERT(pShape);
    
    pShape->m_data.plane.m_Color = rColor;
    pShape->m_data.plane.m_vPoint[0] = aPoint[0];
    pShape->m_data.plane.m_vPoint[1] = aPoint[1];
    pShape->m_data.plane.m_vPoint[2] = aPoint[2];
    pShape->m_data.plane.m_vPoint[3] = aPoint[3];
};


void CDebugShapeContainer::ShowBox(const RwV3d aPoint[8], const RwRGBA& rColor)
{
    ASSERT(aPoint);
    
    SHAPE* pShape = ShapeAlloc(SHAPE::TYPE_BOX);
	ASSERT(pShape);

	pShape->m_data.box.m_Color = rColor;
	pShape->m_data.box.m_vPoint[0] = aPoint[0];
	pShape->m_data.box.m_vPoint[1] = aPoint[1];
	pShape->m_data.box.m_vPoint[2] = aPoint[2];
	pShape->m_data.box.m_vPoint[3] = aPoint[3];
	pShape->m_data.box.m_vPoint[4] = aPoint[4];
	pShape->m_data.box.m_vPoint[5] = aPoint[5];
	pShape->m_data.box.m_vPoint[6] = aPoint[6];
	pShape->m_data.box.m_vPoint[7] = aPoint[7];
};


void CDebugShapeContainer::ShowLabel(const RwV3d* pvPosition, const char* pszLabel, const RwRGBA& rColor, float fHeight)
{
    ASSERT(pvPosition);
    ASSERT(std::strlen(pszLabel) < COUNT_OF(LABEL::m_szLabel));
    
    SHAPE* pShape = ShapeAlloc(SHAPE::TYPE_LABEL);
    ASSERT(pShape);

    std::strcpy(pShape->m_data.label.m_szLabel, pszLabel);
    pShape->m_data.label.m_Color        = rColor;
    pShape->m_data.label.m_fHeight      = fHeight;
    pShape->m_data.label.m_vPosition    = *pvPosition;
};


CDebugShapeContainer::SHAPE* CDebugShapeContainer::ShapeAlloc(SHAPE::TYPE type)
{
    ASSERT(m_nShapeNum < COUNT_OF(m_aShape));
    
    if (m_listShapePool.empty())
        return nullptr;

    SHAPE* pNode = m_listShapePool.front();
    m_listShapePool.erase(pNode);    
    if (IsShape2D(type))
        m_listShapeAlloc2D.push_back(pNode);
    else
        m_listShapeAlloc3D.push_back(pNode);

    pNode->m_type = type;
    pNode->m_fTime = 0.0f;
    pNode->m_fTimeEnd = CDebugShape::m_fDuration;

    ++m_nShapeNum;
    
    return pNode;
};


void CDebugShapeContainer::CleanupShapeList(CList<SHAPE>& list)
{
    auto it = list.begin();
    while (it)
    {
        SHAPE* pShape = &(*it);
        if (pShape->m_fTime >= pShape->m_fTimeEnd)
        {
            it = list.erase(it);
            m_listShapePool.push_back(pShape);

            ASSERT(m_nShapeNum > 0);
            --m_nShapeNum;
        }
        else
        {
            ++it;
        };
    };
};


void CDebugShapeContainer::DrawSphere(SHAPE_SPHERE* pShapeSphere)
{
    if (!IsAnyPointOnScreen(&pShapeSphere->m_vPosition, 1))
        return;

    RwIm3DVertex aVertex[(SPHERE_DIV_X + 1) * (SPHERE_DIV_Y + 1)] = { 0 };
    RwImVertexIndex aIndex[(SPHERE_DIV_X * SPHERE_DIV_Y) * 6] = { 0 };    
    int32 nVertexNum = 0;
    int32 nIndexNum = 0;
    
    float fRadius = pShapeSphere->m_fRadius;
    RwRGBA Color = pShapeSphere->m_Color;
    RwV3d vPosition = pShapeSphere->m_vPosition;

    for (int32 i = 0; i <= SPHERE_DIV_Y; ++i)
    {
        for (int32 j = 0; j <= SPHERE_DIV_X; ++j)
        {
            RwIm3DVertex* pVertex = &aVertex[nVertexNum++];
			ASSERT(nVertexNum <= COUNT_OF(aVertex));

            float fDeltaX = (float(j) / float(SPHERE_DIV_X));
            float fDeltaY = (float(i) / float(SPHERE_DIV_Y));

            pVertex->objVertex.x = fRadius * Math::Cos(fDeltaX * 2.0f * Math::PI) * Math::Sin(fDeltaY * Math::PI);
            pVertex->objVertex.y = fRadius * Math::Cos(fDeltaY * Math::PI);
            pVertex->objVertex.z = fRadius * Math::Sin(fDeltaX * 2.0f * Math::PI) * Math::Sin(fDeltaY * Math::PI);    

            Math::Vec3_Add(&pVertex->objVertex, &pVertex->objVertex, &vPosition);

            pVertex->color = RWRGBALONGEX(Color);
        };
    };

    for (int32 i = 0; i < SPHERE_DIV_Y; ++i)
    {
        for (int32 j = 0; j < SPHERE_DIV_X; ++j)
        {
			aIndex[nIndexNum + 0] = (RwImVertexIndex)(i * (SPHERE_DIV_X + 1) + j);
			aIndex[nIndexNum + 1] = (RwImVertexIndex)((i + 1) * (SPHERE_DIV_X + 1) + j);
			aIndex[nIndexNum + 2] = (RwImVertexIndex)((i + 1) * (SPHERE_DIV_X + 1) + j + 1);
			aIndex[nIndexNum + 3] = (RwImVertexIndex)(i * (SPHERE_DIV_X + 1) + j);
			aIndex[nIndexNum + 4] = (RwImVertexIndex)((i + 1) * (SPHERE_DIV_X + 1) + j + 1);
			aIndex[nIndexNum + 5] = (RwImVertexIndex)(i * (SPHERE_DIV_X + 1) + j + 1);

            nIndexNum += 6;
            ASSERT(nIndexNum <= COUNT_OF(aIndex));
        };
    };

    Accumulate(aVertex, nVertexNum, aIndex, nIndexNum);
};


void CDebugShapeContainer::DrawLine(SHAPE_LINE* pShapeLine)
{
    //
    //  TODO
    //
};


void CDebugShapeContainer::DrawPlane(SHAPE_PLANE* pShapePlane)
{
    if (!IsAnyPointOnScreen(pShapePlane->m_vPoint, COUNT_OF(pShapePlane->m_vPoint)))
        return;

    RwIm3DVertex aVertex[4] = { 0 };
    
    for (int32 i = 0; i < COUNT_OF(aVertex); i++)
    {
        aVertex[i].objVertex = pShapePlane->m_vPoint[i];
        aVertex[i].color = RWRGBALONGEX(pShapePlane->m_Color);
    };

    RwImVertexIndex aIndex[] =
    {
        0, 1, 2,
        3, 2, 1
    };
    
    Accumulate(aVertex, COUNT_OF(aVertex), aIndex, COUNT_OF(aIndex));
};


void CDebugShapeContainer::DrawBox(SHAPE_BOX* pShapeBox)
{
    if (!IsAnyPointOnScreen(pShapeBox->m_vPoint, COUNT_OF(pShapeBox->m_vPoint)))
        return;

    RwIm3DVertex aVertex[8] = { 0 };
    
    for (int32 i = 0; i < COUNT_OF(aVertex); i++)
    {
        aVertex[i].objVertex = pShapeBox->m_vPoint[i];
        aVertex[i].color = RWRGBALONGEX(pShapeBox->m_Color);
    };
    
    //             6           4
    //              x-----------x
    //             /|          /|
    //          2 / |       0 / |
    //           x--|--------x  |
    //           |  x--------|--x 
    //           | / 7       | / 5
    //           |/          |/
    //           x-----------x 
    //          3             1

    RwImVertexIndex aIndex[] =
    {
        // front
        0, 1, 2,
        3, 2, 1,

        // right
        0, 1, 5,
        4, 0, 5,

        // left
        2, 3, 7,
        6, 2, 7,

        // back
        6, 7, 5,
        4, 6, 5,

        // top
        6, 2, 0,
        4, 6, 0,

        // bottom
        7, 3, 1,
        5, 7, 1
    };

    Accumulate(aVertex, COUNT_OF(aVertex), aIndex, COUNT_OF(aIndex));
};


void CDebugShapeContainer::DrawLabel(LABEL* pLabel)
{
    RwV3d vScPos = Math::VECTOR3_ZERO;
    RwV3d vPt = pLabel->m_vPosition;
        
    if (IsAnyPointOnScreen(&vPt, 1, &vScPos))
    {
        RwV3d vEye = Math::VECTOR3_ZERO;
        CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
        pMapCamera->GetEye(&vEye);

        RwV3d vDistance = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDistance, &vPt, &vEye);
        vDistance.y = 0.0f;
        float fDistance = Math::Vec3_Length(&vDistance);
        
		float fHeight = ((pLabel->m_fHeight / fDistance) * pLabel->m_fHeight);
        if(fHeight <= 0.0f)
            return;

        RwRGBA TextColor = pLabel->m_Color;
        RwV2d vTextPosition = Math::VECTOR2_ZERO;

        vTextPosition.x = CSprite::m_fVirtualScreenX + (vScPos.x * CSprite::m_fVirtualScreenW);
        vTextPosition.y = CSprite::m_fVirtualScreenY + (vScPos.y * CSprite::m_fVirtualScreenH);

		CGameFont::SetHeight(fHeight);
        CGameFont::SetRGBA(TextColor.red, TextColor.green, TextColor.blue, TextColor.alpha);
        CGameFont::Show(pLabel->m_szLabel, vTextPosition.x, vTextPosition.y);
    };
};


void CDebugShapeContainer::Accumulate(RwIm3DVertex* pVertex, int32 nVertexNum, RwImVertexIndex* pIndex, int32 nIndexNum)
{
    ASSERT(pVertex);
    ASSERT(nVertexNum > 0);
    ASSERT(pIndex);
    ASSERT(nIndexNum > 0);
    ASSERT((m_nVertexNum + nVertexNum) <= m_nVertexMax);
    ASSERT((m_nIndexNum + nIndexNum) <= m_nIndexMax);

    std::memcpy(&m_pVertexBuffer[m_nVertexNum], pVertex, sizeof(*pVertex) * nVertexNum);
    std::memcpy(&m_pIndexBuffer[m_nIndexNum], pIndex, sizeof(*pIndex) * nIndexNum);

    int32 nIndexOffsetBuff = m_nIndexOffset;
	for (int32 i = m_nIndexNum; i < (m_nIndexNum + nIndexNum); ++i)
		m_pIndexBuffer[i] = (m_pIndexBuffer[i] + (RwImVertexIndex)nIndexOffsetBuff);

    m_nIndexOffset += nVertexNum;
    m_nIndexNum += nIndexNum;
    m_nVertexNum += nVertexNum;
};


void CDebugShapeContainer::Flush(void)
{
    if (!m_nVertexNum)
        return;

	if (!m_nIndexNum)
		return;
    
    const uint32 uFlags = rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA;
    
    if (RwIm3DTransform(m_pVertexBuffer, m_nVertexNum, nullptr, uFlags))
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, m_pIndexBuffer, m_nIndexNum);
        RwIm3DEnd();
    };
};


void CDebugShapeContainer::RSPush3D(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
	RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
	RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
};


void CDebugShapeContainer::RSPop3D(void)
{
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
	RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
};


void CDebugShapeContainer::RSPush2D(void)
{
    CSystem2D::PushRenderState();
};


void CDebugShapeContainer::RSPop2D(void)
{
    CSystem2D::PopRenderState();
};


bool CDebugShapeContainer::IsShape2D(SHAPE::TYPE type) const
{
    return (type == SHAPE::TYPE_LABEL);
};


bool CDebugShapeContainer::IsAnyPointOnScreen(const RwV3d* aPt, int32 nNumPt, RwV3d* pvScreenPosRet) const
{
    RwMatrix matrixView;
    RwMatrixSetIdentityMacro(&matrixView);
    CGameProperty::GetCameraViewMatrix(&matrixView);

    for (int32 i = 0; i < nNumPt; ++i)
    {
        RwV3d vScreenPos = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vScreenPos, &aPt[i], &matrixView);

        if (vScreenPos.z > 1.0f)
        {
            vScreenPos.x *= (1.0f / vScreenPos.z);
            vScreenPos.y *= (1.0f / vScreenPos.z);

            if (vScreenPos.z >= 1.0f &&
                vScreenPos.z <= 100.0f &&
                vScreenPos.x >= 0.0f &&
                vScreenPos.x <= 1.0f &&
                vScreenPos.y >= 0.0f &&
                vScreenPos.y <= 1.0f)
            {
                if (pvScreenPosRet)
                    *pvScreenPosRet = vScreenPos;
                
                return true;
            };
        };
    };

    return false;
};


static const RwRGBA DEFAULT_COLOR = { 0xFF, 0xFF, 0xFF, 0xFF };
static CDebugShapeContainer* s_pDebugShapeContainer = nullptr;


static inline CDebugShapeContainer& DebugShapeContainer(void)
{
    ASSERT(s_pDebugShapeContainer);
    return *s_pDebugShapeContainer;
};


/*static*/ float CDebugShape::m_fLabelHeight = 12.0f;
/*static*/ float CDebugShape::m_fDuration = 0.0f;


/*static*/ void CDebugShape::Initialize(void)
{
    if (!s_pDebugShapeContainer)
    {
        s_pDebugShapeContainer = new CDebugShapeContainer;
    };
};


/*static*/ void CDebugShape::Terminate(void)
{
    if (s_pDebugShapeContainer)
    {
        delete s_pDebugShapeContainer;
        s_pDebugShapeContainer = nullptr;
    };
};


/*static*/ void CDebugShape::Period(float dt)
{
    DebugShapeContainer().Run(dt);
};


/*static*/ void CDebugShape::FrameBegin(void)
{
    DebugShapeContainer().FrameBegin();
};


/*static*/ void CDebugShape::FrameEnd(void)
{
    DebugShapeContainer().FrameEnd();
};


/*static*/ void CDebugShape::Draw3D(void)
{
    DebugShapeContainer().Draw3D();
};


/*static*/ void CDebugShape::Draw2D(void)
{
    DebugShapeContainer().Draw2D();
};


/*static*/ void CDebugShape::ShowSphere(const RwSphere* pSphere)
{
    ShowSphere(pSphere, DEFAULT_COLOR);
};


/*static*/ void CDebugShape::ShowSphere(const RwSphere* pSphere, const RwRGBA& rColor)
{
    DebugShapeContainer().ShowSphere(pSphere, rColor);
};


/*static*/ void CDebugShape::ShowLine(const RwLine* pLine)
{
    ShowLine(pLine, DEFAULT_COLOR, DEFAULT_COLOR);
};


/*static*/ void CDebugShape::ShowLine(const RwLine* pLine, const RwRGBA& rColorStart, const RwRGBA& rColorEnd)
{
    DebugShapeContainer().ShowLine(pLine, rColorStart, rColorEnd);
};


/*static*/ void CDebugShape::ShowPlane(const RwV3d aPoint[4])
{
    ShowPlane(aPoint, DEFAULT_COLOR);
};


/*static*/ void CDebugShape::ShowPlane(const RwV3d aPoint[4], const RwRGBA& rColor)
{
    DebugShapeContainer().ShowPlane(aPoint, rColor);
};


/*static*/ void CDebugShape::ShowBox(const RwV3d aPoint[8])
{
	ShowBox(aPoint, DEFAULT_COLOR);
};


/*static*/ void CDebugShape::ShowBox(const RwV3d aPoint[8], const RwRGBA& rColor)
{
	DebugShapeContainer().ShowBox(aPoint, rColor);
};


/*static*/ void CDebugShape::ShowLabel(const RwV3d* pvPosition, const char* pszLabel)
{
    ShowLabel(pvPosition, pszLabel, DEFAULT_COLOR);
};


/*static*/ void CDebugShape::ShowLabel(const RwV3d* pvPosition, const char* pszLabel, const RwRGBA& rColor)
{
    DebugShapeContainer().ShowLabel(pvPosition, pszLabel, rColor, m_fLabelHeight);
};