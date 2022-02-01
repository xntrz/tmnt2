#include "RideGimmickMove.hpp"


CRideGimmickMove::CRideGimmickMove(void)
: m_vRotation(Math::VECTOR3_ZERO)
, m_vMoveDirection(Math::VECTOR3_ZERO)
, m_fSpeed(0.0f)
, m_vVelocity(Math::VECTOR3_ZERO)
, m_fTimer(0.0f)
{
    ;
};


CRideGimmickMove::~CRideGimmickMove(void)
{
    ;
};


CRideGimmickMove::RESULT CRideGimmickMove::OnMove(float dt)
{
    return RESULT_NONE;
};


void CRideGimmickMove::SetVelocity(const RwV3d* pvVelocity)
{
    ASSERT(pvVelocity);
    m_vVelocity = *pvVelocity;
};


void CRideGimmickMove::SetRotation(const RwV3d* pvRotation)
{
    ASSERT(pvRotation);
    m_vRotation = *pvRotation;
};


void CRideGimmickMove::SetMoveDirection(const RwV3d* pvMoveDirection)
{
    ASSERT(pvMoveDirection);
    m_vMoveDirection = *pvMoveDirection;
};


void CRideGimmickMove::SetSpeed(float fSpeed)
{
    m_fSpeed = fSpeed;
};


void CRideGimmickMove::GetRotation(RwV3d* pvRotation) const
{
    ASSERT(pvRotation);
    *pvRotation = m_vRotation;
};