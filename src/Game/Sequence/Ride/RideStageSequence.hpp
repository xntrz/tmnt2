#pragma once

#include "Game/Sequence/Stage/StageBaseSequence.hpp"


class CRideStageSequence : public CStageBaseSequence
{
public:
    static CProcess* Instance(void);

    CRideStageSequence(void);
    virtual ~CRideStageSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnDraw(void) const override;
    virtual void OnStateDetached(STATE state) override;
};