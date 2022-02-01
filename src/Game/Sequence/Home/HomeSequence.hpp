#pragma once

#include "System/Common/Process/Sequence.hpp"


class CHomeSequence final : public CSequence
{
public:
    static CProcess* Instance(void);

    CHomeSequence(void);
    virtual ~CHomeSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
    int32 Branch(int32 iLabel, const void* param);
    
private:
    int32 m_iCallLabel;
};