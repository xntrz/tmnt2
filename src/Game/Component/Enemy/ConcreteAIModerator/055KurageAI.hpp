#pragma once

#include "BaseBatAI.hpp"


class C055KurageAI : public CBaseBatAI
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C055KurageAI(CEnemyCharacter* pEnemyChr);
    virtual ~C055KurageAI(void);
    virtual CAIThinkOrder::ORDER ActionSelector(int32 iNo) override;
    virtual bool OnActionOfWait(int32 iNo) override;
    virtual bool OnActionOfMove(int32 iNo) override;
    virtual bool OnActionOfRun(int32 iNo) override;
    virtual bool OnActionOfAttack(int32 iNo) override;
    virtual UNDERACTION OnUnderAllActions(void) override;
    virtual UNDERACTION OnUnderMove(void) override;
    virtual bool IsTakeAttack(void) override;
    virtual void OnSetActionOfRun(void);
    virtual void OnSetDirection(float fDir);
    virtual float OnGetDirection(void) const;
    virtual bool IsTakeBombing(void);

private:
    bool m_bBombingRequest;
    float m_fDirection;
    float m_fAttackWaitTimer;
};