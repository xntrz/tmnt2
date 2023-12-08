#include "UtromGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/WorldMap.hpp"


CUtromGimmickMove::CUtromGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_fRadius(0.0f)
{
    ;
};


CUtromGimmickMove::~CUtromGimmickMove(void)
{
    ;
};


CUtromGimmickMove::RESULT CUtromGimmickMove::OnMove(float dt)
{
    const int32 ofs = offsetof(RwCamera, frustumCorners[2]);
    RESULT ret = RESULT_NONE;

    m_vVelocity.y += (CGameProperty::GetGravity() * dt);

    RwV3d vDltPos = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltPos, &m_vVelocity, dt);

    RwV3d vNewPos = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewPos, &m_vPosition, &vDltPos);

    if (CWorldMap::CheckCollisionSphereMove(&vNewPos, m_fRadius, &vDltPos, CWorldMap::CHECKFLAG_NONE))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        if (!FLAG_TEST_ANY(pResult->m_attribute, MAPTYPES::ATTRIBUTE_ONEWAY))
        {
            Math::Vec3_Scale(&vDltPos, &pResult->m_vNormal, m_fRadius);
            Math::Vec3_Add(&vNewPos, &pResult->m_vClosestPt, &vDltPos);

            RwV3d vVelocity = Math::VECTOR3_ZERO;
            Math::Vec3_Negate(&vVelocity, &m_vVelocity);

            float fRefCoeff = Math::Vec3_Dot(&pResult->m_vNormal, &vVelocity) * 1.05f;

            RwV3d vNewVel = Math::VECTOR3_ZERO;
            Math::Vec3_Scale(&vNewVel, &pResult->m_vNormal, fRefCoeff);
            Math::Vec3_Sub(&m_vVelocity, &vNewVel, &vVelocity);
            Math::Vec3_Scale(&m_vVelocity, &m_vVelocity, 0.8f);
            
            ret = RESULT_COLLIDED;
        };
    };

    m_vPosition = vNewPos;

    ASSERT(m_vPosition.y > -100.0f);

    return ret;
};