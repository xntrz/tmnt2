#include "PathMoveGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/CameraDataMan.hpp"


CPathMoveGimmickMove::CPathMoveGimmickMove(void)
: m_pszPathName(nullptr)
, m_fPathLength(0.0f)
, m_fPathT(0.0f)
, m_fPathTimeRate(1.0f)
, m_vRotation(Math::VECTOR3_ZERO)
{
    ;
};


CPathMoveGimmickMove::~CPathMoveGimmickMove(void)
{
    ;
};


CPathMoveGimmickMove::RESULT CPathMoveGimmickMove::OnMove(float dt)
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


void CPathMoveGimmickMove::SetPathTimeRate(float fTimeRate)
{
    m_fPathTimeRate = fTimeRate;
};


void CPathMoveGimmickMove::UpdatePathTime(float dt)
{
    m_fPathT += (m_fPathTimeRate * dt);
    if (m_fPathT > 1.0f)
        m_fPathT = 1.0f;
};


void CPathMoveGimmickMove::UpdateRotation(float dt)
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


void CPathMoveGimmickMove::InitPath(const char* pszPathName, float fPathT)
{
    ASSERT(pszPathName);
    m_pszPathName = pszPathName;

    int32 PathId = CCameraDataManager::GetPathIDFromName(m_pszPathName);
    ASSERT(PathId >= 0);

    ASSERT((fPathT >= 0.0f) && (fPathT <= 1.0f));
    CCameraDataManager::GetSplinePos(&m_vPosition, PathId, fPathT);
};


float CPathMoveGimmickMove::GetPathLength(const char* pszPathName)
{
    float fResult = 0.0f;

    ASSERT(pszPathName);

    int32 PathId = CCameraDataManager::GetPathIDFromName(pszPathName);
    ASSERT(PathId >= 0);

    RwV3d vPosPost = Math::VECTOR3_ZERO;
    CCameraDataManager::GetSplinePos(&vPosPost, PathId, 0.0f);

    SetPathLength(0.0f);

    const float fStep = (1.0f / 100000.0f);
    float fPathT = 0.0f;
    
    while (fPathT <= 1.0f)
    {
        RwV3d vPosPre = vPosPost;
        CCameraDataManager::GetSplinePos(&vPosPost, PathId, fPathT);

        RwV3d vPosDlt = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vPosDlt, &vPosPost, &vPosPre);
        
        fResult += Math::Vec3_Length(&vPosDlt);
        fPathT += fStep;
    };

    return fResult;
};


float CPathMoveGimmickMove::GetPathTByMeterPerSecond(float fMps)
{
    ASSERT(m_pszPathName);

    int32 PathId = CCameraDataManager::GetPathIDFromName(m_pszPathName);
    ASSERT(PathId >= 0);

    float fDistance = fMps * CGameProperty::GetElapsedTime();
    float fDelta = 0.0f;

    float fDeltaPath = CGameProperty::GetElapsedTime() * (fMps / m_fPathLength) * 0.04f;
    float fPathT = m_fPathT;

    RwV3d vPosPost = Math::VECTOR3_ZERO;
    CCameraDataManager::GetSplinePos(&vPosPost, PathId, fPathT);

    if (fDistance <= 0.0f)
    {
        while (true)
        {
            RwV3d vPosPre = vPosPost;
            CCameraDataManager::GetSplinePos(&vPosPost, PathId, fPathT);

            RwV3d vPosDlt = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vPosDlt, &vPosPost, &vPosPre);

            fPathT -= fDeltaPath;
            fDelta += std::fabs(Math::Vec3_Length(&vPosDlt));

            if (fPathT <= 0.0f)
                break;

            if (fDelta >= std::fabs(fDistance))
                break;
        };
    }
    else
    {
        while (true)
        {
            RwV3d vPosPre = vPosPost;
            CCameraDataManager::GetSplinePos(&vPosPost, PathId, fPathT);

            RwV3d vPosDlt = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vPosDlt, &vPosPost, &vPosPre);

            fPathT += fDeltaPath;
            fDelta += std::fabs(Math::Vec3_Length(&vPosDlt));

            if (fPathT >= 1.0f)
                break;

            if (fDelta >= fDistance)
                break;
        };
    };

    return Clamp(fPathT, 0.0f, 1.0f);
};


float CPathMoveGimmickMove::GetNearestPathT(RwV3d* pvPosition)
{
    ASSERT(m_pszPathName);
    
    int32 PathId = CCameraDataManager::GetPathIDFromName(m_pszPathName);
    ASSERT(PathId >= 0);

    float fPrePathT = TYPEDEF::FLOAT_MAX;
	float fPathT = m_fPathT;

    while (true)
    {
        float fPathDiff = std::fabs(fPrePathT - fPathT);
        if (fPathDiff <= 0.001f)
            break;

        fPrePathT = fPathT;
        fPathT = CCameraDataManager::FindNearestPosValueLight(pvPosition, PathId, fPathT);

        if (fPathT >= 1.0f)
            return 1.0f;

        if (fPathT <= 0.0f)
            return 0.0f;
    };
   
	return fPathT;
};


bool
CPathMoveGimmickMove::IsAccelerationToAimPathT(
    float fAimPathT,
    float fAimPathTimerate,
    float fAimPathTimerateIntervalStep,
    float fAimPathTimerateInterval
)
{
    fAimPathTimerateIntervalStep = std::fabs(fAimPathTimerateIntervalStep);
    
    float fDir = (fAimPathTimerate < m_fPathTimeRate ? -1.0f : 1.0f);
    float fDlt = std::fabs(m_fPathTimeRate - fAimPathTimerate);
    int32 nDiv = int32(fDlt / fAimPathTimerateIntervalStep);
    if (nDiv >= 300)
        nDiv = 300;
    
    float fPathT = m_fPathT;
    float fPathTimerate = m_fPathTimeRate;
    
    for (int32 i = 0; i < nDiv; ++i)
    {
        fPathT += (fAimPathTimerateInterval * fPathTimerate);
        fPathTimerate += (fAimPathTimerateIntervalStep * fDir);
    };

    return (fPathT > fAimPathT);
};