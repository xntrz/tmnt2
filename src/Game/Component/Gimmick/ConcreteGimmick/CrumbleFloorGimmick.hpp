#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CCrumbleFloorGimmickMove;


class CCrumbleFloorGimmick final : public CGimmick
{
public:
    CCrumbleFloorGimmick(const char* pszName, void* pParam);
    virtual ~CCrumbleFloorGimmick(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    CCrumbleFloorGimmickMove*   m_pCrumbleFloorMove;
    CNormalGimmickModel         m_model;
    uint32                      m_hAtari;    
};