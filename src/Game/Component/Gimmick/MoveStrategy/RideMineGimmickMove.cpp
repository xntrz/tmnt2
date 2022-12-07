#include "RideMineGimmickMove.hpp"


/*static*/ float CRideMineGimmickMove::m_fT = 2.0f;
/*static*/ float CRideMineGimmickMove::m_fA = 2.0f;


CRideMineGimmickMove::CRideMineGimmickMove(int32 iNum)
: CRideGimmickMove()
{
    RwV3d vDir = { 1.0f, 0.0f, 0.0f };
    SetMoveDirection(&vDir);

    if (iNum % 2)
    {
        m_fTimer = m_fT * 0.5f;
    }
    else
    {
        m_fTimer = 0.0f;
    };
};


CRideMineGimmickMove::~CRideMineGimmickMove(void)
{
    ;
};


CRideMineGimmickMove::RESULT CRideMineGimmickMove::OnMove(float dt)
{
    m_fSpeed = Math::Cos((Math::PI2 / m_fT) * m_fTimer) * m_fA * (Math::PI2 / m_fT);

    m_fTimer += dt;

    m_vVelocity.x = m_vMoveDirection.x * (dt * m_fSpeed);
    m_vVelocity.y = m_vMoveDirection.y * (dt * m_fSpeed);
    m_vVelocity.z = m_vMoveDirection.z * (dt * m_fSpeed);

    m_vPosition.x += m_vVelocity.x;
    m_vPosition.y += m_vVelocity.y;
    m_vPosition.z += m_vVelocity.z;

    m_vRotation.y += (dt * Math::PI2);

    return RESULT_NONE;
};