#include "NormalStageSeqState.hpp"
#include "NormalStageSequence.hpp"


CLoadNormalStageSeqState::CLoadNormalStageSeqState(STAGEID::VALUE idStage)
: CLoadStageSeqState(idStage)
{
    ;
};


void CLoadNormalStageSeqState::OnDetach(CStageBaseSequence* pSeq)
{
    pSeq->Stage().Start();
    pSeq->Stage().AddStageObjects();
};