#pragma once

#include "System/Common/Process/Sequence.hpp"


class CStaffRollSequence final : public CSequence
{
public:
    static CProcess* Instance(void);

    CStaffRollSequence(void);
    virtual ~CStaffRollSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
};