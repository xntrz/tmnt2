#pragma once

#include "System/Common/Process/Process.hpp"


class CSoftwareReset : public CProcess
{
public:
    static CProcess* Instance(void);
    static void Request(void);
    static bool IsRequested(void);
    static void Complete(void);
    static void Initialize(void);
    static void Terminate(void);

    CSoftwareReset(void);
    virtual ~CSoftwareReset(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
    void ExecReset(void);
    void CheckDebugRequest(void);

private:
    float m_fTimer;
};