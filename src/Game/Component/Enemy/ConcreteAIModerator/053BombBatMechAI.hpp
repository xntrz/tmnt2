#pragma once

#include "BaseBatAI.hpp"


class C053BombBatMechAI : public CBaseBatAI
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C053BombBatMechAI(CEnemyCharacter* pEnemyChr);
    virtual ~C053BombBatMechAI(void);
    virtual CAIThinkOrder::ORDER ActionSelector(int32 iNo) override;
    virtual bool OnActionOfAttack(int32 iNo) override;
    virtual UNDERACTION OnUnderMove(void) override;
    virtual bool IsTakeBodyBlow(void);

private:
    bool m_bBodyBlowRequest;
};