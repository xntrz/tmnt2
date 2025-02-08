#pragma once

#ifdef _DEBUG

#include "System/Common/Process/Process.hpp"


class CDebugMenuCtrl;


class CDebugMenuProcess final : public CProcess
{
public:
    static CProcess* Instance(void);
    static void Initialize(CProcess* pSender);
    static void Terminate(CProcess* pSender);

    /*  menu may be extended in any sequence
        it will reset to default after return
        (see example in EnemyTestStageSeqState) */
    static CDebugMenuCtrl& Menu(void);

    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
};

#endif /* _DEBUG */