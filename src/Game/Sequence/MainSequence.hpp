#pragma once

#include "System/Common/Process/Sequence.hpp"


class CMainSequence final : public CSequence
{
public:
    static CProcess* Instance(void);

    CMainSequence(void);
    virtual ~CMainSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
};