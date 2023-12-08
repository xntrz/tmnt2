#include "GSMachineGimmickMove.hpp"

#include "Game/System/Map/CameraDataMan.hpp"


CGSMachineGimmickMove::CGSMachineGimmickMove(void)
{
    SetPathTimeRate(0.01f);
};


CGSMachineGimmickMove::~CGSMachineGimmickMove(void)
{
    ;
};


void CGSMachineGimmickMove::UpdatePathTime(float dt)
{
    m_fPathT += (m_fPathTimeRate * dt);
    m_fPathT = Clamp(m_fPathT, 0.0f, 1.0f);
};


void CGSMachineGimmickMove::UpdateRotation(float dt)
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

    if (!Math::Vec3_IsEqual(&vVelocity, &Math::VECTOR3_ZERO))
    {
        if (vVelocity.x <= 0.0f)
            m_vRotation.y = Math::PI - Math::ACos(vVelocity.z);
        else
            m_vRotation.y = Math::PI + Math::ACos(vVelocity.z);

        if (m_fPathTimeRate < 0.0f)
            m_vRotation.y += Math::PI;
    };
};