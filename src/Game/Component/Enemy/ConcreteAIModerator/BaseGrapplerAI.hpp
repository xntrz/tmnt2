#pragma once

#include "BaseGeneralEnemyAI.hpp"


class CBaseGrapplerAI : public CBaseGeneralEnemyAIModerator
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseGrapplerAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseGrapplerAI(void);
    virtual CAIThinkOrder::ORDER ActionSelector(int32 iNo) override;
    virtual bool OnActionOfWait(int32 iNo) override;
    virtual bool OnActionOfMove(int32 iNo) override;
    virtual bool OnActionOfRun(int32 iNo) override;
    virtual bool OnActionOfAttack(int32 iNo) override;
    virtual bool OnActionOfDefence(int32 iNo) override;
    virtual UNDERACTION OnUnderAllActions(void) override;
    virtual UNDERACTION OnUnderWait(void) override;
    virtual bool IsMoveEndForTargetNumber(void) override;
    virtual bool IsMoveEndForTargetPosition(void) override;
    virtual bool IsTakeAttack(void);
    virtual bool IsTakeRunAttack(void);
    virtual bool IsTakeGuard(void);
};