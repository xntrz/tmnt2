#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CMineGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_ON,
        STATE_EXPLODE,
        STATE_DELETE,
    };

public:
    CMineGimmick(const char* pszName, void* pParam);
    virtual ~CMineGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    bool IsEnableStateChange(STATE eState) const;

private:
    static bool m_bAnimOwnerExist;
    CNormalGimmickModel m_model;
    STATE m_eState;
    float m_fRadius;
    float m_fCounter;
    float m_fHeight;
    uint32 m_hAtari;
    bool m_bAnimOwner;
    void* m_pAnimatedMaterials;
};