#include "CuttingGimmickMove.hpp"


CCuttingGimmickMove::CCuttingGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_vRotVelocity(Math::VECTOR3_ZERO)
{
    ;    
};


CCuttingGimmickMove::~CCuttingGimmickMove(void)
{
    ;
};


CCuttingGimmickMove::RESULT CCuttingGimmickMove::OnMove(float dt)
{
    RwV3d vDltPos = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltPos, &m_vVelocity, dt);

    RwV3d vNewPos = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewPos, &m_vPosition, &vDltPos);

    m_vPosition = vNewPos;

    RwV3d vDltRot = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltRot, &m_vRotVelocity, dt);

    RwV3d vNewRot = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewRot, &m_vRotation, &vDltRot);

    m_vRotation = vNewRot;

    return RESULT_NONE;
};