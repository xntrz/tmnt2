#include "RollingRockGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/WorldMap.hpp"


CRollingRockGimmickMove::CRollingRockGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_vTheta(Math::VECTOR3_ZERO)
, m_fOmega(0.0f)
, m_fHitRadius(0.0f)
, m_vInitPosition(Math::VECTOR3_ZERO)
, m_bReset(false)
{
    ;
};


CRollingRockGimmickMove::~CRollingRockGimmickMove(void)
{
    ;
};


CRollingRockGimmickMove::RESULT CRollingRockGimmickMove::OnMove(float dt)
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

    if (m_vPosition.y < -100.0f)
        m_bReset = true;

    //
    //  check collision
    //
    if (CWorldMap::CheckCollisionSphereMove(&vNewPos, m_fHitRadius, &vDltPos, CWorldMap::CHECKFLAG_ONEWAY))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        if (FLAG_TEST_ANY(pResult->m_attribute, MAPTYPES::ATTRIBUTE_DEATH))
        {
            m_bReset = true;
        }
        else if (!FLAG_TEST_ANY(pResult->m_attribute, MAPTYPES::ATTRIBUTE_ONEWAY))
        {
            Math::Vec3_Scale(&vDltPos, &pResult->m_vNormal, m_fHitRadius);
            Math::Vec3_Add(&vNewPos, &pResult->m_vClosestPt, &vDltPos);

            RwV3d vVelocity = Math::VECTOR3_ZERO;
            Math::Vec3_Negate(&vVelocity, &m_vVelocity);

            float fRefCoeff = Math::Vec3_Dot(&pResult->m_vNormal, &vVelocity) * 1.05f;

            RwV3d vNewVel = Math::VECTOR3_ZERO;
            Math::Vec3_Scale(&vNewVel, &pResult->m_vNormal, fRefCoeff);
            Math::Vec3_Sub(&m_vVelocity, &vNewVel, &vVelocity);

            ret = RESULT_COLLIDED;
        };
    };

    //
    //  update rotation
    //
    float fSpeed = Math::Vec3_Length(&m_vVelocity);
	float fDlt = (dt / m_fHitRadius);

	m_fOmega = fSpeed * fDlt;

	m_vTheta.x += m_fOmega;
	m_vTheta.y = 0.0f;
	m_vTheta.z = 0.0f;

	RwV3d vVelocityXZ = { m_vVelocity.x, 0.0f, m_vVelocity.z };
	float fSpeedXZ = Math::Vec3_Length(&vVelocityXZ);

	if (fSpeedXZ >= 0.001f)
	{
		Math::Vec3_Normalize(&vVelocityXZ, &vVelocityXZ);

		m_vTheta.y = (vVelocityXZ.x <= 0.0f ? -Math::ACos(vVelocityXZ.z) : Math::ACos(vVelocityXZ.z));
	}
	else
	{
		m_vTheta = Math::VECTOR3_ZERO;
	};

    //
    //  set new position
    //
    m_vPosition = vNewPos;

    return ret;
};


void CRollingRockGimmickMove::Reset(void)
{
    m_vVelocity = Math::VECTOR3_ZERO;
    SetPosition(&m_vInitPosition);

    m_bReset = false;
};