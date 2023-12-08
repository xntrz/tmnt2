#include "NormalGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/WorldMap.hpp"


CNormalGimmickMove::CNormalGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_fRadius(0.0f)
{
    ;
};


CNormalGimmickMove::~CNormalGimmickMove(void)
{
    ;
};


CNormalGimmickMove::RESULT CNormalGimmickMove::OnMove(float dt)
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
    //  check collision
    //
    if (CWorldMap::CheckCollisionSphereMove(&vNewPos, m_fRadius, &vDltPos, CWorldMap::CHECKFLAG_NONE))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        Math::Vec3_Scale(&vDltPos, &pResult->m_vNormal, m_fRadius);
        Math::Vec3_Add(&vNewPos, &pResult->m_vClosestPt, &vDltPos);

        m_vVelocity = Math::VECTOR3_AXIS_Z;

        ret = RESULT_COLLIDED;
    };

    m_vPosition = vNewPos;

    return ret;
};