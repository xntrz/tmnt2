#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CommonEnemyObserver.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"


class CBaseChr6045 : public CEnemyCharacter
{
public:
    CBaseChr6045(ENEMYID::VALUE enemyId);
    virtual ~CBaseChr6045(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter) override;
    virtual void Run(void) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;

protected:
    CCommonEnemyObserver::CBaseThrow* m_pThrowObserver;
    CEnemyUtils::CKnockBackControl m_knockBackCtrl;
    bool m_bFlag;
    CAIModerator* m_pAIModerator;
}; 