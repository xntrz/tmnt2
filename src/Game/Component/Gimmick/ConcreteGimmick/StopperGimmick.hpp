#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CBodyHitData;


class CStopperGimmick : public CGimmick
{
public:
    CStopperGimmick(const char* pszName, void* pParam);
    virtual ~CStopperGimmick(void);
    virtual void PostMove(void) override;
    void setBodyHitDataState(bool bState);
    bool isNearRaphRockGimmick(void);

protected:
    RwV3d               m_vPosition;
    CNormalGimmickModel m_model;
    uint32              m_hAtari;
    CBodyHitData*       m_apBodyHitData[3];
    bool                m_bIsPushingBig;
};