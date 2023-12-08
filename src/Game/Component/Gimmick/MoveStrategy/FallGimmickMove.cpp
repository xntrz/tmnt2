#include "FallGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/FallGimmick.hpp"
#include "Game/System/Map/WorldMap.hpp"


CFallGimmickMove::CFallGimmickMove(int32 movetype, float fReflection, float fCollisionRadius)
: m_movetype(movetype)
, m_vRotation(Math::VECTOR3_ZERO)
, m_vVelocity(Math::VECTOR3_ZERO)
, m_vRotationVelocity(Math::VECTOR3_ZERO)
, m_fReflection(fReflection)
, m_fCollisionRadius(fCollisionRadius)
, m_bColRadiusAutoChangeEnable(false)
{
    ASSERT((movetype >= CFallGimmick::MOVETYPE_START) && (movetype < CFallGimmick::MOVETYPE_NUM));
};


CFallGimmickMove::~CFallGimmickMove(void)
{
    ;
};


CFallGimmickMove::RESULT CFallGimmickMove::OnMove(float dt)
{
    RESULT eResult = RESULT_NONE;
    
    m_vVelocity.y += (CGameProperty::GetGravity() * dt);

    RwV3d vFrameVel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vFrameVel, &m_vVelocity, dt);

    RwV3d vFramePos = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vFramePos, &m_vPosition, &vFrameVel);

    RwV3d vFrameRotVel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vFrameRotVel, &m_vRotationVelocity, dt);

    RwV3d vFrameRot = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vFrameRot, &m_vRotation, &vFrameRotVel);

    collisionRadiusAutoChange();

    if (CWorldMap::CheckCollisionSphereMove(&vFramePos, m_fCollisionRadius, &vFrameVel, CWorldMap::CHECKFLAG_NONE))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        if (m_movetype == CFallGimmick::MOVETYPE_BOUND)
        {
            Math::Vec3_Scale(&vFrameVel, &pResult->m_vNormal, m_fCollisionRadius);
            Math::Vec3_Add(&vFramePos, &pResult->m_vClosestPt, &vFrameVel);

            if (pResult->m_type == MAPTYPES::HITTYPE_GIMMICK)
                Math::Vec3_Add(&vFramePos, &vFramePos, &pResult->m_mapobj.m_vVelocity);

            RwV3d vDlt = Math::VECTOR3_ZERO;
            Math::Vec3_Scale(
                &vDlt,
                &pResult->m_vNormal,
                Math::Vec3_Dot(&m_vVelocity, &pResult->m_vNormal) * (m_fReflection + 1.0f)
            );

            Math::Vec3_Sub(&m_vVelocity, &m_vVelocity, &vDlt);
        };

        eResult = RESULT_COLLIDED;
    };

    m_vPosition = vFramePos;
    m_vRotation = vFrameRot;
    
    return eResult;
};


void CFallGimmickMove::collisionRadiusAutoChange(void)
{
    if (m_bColRadiusAutoChangeEnable)
		m_fCollisionRadius = Clamp(Math::FAbs(m_vVelocity.y) / 98.0f * 1.7f, 0.35f, 1.7f);
};