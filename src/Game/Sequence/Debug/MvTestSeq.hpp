#pragma once

#include "System/Common/Process/Sequence.hpp"


class CMvTest;


class CMvTestSeq final : public CSequence
{
public:
    static CProcess* Instance(void);

    CMvTestSeq(void);
    virtual ~CMvTestSeq(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* param) override;
    virtual void OnDraw(void) const override;

private:
    CMvTest* m_pImpl;
};