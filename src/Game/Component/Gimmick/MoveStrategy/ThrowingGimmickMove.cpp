#include "ThrowingGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/WorldMap.hpp"


CThrowingGimmickMove::CThrowingGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_vRotVelocity(Math::VECTOR3_ZERO)
, m_fRadius(0.0f)
{
    ;
};


CThrowingGimmickMove::~CThrowingGimmickMove(void)
{
    ;
};


CThrowingGimmickMove::RESULT CThrowingGimmickMove::OnMove(float dt)
{
    RESULT Result = RESULT_NONE;

    float fSpeed = Math::Vec3_Length(&m_vVelocity);    
    if (Math::FEqual(fSpeed, 0.0f))
        return Result;
    
    m_vVelocity.y += (CGameProperty::GetGravity() * dt);
    
    RwV3d vFrameVel = Math::VECTOR3_ZERO;
    RwV3d vFramePos = Math::VECTOR3_ZERO;
    
    Math::Vec3_Scale(&vFrameVel, &m_vVelocity, dt);
    Math::Vec3_Add(&vFramePos, &m_vPosition, &vFrameVel);
    vFramePos.y += m_fRadius;

	RwV3d vFrameRotVel = Math::VECTOR3_ZERO;
	Math::Vec3_Scale(&vFrameRotVel, &m_vRotVelocity, dt);
	Math::Vec3_Add(&m_vRotation, &m_vRotation, &vFrameRotVel);

    if (CWorldMap::CheckCollisionSphereMove(&vFramePos, m_fRadius, &vFrameVel, CWorldMap::CHECKFLAG_NONE))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        if (!IS_FLAG_SET(pResult->m_attribute, MAPTYPES::ATTRIBUTE_ONEWAY))
        {
            Result = RESULT_COLLIDED;

            m_vVelocity = Math::VECTOR3_ZERO;
            m_vRotVelocity = Math::VECTOR3_ZERO;

            vFramePos.x = pResult->m_vClosestPt.x + (m_fRadius * pResult->m_vNormal.x);
            vFramePos.y = pResult->m_vClosestPt.y + (m_fRadius * pResult->m_vNormal.y);
            vFramePos.z = pResult->m_vClosestPt.z + (m_fRadius * pResult->m_vNormal.z);
        };
    };

    vFramePos.y -= m_fRadius;    
    m_vPosition = vFramePos;

    return Result;
};


void CThrowingGimmickMove::SetRadius(float fRadius)
{
    m_fRadius = fRadius;
};


void CThrowingGimmickMove::SetVelocity(const RwV3d* pvVelocity)
{
    ASSERT(pvVelocity);
    
    m_vVelocity = *pvVelocity;
};


void CThrowingGimmickMove::SetRotation(const RwV3d* pvRotation)
{
    ASSERT(pvRotation);

    m_vRotation = *pvRotation;
};


void CThrowingGimmickMove::SetRotVelocity(const RwV3d* pvRotVel)
{
    ASSERT(pvRotVel);

    m_vRotVelocity = *pvRotVel;
};


void CThrowingGimmickMove::GetRotation(RwV3d* pvRotation) const
{
    ASSERT(pvRotation);

    *pvRotation = m_vRotation;
};


void CThrowingGimmickMove::GetRotVelocity(RwV3d* pvRotVel)
{
    ASSERT(pvRotVel);

    *pvRotVel = m_vRotVelocity;
};