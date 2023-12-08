#include "FallRubbleGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/WorldMap.hpp"


CFallRubbleGimmickMove::CFallRubbleGimmickMove(float fRefCoe, float fBodyHitRadius)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_vRotVelocity(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_fE(fRefCoe)
, m_fBodyHitRadius(fBodyHitRadius)
{
    ;
};


CFallRubbleGimmickMove::~CFallRubbleGimmickMove(void)
{
    ;
};


CFallRubbleGimmickMove::RESULT CFallRubbleGimmickMove::OnMove(float dt)
{
    RESULT ret = RESULT_NONE;

    //
    //  apply gravity
    //
    m_vVelocity.y += (CGameProperty::GetGravity() * dt);

    //
    //  integrate position
    //
    RwV3d vDltPos = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltPos, &m_vVelocity, dt);
    
    RwV3d vNewPos = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewPos, &m_vPosition, &vDltPos);

    //
    //  integrate rotation
    //
    RwV3d vDltRot = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltRot, &m_vRotVelocity, dt);

    RwV3d vNewRot = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewRot, &m_vRotation, &vDltRot);

    m_vRotation = vNewRot;

    //
    //  check collision
    //
    if (CWorldMap::CheckCollisionSphereMove(&vNewPos, m_fBodyHitRadius, &vDltPos, CWorldMap::CHECKFLAG_NONE))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        Math::Vec3_Scale(&vDltPos, &pResult->m_vNormal, m_fBodyHitRadius);
        Math::Vec3_Add(&vNewPos, &pResult->m_vClosestPt, &vDltPos);

        float fRefCoeff = Math::Vec3_Dot(&pResult->m_vNormal, &m_vVelocity) * (m_fE + 1.0f);

        RwV3d vNewVel = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vNewVel, &pResult->m_vNormal, fRefCoeff);
        Math::Vec3_Sub(&m_vVelocity, &m_vVelocity, &vNewVel);

        ret = RESULT_COLLIDED;
    };

    m_vPosition = vNewPos;
    
    return ret;
};