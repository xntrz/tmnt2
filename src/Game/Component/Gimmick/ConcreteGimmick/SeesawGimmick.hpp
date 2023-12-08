#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CSeesawGimmickMove;


class CSeesawGimmick final : public CGimmick
{
public:
    CSeesawGimmick(const char* pszName, void* pParam);
    virtual ~CSeesawGimmick(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    CSeesawGimmickMove* m_pSeesawMove;
    int32 m_iAxisNum;
    CNormalGimmickModel m_model;
    uint32 m_hAtari;
    bool m_bIsSoundCall;
    float m_fSoundCallTimer;
};