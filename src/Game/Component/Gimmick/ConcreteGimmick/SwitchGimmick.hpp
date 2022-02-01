#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CSwitchGimmick : public CGimmick
{
protected:
    enum SWITCHTYPE
    {
        SWITCHTYPE_SLOTMACHINE_LEVER = 0,
        SWITCHTYPE_TOGGLE,
        SWITCHTYPE_ONCE,
        SWITCHTYPE_UNK, // TODO unknown SwitchGimmick type

        SWITCHTYPENUM,
    };

    enum SWITCHSTATE
    {
        SWITCHSTATE_OFF = 0,
        SWITCHSTATE_ON,
    };

public:
    CSwitchGimmick(const char* pszName, void* pParam);
    virtual ~CSwitchGimmick(void);
    virtual GIMMICKTYPES::FEATURE GetFeatures(void) const override;
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;

protected:
    void onSwitchChange(void);

protected:
    char m_szTargetGimmickName[GAMEOBJECTTYPES::NAME_MAX];
    SWITCHTYPE m_switchtype;
    SWITCHSTATE m_switchstate;
    CNormalGimmickModel m_model;
    float m_fTimer;
    bool m_bEnable;
};