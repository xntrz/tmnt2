#include "RideCamera.hpp"
#include "RideStage.hpp"

#include "Game/System/Map/CameraDataMan.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Map/MapCamera.hpp"


CRideCamera::CRideCamera(void)
: m_fPathT(0.0f)
, m_nEyePathID(-1)
, m_nAtPathID(-1)    
{
    m_szCameraAreaName[0] = '\0';
};


CRideCamera::~CRideCamera(void)
{
    ;
};


void CRideCamera::Update(CMapCamera* pMapCamera)
{
    static char s_szName[32];

    RwV3d vBasisPoint = Math::VECTOR3_ZERO;
    CRideStage::GetBasisPoint(&vBasisPoint);

    CWorldMap::GetCameraAreaName(&vBasisPoint, s_szName);
    if(s_szName[0] == '\0')
        std::strcpy(s_szName, "p0");

    if (std::strcmp(m_szCameraAreaName, s_szName))
    {
        std::strcpy(m_szCameraAreaName, s_szName);

        char szPathEyeName[32];
        szPathEyeName[0] = '\0';
        std::strcpy(szPathEyeName, s_szName);
        std::strcat(szPathEyeName, "_m");
        
        m_nEyePathID = CCameraDataManager::GetPathIDFromName(szPathEyeName);
        ASSERT(m_nEyePathID != -1);

        char szPathAtName[32];
        szPathAtName[0] = '\0';        
        std::strcpy(szPathAtName, s_szName);
        std::strcat(szPathAtName, "_t");

        m_nAtPathID = CCameraDataManager::GetPathIDFromName(szPathAtName);
        ASSERT(m_nAtPathID != -1);

        m_fPathT = 0.0f;
    };

    m_fPathT = CCameraDataManager::FindNearestPosValueLight(&vBasisPoint, m_nAtPathID, m_fPathT);

    RwV3d vAt = Math::VECTOR3_ZERO;
    CCameraDataManager::GetSplinePos(&vAt, m_nAtPathID, m_fPathT);

    RwV3d vEye = Math::VECTOR3_ZERO;
    CCameraDataManager::GetSplinePos(&vEye, m_nEyePathID, m_fPathT);

    pMapCamera->SetLookat(&vEye, &vAt);
};