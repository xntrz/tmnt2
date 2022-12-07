#include "PathGimmickMove.hpp"

#include "Game/System/Map/CameraDataMan.hpp"


CPathGimmickMove::CPathGimmickMove(void)
: m_pszPathName(nullptr)
, m_fPathLength(0.0f)
, m_fPathT(0.0f)
, m_fPathTimeRate(1.0f)
, m_vRotation(Math::VECTOR3_ZERO)
{
    ;
};


CPathGimmickMove::~CPathGimmickMove(void)
{
    ;
};


CPathGimmickMove::RESULT CPathGimmickMove::OnMove(float dt)
{
    ASSERT(m_pszPathName);
    
    int32 PathId = CCameraDataManager::GetPathIDFromName(m_pszPathName);
    ASSERT(PathId >= 0);

    ASSERT((m_fPathT >= 0.0f) && (m_fPathT <= 1.0f));
    CCameraDataManager::GetSplinePos(&m_vPosition, PathId, m_fPathT);

    UpdatePathTime(dt);
    UpdateRotation(dt);

    return RESULT_NONE;
};


void CPathGimmickMove::SetPathTimeRate(float fTimeRate)
{
    m_fPathTimeRate = fTimeRate;
};


void CPathGimmickMove::UpdatePathTime(float dt)
{
    m_fPathT += (m_fPathTimeRate * dt);
    if (m_fPathT > 1.0f)
        m_fPathT = 1.0f;
};


void CPathGimmickMove::UpdateRotation(float dt)
{
    if ((m_fPathT > 0.0f) && (m_fPathT < 1.0f))
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
        
        if (!Math::FEqual(vVelocity.x, 0.0f) ||
            !Math::FEqual(vVelocity.y, 0.0f) ||
            !Math::FEqual(vVelocity.z, 0.0f))
        {
            if (vVelocity.x <= 0.0f)
                m_vRotation.y = -Math::ACos(vVelocity.z);
            else
                m_vRotation.y = Math::ACos(vVelocity.z);
        };
    };
};


void CPathGimmickMove::InitPath(const char* pszPathName, float fPathT)
{
    ASSERT(pszPathName);
    m_pszPathName = pszPathName;

    int32 PathId = CCameraDataManager::GetPathIDFromName(m_pszPathName);
    ASSERT(PathId >= 0);

    ASSERT((fPathT >= 0.0f) && (fPathT <= 1.0f));
    CCameraDataManager::GetSplinePos(&m_vPosition, PathId, m_fPathT);
};


float CPathGimmickMove::GetPathLength(const char* pszPathName)
{
    float fResult = 0.0f;

    ASSERT(pszPathName);

    int32 PathId = CCameraDataManager::GetPathIDFromName(pszPathName);
    ASSERT(PathId >= 0);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    CCameraDataManager::GetSplinePos(&vPosition, PathId, 0.0f);
    
    m_fPathLength = 0.0f;

    const float fStep = (1.0f / 100000.0f);
    float fPathT = 0.0f;
    RwV3d vPostPosition = Math::VECTOR3_ZERO;
    RwV3d vTmp = Math::VECTOR3_ZERO;
    while (fPathT < 1.0f)
    {
        vPostPosition = vPosition;        
        ASSERT((fPathT >= 0.0f) && (fPathT <= 1.0f));        
        CCameraDataManager::GetSplinePos(&vPosition, PathId, fPathT);
        Math::Vec3_Sub(&vTmp, &vPosition, &vPostPosition);        
        fResult += Math::Vec3_Length(&vTmp);
        fPathT += fStep;        
    };

    vPostPosition = vPosition;
    CCameraDataManager::GetSplinePos(&vPosition, PathId, 1.0f);
    Math::Vec3_Sub(&vTmp, &vPosition, &vPostPosition);
    fResult += Math::Vec3_Length(&vTmp);

    return fResult;
};


float CPathGimmickMove::GetPathTByMeterPerSecond(float fMps)
{
    //
    //  TODO
    //
    ASSERT(false);
    return 0.0f;
};


float CPathGimmickMove::GetNearestPathT(const RwV3d* pvPosition)
{
    //
    //  TODO
    //
    ASSERT(false);
    return 0.0f;
};


bool CPathGimmickMove::IsAccelerationToAimPathT(float, float, float, float)
{
    //
    //  TODO
    //
    ASSERT(false);
    return false;
};


void CPathGimmickMove::SetRotation(const RwV3d* pvRot)
{
    m_vRotation = *pvRot;
};


void CPathGimmickMove::SetPathLength(float fPathLen)
{
    m_fPathLength = fPathLen;
};


void CPathGimmickMove::GetRotation(RwV3d* pvRot)
{
    *pvRot = m_vRotation;
};


bool CPathGimmickMove::IsPathEnd(void) const
{
    return (m_fPathT >= 1.0f);
};