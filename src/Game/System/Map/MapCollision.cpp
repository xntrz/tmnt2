#include "MapCollision.hpp"
#include "MapAttribute.hpp"

#include "Game/System/Hit/Intersection.hpp"


static inline bool isFaceToFaceVectors(const RwV3d* v1, const RwV3d* v2)
{
    float d = Math::Vec3_Dot(v1, v2);
    float c = Math::Cos(MATH_DEG2RAD(135.0f));

    return (c < d);
};


static inline void checkWallJump(CMapCollision::COLLISIONPARAM* pCollisionParam,
                                 RpCollisionTriangle* pCollTriangle,
                                 RwV3d* pPos,
                                 float fDistance)
{
    if (fDistance > pCollisionParam->m_mapwall.m_fDistance)
        return;

    if (!pCollisionParam->m_pvVelocity)
        return;

    if (!isFaceToFaceVectors(pCollisionParam->m_pvVelocity, &pCollTriangle->normal))
        return;

    pCollisionParam->m_mapwall.m_fDistance  = fDistance;
    pCollisionParam->m_mapwall.m_bHit       = true;
    pCollisionParam->m_mapwall.m_vClosestPt = *pPos;
    pCollisionParam->m_mapwall.m_vNormal    = pCollTriangle->normal;
};


/*static*/ void CMapCollision::GetMaterialTextureName(char* pszName, int32 nMaterialID, RpWorld* pWorldCollision)
{
    ASSERT(pWorldCollision);
    ASSERT(pszName);
    ASSERT(nMaterialID >= 0);
    ASSERT(nMaterialID < RpWorldGetNumMaterialsMacro(pWorldCollision));

	RpMaterial* pMaterial = RpWorldGetMaterialMacro(pWorldCollision, nMaterialID);
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


/*static*/ void CMapCollision::GetMaterialColor(RwRGBA& rColor, int32 nMaterialID, RpWorld* pWorldCollision)
{
    ASSERT(pWorldCollision);
    ASSERT(nMaterialID >= 0);
    ASSERT(nMaterialID < RpWorldGetNumMaterialsMacro(pWorldCollision));

    rColor = RpWorldGetMaterialMacro(pWorldCollision, nMaterialID)->color;
};


/*static*/ MAPTYPES::ATTRIBUTE
CMapCollision::GetMapAttribute(RpWorldSector* pSector,
                               RpCollisionTriangle* pCollTriangle,
                               RpWorld* pWorldCollision,
                               RwRGBA& rColor,
                               char* pszAttributeName)
{
    char szAttributeName[32];
    szAttributeName[0] = '\0';
    
    ASSERT(pCollTriangle->index >= 0);
    ASSERT(pCollTriangle->index < pSector->numTriangles);

    int32 nMaterialID = pSector->triangles[pCollTriangle->index].matIndex;

    GetMaterialColor(rColor, nMaterialID, pWorldCollision);
    GetMaterialTextureName(szAttributeName, nMaterialID, pWorldCollision);

    if (pszAttributeName)
        std::strcpy(pszAttributeName, szAttributeName);

    return CMapAttribute::CheckAttribute(szAttributeName);
};


/*static*/ RpCollisionTriangle*
CMapCollision::CollisionPointCallback(RpIntersection* pIntersection,
                                      RpWorldSector* pSector,
                                      RpCollisionTriangle* pCollTriangle,
                                      float fDistance,
                                      void* pData)
{
    COLLISIONPARAM* pCollisionParam = static_cast<COLLISIONPARAM*>(pData);

    if (fDistance >= pCollisionParam->m_fDistance)
        return pCollTriangle;

    pCollisionParam->m_attribute = GetMapAttribute(pSector,
                                                   pCollTriangle,
                                                   pCollisionParam->m_pWorldCollision,
                                                   pCollisionParam->m_Color);

    pCollisionParam->m_fDistance = fDistance;
    pCollisionParam->m_vNormal   = pCollTriangle->normal;

    float fDist2Plane =
        Math::Vec3_Dot(pCollTriangle->vertices[0], &pCollisionParam->m_vNormal) -
        Math::Vec3_Dot(&pIntersection->t.point, &pCollisionParam->m_vNormal);

    pCollisionParam->m_vClosestPt.x = ((pCollisionParam->m_vNormal.x * fDist2Plane) + pIntersection->t.point.x);
    pCollisionParam->m_vClosestPt.y = ((pCollisionParam->m_vNormal.y * fDist2Plane) + pIntersection->t.point.y);
    pCollisionParam->m_vClosestPt.z = ((pCollisionParam->m_vNormal.z * fDist2Plane) + pIntersection->t.point.z);

    return pCollTriangle;
};


/*static*/ RpCollisionTriangle*
CMapCollision::CollisionLineCallback(RpIntersection* pIntersection,
                                     RpWorldSector* pSector,
                                     RpCollisionTriangle* pCollTriangle,
                                     float fDistance,
                                     void* pData)
{
    COLLISIONPARAM* pCollisionParam = static_cast<COLLISIONPARAM*>(pData);

    if (fDistance >= pCollisionParam->m_fDistance)
        return pCollTriangle;

    pCollisionParam->m_attribute = GetMapAttribute(pSector,
                                                   pCollTriangle,
                                                   pCollisionParam->m_pWorldCollision,
                                                   pCollisionParam->m_Color,
                                                   pCollisionParam->m_szAttributeName);

    pCollisionParam->m_fDistance = fDistance;
    pCollisionParam->m_vNormal   = pCollTriangle->normal;

    RwV3d vTmp = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vTmp, &pIntersection->t.line.end, &pIntersection->t.line.start);

    pCollisionParam->m_vClosestPt.x = ((vTmp.x * fDistance) + pIntersection->t.line.start.x);
    pCollisionParam->m_vClosestPt.y = ((vTmp.y * fDistance) + pIntersection->t.line.start.y);
    pCollisionParam->m_vClosestPt.z = ((vTmp.z * fDistance) + pIntersection->t.line.start.z);

    return pCollTriangle;
};


/*static*/ RpCollisionTriangle*
CMapCollision::CollisionSphereCallback(RpIntersection* pIntersection,
                                       RpWorldSector* pSector,
                                       RpCollisionTriangle* pCollTriangle,
                                       float fDistance,
                                       void* pData)
{
    COLLISIONPARAM* pCollisionParam = static_cast<COLLISIONPARAM*>(pData);

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

        MAPTYPES::ATTRIBUTE attribute = GetMapAttribute(pSector,
                                                        pCollTriangle,
                                                        pCollisionParam->m_pWorldCollision,
                                                        pCollisionParam->m_Color);

        if (fDist <= pCollisionParam->m_fDistance)            
        {
            if (!(pCollisionParam->m_checkflag & CWorldMap::CHECKFLAG_ONEWAY) ||
                 (attribute != MAPTYPES::ATTRIBUTE_ONEWAY))
            {
                pCollisionParam->m_vClosestPt.x = vProjPos.x;
                pCollisionParam->m_vClosestPt.y = vProjPos.y;
                pCollisionParam->m_vClosestPt.z = vProjPos.z;
                pCollisionParam->m_fDistance    = fDist;
                pCollisionParam->m_vNormal      = pCollTriangle->normal;
                pCollisionParam->m_attribute    = attribute;
            };
        };

        if ((pCollisionParam->m_checkflag & CWorldMap::CHECKFLAG_WALLJUMP) ||
            (attribute == MAPTYPES::ATTRIBUTE_JUMP))
        {
            checkWallJump(pCollisionParam, pCollTriangle, &vProjPos, fDist);            
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

            MAPTYPES::ATTRIBUTE attribute = GetMapAttribute(pSector,
                                                            pCollTriangle,
                                                            pCollisionParam->m_pWorldCollision,
                                                            pCollisionParam->m_Color);

            if ((pCollisionParam->m_fDistance + 0.001f) >= fDist)
            {
                if (!(pCollisionParam->m_checkflag & CWorldMap::CHECKFLAG_ONEWAY) ||
                     (attribute != MAPTYPES::ATTRIBUTE_ONEWAY))
                {
                    pCollisionParam->m_vClosestPt.x = vClosestPt.x;
                    pCollisionParam->m_vClosestPt.y = vClosestPt.y;
                    pCollisionParam->m_vClosestPt.z = vClosestPt.z;
                    pCollisionParam->m_fDistance = fDist;                    
                    Math::Vec3_Normalize(&pCollisionParam->m_vNormal, &vTemp);
                    pCollisionParam->m_attribute = attribute;
                };
            };

            if ((pCollisionParam->m_checkflag & CWorldMap::CHECKFLAG_WALLJUMP) ||
                (attribute == MAPTYPES::ATTRIBUTE_JUMP))
            {
                checkWallJump(pCollisionParam, pCollTriangle, &vProjPos, fDist);
            };
        };
    };

    return pCollTriangle;
};


/*static*/ RpCollisionTriangle*
CMapCollision::CollisionBBoxCallback(RpIntersection* pIntersection,
                                     RpWorldSector* pSector,
                                     RpCollisionTriangle* pCollTriangle,
                                     float fDistance,
                                     void* pData)
{
    COLLISIONPARAM* pCollisionParam = static_cast<COLLISIONPARAM*>(pData);

    if (fDistance >= pCollisionParam->m_fDistance)
        return pCollTriangle;

    pCollisionParam->m_attribute = GetMapAttribute(pSector,
                                                   pCollTriangle,
                                                   pCollisionParam->m_pWorldCollision,
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


/*static*/ void CMapCollision::CheckMapCollision(RpIntersection* pIntersection, COLLISIONPARAM* pCollisionParam)
{
    ASSERT(pIntersection);
    ASSERT(pCollisionParam);
    ASSERT(pCollisionParam->m_pWorldCollision);
    
    RpIntersectionCallBackWorldTriangle callback = nullptr;
    
    switch (pIntersection->type)
    {
    case rpINTERSECTLINE:
        callback = CollisionLineCallback;
        break;

    case rpINTERSECTPOINT:
        callback = CollisionPointCallback;
        break;

    case rpINTERSECTSPHERE:
        callback = CollisionSphereCallback;
        break;

    case rpINTERSECTBOX:
        callback = CollisionBBoxCallback;
        break;

    default:
        ASSERT(false);
        break;
    };

    if (callback)
    {
        RpCollisionWorldForAllIntersections(pCollisionParam->m_pWorldCollision,
                                            pIntersection,
                                            callback,
                                            pCollisionParam);
    };
};