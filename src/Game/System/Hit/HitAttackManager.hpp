#pragma once


class CHitAttackData;
class CHitCatchData;


class CHitAttackManager
{    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Pause(void);
    static void Resume(void);
    static void RegistAttack(const CHitAttackData* pHitAttack);
    static void RegistCatch(const CHitCatchData* pHitCatch);
};