#pragma once


class CBodyHitData;


class CBodyHitManager
{
public:
#ifdef _DEBUG    
    static bool m_bDebugDrawSphere;
#endif
    
    static void Initialize(void);
    static void Terminate(void);
    static CBodyHitData* AllocData(void);
    static void FreeData(CBodyHitData* pBodyHitData);
    static bool CheckHit(CBodyHitData* pBodyHitData, const RwV3d* pMoveVelocity, RwV3d* pResult);
};