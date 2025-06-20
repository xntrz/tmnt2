#include "ApproachingTrailerGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"


CApproachingTrailerGimmickMove::CApproachingTrailerGimmickMove(void)
: m_fTimerCounter(0.0f)
, m_vPrePos(Math::VECTOR3_ZERO)
{
    SetPathTimeRate(0.05f);
};


/*virtual*/ void CApproachingTrailerGimmickMove::UpdateRotation(float dt) /*override*/
{
    if (m_fTimerCounter < 0.025f)
    {
        m_fTimerCounter += dt;
        m_vPrePos = m_vPosition;
    }
    else
    {
        CPathMoveGimmickMove::UpdateRotation(dt);
        m_fTimerCounter = 0.0f;
    };
};


//
// *********************************************************************************
//


CApproachingTrailerHeadGimmickMove::CApproachingTrailerHeadGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_fTimerCounter(5.0f)
, m_bEnd(false)
{
    ;
};


/*virtual*/ CApproachingTrailerHeadGimmickMove::RESULT
CApproachingTrailerHeadGimmickMove::OnMove(float dt) /*override*/
{
    Math::Vec3_Add(&m_vPosition, &m_vPosition, &m_vVelocity);

    m_vVelocity.z += (m_fTimerCounter >= 4.0f ? (dt * 0.5f) : -(dt * 0.1f));

    if (m_fTimerCounter <= 0.0f)
        m_bEnd = true;
    else
        m_fTimerCounter -= dt;

    return RESULT_NONE;
};


void CApproachingTrailerHeadGimmickMove::GetRotation(RwV3d* pvRot) const
 { 
    *pvRot = m_vRotation; 
};


void CApproachingTrailerHeadGimmickMove::SetRotation(const RwV3d* pvRot)
{
    m_vRotation = *pvRot;
};

