#pragma once

#include "BaseGrapplerAI.hpp"


class C001PurpleDragonGangAI : public CBaseGrapplerAI
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C001PurpleDragonGangAI(CEnemyCharacter* pEnemyChr);
    virtual ~C001PurpleDragonGangAI(void);
    virtual CAIThinkOrder::ORDER ActionSelector(int32 iNo) override;
    virtual bool OnActionOfAttack(int32 iNo) override;
    virtual UNDERACTION OnUnderAllActions(void) override;
    virtual bool IsTakeAttack(void) override;
    virtual bool IsTakeThrowGrenade(void);

private:
    bool m_bThrowGrenadeRequest;
    bool m_bCheckThrowGrenadeRequest;
    int32 m_prevHP;
    float m_fThrowGrenadeTimer;
};