#include "FenceGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/WorldMap.hpp"


CFenceGimmickMove::CFenceGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_fRadius(0.0f)
, m_vRotation(Math::VECTOR3_ZERO)
, m_vRotVelocity(Math::VECTOR3_ZERO)
{
    ;
};


CFenceGimmickMove::~CFenceGimmickMove(void)
{
    ;
};


CFenceGimmickMove::RESULT CFenceGimmickMove::OnMove(float dt)
{
    RESULT ret = RESULT_NONE;

    //
    //  check for zero speed
    //
    float fSpeed = Math::Vec3_Length(&m_vVelocity);
    if (Math::FEqual(fSpeed, 0.0f))
        return ret;

    //
    //  apply gravity
    //
    m_vVelocity.y += (CGameProperty::GetGravity() * dt);

    //
    //  integrate pos
    //
    RwV3d vDltPos = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltPos, &m_vVelocity, dt);

    RwV3d vNewPos = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewPos, &m_vPosition, &vDltPos);

    vNewPos.y += m_fRadius;

    //
    //  integrate & apply rot
    //
    RwV3d vDltRot = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltRot, &m_vRotVelocity, dt);
    
    RwV3d vNewRot = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewRot, &m_vRotation, &vDltRot);

    m_vRotation = vNewRot;

    //
    //  check collision
    //
    if (CWorldMap::CheckCollisionSphereMove(&vNewPos, 0.1f, &vDltRot, CWorldMap::CHECKFLAG_NONE))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        Math::Vec3_Scale(&vDltPos, &pResult->m_vNormal, 0.1f);
        Math::Vec3_Add(&vNewPos, &pResult->m_vClosestPt, &vDltPos);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Math::Vec3_Negate(&vVelocity, &m_vVelocity);

        float fRefCoeff = Math::Vec3_Dot(&pResult->m_vNormal, &vVelocity) * 1.05f;

        RwV3d vNewVel = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vNewVel, &pResult->m_vNormal, fRefCoeff);
        Math::Vec3_Sub(&m_vVelocity, &vNewVel, &vVelocity);

        ret = RESULT_COLLIDED;
    };

    //
    //  apply new pos
    //
    vNewPos.y -= m_fRadius;
    m_vPosition = vNewPos;

    return ret;
};