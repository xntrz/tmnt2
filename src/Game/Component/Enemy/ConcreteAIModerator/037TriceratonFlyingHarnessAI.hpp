#pragma once

#include "BaseFlyingEnemyAI.hpp"


class CPlayerCharacter;


class C037TriceratonFlyingHarnessAI : public CBaseFlyingEnemyAI
{
public:
    static float CANCEL_TIMER_ATTACK_READY;
    static float CANCEL_TIMER_ATTACK_AWAY;
    static float CANCEL_TIMER_PURPOSE;

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C037TriceratonFlyingHarnessAI(CEnemyCharacter* pEnemyChr);
    virtual ~C037TriceratonFlyingHarnessAI(void);
    virtual CAIThinkOrder::ORDER ActionSelector(int32 iNo) override;
    virtual bool OnActionOfWait(int32 iNo) override;
    virtual bool OnActionOfMove(int32 iNo) override;
    virtual bool OnActionOfRun(int32 iNo) override;
    virtual bool OnActionOfAttack(int32 iNo) override;
    virtual UNDERACTION OnUnderAllActions(void) override;
    virtual UNDERACTION OnUnderWait(void) override;
    virtual bool IsMoveEndForTargetNumber(void) override;
    virtual bool IsMoveEndForTargetPosition(void) override;
    virtual bool IsTakeAttack(void);
    virtual bool IsTakeRun(void);
    virtual CPlayerCharacter* OnGetTarget(void);
    virtual void OnSetDirection(float fDir);
    virtual float OnGetDirection(void);

private:
    int32 m_prevHP;
    float m_fDirection;
};