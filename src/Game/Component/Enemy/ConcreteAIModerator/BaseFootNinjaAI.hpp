#pragma once

#include "BaseGrapplerAI.hpp"


class CBaseFootNinjaAI : public CBaseGrapplerAI
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseFootNinjaAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseFootNinjaAI(void);
    virtual CAIThinkOrder::ORDER ActionSelector(int32 iNo) override;
    virtual bool OnActionOfAttack(int32 iNo) override;
    virtual UNDERACTION OnUnderAllActions(void) override;
    virtual UNDERACTION OnUnderWait(void) override;
    virtual bool IsTakeBackAway(void);
    virtual bool IsTakeJumpAttack(void);

private:
    bool m_bBackAwayRequest;
    float m_fBackAwayTimer;
    bool m_bJumpAttackRequest;
};