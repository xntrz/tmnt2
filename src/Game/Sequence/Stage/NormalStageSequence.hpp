#pragma once

#include "StageBaseSequence.hpp"


class CNormalStageSequence : public CStageBaseSequence
{
public:
    static CProcess* Instance(void);

    CNormalStageSequence(void);
    virtual ~CNormalStageSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnStateDetached(STATE state) override;
};