#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CConsoleGimmick : public CGimmick
{
protected:
    enum STATE
    {
        STATE_WAIT = 0,
        STATE_START_INPUT,
        STATE_INPUT,
        STATE_UNLOCK,
    };

    enum COLLTYPE
    {
        COLLTYPE_ON = 0,
        COLLTYPE_OFF,
    };

public:
    CConsoleGimmick(const char* pszName, void* pParam);
    virtual ~CConsoleGimmick(void);
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    void GetDirection(RwV3d* pvDirection);
    void consoleOn(void);
    void collisionCreate(COLLTYPE colltype);
    void collisionDestroy(void);

protected:
    CNormalGimmickModel m_model;
    uint32              m_hAtari;
    bool                m_bFlashFlag;
    STATE               m_state;
    float               m_fTimer;
    RwV3d               m_vPosition;
    RwV3d               m_vDirection;
    char                m_szTargetGimmickName[GAMEOBJECTTYPES::NAME_MAX];
    int32               m_nHitNum;
    float               m_fInvinT;
};