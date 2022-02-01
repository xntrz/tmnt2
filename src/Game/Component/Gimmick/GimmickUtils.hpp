#pragma once

#include "GimmickParam.hpp"


class CHitAttackData;


class CGimmickUtils
{
public:
    static void QuaternionToRotationMatrix(RwMatrix* pOutMatrix, const GIMMICKPARAM::QUATERNION* pQuat);
    static void QuaternionFromAxisAngle(GIMMICKPARAM::QUATERNION* pQuat, const RwV3d* pvAxis, float fRotAngle);
    static float QuaternionToRotationY(const GIMMICKPARAM::QUATERNION* pQuat);
    static void MatrixToRotation(RwMatrix* pMatrix, RwV3d* pvRotation);
    static float CalcNearestPlayerDistance(const RwV3d* pvPos, RwV3d* pvPlayerPosition = nullptr);
    static float CalcNearestPlayerDistanceXZ(const RwV3d* pvPos, RwV3d* pvPlayerPosition = nullptr);
    static void PlayHitEffect(CHitAttackData* pAttack);
    static bool IsPositionVisible(const RwV3d* pvPos);
    static bool IsPositionVisible(const RwV3d* pvPos, float fRadius, bool bViewMatTest);
};