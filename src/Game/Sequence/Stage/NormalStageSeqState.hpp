#pragma once

#include "StageSeqState.hpp"


class CLoadNormalStageSeqState : public CLoadStageSeqState
{
public:
    CLoadNormalStageSeqState(STAGEID::VALUE idStage);
    virtual void OnDetach(CStageBaseSequence* pSeq) override;
};