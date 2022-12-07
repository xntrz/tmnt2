#include "MadCarGimmickMove.hpp"

#include "Game/System/Map/CameraDataMan.hpp"


CMadCarGimmickMove::CMadCarGimmickMove(void)
: m_bCarDirection(false)
{
    SetPathTimeRate(0.1f);
};


CMadCarGimmickMove::~CMadCarGimmickMove(void)
{
    ;
};


void CMadCarGimmickMove::UpdatePathTime(float dt)
{
    m_fPathT += (m_fPathTimeRate * dt);
    if (m_fPathT <= 1.0f)
    {
        if (m_fPathT < 0.0f)
            m_fPathT = 1.0f;
    }
    else
    {
        m_fPathT = 1.0f;
    };
};


void CMadCarGimmickMove::UpdateRotation(float dt)
{
    ASSERT(m_pszPathName);

    int32 PathId = CCameraDataManager::GetPathIDFromName(m_pszPathName);
    ASSERT(PathId >= 0);

    ASSERT((m_fPathT >= 0.0f) && (m_fPathT <= 1.0f));
    RwV3d vNextPosition = Math::VECTOR3_ZERO;
    CCameraDataManager::GetSplinePos(&vNextPosition, PathId, m_fPathT);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vVelocity, &vNextPosition, &m_vPosition);
    vVelocity.y = 0.0f;
    Math::Vec3_Normalize(&vVelocity, &vVelocity);

    if (m_bCarDirection)
        m_vRotation.y = Math::ACos(vVelocity.x) + Math::PI05;
    else
        m_vRotation.y = Math::PI05 - Math::ACos(vVelocity.x);
};


void CMadCarGimmickMove::SetCarDirection(bool State)
{
    m_bCarDirection = State;
};


bool CMadCarGimmickMove::IsCarDirection(void) const
{
    return m_bCarDirection;
};