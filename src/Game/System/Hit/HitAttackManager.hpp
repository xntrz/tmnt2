#pragma once


class CHitAttackData;
class CHitCatchData;


class CHitAttackManager
{    
public:
#ifdef _DEBUG    
    static bool m_bDebugDrawSphereAttack;
    static bool m_bDebugDrawSphereCatch;
#endif
    
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Pause(void);
    static void Resume(void);
    static void RegistAttack(const CHitAttackData* pHitAttack);
    static void RegistCatch(const CHitCatchData* pHitCatch);
};