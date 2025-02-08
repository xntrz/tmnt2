#include "HomeCamera.hpp"
#include "Home2D.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "System/Common/Controller.hpp"


/*static*/ const CHomeCamera::CAMERA_POS_INFO CHomeCamera::m_aCameraPosInfo[]
{
    { 8.6f, -14.4f  },
    { 2.0f, 0.0f    },
    { 3.0f, 2.0f    },
    { 2.0f, 10.0f   },
    { 2.0f, 12.0f   },
    { 2.0f, 22.0f   },
    { 0.5f, 0.5f    },
    { 0.5f, 0.5f    },
    { 1.0f, Math::PI},
};


CHomeCamera::CHomeCamera(void)
: m_mode(MODE_OVERLOOK)
, m_iDoor(0)
, m_iNextDoor(0)
, m_fTimeRate(0.0f)
{
    ;
};


CHomeCamera::~CHomeCamera(void)
{
    ;
};


void CHomeCamera::Update(CMapCamera* pMapCamera)
{
    switch (m_mode)
    {
    case MODE_OVERLOOK:
        {
            HOMETYPES::PADINPUT padinput = GetPadInput();
            if (padinput)
            {
                SetNextDoorFromPadInput(padinput);
                m_mode = MODE_OVERLOOK_TO_DOOR;
                m_fTimeRate = 0.0f;
            };
        }
        break;
        
    case MODE_DOOR:
        {
            HOMETYPES::PADINPUT padinput = GetPadInput();
            switch (padinput)
            {
            case HOMETYPES::PADINPUT_OK:
                {
                    m_mode = MODE_DOOR_TO_ENTER;
                    m_fTimeRate = 0.0f;
                }
                break;
                
            case HOMETYPES::PADINPUT_LEFT:
            case HOMETYPES::PADINPUT_DOWN:
            case HOMETYPES::PADINPUT_RIGHT:
                {
                    SetNextDoorFromPadInput(padinput);
                    m_mode = MODE_DOOR_TO_DOOR;
                    m_fTimeRate = 0.0f;
                }
                break;

            default:
                {
                    m_fTimeRate += CGameProperty::GetElapsedTime() * 0.1f;
                    if (m_fTimeRate >= 1.0f)
                    {
                        m_mode = MODE_DOOR_TO_OVERLOOK;
                        m_fTimeRate = 0.0f;
                    };
                }
                break;
            };
        }
        break;
        
    case MODE_OVERLOOK_TO_DOOR:
    case MODE_DOOR_TO_DOOR:
        {
            m_fTimeRate += (CGameProperty::GetElapsedTime() + CGameProperty::GetElapsedTime());
            if (m_fTimeRate >= 1.0f)
            {
                m_iNextDoor = (m_iNextDoor + HOMETYPES::DOORKINDNUM) % HOMETYPES::DOORKINDNUM;
                m_iDoor = (m_iNextDoor + HOMETYPES::DOORKINDNUM) % HOMETYPES::DOORKINDNUM;
                CHome2D::ResetTime();
                m_mode = MODE_DOOR;
                m_fTimeRate = 0.0f;
            };
        }
        break;
        
    case MODE_DOOR_TO_OVERLOOK:
        {
            m_fTimeRate += (CGameProperty::GetElapsedTime() + CGameProperty::GetElapsedTime());
            if (m_fTimeRate >= 1.0f)
            {
                m_mode = MODE_OVERLOOK;
                m_fTimeRate = 0.0f;
            };
        }
        break;
        
    case MODE_DOOR_TO_ENTER:
        {
            m_fTimeRate += (CGameProperty::GetElapsedTime() + CGameProperty::GetElapsedTime());
            if (m_fTimeRate >= 1.0f)
            {
                m_mode = MODE_END;
                m_fTimeRate = 0.0f;
            };
        }
        break;

    default:
        break;
    };

    SetCamera(pMapCamera);
};


void CHomeCamera::SetCamera(CMapCamera* pMapCamera)
{
    RwV3d vAt = Math::VECTOR3_ZERO;
    RwV3d vEye = Math::VECTOR3_ZERO;

    switch (m_mode)
    {
    case MODE_OVERLOOK:
        {
            GetCameraPosInfo(&vEye, HOMETYPES::CAMERAKIND_OVERLOOK, HOMETYPES::CAMERA_POS_INFO_KIND_EYE);
            GetCameraPosInfo(&vAt, HOMETYPES::CAMERAKIND_OVERLOOK, HOMETYPES::CAMERA_POS_INFO_KIND_AT);
        }
        break;

    case MODE_DOOR:
    case MODE_DOOR_TO_DOOR:
        {            
            GetCameraPosInfo(&vEye, HOMETYPES::CAMERAKIND_DOOR, HOMETYPES::CAMERA_POS_INFO_KIND_EYE);
            GetCameraPosInfo(&vAt, HOMETYPES::CAMERAKIND_DOOR, HOMETYPES::CAMERA_POS_INFO_KIND_AT);
        }
        break;

    case MODE_OVERLOOK_TO_DOOR:
        {
            GetEyeAt(&vEye, &vAt, HOMETYPES::CAMERAKIND_OVERLOOK, HOMETYPES::CAMERAKIND_DOOR);
        }
        break;

    case MODE_DOOR_TO_OVERLOOK:
        {
            GetEyeAt(&vEye, &vAt, HOMETYPES::CAMERAKIND_DOOR, HOMETYPES::CAMERAKIND_OVERLOOK);
        }
        break;

    case MODE_DOOR_TO_ENTER:
        {
            GetEyeAt(&vEye, &vAt, HOMETYPES::CAMERAKIND_DOOR, HOMETYPES::CAMERAKIND_ENTER);
        }
        break;

    case MODE_END:
        {
            GetCameraPosInfo(&vEye, HOMETYPES::CAMERAKIND_ENTER, HOMETYPES::CAMERA_POS_INFO_KIND_EYE);
            GetCameraPosInfo(&vAt, HOMETYPES::CAMERAKIND_ENTER, HOMETYPES::CAMERA_POS_INFO_KIND_AT);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    pMapCamera->SetLookat(&vEye, &vAt);
};


void CHomeCamera::SetNextDoorFromPadInput(HOMETYPES::PADINPUT padinput)
{
    switch (padinput)
    {
    case HOMETYPES::PADINPUT_OK:
    case HOMETYPES::PADINPUT_UP:
        m_iNextDoor = m_iDoor;
        break;
        
    case HOMETYPES::PADINPUT_LEFT:
        m_iNextDoor = m_iDoor + 1;
        break;
        
    case HOMETYPES::PADINPUT_DOWN:
        m_iNextDoor = m_iDoor - 2;
        break;
        
    case HOMETYPES::PADINPUT_RIGHT:
        m_iNextDoor = m_iDoor - 1;
        break;

    default:
        break;
    };
};


HOMETYPES::PADINPUT CHomeCamera::GetPadInput(void) const
{
    int32 iController = CGameData::Attribute().GetVirtualPad();
    int16 stickX = CController::GetAnalog(iController, CController::ANALOG_LSTICK_X);
    int16 stickY = CController::GetAnalog(iController, CController::ANALOG_LSTICK_Y);

    if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
    {
        if (GetDoorStatus(HOMETYPES::DOORPOSITION_FRONT))
            return HOMETYPES::PADINPUT_OK;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LUP) ||
        (stickY > (TYPEDEF::SINT16_MAX / 2)))
    {
        if (GetDoorStatus(HOMETYPES::DOORPOSITION_FRONT))
            return HOMETYPES::PADINPUT_UP;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LDOWN) ||
        (stickY < (TYPEDEF::SINT16_MIN / 2)))
    {
        if (GetDoorStatus(HOMETYPES::DOORPOSITION_BACK))
            return HOMETYPES::PADINPUT_DOWN;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LLEFT) ||
        (stickX < (TYPEDEF::SINT16_MIN / 2)))
    {
        if (GetDoorStatus(HOMETYPES::DOORPOSITION_LEFT))
            return HOMETYPES::PADINPUT_LEFT;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LRIGHT) ||
        (stickX > (TYPEDEF::SINT16_MAX / 2)))
    {
        if (GetDoorStatus(HOMETYPES::DOORPOSITION_RIGHT))
            return HOMETYPES::PADINPUT_RIGHT;
    };

    return HOMETYPES::PADINPUT_NONE;
};


HOMETYPES::DOORKIND CHomeCamera::GetDoorKind(HOMETYPES::DOORPOSITION doorposition) const
{
    return HOMETYPES::DOORKIND((doorposition + m_iDoor) % HOMETYPES::DOORKINDNUM);
};


HOMETYPES::DOORSTATUS CHomeCamera::GetDoorStatus(HOMETYPES::DOORKIND doorkind) const
{
    HOMETYPES::DOORSTATUS Result = HOMETYPES::DOORSTATUS_CLOSE;
    
    switch (doorkind)
    {
    case HOMETYPES::DOORKIND_EXIT:
        {
            Result = HOMETYPES::DOORSTATUS_OPEN;
        }
        break;

    case HOMETYPES::DOORKIND_APRIL:
        {
            switch (CGameData::Record().Antique().GetShopState())
            {
            case CAntiqueRecord::SHOPSTATE_OPEN:
                {
                    Result = HOMETYPES::DOORSTATUS_NEW;
                }
                break;

            case CAntiqueRecord::SHOPSTATE_NORMAL:
            case CAntiqueRecord::SHOPSTATE_COMPLETED:
                {
                    if (CGameData::Record().Antique().IsNewAntiqueExisted())
                        Result = HOMETYPES::DOORSTATUS_UPDATE;
                    else
                        Result = HOMETYPES::DOORSTATUS_OPEN;
                }
                break;
            };
        }
        break;

    case HOMETYPES::DOORKIND_COMPUTER:
        {
            switch (CGameData::Record().Database().GetDatabaseState())
            {
            case CDatabaseRecord::DBSTATE_OPEN:
                {
                    Result = HOMETYPES::DOORSTATUS_NEW;
                }
                break;

            case CDatabaseRecord::DBSTATE_NORMAL:
                {
                    if (CGameData::Record().Database().IsNewItemExisted())
                        Result = HOMETYPES::DOORSTATUS_UPDATE;
                    else
                        Result = HOMETYPES::DOORSTATUS_OPEN;
                }
                break;
            };
        }
        break;

    case HOMETYPES::DOORKIND_TOURNAMENT:
        {
            switch (CGameData::Record().Nexus().GetNexusState())
            {
            case CNexusRecord::NEXUSSTATE_OPEN:
                {
                    Result = HOMETYPES::DOORSTATUS_NEW;
                }
                break;
                
            case CNexusRecord::NEXUSSTATE_NORMAL:
                {
                    if (CGameData::Record().Nexus().IsNewTournamentExisted())
                        Result = HOMETYPES::DOORSTATUS_UPDATE;
                    else
                        Result = HOMETYPES::DOORSTATUS_OPEN;
                }
                break;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return Result;
};


HOMETYPES::DOORSTATUS CHomeCamera::GetDoorStatus(HOMETYPES::DOORPOSITION doorposition) const
{
    return GetDoorStatus(GetDoorKind(doorposition));
};


void CHomeCamera::GetEyeAt(RwV3d* pvEye, RwV3d* pvAt, HOMETYPES::CAMERAKIND kind) const
{
    GetCameraPosInfo(pvEye, kind, HOMETYPES::CAMERA_POS_INFO_KIND_EYE);
    GetCameraPosInfo(pvAt, kind, HOMETYPES::CAMERA_POS_INFO_KIND_AT);
};


void CHomeCamera::GetEyeAt(RwV3d* pvEye, RwV3d* pvAt, HOMETYPES::CAMERAKIND kindFrom, HOMETYPES::CAMERAKIND kindTo) const
{
    RwV3d vEyeTo = Math::VECTOR3_ZERO;
    RwV3d vAtTo = Math::VECTOR3_ZERO;
    RwV3d vEyeFrom = Math::VECTOR3_ZERO;
    RwV3d vAtFrom = Math::VECTOR3_ZERO;

    float t = 1.0f - (1.0f - m_fTimeRate) * (1.0f - m_fTimeRate);

    GetCameraPosInfo(&vEyeFrom, kindFrom, HOMETYPES::CAMERA_POS_INFO_KIND_EYE);
    GetCameraPosInfo(&vEyeTo, kindTo, HOMETYPES::CAMERA_POS_INFO_KIND_EYE);
    Math::Vec3_Lerp(pvEye, &vEyeFrom, &vEyeTo, t);
    
    GetCameraPosInfo(&vAtFrom, kindFrom, HOMETYPES::CAMERA_POS_INFO_KIND_AT);
    GetCameraPosInfo(&vAtTo, kindTo, HOMETYPES::CAMERA_POS_INFO_KIND_AT);
    Math::Vec3_Lerp(pvAt, &vAtFrom, &vAtTo, t);
};


void CHomeCamera::GetCameraPosInfo(RwV3d* pvPos, HOMETYPES::CAMERAKIND camerakind, HOMETYPES::CAMERA_POS_INFO_KIND infokind) const
{
    float fRad = 0.0f;
    
    if (m_iDoor == m_iNextDoor)
    {
        fRad = GetRadian(m_iDoor);
    }
    else
    {
        fRad = (1.0f - (1.0f - m_fTimeRate) * (1.0f - m_fTimeRate));
        fRad *= (GetRadian(m_iNextDoor) - GetRadian(m_iDoor));
        fRad += GetRadian(m_iDoor);
    };

    int32 nIndex = infokind + 2 * camerakind;
    ASSERT(nIndex >= 0 && nIndex < COUNT_OF(m_aCameraPosInfo));
    
    const CAMERA_POS_INFO* pPosInfo = &m_aCameraPosInfo[nIndex];

    pvPos->x = Math::Sin(fRad) * pPosInfo->m_fRadius;
    pvPos->y = pPosInfo->m_fHeight;
    pvPos->z = Math::Cos(fRad) * pPosInfo->m_fRadius;
};


float CHomeCamera::GetRadian(int32 doorkind) const
{
    return (float(doorkind) * Math::PI2) / 4.0f;
};


CHomeCamera::MODE CHomeCamera::GetMode(void) const
{
    return m_mode;
};