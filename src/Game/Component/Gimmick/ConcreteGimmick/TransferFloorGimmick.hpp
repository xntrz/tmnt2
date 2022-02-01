#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CLinearTransferFloorGimmickMove;


class CLinearTransferFloorGimmick : public CGimmick
{
public:
    CLinearTransferFloorGimmick(const char* pszName, void* pParam);
    virtual ~CLinearTransferFloorGimmick(void);
    virtual void PreMove(void);
    virtual void PostMove(void);
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype);

protected:
    uint32 m_hAtari;
    CNormalGimmickModel m_model;
    CLinearTransferFloorGimmickMove* m_pLinearMove;
    RwV3d m_vPrePos;
};