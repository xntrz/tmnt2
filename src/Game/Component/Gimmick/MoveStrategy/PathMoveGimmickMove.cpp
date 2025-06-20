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


/*virtual*/ CPathMoveGimmickMove::RESULT CPathMoveGimmickMove::OnMove(float dt) /*override*/
{
    GetSplinePos(&m_vPosition, m_fPathT);

    UpdatePathTime(dt);
    UpdateRotation(dt);

    return RESULT_NONE;
};


/*virtual*/ void CPathMoveGimmickMove::SetPathTimeRate(float fTimeRate) /*override*/
{
    m_fPathTimeRate = fTimeRate;
};


/*virtual*/ void CPathMoveGimmickMove::UpdatePathTime(float dt) /*override*/
{
    m_fPathT += (m_fPathTimeRate * dt);
    if (m_fPathT > 1.0f)
        m_fPathT = 1.0f;
};


/*virtual*/ void CPathMoveGimmickMove::UpdateRotation(float dt) /*override*/
{
    if ((m_fPathT > 0.0f) && (m_fPathT < 1.0f))
    {
        RwV3d vNextPosition = Math::VECTOR3_ZERO;
        GetSplinePos(&vNextPosition, m_fPathT);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vVelocity, &vNextPosition, &m_vPosition);
        vVelocity.y = 0.0f;

        Math::Vec3_Normalize(&vVelocity, &vVelocity);
        
        if ((vVelocity.x != 0.0f) ||
            (vVelocity.y != 0.0f) ||
            (vVelocity.z != 0.0f))
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

    GetSplinePos(&m_vPosition, fPathT);
};


float CPathMoveGimmickMove::GetPathLength(const char* pszPathName)
{
    ASSERT(pszPathName);

    int32 iPathID = CCameraDataManager::GetPathIDFromName(pszPathName);
    ASSERT(iPathID >= 0);

    SetPathLength(0.0f);

    const float fStep = (1.0f / 100000.0f);

    float fPathT = 0.0f;
    float fResult = 0.0f;

    RwV3d vPosPost = Math::VECTOR3_ZERO;
    GetSplinePos(&vPosPost, iPathID, 0.0f);

    while (fPathT < 1.0f)
    {
        RwV3d vPosPre = vPosPost;
        GetSplinePos(&vPosPost, iPathID, fPathT);

        RwV3d vPosDlt = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vPosDlt, &vPosPost, &vPosPre);
        
        fResult += Math::Vec3_Length(&vPosDlt);
        fPathT  += fStep;
    };

    RwV3d vPosPre = vPosPost;
    GetSplinePos(&vPosPost, iPathID, 1.0f);

    RwV3d vPosDlt = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vPosDlt, &vPosPost, &vPosPre);

    fResult += Math::Vec3_Length(&vPosDlt);

    return fResult;
};


float CPathMoveGimmickMove::GetPathTByMeterPerSecond(float fMps)
{
    ASSERT(m_pszPathName);

    int32 iPathID = CCameraDataManager::GetPathIDFromName(m_pszPathName);
    ASSERT(iPathID >= 0);
    
    float fDistance = fMps * CGameProperty::GetElapsedTime();
    float fDelta = 0.0f;

	float fDeltaPath = std::fabs((fDistance * 0.04f) / m_fPathLength);
    float fPathT = m_fPathT;

    RwV3d vPosPost = Math::VECTOR3_ZERO;
    CCameraDataManager::GetSplinePos(&vPosPost, iPathID, fPathT);

    if (fDistance <= 0.0f)
    {
        while (true)
        {
            RwV3d vPosPre = vPosPost;
            CCameraDataManager::GetSplinePos(&vPosPost, iPathID, fPathT);

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
            CCameraDataManager::GetSplinePos(&vPosPost, iPathID, fPathT);

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
    
    int32 iPathID = CCameraDataManager::GetPathIDFromName(m_pszPathName);
    ASSERT(iPathID >= 0);

    float fPrePathT = TYPEDEF::FLOAT_MAX;
	float fPathT = m_fPathT;

    while (true)
    {
        float fPathDiff = std::fabs(fPrePathT - fPathT);
        if (fPathDiff <= 0.001f)
            break;

        fPrePathT = fPathT;
        fPathT = CCameraDataManager::FindNearestPosValueLight(pvPosition, iPathID, fPathT);

        if (fPathT >= 1.0f)
            return 1.0f;

        if (fPathT <= 0.0f)
            return 0.0f;
    };
   
	return fPathT;
};


bool
CPathMoveGimmickMove::IsAccelerationToAimPathT(float fAimPathT,
                                               float fAimPathTimerate,
                                               float fAimPathTimerateIntervalStep,
                                               float fAimPathTimerateInterval)
{
    fAimPathTimerateIntervalStep = std::fabs(fAimPathTimerateIntervalStep);
    
    float fDir = (fAimPathTimerate < m_fPathTimeRate ? -1.0f : 1.0f);
    float fDlt = std::fabs(m_fPathTimeRate - fAimPathTimerate);

    int32 nDiv = static_cast<int32>(fDlt / fAimPathTimerateIntervalStep);
    ASSERT(nDiv < 300);

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


void CPathMoveGimmickMove::GetSplinePos(RwV3d* pvPos, float fPathT) const
{
    GetSplinePos(pvPos, m_pszPathName, fPathT);
};


void CPathMoveGimmickMove::GetSplinePos(RwV3d* pvPos, const char* pszPathName, float fPathT) const
{    
    ASSERT(pszPathName != nullptr);

    GetSplinePos(pvPos, CCameraDataManager::GetPathIDFromName(pszPathName), fPathT);
};


void CPathMoveGimmickMove::GetSplinePos(RwV3d* pvPos, int32 iPathID, float fPathT) const
{
    ASSERT(iPathID != -1);
    ASSERT(fPathT >= 0.0f);
    ASSERT(fPathT <= 1.0f);

    CCameraDataManager::GetSplinePos(pvPos, iPathID, fPathT);
};


void CPathMoveGimmickMove::SetPathLength(float fPathLen)
{
    m_fPathLength = fPathLen;
};


void CPathMoveGimmickMove::SetRotation(const RwV3d* pvRot)
{
    m_vRotation = *pvRot;
};


void CPathMoveGimmickMove::GetRotation(RwV3d* pvRot) const
{
    *pvRot = m_vRotation;
};


bool CPathMoveGimmickMove::IsPathEnd(void) const
{
    return (m_fPathT >= 1.0f);
};


float CPathMoveGimmickMove::GetPathT(void) const
{
    return m_fPathT;
};