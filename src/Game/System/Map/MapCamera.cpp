#include "MapCamera.hpp"
#include "WorldMap.hpp"
#include "CameraDataMan.hpp"

#include "Game/Component/Effect/WaterSplash.hpp"
#include "Game/Component/Effect/Snow.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Controller.hpp"


class CMapCamera::CIntroduction
{
private:
    static const int32 DEMONUM = 3;
    static const float DEMODURATION;

    struct DEMOCAMERAINFO
    {
        float m_fPathTime;
        float m_fTimer;
        int32 m_nPathEyeID;
        int32 m_nPathLookatID;
    };
    
public:
    CIntroduction(void);
    ~CIntroduction(void);
    void Set(void);
    void Skip(void);
    bool Update(RwV3d* pvEye, RwV3d* pvAt);

private:
    int32 m_nNumDemo;
    DEMOCAMERAINFO m_aDemoCameraInfo[DEMONUM];
    int32 m_nDemoID;
    bool m_bEnd;
};


class CMapCamera::CVibration
{
public:
    CVibration(void);
    ~CVibration(void);
    void Set(float fPower, float fTime, uint32 uFreq);
    void Period(void);
    void Update(RwV3d* pvEye, RwV3d* pvAt);
    bool IsVibrating(void) const;

private:
    float m_fVibTime;
    float m_fVibPower;
    float m_fTimer;
    float m_fTheta;
    uint32 m_uFreq;
    RwV3d m_vVibration;
};


/*static*/ const float CMapCamera::CIntroduction::DEMODURATION = 2.0f;


CMapCamera::CIntroduction::CIntroduction(void)
: m_nNumDemo(0)
, m_nDemoID(0)
, m_bEnd(false)
{

};


CMapCamera::CIntroduction::~CIntroduction(void)
{
    ;
};


void CMapCamera::CIntroduction::Set(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aDemoCameraInfo); ++i)
    {
        char szPathName[16];

        szPathName[0] = '\0';
        std::sprintf(szPathName, "d%d", i);
        
        int32 nPathEyeID = CCameraDataManager::GetPathIDFromName(szPathName);
        if (nPathEyeID < 0)
            break;

        szPathName[0] = '\0';
        std::sprintf(szPathName, "d%d_t", i);
        
        int32 nPathLookatID = CCameraDataManager::GetPathIDFromName(szPathName);
        if (nPathLookatID < 0)
            break;

        m_aDemoCameraInfo[i].m_nPathEyeID = nPathEyeID;
        m_aDemoCameraInfo[i].m_nPathLookatID = nPathLookatID;
        m_aDemoCameraInfo[i].m_fTimer = 0.0f;
        m_aDemoCameraInfo[i].m_fPathTime = 0.0f;

        ++m_nNumDemo;
    };

    if (!m_nNumDemo)
        m_bEnd = true;
};


void CMapCamera::CIntroduction::Skip(void)
{
    if (m_nNumDemo)
    {
        const float fDemoDurationTotal = (DEMONUM * DEMODURATION);
        
        m_nDemoID = m_nNumDemo - 1;
        m_aDemoCameraInfo[m_nNumDemo - 1].m_fPathTime = 1.0f;
        m_aDemoCameraInfo[m_nNumDemo - 1].m_fTimer = fDemoDurationTotal / m_nNumDemo;
    };
};


bool CMapCamera::CIntroduction::Update(RwV3d* pvEye, RwV3d* pvAt)
{
    if (m_bEnd)
        return false;

    ASSERT(m_nDemoID < m_nNumDemo);
    
    DEMOCAMERAINFO* pCameraInfo = &m_aDemoCameraInfo[m_nDemoID];

    float fDemoDurationTotal = (DEMONUM * DEMODURATION);
    float fSplineT = pCameraInfo->m_fTimer / (fDemoDurationTotal / float(m_nNumDemo));

    CCameraDataManager::GetSplinePos(pvEye, pCameraInfo->m_nPathEyeID, fSplineT);
    CCameraDataManager::GetSplinePos(pvAt, pCameraInfo->m_nPathLookatID, fSplineT);

    pCameraInfo->m_fTimer += CGameProperty::GetElapsedTime();

    if (pCameraInfo->m_fTimer > (fDemoDurationTotal / float(m_nNumDemo)))
    {
        ++m_nDemoID;

        if (m_nDemoID > (m_nNumDemo - 1))
        {
            m_bEnd = true;
        };
    };

    return true;
};


CMapCamera::CVibration::CVibration(void)
: m_fVibTime(0.0f)
, m_fVibPower(0.0f)
, m_fTimer(0.0f)
, m_fTheta(0.0f)
, m_uFreq(0)
, m_vVibration(Math::VECTOR3_ZERO)
{
    ;
};


CMapCamera::CVibration::~CVibration(void)
{
    ;
};


void CMapCamera::CVibration::Set(float fPower, float fTime, uint32 uFreq)
{
    m_fTimer = fTime;
    m_fVibTime = fTime;
    m_fVibPower = fPower;
    m_uFreq = uFreq;
};


void CMapCamera::CVibration::Period(void)
{
    if (IsVibrating())
    {
        float fDltRadian = Math::Sin(m_fTheta);

        m_vVibration = Math::VECTOR3_ZERO;
        m_vVibration.y = (fDltRadian * m_fVibPower);

        m_fTheta += (CGameProperty::GetElapsedTime() * float(m_uFreq) * Math::PI2);
        m_fTimer -= CGameProperty::GetElapsedTime();
    };
};


void CMapCamera::CVibration::Update(RwV3d* pvEye, RwV3d* pvAt)
{
    ASSERT(pvEye);
    ASSERT(pvAt);
    
    if (IsVibrating())
    {
        Math::Vec3_Add(pvEye, pvEye, &m_vVibration);
        Math::Vec3_Add(pvAt, pvAt, &m_vVibration);
    };
};


bool CMapCamera::CVibration::IsVibrating(void) const
{
    return (m_fTimer > 0.0f);
};


/*static*/ float CMapCamera::m_fDefaultViewSize = 0.5f;
/*static*/ float CMapCamera::m_fDefaultClipFar = 800.0f;
/*static*/ float CMapCamera::m_fDefaultClipNear = 0.1f;
/*static*/ float CMapCamera::m_fDefaultFogDist = 5.0f;
/*static*/ RwV3d CMapCamera::m_vDefaultLookat = { 0.0f, 0.0f, 0.0f };
/*static*/ RwV3d CMapCamera::m_vDefaultEye = { 0.0f, 5.0f, -8.0f };


CMapCamera::CMapCamera(void)
: m_pCamera(nullptr)
, m_vEye(Math::VECTOR3_ZERO)
, m_vAt(Math::VECTOR3_ZERO)
, m_fDist(0.0f)
, m_fViewSize(m_fDefaultViewSize)
, m_fRotY(0.0f)
, m_fHeight(0.0f)
, m_fLookatOffsetY(0.0f)
, m_fPathTime(0.0f)
, m_fPrePathTime(0.0f)
, m_fLookatViewAreaRadius(0.0f)
, m_fChangeTimer(0.0f)
, m_fRequestZoom(0.0f)
, m_mode(MODE_AUTOCHANGE)
, m_pathmode(PATHMODE_SINGLEPLAYER)
, m_bChangeMoment(false)
, m_pIntroduction(nullptr)
, m_pVibration(nullptr)
, m_fPitch(0.0f)
{
    m_pCamera = CCamera::GetCamera();
    ASSERT(m_pCamera);

    RwCamera* pCamera = m_pCamera->GetRwCamera();
    RwFrame* pFrame = RwCameraGetFrameMacro(pCamera);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

    const CWorldMap::MAPINFO* pMapInfo = CWorldMap::GetMapInfo();
    if (pMapInfo)
    {
        SetFarClipPlane(pMapInfo->m_fFarClipDist);
        SetNearClipPlane(pMapInfo->m_fNearClipDist);
        SetFogDistance(pMapInfo->m_foginfo.m_fDist);
    }
    else
    {
        SetFarClipPlane(m_fDefaultClipFar);
        SetNearClipPlane(m_fDefaultClipNear);
        SetFogDistance(m_fDefaultFogDist);
    };

    SetViewWindow(m_fViewSize, TYPEDEF::DEFAULT_ASPECTRATIO);
    SetLookat(&m_vDefaultEye, &m_vDefaultLookat);
    m_fRequestZoom = 1.0f;
    m_szCameraAreaName[0] = '\0';

    IntroductionCreate();
};


CMapCamera::~CMapCamera(void)
{
    if (m_pIntroduction)
    {
        delete m_pIntroduction;
        m_pIntroduction = nullptr;
    };

    if (m_pVibration)
    {
        delete m_pVibration;
        m_pVibration = nullptr;
    };

    ASSERT(m_pCamera);
    m_pCamera->Release();
    m_pCamera = nullptr;
};


bool CMapCamera::BeginScene(void)
{
    ASSERT(m_pCamera);

    return m_pCamera->BeginScene();
};


void CMapCamera::EndScene(void)
{
    ASSERT(m_pCamera);
    
    m_pCamera->EndScene();
};


void CMapCamera::WorldAddCamera(RpWorld* pWorld)
{
    ASSERT(pWorld);
    ASSERT(m_pCamera);

    if (pWorld && m_pCamera)
    {
        RpWorldAddCamera(pWorld, m_pCamera->GetRwCamera());
    };
};


void CMapCamera::WorldRemoveCamera(RpWorld* pWorld)
{
    ASSERT(pWorld);
    ASSERT(m_pCamera);

    if (pWorld && m_pCamera)
    {
        RpWorldRemoveCamera(pWorld, m_pCamera->GetRwCamera());
    };
};


void CMapCamera::Update(const RwV3d* pvAt)
{
    Update(pvAt, 1.0f);
};


void CMapCamera::Update(const RwV3d* pvAt, float fZoom)
{
    VibrationUpdate();

    if (!CGameData::Attribute().IsPlayDemoMode())
    {
        uint32 uDigitalTriggerUnlocked = CController::GetDigitalTrigger(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);
        uint32 uDigitalTriggerLocked = CController::GetDigitalTrigger(CController::CONTROLLER_LOCKED_ON_VIRTUAL);

        if (CGamepad::CheckFunction(uDigitalTriggerUnlocked, CGamepad::FUNCTION_SWITCH_CAM)
            || CGamepad::CheckFunction(uDigitalTriggerLocked, CGamepad::FUNCTION_SWITCH_CAM))
        {
            int32 pathmode = m_pathmode;
            ++pathmode;
            pathmode %= PATHMODEMAX;
            m_pathmode = PATHMODE(pathmode);
        };
    };

    m_fRequestZoom = fZoom;

    switch (m_mode)
    {
    case MODE_MANUAL:
        {
            RwFrame* pFrame = RwCameraGetFrameMacro(m_pCamera->GetRwCamera());
            RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

            m_vEye = pMatrix->pos;
            m_vAt = pMatrix->at;
        }
        UpdateManualCamera(pvAt);
        break;

    case MODE_AUTOCHANGE:
        UpdateAutoChangeCamera(pvAt);
        break;
        
    case MODE_INTRODUCTION:
        IntroductionUpdate();
        break;

    default:
        ASSERT(false);
        break;
    };

    PostUpdate();

    const CWorldMap::MAPINFO* pMapInfo = CWorldMap::GetMapInfo();
    if (pMapInfo)
    {
        switch (pMapInfo->m_weather)
        {
        case CWorldMap::WEATHER_RAIN:
            CWaterSplashManager::SetBasisPosition(&m_vAt);
            break;
            
        case CWorldMap::WEATHER_SNOW:
            CSnowManager::SetCameraPosition(this);        
            break;
        };
    };
};


void CMapCamera::PostUpdate(void)
{
    RwV3d vec = Math::VECTOR3_ZERO;

    Math::Vec3_Sub(&vec, &m_vAt, &m_vEye);

    float fLen = Math::Vec3_Length(&vec);
    fLen = Math::FAbs(fLen);

    m_fLookatViewAreaRadius = fLen * m_fViewSize;

    if (!std::strcmp(m_szCameraAreaName, "f"))
        m_fLookatViewAreaRadius = 1000.0f;

    m_bChangeMoment = false;
};


void CMapCamera::UpdateMoment(const RwV3d* pvAt)
{
    m_bChangeMoment = true;
    Update(pvAt, 1.0f);
};


void CMapCamera::UpdateAutoChangeCamera(const RwV3d* pvAt)
{
    RwV3d vCheckPoint = *pvAt;
    char szName[32];

    szName[0] = '\0';

    if (CWorldMap::GetCameraAreaName(&vCheckPoint, szName))
    {
        if (m_bChangeMoment || (m_fChangeTimer <= 0.0f && std::strcmp(m_szCameraAreaName, szName)))
        {
			std::strcpy(m_szCameraAreaName, szName);
			ResetCameraForCameraAreaChange(pvAt);
        };

        if (!std::strncmp(m_szCameraAreaName, "p", 1) ||
            !std::strncmp(m_szCameraAreaName, "P", 1))
        {
            UpdatePathCamera(pvAt, false);
        }
        else if (!std::strncmp(m_szCameraAreaName, "e", 1) ||
                !std::strncmp(m_szCameraAreaName, "E", 1))
        {
            UpdatePathCamera(pvAt, true);
        }
        else if (!std::strncmp(m_szCameraAreaName, "t", 1) ||
                !std::strncmp(m_szCameraAreaName, "T", 1))
        {
            UpdateSetCamera(pvAt);
        }
        else if (!std::strncmp(m_szCameraAreaName, "f", 1) ||
                !std::strncmp(m_szCameraAreaName, "F", 1))
        {
            UpdateFixedCamera(pvAt);
        };
    };
};


void CMapCamera::ResetCameraForCameraAreaChange(const RwV3d* pvAt)
{
    if (!std::strncmp(m_szCameraAreaName, "p", 1) ||
        !std::strncmp(m_szCameraAreaName, "P", 1) ||
        !std::strncmp(m_szCameraAreaName, "e", 1) ||
        !std::strncmp(m_szCameraAreaName, "E", 1))
    {
        char szAtCameraName[32];

        szAtCameraName[0] = '\0';
        MakePathAtCameraName(szAtCameraName, m_szCameraAreaName);
        
        int32 nPathID = CCameraDataManager::GetPathIDFromName(szAtCameraName);
        if (nPathID >= 0)
        {
            RwV3d vec = *pvAt;
            float fValue = CCameraDataManager::FindNearestPosValue(&vec, nPathID, 100);
            
            m_fPathTime = fValue;
            m_fPrePathTime = fValue;
            m_fChangeTimer = 1.5f;
        };
    };
};


void CMapCamera::MakePathAtCameraName(char* pszPathAtName, const char* pszCameraAreaName)
{
    std::strcpy(pszPathAtName, pszCameraAreaName);
    std::strcat(pszPathAtName, "_t");
};


void CMapCamera::MakePathEyeCameraName(char* pszPathEyeName, const char* pszCameraAreaName)
{
    std::strcpy(pszPathEyeName, pszCameraAreaName);

    switch (m_pathmode)
    {
    case PATHMODE_SINGLEPLAYER:
        {
            std::strcat(pszPathEyeName, "_s");
            
            int32 nPathID = CCameraDataManager::GetPathIDFromName(pszPathEyeName);
            if (nPathID < 0)
            {
                std::strcpy(pszPathEyeName, pszCameraAreaName);
                std::strcat(pszPathEyeName, "_m");
            };
        }
        break;

    case PATHMODE_MULTIPLAYER:
        {
            std::strcat(pszPathEyeName, "_m");
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CMapCamera::MakeSetCameraName(char* pszSetCameraName, const char* pszCameraAreaName)
{
    bool bFound = true;

    std::strcpy(pszSetCameraName, pszCameraAreaName);

    switch (m_pathmode)
    {
    case PATHMODE_SINGLEPLAYER:
        {
            std::strcat(pszSetCameraName, "_s");

            int32 nPathID = CCameraDataManager::GetSetCamIDFromName(pszSetCameraName);
            if (nPathID < 0)
                bFound = false;
        }
        break;

    case PATHMODE_MULTIPLAYER:
        {
            std::strcat(pszSetCameraName, "_m");
            
            int32 nPathID = CCameraDataManager::GetSetCamIDFromName(pszSetCameraName);
            if (nPathID < 0)
                bFound = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (!bFound)
    {
        std::strcpy(pszSetCameraName, pszCameraAreaName);
    };
};


void CMapCamera::UpdateManualCamera(const RwV3d* pvAt)
{
    float fSpeed = 25.0f * CGameProperty::GetElapsedTime();
    int16 nAnalogRX = CController::GetAnalog(0, CController::ANALOG_LSTICK_X);
    int16 nAnalogRY = CController::GetAnalog(0, CController::ANALOG_LSTICK_Y);

    float rx = 0.0f;
    float ry = 0.0f;

    if (nAnalogRX >= 0)
        rx = float(nAnalogRX / TYPEDEF::SINT16_MAX);
    else
        rx = -float(nAnalogRX / TYPEDEF::SINT16_MIN);

    if (nAnalogRY >= 0)
        ry = float(nAnalogRY / TYPEDEF::SINT16_MAX);
    else
        ry = -float(nAnalogRY / TYPEDEF::SINT16_MIN);

    if (m_fPitch + ry >= 89.0f || m_fPitch + ry <= -89.0f)
        ry = 0.0f;
    
    m_fPitch += ry;

    RwFrame* pFrame = RwCameraGetFrame(m_pCamera->GetRwCamera());    
    RwV3d vDelta = Math::VECTOR3_ZERO;
    RwV3d vPos = *RwMatrixGetPos(RwFrameGetMatrix(pFrame));    
    RwV3dScale(&vDelta, &vPos, -1.0f);
    RwFrameTranslate(pFrame, &vDelta, rwCOMBINEPOSTCONCAT);
    
    {
        RwV3d vRight = *RwMatrixGetRight(RwFrameGetMatrix(pFrame));
        RwFrameRotate(pFrame, &vRight, -ry * 2.0f, rwCOMBINEPOSTCONCAT);
        RwFrameRotate(pFrame, &Math::VECTOR3_AXIS_Y, -rx * 2.0f, rwCOMBINEPOSTCONCAT);
    }
    
    RwFrameTranslate(pFrame, &vPos, rwCOMBINEPOSTCONCAT);

    int32 iController = CGameData::Attribute().GetVirtualPad();
    
    if (CController::GetDigital(iController, CController::DIGITAL_UP))
    {
        RwV3d vAt = *RwMatrixGetAt(RwFrameGetMatrix(pFrame));
        RwV3dScale(&vAt, &vAt, fSpeed);
        RwFrameTranslate(pFrame, &vAt, rwCOMBINEPOSTCONCAT);
    }
    else if (CController::GetDigital(iController, CController::DIGITAL_DOWN))
    {
        RwV3d vAt = *RwMatrixGetAt(RwFrameGetMatrix(pFrame));
        RwV3dScale(&vAt, &vAt, -fSpeed);
        RwFrameTranslate(pFrame, &vAt, rwCOMBINEPOSTCONCAT);
    };
    
    if (CController::GetDigital(iController, CController::DIGITAL_RIGHT))
    {
        RwV3d vRight = *RwMatrixGetRight(RwFrameGetMatrix(pFrame));
        RwV3dScale(&vRight, &vRight, -fSpeed);
        RwFrameTranslate(pFrame, &vRight, rwCOMBINEPOSTCONCAT);
    }
    else if (CController::GetDigital(iController, CController::DIGITAL_LEFT))
    {
        RwV3d vRight = *RwMatrixGetRight(RwFrameGetMatrix(pFrame));
        RwV3dScale(&vRight, &vRight, fSpeed);
        RwFrameTranslate(pFrame, &vRight, rwCOMBINEPOSTCONCAT);
    };
};


void CMapCamera::UpdatePathCamera(const RwV3d* pvAt, bool bExPath)
{
    RwV3d vEye = Math::VECTOR3_ZERO;
    RwV3d vAt = Math::VECTOR3_ZERO;
    RwV3d vPathLookatPos = Math::VECTOR3_ZERO;
    RwV3d vFrontEyePos = Math::VECTOR3_ZERO;
    RwV3d vLineV = Math::VECTOR3_ZERO;
    RwV3d vNewEye = Math::VECTOR3_ZERO;
    int32 nAtPathID = -1;
    int32 nEyePathID = -1;
    float fFOV = 0.0f;
    char szPathName[32];

    szPathName[0] = '\0';

    vAt = *pvAt;
    vAt.y += m_fLookatOffsetY;

    if (m_bChangeMoment)
    {
        m_vAt = vAt;
    }
    else
    {
        Math::Vec3_Lerp(&m_vAt, &m_vAt, &vAt, 0.2f);
    };

    if (std::strcmp(m_szCameraAreaName, ""))
    {
        MakePathEyeCameraName(szPathName, m_szCameraAreaName);
        nEyePathID = CCameraDataManager::GetPathIDFromName(szPathName);

        MakePathAtCameraName(szPathName, m_szCameraAreaName);
        nAtPathID = CCameraDataManager::GetPathIDFromName(szPathName);
    };

    if (nEyePathID >= 0 && nAtPathID >= 0)
    {
        m_fPathTime = CCameraDataManager::FindNearestPosValueLight(&m_vAt, nAtPathID, m_fPathTime);

        if (CCameraDataManager::GetPathType(nAtPathID) == 1)
        {
            if (m_fPathTime > m_fPrePathTime)
            {
                if (m_fPathTime <= 0.00001f)
                    m_fPathTime = 0.99f;
            }
            else if (m_fPrePathTime > m_fPrePathTime && m_fPrePathTime > 0.99f)
            {
                m_fPathTime = 0.00001f;
            };
        };
            
        CCameraDataManager::GetSplinePos(&vEye, nEyePathID, m_fPathTime);

        if (bExPath)
        {
            float fFrontPathT = m_fPathTime + 0.01f;
            if (fFrontPathT > 1.0f)
                fFrontPathT = m_fPathTime - 0.01f;

            CCameraDataManager::GetSplinePos(&vFrontEyePos, nEyePathID, fFrontPathT);

            Math::Vec3_Sub(&vLineV, &vFrontEyePos, &vEye);
            vLineV.y = 0.0f;
            Math::Vec3_Normalize(&vLineV, &vLineV);

            float fDist = (((pvAt->x - vEye.x) * -vLineV.z) + ((pvAt->z - vEye.z) * vLineV.x)) /
                (-vLineV.z * -vLineV.z + vLineV.y * vLineV.y + vLineV.x * vLineV.x);

            vEye.x += (-vLineV.z * fDist);
            vEye.z += (vLineV.x * fDist);
        };

        CCameraDataManager::GetSplinePos(&vPathLookatPos, nAtPathID, m_fPathTime);

        float fTmp = vPathLookatPos.y - 5.0f;
        if (fTmp > m_vAt.y)
            m_vAt.y = fTmp;

        m_fPrePathTime = m_fPathTime;

        RwV3d vNewEye = Math::VECTOR3_ZERO;
        vNewEye.x = m_vAt.x + ((vEye.x - m_vAt.x) * m_fRequestZoom);
        vNewEye.y = m_vAt.y + ((vEye.y - m_vAt.y) * m_fRequestZoom);
        vNewEye.z = m_vAt.z + ((vEye.z - m_vAt.z) * m_fRequestZoom);

        if (m_fRequestZoom > 1.0f && CWorldMap::CheckCollisionLine(&vEye, &vNewEye))
            vNewEye = *CWorldMap::GetCollisionResultClosestPoint();

        vEye = vNewEye;

        m_fChangeTimer -= CGameProperty::GetElapsedTime();
        if (m_fChangeTimer <= 0.0f)
            Math::Vec3_Lerp(&m_vEye, &m_vEye, &vEye, 0.2f);
        else
            Math::Vec3_Lerp(&m_vEye, &m_vEye, &vEye, 0.1f);

        if (m_bChangeMoment)
            m_vEye = vEye;

        UpdateLookat();
        m_fViewSize += (TYPEDEF::DEFAULT_VIEWWINDOW - m_fViewSize) * 0.1f;
        SetViewWindow(m_fViewSize);
    };
};


void CMapCamera::UpdateSetCamera(const RwV3d* pvAt)
{
    RwV3d vEye = Math::VECTOR3_ZERO;
    RwV3d vAt = Math::VECTOR3_ZERO;
    int32 nCamID = -1;
    float fFOV = 0.0f;
    char szSetCamera[32];

    szSetCamera[0] = '\0';

    vAt = *pvAt;
    vAt.y += m_fLookatOffsetY;

    if (m_bChangeMoment)
    {
        m_vAt = vAt;
    }
    else
    {
        Math::Vec3_Lerp(&m_vAt, &m_vAt, &vAt, 0.4f);
    };

    if (!std::strcmp(m_szCameraAreaName, ""))
    {
        nCamID = CCameraDataManager::GetSetCamIDNearestPos(&m_vAt);
    }
    else
    {
        MakeSetCameraName(szSetCamera, m_szCameraAreaName);
        nCamID = CCameraDataManager::GetSetCamIDNearestPosFromName(&m_vAt, szSetCamera);
    };

    if (nCamID >= 0)
    {
        vEye = *CCameraDataManager::GetSetCamPosEye(nCamID);
        if (m_bChangeMoment)
        {
            m_vEye = vEye;
        }
        else
        {
            Math::Vec3_Lerp(&m_vEye, &m_vEye, &vEye, 0.04f);
        };

		UpdateLookat();

        fFOV = CCameraDataManager::GetSetCamFov(nCamID);
        m_fViewSize += (Math::Tan(fFOV * 0.5f) - m_fViewSize) * 0.1f;
        SetViewWindow(m_fViewSize);
    };
};


void CMapCamera::UpdateFixedCamera(const RwV3d* pvAt)
{
    int32 nCamID = -1;

    if (std::strcmp(m_szCameraAreaName, ""))
    {
        nCamID = CCameraDataManager::GetSetCamIDNearestPosFromName(&m_vAt, m_szCameraAreaName);
    };

    if (nCamID >= 0)
    {
        float fFov = 0.0f;
        RwV3d vAt = Math::VECTOR3_ZERO;
        RwV3d vEye = Math::VECTOR3_ZERO;
        
        vEye = *CCameraDataManager::GetSetCamPosEye(nCamID);
        vAt = *CCameraDataManager::GetSetCamPosLookat(nCamID);
        fFov = CCameraDataManager::GetSetCamFov(nCamID);

        if (m_bChangeMoment)
        {
            m_vEye = vEye;
            m_vAt = vAt;
        }
        else
        {
            Math::Vec3_Lerp(&m_vAt, &m_vAt, &vAt, 0.04f);
            Math::Vec3_Lerp(&m_vEye, &m_vEye, &vEye, 0.025f);
        };
        
        UpdateLookat();

        m_fViewSize = Math::Tan(fFov * 0.5f);        
        m_fChangeTimer -= CGameProperty::GetElapsedTime();
        SetViewWindow(m_fViewSize);
    };
};


void CMapCamera::UpdateLookat(void)
{
    if (m_pVibration)
    {
        m_pVibration->Update(&m_vEye, &m_vAt);
    };
    
    LookAt(&m_vEye, &m_vAt, &Math::VECTOR3_AXIS_Y);
};


void CMapCamera::LookAt(const RwV3d* pvEye, const RwV3d* pvAt, const RwV3d* pvUp)
{
    ASSERT(m_pCamera);
    
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    Math::Matrix_LookAt(&matrix, pvEye, pvAt, pvUp);
	matrix.flags = 3;
    RwFrame* pCameraFrame = RwCameraGetFrame(m_pCamera->GetRwCamera());
    ASSERT(pCameraFrame);
    
    RwFrameTransform(pCameraFrame, &matrix, rwCOMBINEREPLACE);
};


void CMapCamera::Skip(void)
{
    if (m_pIntroduction)
    {
        m_pIntroduction->Skip();
    };
};


void CMapCamera::SetLookat(const RwV3d* pvEye, const RwV3d* pvAt)
{
    RwMatrix matrix;
    RwV3d vec = Math::VECTOR3_ZERO;

    RwMatrixSetIdentityMacro(&matrix);

    m_vEye = *pvEye;
    m_vAt = *pvAt;

    Math::Matrix_LookAt(&matrix, &m_vEye, &m_vAt, &Math::VECTOR3_AXIS_Y);

    RwFrame* pCameraFrame = RwCameraGetFrame(m_pCamera->GetRwCamera());    
    RwFrameTransform(pCameraFrame, &matrix, rwCOMBINEREPLACE);

    Math::Vec3_Sub(&vec, &m_vAt, &m_vEye);

    m_fHeight = Math::FAbs(vec.y);
    m_fRotY = Math::ATan2(vec.x, vec.z);
    
    vec.y = 0.0f;
    m_fDist = Math::Vec3_Length(&vec);
};


void CMapCamera::SetCameraVibration(float fPower, float fTime, uint32 uFreq)
{
    VibrationCreate(fPower, fTime, uFreq);
};


void CMapCamera::SetFarClipPlane(float fFarClipPlane)
{
    ASSERT(m_pCamera);
    ASSERT(m_pCamera->GetRwCamera());
    
    RwCameraSetFarClipPlane(m_pCamera->GetRwCamera(), fFarClipPlane);
};


void CMapCamera::SetNearClipPlane(float fNearClipPlane)
{
    ASSERT(m_pCamera);
    ASSERT(m_pCamera->GetRwCamera());

    RwCameraSetNearClipPlane(m_pCamera->GetRwCamera(), fNearClipPlane);
};


void CMapCamera::SetFogDistance(float fFogDist)
{
    ASSERT(m_pCamera);
    ASSERT(m_pCamera->GetRwCamera());

    RwCameraSetFogDistance(m_pCamera->GetRwCamera(), fFogDist);
};


void CMapCamera::SetViewWindow(float fViewSize)
{
    SetViewWindow(fViewSize, TYPEDEF::DEFAULT_ASPECTRATIO);
};


void CMapCamera::SetViewWindow(float fViewSize, float fAspect)
{
    ASSERT(m_pCamera);
    ASSERT(m_pCamera->GetRwCamera());

    RwV2d ViewWindow;
    ViewWindow.x = fViewSize;
    ViewWindow.y = (fViewSize / fAspect);

    RwCameraSetViewWindow(m_pCamera->GetRwCamera(), &ViewWindow);
};


void CMapCamera::SetCameraMode(MODE mode)
{
    m_mode = mode;
    m_bChangeMoment = true;
    
    switch (m_mode)
    {
    case MODE_MANUAL:
        RwFrameSetIdentity(RwCameraGetFrame(m_pCamera->GetRwCamera()));
        m_fPitch = 0.0f;
        break;
        
    case MODE_INTRODUCTION:
        IntroductionCreate();
        break;
    };
};


CMapCamera::MODE CMapCamera::GetCameraMode(void) const
{
    return m_mode;
};


void CMapCamera::SetPathMode(PATHMODE pathmode)
{
    m_pathmode = pathmode;
};


bool CMapCamera::IsPosVisible(const RwV3d* pvPos)
{
    ASSERT(m_pCamera);
    
    RwV3d vec = Math::VECTOR3_ZERO;
    RwMatrix* pViewMatrix = RwCameraGetViewMatrix(m_pCamera->GetRwCamera());
    
    RwV3dTransformPoints(&vec, pvPos, 1, pViewMatrix);

    if (vec.z > 0.0f)
    {
        vec.x *= (1.0f / vec.z);
        vec.y *= (1.0f / vec.z);

        if (vec.x >= 0.0f &&
            vec.x <= 1.0f &&
            vec.y >= 0.0f &&
            vec.y <= 1.0f &&
            vec.z >= RwCameraGetNearClipPlane(m_pCamera->GetRwCamera()) &&
            vec.z <= RwCameraGetFarClipPlane(m_pCamera->GetRwCamera()))
        {
            return true;
        };
    };

    return false;
};


float CMapCamera::CalcNiceZoon(RwV3d* avPos, int32 nNumPos)
{
    if (nNumPos <= 0)
        return 1.0f;

    RwMatrix* pViewMat = RwCameraGetViewMatrixMacro(m_pCamera->GetRwCamera());
    ASSERT(pViewMat);

    float fMaxScrnX = 0.0f;
    float fMaxScrnY = 0.0f;

    for (int32 i = 0; i < nNumPos; ++i)
    {
        RwV3d vScrnPos = Math::VECTOR3_ZERO;

        RwV3dTransformPoint(&vScrnPos, &avPos[i], pViewMat);

        if (vScrnPos.z > 0.0f)
        {
            float fTmp = 1.0f / vScrnPos.z;

            vScrnPos.x = ((vScrnPos.x * fTmp) * 2.0f) - 1.0f;
            vScrnPos.y = ((vScrnPos.y * fTmp) * 2.0f) - 1.0f;
            
            vScrnPos.x = Math::FAbs(vScrnPos.x);
            vScrnPos.y = Math::FAbs(vScrnPos.y);

            if (fMaxScrnX < vScrnPos.x)
                fMaxScrnX = vScrnPos.x;

            if (fMaxScrnY < vScrnPos.y)
                fMaxScrnY = vScrnPos.y;
        };
    };

    if (fMaxScrnX <= 1.0f && fMaxScrnY <= 1.0f)
        return 1.0f;

    float fZoom = Math::Max(fMaxScrnX, fMaxScrnY);
    fZoom = Math::Clamp(fZoom, fZoom, 3.0f);

    return fZoom;
};


RwCamera* CMapCamera::GetRwCamera(void) const
{
    return m_pCamera->GetRwCamera();
};


bool CMapCamera::IsIntroductionEnd(void) const
{
    return (m_pIntroduction == nullptr);
};


float CMapCamera::GetViewRatio(void) const
{
	return (m_fDefaultViewSize / m_fViewSize);
};


void CMapCamera::GetEye(RwV3d* pvEye) const
{
    ASSERT(pvEye);

    *pvEye = m_vEye;
};


void CMapCamera::GetLookat(RwV3d* pvLookat) const
{
    ASSERT(pvLookat);

    *pvLookat = m_vAt;
};


float CMapCamera::GetViewAreaRadius(void) const
{
    return m_fLookatViewAreaRadius;
};


void CMapCamera::IntroductionCreate(void)
{
    if (!m_pIntroduction)
    {
        m_pIntroduction = new CIntroduction;
        ASSERT(m_pIntroduction);
        m_pIntroduction->Set();
    };
};


void CMapCamera::IntroductionDestroy(void)
{
    if (m_pIntroduction)
    {
        delete m_pIntroduction;
        m_pIntroduction = nullptr;
    };
};


void CMapCamera::IntroductionUpdate(void)
{
    if (m_pIntroduction)
    {
        RwV3d vEye = Math::VECTOR3_ZERO;
        RwV3d vAt = Math::VECTOR3_ZERO;
        
        if (m_pIntroduction->Update(&vEye, &vAt))
        {
            m_vEye = vEye;
            m_vAt = vAt;

            LookAt(&m_vEye, &m_vAt, &Math::VECTOR3_AXIS_Y);
            SetViewWindow(m_fViewSize);
        }
        else
        {
            IntroductionDestroy();
        };
    };
};


void CMapCamera::VibrationCreate(float fPower, float fTime, uint32 uFreq)
{
    if (!m_pVibration)
    {
        m_pVibration = new CVibration;
        ASSERT(m_pVibration);
        m_pVibration->Set(fPower, fTime, uFreq);
    };
};


void CMapCamera::VibrationDestroy(void)
{
    if (m_pVibration)
    {
        delete m_pVibration;
        m_pVibration = nullptr;
    };
};


void CMapCamera::VibrationUpdate(void)
{
    if (m_pVibration)
    {
        m_pVibration->Period();
        
        if (!m_pVibration->IsVibrating())
            VibrationDestroy();
    };
};

