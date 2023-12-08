#pragma once

#ifdef _DEBUG

#include "System/Common/Process/Process.hpp"


class CDbgParamCtrlProcess final : public CProcess
{
public:
    static CProcess* Instance(void);
    static void Initialize(CProcess* pSender);
    static void Terminate(CProcess* pSender);
    
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
};

#endif /* _DEBUG */