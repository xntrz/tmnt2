#pragma once

#include "BaseBatAI.hpp"


class C056KabutoAI : public CBaseBatAI
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C056KabutoAI(CEnemyCharacter* pEnemyChr);
    virtual ~C056KabutoAI(void);
    virtual bool OnActionOfAttack(int32 iNo) override;
};