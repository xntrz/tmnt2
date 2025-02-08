#pragma once

#include "BaseGeneralEnemyAI.hpp"


class CBaseFlyingEnemyAI : public CBaseGeneralEnemyAIModerator
{
protected:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_ATTACK_READY,
        STATE_ATTACK,        
        STATE_ATTACK_AWAY,
        STATE_PURPOSE,
    };

public:
    CBaseFlyingEnemyAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseFlyingEnemyAI(void);
    virtual UNDERACTION OnUnderMove(void) override;
    virtual bool IsHitWall(float fDirection);
    void SetCancelTimer(float t);
    void UpdateCancelTimer(void);
    void SetState(STATE state);
    STATE GetState(void) const;

protected:
    STATE m_state;
    float m_fCancelTimer;
};