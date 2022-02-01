#pragma once

#include "HomeTypes.hpp"

#include "Game/Component/GameMain/GameStageCameraUpdater.hpp"


class CHomeCamera : public IGameStageCameraUpdater
{
public:
    enum MODE
    {
        MODE_OVERLOOK = 0,
        MODE_DOOR,
        MODE_OVERLOOK_TO_DOOR,
        MODE_DOOR_TO_OVERLOOK,
        MODE_DOOR_TO_DOOR,
        MODE_DOOR_TO_ENTER,
        MODE_END,
    };

    struct CAMERA_POS_INFO
    {
        float m_fHeight;
        float m_fRadius;
    };

public:
    CHomeCamera(void);
    virtual ~CHomeCamera(void);
    virtual void Update(CMapCamera* pMapCamera) override;
    void SetCamera(CMapCamera* pMapCamera);
    void SetNextDoorFromPadInput(HOMETYPES::PADINPUT padinput);
    HOMETYPES::PADINPUT GetPadInput(void) const;
    HOMETYPES::DOORKIND GetDoorKind(HOMETYPES::DOORPOSITION doorposition) const;
    HOMETYPES::DOORSTATUS GetDoorStatus(HOMETYPES::DOORKIND doorkind) const;
    HOMETYPES::DOORSTATUS GetDoorStatus(HOMETYPES::DOORPOSITION doorposition) const;
    void GetEyeAt(RwV3d* pvEye, RwV3d* pvAt, HOMETYPES::CAMERAKIND kind) const;
    void GetEyeAt(RwV3d* pvEye, RwV3d* pvAt, HOMETYPES::CAMERAKIND kindFrom, HOMETYPES::CAMERAKIND kindTo) const;
    void GetCameraPosInfo(RwV3d* pvPos, HOMETYPES::CAMERAKIND camerakind, HOMETYPES::CAMERA_POS_INFO_KIND infokind) const;
    float GetRadian(int32 doorkind) const;
    MODE GetMode(void) const;
    
private:
    static const CAMERA_POS_INFO m_aCameraPosInfo[];
    MODE m_mode;
    int32 m_iDoor;
    int32 m_iNextDoor;
    float m_fTimeRate;
};