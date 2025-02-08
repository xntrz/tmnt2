#pragma once

#include "BaseGeneralEnemyAI.hpp"


class CBaseGunnerAI : public CBaseGeneralEnemyAIModerator
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseGunnerAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseGunnerAI(void);
    virtual CAIThinkOrder::ORDER ActionSelector(int32 iNo) override;
    virtual bool OnActionOfWait(int32 iNo) override;
    virtual bool OnActionOfMove(int32 iNo) override;
    virtual bool OnActionOfRun(int32 iNo) override;
    virtual bool OnActionOfAttack(int32 iNo) override;
    virtual UNDERACTION OnUnderAllActions(void) override;
    virtual UNDERACTION OnUnderWait(void) override;
    virtual bool IsMoveEndForTargetNumber(void) override;
    virtual bool IsMoveEndForTargetPosition(void) override;
    virtual bool IsTakeFire(void);
    virtual bool IsTakeRun(void);

private:
    float m_fFireTimeout;
};