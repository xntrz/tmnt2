#pragma once

#include "System/Common/Process/Process.hpp"


class CDebugProc : public CProcess
{
public:
    static CProcess* Instance(void);
    static void Initialize(CProcess* pCurrent);
    static void Terminate(CProcess* pCurrent);

    CDebugProc(void);
    virtual ~CDebugProc(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
};