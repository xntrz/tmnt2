#pragma once

#include "System/Common/Process/Sequence.hpp"


class CSdTest;


class CSdTestSeq final : public CSequence
{
public:
    static CProcess* Instance(void);

    CSdTestSeq(void);
    virtual ~CSdTestSeq(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* param) override;
    virtual void OnDraw(void) const override;

private:
    CSdTest* m_pImpl;
};