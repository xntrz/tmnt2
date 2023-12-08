#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CRollingRockGimmickMove;
class CModuleManager;


class CRollingRockGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_NONE = 0,
        STATE_MOVING,
        STATE_CAPTURED,
    };
    
public:
    CRollingRockGimmick(const char* pszName, void* pParam);
    virtual ~CRollingRockGimmick(void);
    virtual void Draw(void) const override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnTouchedDown(void) override;

private:
    STATE m_eState;
    int32 m_nPower;
    CRollingRockGimmickMove* m_pRollingMove;
    CNormalGimmickModel m_model;
    CModuleManager* m_pModuleMan;
    int32 m_nBoundSE;
};
