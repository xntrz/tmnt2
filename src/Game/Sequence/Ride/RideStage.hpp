#pragma once

#include "RideTypes.hpp"


class CHitCatchData;
class CHitAttackData;


class CRideStage
{
public:
    static bool m_bSpace;
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Draw(void);
    static void OnLoaded(void);
    static void GetBasisPoint(RwV3d* pvBasisPoint);
    static void GetBasisVector(RwV3d* pvBasisVector);
    static int32 GetScore(RIDETYPES::SCOREKIND scorekind);
    static void AddScore(RIDETYPES::SCOREKIND scorekind, CHitCatchData* pHitCatchData);
    static void NotifyGimmickDestroyed(CHitAttackData* pHitAttackData);
    static float GetTime(void);
    static bool GetShadowFlag(void);
    static void UpdateResultData(void);
    static void SetMoveLimit(void);
    static float GetMoveLimitZMax(void);
    static float GetMoveLimitZMin(void);
    static float GetMoveLimitXMax(void);
    static float GetMoveLimitXMin(void);
    static float GetMoveLimitYMax(void);
    static float GetMoveLimitYMin(void);
};