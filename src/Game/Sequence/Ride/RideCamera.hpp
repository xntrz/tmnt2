#pragma once

#include "Game/Component/GameMain/GameStageCameraUpdater.hpp"


class CRideCamera : public IGameStageCameraUpdater
{
public:
    CRideCamera(void);
    virtual ~CRideCamera(void);
    virtual void Update(CMapCamera* pMapCamera) override;

private:
    float m_fPathT;
    int32 m_nEyePathID;
    int32 m_nAtPathID;
    char m_szCameraAreaName[32];
};