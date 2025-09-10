#include "GimmickUtils.hpp"

#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Map/MapCamera.hpp"


/*static*/ void CGimmickUtils::QuaternionToRotationMatrix(RwMatrix* pOutMatrix, const GIMMICKPARAM::QUATERNION* pQuat)
{
    ASSERT(pOutMatrix);
    ASSERT(pQuat);

    float xc = pQuat->x + pQuat->x;
    float yc = pQuat->y + pQuat->y;
    float zc = pQuat->z + pQuat->z;
    
    float xx = xc * pQuat->x;
    float xy = yc * pQuat->x;
    float xz = zc * pQuat->x;
    
    float wx = xc * pQuat->w;
    float wy = yc * pQuat->w;
    float wz = zc * pQuat->w;
    
    float yy = yc * pQuat->y;
    float yz = zc * pQuat->y;
    
    float zz = zc * pQuat->z;

    RwV3d vRight = Math::VECTOR3_ZERO;
    RwV3d vUp = Math::VECTOR3_ZERO;
    RwV3d vAt = Math::VECTOR3_ZERO;
    
    vRight.x = 1.0f - (zz + yy);
    vRight.y = wz + xy;
    vRight.z = xz - wy;

    vUp.x = xy - wz;
    vUp.y = 1.0f - (zz + xx);
    vUp.z = wx + yz;
    
    vAt.x = wy + xz;
    vAt.y = yz - wx;
    vAt.z = 1.0f - (yy + xx);

    Math::Matrix_Update(pOutMatrix, &vRight, &vUp, &vAt);
};


/*static*/ void CGimmickUtils::QuaternionFromAxisAngle(GIMMICKPARAM::QUATERNION* pQuat, const RwV3d* pvAxis, float fRotAngle)
{
    ASSERT(pQuat);
    ASSERT(pvAxis);

    pQuat->x = pvAxis->x * Math::Sin(fRotAngle * 0.5f);
    pQuat->y = pvAxis->y * Math::Sin(fRotAngle * 0.5f);
    pQuat->z = pvAxis->z * Math::Sin(fRotAngle * 0.5f);
    pQuat->w = Math::Cos(fRotAngle * 0.5f);
};


/*static*/ float CGimmickUtils::QuaternionToRotationY(const GIMMICKPARAM::QUATERNION* pQuat)
{
    ASSERT(pQuat);
    
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);

    QuaternionToRotationMatrix(&matrix, pQuat);

    return Math::ATan2(matrix.at.x, matrix.at.z);
};


/*static*/ void CGimmickUtils::MatrixToRotation(RwMatrix* pMatrix, RwV3d* pvRotation)
{
    RwV3d vAt = pMatrix->at;
    RwV3d vRight = pMatrix->right;
    RwV3d vUp = pMatrix->up;

    if (vAt.z == 0.0f)
    {
        pvRotation->z = 0.0f;
        pvRotation->y = -Math::ATan2(vRight.z, vRight.x);

        float fTmp = 1.0f - (vAt.y * vAt.y);
        if (fTmp < 0.0f)
            fTmp = 0.0f;
        
        pvRotation->x = -Math::ATan2(
            vAt.y,
            Math::Sqrt(fTmp) * (vRight.y < 0.0f ? 1.0f : -1.0f)
        );
    }
    else
    {
        pvRotation->z = -Math::ATan2(
            -vRight.y * (vAt.z <= 0.0f ? -1.0f : 1.0f),
            vUp.y * (vAt.z <= 0.0f ? -1.0f : 1.0f)
        );

        float fTmp = 1.0f - (vAt.y * vAt.y);
        if (fTmp < 0.0f)
            fTmp = 0.0f;

        pvRotation->x = -Math::ATan2(
            vAt.y,
            Math::Sqrt(fTmp) * (vAt.z <= 0.0f ? -1.0f : 1.0f)
        );

        pvRotation->y = -Math::ATan2(
            -vAt.x * (vAt.z <= 0.0f ? -1.0f : 1.0f),
            vAt.z * (vAt.z <= 0.0f ? -1.0f : 1.0f)
        );
    };
};


/*static*/ float CGimmickUtils::CalcNearestPlayerDistance(const RwV3d* pvPos, RwV3d* pvPlayerPosition)
{
    ASSERT(pvPos);
    
    float fNearestDistance = TYPEDEF::FLOAT_MAX;

    for (int32 i = 0; i < CGameProperty::GetPlayerNum(); ++i)
    {
        IGamePlayer* pGamePlayer = CGameProperty::Player(i);
        if (!pGamePlayer->IsAlive())
            continue;

        RwV3d vGameplayerPos = Math::VECTOR3_ZERO;
        pGamePlayer->GetPosition(&vGameplayerPos);

        RwV3d vDistance = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDistance, &vGameplayerPos, pvPos);

        float fDistance = Math::Vec3_Length(&vDistance);
        if (fDistance < fNearestDistance)
        {
            fNearestDistance = fDistance;
            if (pvPlayerPosition)
                *pvPlayerPosition = vGameplayerPos;
        };
    };

    return fNearestDistance;
};


/*static*/ float CGimmickUtils::CalcNearestPlayerDistanceXZ(const RwV3d* pvPos, RwV3d* pvPlayerPosition)
{
    ASSERT(pvPos);
    
    float fNearestDistance = TYPEDEF::FLOAT_MAX;

    int32 playerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        IGamePlayer* pGamePlayer = CGameProperty::Player(i);
        if (!pGamePlayer->IsAlive())
            continue;

        RwV3d vGameplayerPos = Math::VECTOR3_ZERO;
        pGamePlayer->GetPosition(&vGameplayerPos);
        
        RwV3d vDistance = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDistance, &vGameplayerPos, pvPos);
        vDistance.y = 0.0f;

        float fDistance = Math::Vec3_Length(&vDistance);
        if (fDistance < fNearestDistance)
        {
            fNearestDistance = fDistance;
            if (pvPlayerPosition)
                *pvPlayerPosition = vGameplayerPos;
        };
    };
    
    return fNearestDistance;
};


/*static*/ void CGimmickUtils::PlayHitEffect(CHitAttackData* pAttack)
{
    ASSERT(pAttack);

    RwV3d vPosition = Math::VECTOR3_ZERO;

    switch (pAttack->GetShape())
    {
    case CHitAttackData::SHAPE_SPHERE:
        vPosition = pAttack->GetSphere()->center;
        break;

    case CHitAttackData::SHAPE_LINE:
        vPosition = *pAttack->GetHitPos();
        break;

    default:
        ASSERT(false);
        break;
    };

    EFFECT_GENERIC::CallHitEffect(pAttack, &vPosition);
    CGameSound::PlayOtherDamageSE(pAttack->GetObject(), pAttack->GetCatchObject());
};


/*static*/ bool CGimmickUtils::IsPositionVisible(const RwV3d* pvPos)
{
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    CGameProperty::GetCameraViewMatrix(&matrix);
    
    RwV3d vScreenPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vScreenPos, pvPos, &matrix);

    if (vScreenPos.z > 0.0f)
    {
        vScreenPos.x *= (1.0f / vScreenPos.z);
        vScreenPos.y *= (1.0f / vScreenPos.z);

        if ((vScreenPos.z >= 1.0f)    &&
            (vScreenPos.z <= 100.0f)  &&
            (vScreenPos.x >= 0.0f)    &&
            (vScreenPos.x <= 1.0f)    &&
            (vScreenPos.y >= 0.0f)    &&
            (vScreenPos.y <= 1.0f))
        {
            return true;
        };
    };

    return false;
};


/*static*/ bool CGimmickUtils::IsPositionVisible(const RwV3d* pvPos, float fRadius, bool bTestZ)
{
    if (bTestZ)
    {
        RwMatrix matrix;
        RwMatrixSetIdentityMacro(&matrix);
        CGameProperty::GetCameraViewMatrix(&matrix);

        RwV3d vScreenPos = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vScreenPos, pvPos, &matrix);

        if ((vScreenPos.z < 1.0f) ||
            (vScreenPos.z > 100.0f))
            return false;
    };

    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    if (pMapCamera && pMapCamera->FrustumSphereTest(pvPos, fRadius))
        return true;

    return false;
};