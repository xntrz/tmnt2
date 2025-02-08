#include "MapCollisionModel.hpp"
#include "MapAttribute.hpp"

#include "Game/System/Hit/Intersection.hpp"


class CMapCollisionModelContainer
{
public:
    struct MODELINFO
    {
        RpClump* m_pClump;
        RwV3d m_vVelocity;
        RwV3d m_vRotation;
        float m_fRadius;
        char m_szGimmickObjName[16];
        MAPTYPES::GIMMICKTYPE m_type;
    };

    struct COLLISIONWORK
    {
        RpIntersection* m_pRpIntersection;
        RwV3d* m_pVelocity;
        RwV3d* m_pRotate;
        RwV3d* m_pPrimitiveVelocity;
        char m_szGimmickObjName[16];
        MAPTYPES::GIMMICKTYPE m_type;
    };

public:
    static RpAtomic* CheckModelCollisionCallback(RpAtomic* atomic, void* pData);
    static void GetMaterialTextureName(char* pszName, int32 nMaterialID, RpGeometry* pGeometry);
    static void GetMaterialColor(RwRGBA& rColor, int32 nMaterialID, RpGeometry* pGeometry);
    static MAPTYPES::ATTRIBUTE GetMapAttribute(RpCollisionTriangle* pCollTriangle, RpGeometry* pGeometry, RwRGBA& rColor);
    static RpCollisionTriangle* CollisionPointCallback(RpIntersection* pIntersection, RpCollisionTriangle* pCollTriangle, float fDistance, void* pData);
    static RpCollisionTriangle* CollisionLineCallback(RpIntersection* pIntersection, RpCollisionTriangle* pCollTriangle, float fDistance, void* pData);
    static RpCollisionTriangle* CollisionSphereCallback(RpIntersection* pIntersection, RpCollisionTriangle* pCollTriangle, float fDistance, void* pData);
    static RpCollisionTriangle* CollisionBBoxCallback(RpIntersection* pIntersection, RpCollisionTriangle* pCollTriangle, float fDistance, void* pData);
    static bool CheckModelBoundingSphere(RwV3d* pvPos, float fRadius, RpIntersection* pIntersection);
    static bool TestSphereAndVerticalLine(RwV3d* pvPos, float fRadius, RpIntersection* pIntersection);
    static bool TestSphereAndLine(RwV3d* pvPos, float fRadius, RpIntersection* pIntersection);
    static bool TestSphereAndSphere(RwV3d* pvPos, float fRadius, RpIntersection* pIntersection);
    
    CMapCollisionModelContainer(void);
    ~CMapCollisionModelContainer(void);
    void Period(void);
    uint32 RegistCollisionModel(RpClump* pClump, const char* pszName, MAPTYPES::GIMMICKTYPE type = MAPTYPES::GIMMICKTYPE_NORMAL);
    void RemoveCollisionModel(uint32 hAtari);
    void UpdateCollisionModel(uint32 hAtari, RwV3d* pVelocity, RwV3d* pRotation = nullptr);
    void SetBoundingSphereRadius(uint32 hAtari, float fRadius);
    const CMapCollisionModel::COLLISIONPARAM_MODEL* GetModelCollisionResult(void);    
    bool CheckModelCollision(RpIntersection* pIntersection, RwV3d* pVelocity);

private:
    static CMapCollisionModel::COLLISIONPARAM_MODEL m_CollisionResult;
    MODELINFO m_aModelInfo[64];
};


/*static*/ CMapCollisionModel::COLLISIONPARAM_MODEL CMapCollisionModelContainer::m_CollisionResult;


/*static*/ RpAtomic* CMapCollisionModelContainer::CheckModelCollisionCallback(RpAtomic* atomic, void* pData)
{
    ASSERT(pData);

    if (!atomic)
        return nullptr;

    COLLISIONWORK* pWork = static_cast<COLLISIONWORK*>(pData);

    RpIntersection rpIntersect = { 0 };    
    std::memcpy(&rpIntersect, pWork->m_pRpIntersection, sizeof(rpIntersect));

    CMapCollisionModel::COLLISIONPARAM_MODEL param;    
    std::memset(&param, 0x00, sizeof(param));

    param.m_attribute           = MAPTYPES::ATTRIBUTE_UNKNOWN;
    param.m_fDistance           = TYPEDEF::FLOAT_MAX;
    param.m_pPrimitiveVelocity  = pWork->m_pPrimitiveVelocity;
    param.m_pGeometry           = RpAtomicGetGeometryMacro(atomic);
    param.m_mapobj.m_pLTM       = RwFrameGetLTM(RpAtomicGetFrameMacro(atomic));
    param.m_mapobj.m_vVelocity  = *pWork->m_pVelocity;
    param.m_mapobj.m_vRotate    = *pWork->m_pRotate;
    param.m_mapobj.m_type       = pWork->m_type;
    std::strcpy(param.m_mapobj.m_szGimmickObjName, pWork->m_szGimmickObjName);

    RwMatrix matInvLTM;
    RwMatrixSetIdentityMacro(&matInvLTM);
    Math::Matrix_Invert(&matInvLTM, param.m_mapobj.m_pLTM);

    RwV3d vInvVel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vInvVel, &param.m_mapobj.m_vVelocity, -1.0f);

    RpIntersectionCallBackGeometryTriangle pfnCallback = nullptr;
    
    switch (rpIntersect.type)
    {
    case rpINTERSECTLINE:
        {
            pfnCallback = CollisionLineCallback;
            Math::Vec3_Add(&rpIntersect.t.line.start, &rpIntersect.t.line.start, &vInvVel);
            Math::Vec3_Add(&rpIntersect.t.line.end, &rpIntersect.t.line.end, &vInvVel);
            RwV3dTransformPoint(&rpIntersect.t.line.start, &rpIntersect.t.line.start, &matInvLTM);
            RwV3dTransformPoint(&rpIntersect.t.line.end, &rpIntersect.t.line.end, &matInvLTM);
        }
        break;

    case rpINTERSECTPOINT:
        {
            pfnCallback = CollisionPointCallback;
            Math::Vec3_Add(&rpIntersect.t.point, &rpIntersect.t.point, &vInvVel);
            RwV3dTransformPoint(&rpIntersect.t.point, &rpIntersect.t.point, &matInvLTM);
        }
        break;

    case rpINTERSECTSPHERE:
        {
            pfnCallback = CollisionSphereCallback;
            Math::Vec3_Add(&rpIntersect.t.sphere.center, &rpIntersect.t.sphere.center, &vInvVel);
            RwV3dTransformPoint(&rpIntersect.t.sphere.center, &rpIntersect.t.sphere.center, &matInvLTM);
        }
        break;
        
    case rpINTERSECTBOX:
        {
            pfnCallback = CollisionBBoxCallback;
            Math::Vec3_Add(&rpIntersect.t.box.inf, &rpIntersect.t.box.inf, &vInvVel);
            Math::Vec3_Add(&rpIntersect.t.box.sup, &rpIntersect.t.box.sup, &vInvVel);
            RwV3dTransformPoint(&rpIntersect.t.box.inf, &rpIntersect.t.box.inf, &matInvLTM);
            RwV3dTransformPoint(&rpIntersect.t.box.sup, &rpIntersect.t.box.sup, &matInvLTM);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    ASSERT(pfnCallback);
    RpCollisionGeometryForAllIntersections(param.m_pGeometry, &rpIntersect, pfnCallback, &param);

    if (param.m_fDistance < m_CollisionResult.m_fDistance)
    {
        std::memcpy(&m_CollisionResult, &param, sizeof(m_CollisionResult));
        m_CollisionResult.m_bUpdate = true;
    };

    return atomic;
};


/*static*/ void CMapCollisionModelContainer::GetMaterialTextureName(char* pszName, int32 nMaterialID, RpGeometry* pGeometry)
{
    ASSERT(pszName);
    ASSERT(nMaterialID >= 0);
    ASSERT(nMaterialID < RpGeometryGetNumMaterialsMacro(pGeometry));

    RpMaterial* pMaterial = RpGeometryGetMaterialMacro(pGeometry, nMaterialID);
    if (pMaterial)
    {
        RwTexture* pMaterialTex = RpMaterialGetTextureMacro(pMaterial);
        if (pMaterialTex)
        {
            const char* pszTextureName = RwTextureGetNameMacro(pMaterialTex);

            ASSERT(pszTextureName);
            std::strcpy(pszName, pszTextureName);
        };
    };
};


/*static*/ void CMapCollisionModelContainer::GetMaterialColor(RwRGBA& rColor, int32 nMaterialID, RpGeometry* pGeometry)
{
    ASSERT(nMaterialID >= 0);
    ASSERT(nMaterialID < RpGeometryGetNumMaterialsMacro(pGeometry));

    rColor = RpGeometryGetMaterialMacro(pGeometry, nMaterialID)->color;
};


/*static*/ MAPTYPES::ATTRIBUTE
CMapCollisionModelContainer::GetMapAttribute(RpCollisionTriangle* pCollTriangle, RpGeometry* pGeometry, RwRGBA& rColor)
{
    ASSERT(pCollTriangle->index >= 0);
    ASSERT(pCollTriangle->index < pGeometry->numTriangles);

    char szAttributeName[32];
    szAttributeName[0] = '\0';

    int32 nMaterialID = pGeometry->triangles[pCollTriangle->index].matIndex;

    GetMaterialColor(rColor, nMaterialID, pGeometry);
    GetMaterialTextureName(szAttributeName, nMaterialID, pGeometry);
    
    return CMapAttribute::CheckAttribute(szAttributeName);
};


/*static*/ RpCollisionTriangle*
CMapCollisionModelContainer::CollisionPointCallback(RpIntersection* pIntersection,
                                                    RpCollisionTriangle* pCollTriangle,
                                                    float fDistance,
                                                    void* pData)
{
    CMapCollisionModel::COLLISIONPARAM_MODEL* pCollisionParam = static_cast<CMapCollisionModel::COLLISIONPARAM_MODEL*>(pData);

    if (fDistance >= pCollisionParam->m_fDistance)
        return pCollTriangle;

    pCollisionParam->m_attribute = GetMapAttribute(pCollTriangle,
                                                   pCollisionParam->m_pGeometry,
                                                   pCollisionParam->m_Color);

    pCollisionParam->m_fDistance = fDistance;
    pCollisionParam->m_vNormal   = pCollTriangle->normal;

    float fDist2Plane =
        Math::Vec3_Dot(pCollTriangle->vertices[0], &pCollisionParam->m_vNormal) -
        Math::Vec3_Dot(&pIntersection->t.point, &pCollisionParam->m_vNormal);

    pCollisionParam->m_vClosestPt.x = pCollisionParam->m_vNormal.x * fDist2Plane + pIntersection->t.point.x;
    pCollisionParam->m_vClosestPt.y = pCollisionParam->m_vNormal.y * fDist2Plane + pIntersection->t.point.y;
    pCollisionParam->m_vClosestPt.z = pCollisionParam->m_vNormal.z * fDist2Plane + pIntersection->t.point.z;

    return pCollTriangle;
};


/*static*/ RpCollisionTriangle*
CMapCollisionModelContainer::CollisionLineCallback(RpIntersection* pIntersection,
                                                   RpCollisionTriangle* pCollTriangle,
                                                   float fDistance,
                                                   void* pData)
{
    CMapCollisionModel::COLLISIONPARAM_MODEL* pCollisionParam = static_cast<CMapCollisionModel::COLLISIONPARAM_MODEL*>(pData);

    if (fDistance >= pCollisionParam->m_fDistance)
        return pCollTriangle;

    pCollisionParam->m_attribute = GetMapAttribute(pCollTriangle,
                                                   pCollisionParam->m_pGeometry,
                                                   pCollisionParam->m_Color);

    pCollisionParam->m_fDistance = fDistance;
    pCollisionParam->m_vNormal = pCollTriangle->normal;

    RwV3d vTmp = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vTmp, &pIntersection->t.line.end, &pIntersection->t.line.start);

    pCollisionParam->m_vClosestPt.x = (vTmp.x * fDistance) + pIntersection->t.line.start.x;
    pCollisionParam->m_vClosestPt.y = (vTmp.y * fDistance) + pIntersection->t.line.start.y;
    pCollisionParam->m_vClosestPt.z = (vTmp.z * fDistance) + pIntersection->t.line.start.z;

    return pCollTriangle;
};


/*static*/ RpCollisionTriangle*
CMapCollisionModelContainer::CollisionSphereCallback(RpIntersection* pIntersection,
                                                     RpCollisionTriangle* pCollTriangle,
                                                     float fDistance,
                                                     void* pData)
{
    CMapCollisionModel::COLLISIONPARAM_MODEL* pCollisionParam = static_cast<CMapCollisionModel::COLLISIONPARAM_MODEL*>(pData);

    float fDist2Plane =
        Math::Vec3_Dot(pCollTriangle->vertices[0], &pCollTriangle->normal) -
        Math::Vec3_Dot(&pIntersection->t.sphere.center, &pCollTriangle->normal);

    RwV3d vProjPos = Math::VECTOR3_ZERO;
    vProjPos.x = (pCollTriangle->normal.x * fDist2Plane) + pIntersection->t.sphere.center.x;
    vProjPos.y = (pCollTriangle->normal.y * fDist2Plane) + pIntersection->t.sphere.center.y;
    vProjPos.z = (pCollTriangle->normal.z * fDist2Plane) + pIntersection->t.sphere.center.z;

    if (Intersection::PointWithinTriangle(&vProjPos, pCollTriangle->vertices, &pCollTriangle->normal))
    {
        float fDist = std::fabs(fDist2Plane);
        
        if (fDist < pCollisionParam->m_fDistance)
        {
            pCollisionParam->m_vClosestPt.x = vProjPos.x;
            pCollisionParam->m_vClosestPt.y = vProjPos.y;
            pCollisionParam->m_vClosestPt.z = vProjPos.z;
            pCollisionParam->m_fDistance    = fDist;
            pCollisionParam->m_vNormal      = pCollTriangle->normal;
            pCollisionParam->m_attribute    = GetMapAttribute(pCollTriangle,
                                                              pCollisionParam->m_pGeometry,
                                                              pCollisionParam->m_Color);
        };
    }
    else
    {
        for (int32 i = 0; i < 3; ++i)
        {
            RwV3d vClosestPt = Math::VECTOR3_ZERO;
            Intersection::FindNearestPointOnLine(&vClosestPt,
                                                 &vProjPos,
                                                 pCollTriangle->vertices[i],
                                                 pCollTriangle->vertices[(i + 1) % 3]);

            RwV3d vTemp = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vTemp, &pIntersection->t.sphere.center, &vClosestPt);

            float fDist = Math::Vec3_Length(&vTemp);

            if (fDist <= (pCollisionParam->m_fDistance + 0.001f))
            {
                bool bCorner = false;

                if (((pCollisionParam->m_fDistance - 0.001f) <= fDist) &&
                    ((pCollisionParam->m_fDistance + 0.001f) >= fDist))
                {
                    bCorner = true;
                };

                pCollisionParam->m_vClosestPt.x = vClosestPt.x;
                pCollisionParam->m_vClosestPt.y = vClosestPt.y;
                pCollisionParam->m_vClosestPt.z = vClosestPt.z;
                pCollisionParam->m_fDistance    = fDist;
                pCollisionParam->m_attribute    = GetMapAttribute(pCollTriangle,
                                                                  pCollisionParam->m_pGeometry,
                                                                  pCollisionParam->m_Color);

                if (bCorner)
                    Math::Vec3_Normalize(&pCollisionParam->m_vNormal, &vTemp);
                else
                    pCollisionParam->m_vNormal = pCollTriangle->normal;
            };
        };
    };

    return pCollTriangle;
};


/*static*/ RpCollisionTriangle*
CMapCollisionModelContainer::CollisionBBoxCallback(RpIntersection* pIntersection,
                                                   RpCollisionTriangle* pCollTriangle,
                                                   float fDistance,
                                                   void* pData)
{
    CMapCollisionModel::COLLISIONPARAM_MODEL* pCollisionParam = static_cast<CMapCollisionModel::COLLISIONPARAM_MODEL*>(pData);

    if (fDistance >= pCollisionParam->m_fDistance)
        return pCollTriangle;

    pCollisionParam->m_attribute = GetMapAttribute(pCollTriangle,
                                                   pCollisionParam->m_pGeometry,
                                                   pCollisionParam->m_Color);

    pCollisionParam->m_fDistance = fDistance;
    pCollisionParam->m_vNormal   = pCollTriangle->normal;

    RwV3d** ppVertices = pCollTriangle->vertices;

    pCollisionParam->m_vClosestPt.x =
        (ppVertices[2]->x + ppVertices[1]->x + ppVertices[0]->x) * (1.0f / 3.0f);

    pCollisionParam->m_vClosestPt.y = pIntersection->t.box.inf.y;

    pCollisionParam->m_vClosestPt.z =
        (ppVertices[2]->z + ppVertices[1]->z + ppVertices[0]->z) * (1.0f / 3.0f);

    return pCollTriangle;
};


/*static*/ bool CMapCollisionModelContainer::CheckModelBoundingSphere(RwV3d* pvPos,
                                                                      float fRadius,
                                                                      RpIntersection* pIntersection)
{
    ASSERT(pvPos);
    ASSERT(pIntersection);
    
    bool bResult = false;

	switch (pIntersection->type)
	{
	case rpINTERSECTLINE:
	{
		RwLine* pLine = &pIntersection->t.line;

		if (Intersection::IsLineVertical(pLine))
			bResult = TestSphereAndVerticalLine(pvPos, fRadius, pIntersection);
		else
			bResult = TestSphereAndLine(pvPos, fRadius, pIntersection);
	}
		break;

	case rpINTERSECTSPHERE:
		bResult = TestSphereAndSphere(pvPos, fRadius, pIntersection);
		break;

	default:
		break;
	};

    return bResult;
};


/*static*/ bool CMapCollisionModelContainer::TestSphereAndVerticalLine(RwV3d* pvPos, float fRadius, RpIntersection* pIntersection)
{
    RwSphere sphere = { 0 };
    sphere.center = *pvPos;
    sphere.radius = fRadius;

    return Intersection::LineVerticalAndSphere(&pIntersection->t.line, &sphere);
};


/*static*/ bool CMapCollisionModelContainer::TestSphereAndLine(RwV3d* pvPos, float fRadius, RpIntersection* pIntersection)
{
    RwSphere sphere = { 0 };
    sphere.center = *pvPos;
    sphere.radius = fRadius;
    
    return Intersection::LineAndSphere(&pIntersection->t.line, &sphere);
};


/*static*/ bool CMapCollisionModelContainer::TestSphereAndSphere(RwV3d* pvPos, float fRadius, RpIntersection* pIntersection)
{
	RwV3d vDist = Math::VECTOR3_ZERO;
	Math::Vec3_Sub(&vDist, &pIntersection->t.sphere.center, pvPos);
	Math::Vec3_Abs(&vDist, &vDist);

	float fDistSQ = Math::Vec3_Dot(&vDist, &vDist);
	float fRadiusSQ = (fRadius + pIntersection->t.sphere.radius);

	if (fDistSQ <= (fRadiusSQ *fRadiusSQ))
		return true;

	return false;
};


CMapCollisionModelContainer::CMapCollisionModelContainer(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aModelInfo); ++i)
        std::memset(&m_aModelInfo[i], 0x00, sizeof(m_aModelInfo[i]));

    std::memset(&m_CollisionResult, 0x00, sizeof(m_CollisionResult));
};


CMapCollisionModelContainer::~CMapCollisionModelContainer(void)
{
    ;
};


void CMapCollisionModelContainer::Period(void)
{
    ;
};


uint32 CMapCollisionModelContainer::RegistCollisionModel(RpClump* pClump,
                                                         const char* pszName,
                                                         MAPTYPES::GIMMICKTYPE type /*= MAPTYPES::GIMMICKTYPE_NORMAL*/)
{
    ASSERT(pClump);
    ASSERT(pszName);
    
    MODELINFO* pModelInfo = nullptr;

    for (int32 i = 0; i < COUNT_OF(m_aModelInfo); ++i)
    {
        if (!m_aModelInfo[i].m_pClump)
        {
            pModelInfo = &m_aModelInfo[i];
            break;
        };
    };

    ASSERT(pModelInfo);

    if (pModelInfo)
    {
        std::memset(pModelInfo, 0x00, sizeof(*pModelInfo));

        pModelInfo->m_pClump  = pClump;
        pModelInfo->m_type    = type;
        pModelInfo->m_fRadius = 10.0f;

        ASSERT(std::strlen(pszName) < COUNT_OF(MODELINFO::m_szGimmickObjName));
        std::strcpy(pModelInfo->m_szGimmickObjName, pszName);
    };

    return reinterpret_cast<uint32>(pModelInfo);
};


void CMapCollisionModelContainer::RemoveCollisionModel(uint32 hAtari)
{
    MODELINFO* pModelInfo = reinterpret_cast<MODELINFO*>(hAtari);
    ASSERT(pModelInfo);

    for (int32 i = 0; i < COUNT_OF(m_aModelInfo); ++i)
    {
        if (&m_aModelInfo[i] == pModelInfo)
        {
            pModelInfo->m_pClump = nullptr;
            std::memset(pModelInfo, 0x00, sizeof(*pModelInfo));
            
            return;
        };
    };

    ASSERT(false);
};


void CMapCollisionModelContainer::UpdateCollisionModel(uint32 hAtari, RwV3d* pVelocity, RwV3d* pRotation /*= nullptr*/)
{
    MODELINFO* pModelInfo = reinterpret_cast<MODELINFO*>(hAtari);
    ASSERT(pModelInfo);

    if (pModelInfo && pModelInfo->m_pClump)
    {
        if (pVelocity)
            pModelInfo->m_vVelocity = *pVelocity;
        else
            pModelInfo->m_vVelocity = Math::VECTOR3_ZERO;

        if (pRotation)
            pModelInfo->m_vRotation = *pRotation;
        else
            pModelInfo->m_vRotation = Math::VECTOR3_ZERO;
    };
};


void CMapCollisionModelContainer::SetBoundingSphereRadius(uint32 hAtari, float fRadius)
{
    MODELINFO* pModelInfo = reinterpret_cast<MODELINFO*>(hAtari);
    ASSERT(pModelInfo);

    if (pModelInfo)
        pModelInfo->m_fRadius = fRadius;
};


const CMapCollisionModel::COLLISIONPARAM_MODEL* CMapCollisionModelContainer::GetModelCollisionResult(void)
{
    return &m_CollisionResult;
};


bool CMapCollisionModelContainer::CheckModelCollision(RpIntersection* pIntersection, RwV3d* pVelocity)
{
    bool bResult = false;

    m_CollisionResult.m_fDistance = TYPEDEF::FLOAT_MAX;

    for (int32 i = 0; i < COUNT_OF(m_aModelInfo); ++i)
    {
        MODELINFO* pModelInfo = &m_aModelInfo[i];
        if (!pModelInfo->m_pClump)
            continue;

        RpClump*  pClump  = pModelInfo->m_pClump;
        RwFrame*  pFrame  = RpClumpGetFrameMacro(pClump);
        RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

        if (CheckModelBoundingSphere(&pMatrix->pos, pModelInfo->m_fRadius, pIntersection))
        {
            COLLISIONWORK work = { 0 };

            work.m_type             = pModelInfo->m_type;
            work.m_pRotate          = &pModelInfo->m_vRotation;
            work.m_pVelocity        = &pModelInfo->m_vVelocity;
            work.m_pRpIntersection  = pIntersection;
            std::strcpy(work.m_szGimmickObjName, pModelInfo->m_szGimmickObjName);

            if (pVelocity)
                work.m_pPrimitiveVelocity = pVelocity;

            m_CollisionResult.m_bUpdate = false;
            RpClumpForAllAtomics(pClump, CheckModelCollisionCallback, &work);

            if (m_CollisionResult.m_bUpdate)
            {
                ASSERT(m_CollisionResult.m_mapobj.m_pLTM);

                RwMatrix matLTM;
                RwMatrixSetIdentityMacro(&matLTM);
                std::memcpy(&matLTM, m_CollisionResult.m_mapobj.m_pLTM, sizeof(matLTM));

                RwV3dTransformPoint(&m_CollisionResult.m_vClosestPt, &m_CollisionResult.m_vClosestPt, &matLTM);
                matLTM.pos = Math::VECTOR3_ZERO;
                RwV3dTransformPoint(&m_CollisionResult.m_vNormal, &m_CollisionResult.m_vNormal, &matLTM);

                m_CollisionResult.m_mapobj.m_pLTM = RwFrameGetLTM(RpClumpGetFrameMacro(pClump));

                bResult = true;
            };
        };
    };

    return bResult;
};


static CMapCollisionModelContainer* s_pMapCollisionModelContainer = nullptr;


static inline CMapCollisionModelContainer& MapCollisionModelContainer(void)
{
    ASSERT(s_pMapCollisionModelContainer);
    return *s_pMapCollisionModelContainer;
};


/*static*/ void CMapCollisionModel::Initialize(void)
{
    if (!s_pMapCollisionModelContainer)
        s_pMapCollisionModelContainer = new CMapCollisionModelContainer;
};


/*static*/ void CMapCollisionModel::Terminate(void)
{
    if (s_pMapCollisionModelContainer)
    {
        delete s_pMapCollisionModelContainer;
        s_pMapCollisionModelContainer = nullptr;
    };
};


/*static*/ void CMapCollisionModel::Period(void)
{
    MapCollisionModelContainer().Period();
};


/*static*/ bool CMapCollisionModel::CheckModelCollision(RpIntersection* pIntersection, RwV3d* pVelocity)
{
    return MapCollisionModelContainer().CheckModelCollision(pIntersection, pVelocity);
};


/*static*/ uint32 CMapCollisionModel::RegistCollisionModel(RpClump* pClump, const char* pszName, MAPTYPES::GIMMICKTYPE type)
{
    return MapCollisionModelContainer().RegistCollisionModel(pClump, pszName, type);
};


/*static*/ uint32 CMapCollisionModel::RegistCollisionModel(RpClump* pClump)
{
    return MapCollisionModelContainer().RegistCollisionModel(pClump, "", MAPTYPES::GIMMICKTYPE_NORMAL);
};


/*static*/ void CMapCollisionModel::RemoveCollisionModel(uint32 hAtari)
{
    MapCollisionModelContainer().RemoveCollisionModel(hAtari);
};


/*static*/ void CMapCollisionModel::UpdateCollisionModel(uint32 hAtari, RwV3d* pVelocity, RwV3d* pRotation /*= nullptr*/)
{
    MapCollisionModelContainer().UpdateCollisionModel(hAtari, pVelocity, pRotation);
};


/*static*/ void CMapCollisionModel::SetBoundingSphereRadius(uint32 hAtari, float fRadius)
{
    MapCollisionModelContainer().SetBoundingSphereRadius(hAtari, fRadius);
};


/*static*/ const CMapCollisionModel::COLLISIONPARAM_MODEL* CMapCollisionModel::GetModelCollisionResult(void)
{
    return MapCollisionModelContainer().GetModelCollisionResult();
};