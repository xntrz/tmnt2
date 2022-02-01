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
    RwV3d vFrameVel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vFrameVel, &m_vVelocity, dt);
    Math::Vec3_Add(&m_vPosition, &m_vPosition, &vFrameVel);

    RwV3d vFrameRot = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vFrameRot, &m_vRotVelocity, dt);
    Math::Vec3_Add(&m_vRotation, &m_vRotation, &vFrameRot);

    return RESULT_NONE;
};


void CCuttingGimmickMove::SetVelocity(const RwV3d* pvVelocity)
{
    ASSERT(pvVelocity);
    m_vVelocity = *pvVelocity;
};


void CCuttingGimmickMove::SetRotation(const RwV3d* pvRotation)
{
    ASSERT(pvRotation);
    m_vRotation = *pvRotation;
};


void CCuttingGimmickMove::SetRotVelocity(const RwV3d* pvRotVel)
{
    ASSERT(pvRotVel);
    m_vRotVelocity = *pvRotVel;
};


void CCuttingGimmickMove::GetRotation(RwV3d* pvRotation)
{
    ASSERT(pvRotation);
    *pvRotation = m_vRotation;
};


void CCuttingGimmickMove::GetRotVelocity(RwV3d* pvRotVel)
{
    *pvRotVel = m_vRotVelocity;
};