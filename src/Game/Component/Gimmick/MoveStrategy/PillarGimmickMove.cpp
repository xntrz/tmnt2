#include "PillarGimmickMove.hpp"


CPillarGimmickMove::CPillarGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
{
    ;
};


CPillarGimmickMove::~CPillarGimmickMove(void)
{
    ;
};


CPillarGimmickMove::RESULT CPillarGimmickMove::OnMove(float dt)
{
    RwV3d vFrmVel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vFrmVel, &m_vVelocity, dt);
    Math::Vec3_Add(&m_vPosition, &m_vPosition, &vFrmVel);

    return RESULT_NONE;
};