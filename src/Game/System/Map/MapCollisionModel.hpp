#pragma once

#include "WorldMap.hpp"


class CMapCollisionModel
{
public:
    struct COLLISIONPARAM_MODEL : public CWorldMap::COLLISIONPARAM_BASE
    {
        float m_fDistance;
        char m_szAttributeName[32];
        RwV3d* m_pPrimitiveVelocity;
        RpGeometry* m_pGeometry;
        bool m_bUpdate;
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static bool CheckModelCollision(RpIntersection* pIntersection, RwV3d* pVelocity);
    static uint32 RegistCollisionModel(RpClump* pClump, const char* pszName, MAPTYPES::GIMMICKTYPE type);
    static uint32 RegistCollisionModel(RpClump* pClump);
    static void RemoveCollisionModel(uint32 hAtari);
    static void UpdateCollisionModel(uint32 hAtari, RwV3d* pVelocity, RwV3d* pRotation = nullptr);
    static void SetBoundingSphereRadius(uint32 hAtari, float fRadius);
    static const COLLISIONPARAM_MODEL* GetModelCollisionResult(void);
};