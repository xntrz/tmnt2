#pragma once

#ifdef _DEBUG

#include "System/Common/Process/Process.hpp"


class CDbgFrameCtrlProcess final : public CProcess
{
public:
    static CProcess* Instance(void);
    static void Initialize(CProcess* pSender);
    static void Terminate(CProcess* pSender);

    inline CDbgFrameCtrlProcess(void) {};
    virtual ~CDbgFrameCtrlProcess(void) {};
    virtual bool Attach(void) override;
    virtual void Detach(void) override {};
    virtual void Move(void) override;
    virtual void Draw(void) const override {};

private:
    bool m_bEnable;
    bool m_bFrameProcessing;
};

#endif /* _DEBUG */