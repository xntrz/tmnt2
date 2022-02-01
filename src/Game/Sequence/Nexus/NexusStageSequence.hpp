#pragma once

#include "Game/Sequence/Stage/NormalStageSequence.hpp"


class CNexusStageSequence : public CNormalStageSequence
{
public:
    static CProcess* Instance(void);

    CNexusStageSequence(void);
    virtual ~CNexusStageSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;    
};