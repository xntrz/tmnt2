#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/MoveStrategy/HatchGimmickMove.hpp"


class CHatchGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_ACTIVE,
    };

public:
    CHatchGimmick(const char* pszName, void* pParam);
    virtual ~CHatchGimmick(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    CHatchGimmickMove m_move;
    CNormalGimmickModel m_model;
    STATE m_eState;
    uint32 m_hAtari;
};