#pragma once

#include "WorldMap.hpp"

#include "rpcollis.h"


class CMapCollision
{
public:
    struct COLLISIONPARAM : public CWorldMap::COLLISIONPARAM_BASE
    {
        float m_fDistance;
        char m_szAttributeName[32];
        CWorldMap::CHECKFLAG m_checkflag;
        RpWorld* m_pWorldCollision;
        RwV3d* m_pvVelocity;
    };

public:
    static void GetMaterialTextureName(char* pszName, int32 nMaterialID, RpWorld* pWorldCollision);
    static void GetMaterialColor(RwRGBA& rColor, int32 nMaterialID, RpWorld* pWorldCollision);
    static MAPTYPES::ATTRIBUTE GetMapAttribute(RpWorldSector* pSector, RpCollisionTriangle* pCollTriangle, RpWorld* pWorldCollision, RwRGBA& rColor, char* pszAttributeName = nullptr);
    static RpCollisionTriangle* CollisionPointCallback(RpIntersection* pIntersection, RpWorldSector* pSector, RpCollisionTriangle* pCollTriangle, float fDistance, void* pData);
    static RpCollisionTriangle* CollisionLineCallback(RpIntersection* pIntersection, RpWorldSector* pSector, RpCollisionTriangle* pCollTriangle, float fDistance, void* pData);
    static RpCollisionTriangle* CollisionSphereCallback(RpIntersection* pIntersection, RpWorldSector* pSector, RpCollisionTriangle* pCollTriangle, float fDistance, void* pData);
    static RpCollisionTriangle* CollisionBBoxCallback(RpIntersection* pIntersection, RpWorldSector* pSector, RpCollisionTriangle* pCollTriangle, float fDistance, void* pData);
    static void CheckMapCollision(RpIntersection* pIntersection, COLLISIONPARAM* pCollisionParam);
};