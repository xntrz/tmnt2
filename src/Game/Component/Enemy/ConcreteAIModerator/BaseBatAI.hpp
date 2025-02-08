#pragma once

#include "BaseFlyingEnemyAI.hpp"


class CBaseBatAI : public CBaseFlyingEnemyAI
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseBatAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseBatAI(void);
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

private:
    int32 m_prevHP;
};