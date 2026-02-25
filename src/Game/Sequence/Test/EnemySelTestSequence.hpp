#pragma once

#ifdef _DEBUG

#include "Game/Component/Enemy/EnemyID.hpp"
#include "System/Common/Process/Sequence.hpp"

#include "DebugUtils.hpp"


class CEnemySelTestSequence : public CSequence
{
public:
    static CProcess* Instance(void);

    CEnemySelTestSequence(void);
    virtual ~CEnemySelTestSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

private:
    static ENEMYID::VALUE m_aeID[];
    CDebugMenuCtrl m_menu;
    ENEMYID::VALUE m_eIDLast;
};


#endif /* _DEBUG */