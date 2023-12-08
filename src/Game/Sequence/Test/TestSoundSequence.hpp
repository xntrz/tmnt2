#pragma once

#ifdef _DEBUG

#include "System/Common/Process/Sequence.hpp"


class CTestSoundSequence final : public CSequence
{
public:
    static CProcess* Instance(void);

    CTestSoundSequence(void);
    virtual ~CTestSoundSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
};

#endif /* _DEBUG */