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