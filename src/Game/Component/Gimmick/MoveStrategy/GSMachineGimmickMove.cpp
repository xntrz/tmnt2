#include "GSMachineGimmickMove.hpp"

#include "Game/System/Map/CameraDataMan.hpp"


CGSMachineGimmickMove::CGSMachineGimmickMove(void)
{
    SetPathTimeRate(0.01f);
};


/*virtual*/ CGSMachineGimmickMove::~CGSMachineGimmickMove(void)
{
    ;
};


/*virtual*/ void CGSMachineGimmickMove::UpdatePathTime(float dt) /*override*/
{
    m_fPathT += (m_fPathTimeRate * dt);
    m_fPathT = Clamp(m_fPathT, 0.0f, 1.0f);
};


/*virtual*/ void CGSMachineGimmickMove::UpdateRotation(float dt) /*override*/
{
    if ((m_fPathT > 0.0f) && (m_fPathT < 1.0f))
    {
        ASSERT(m_pszPathName);

        int32 iPathID = CCameraDataManager::GetPathIDFromName(m_pszPathName);
        ASSERT(iPathID >= 0);

        ASSERT((m_fPathT >= 0.0f) && (m_fPathT <= 1.0f));
        RwV3d vNextPosition = Math::VECTOR3_ZERO;
        CCameraDataManager::GetSplinePos(&vNextPosition, iPathID, m_fPathT);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vVelocity, &vNextPosition, &m_vPosition);
        vVelocity.y = 0.0f;
        Math::Vec3_Normalize(&vVelocity, &vVelocity);

        if ((vVelocity.x != 0.0f) ||
            (vVelocity.y != 0.0f) ||
            (vVelocity.z != 0.0f))
        {
            if (vVelocity.x <= 0.0f)
                m_vRotation.y = MATH_PI - Math::ACos(vVelocity.z);
            else
                m_vRotation.y = MATH_PI + Math::ACos(vVelocity.z);

            if (m_fPathTimeRate < 0.0f)
                m_vRotation.y += MATH_PI;
        };
    };    
};