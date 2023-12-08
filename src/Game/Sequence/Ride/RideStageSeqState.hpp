#pragma once

#include "Game/Sequence/Stage/StageSeqState.hpp"


class CLoadRideStageSeqState : public CLoadStageSeqState
{
public:
    CLoadRideStageSeqState(STAGEID::VALUE idStage);
    virtual void LoadPlayers(void) override;
};


class CRideCamera;


class CPlayRideStageSeqState : public IStageSeqState
{
private:
    enum STEP
    {
        STEP_PLAY = 0,
        STEP_PLAYEND,
        STEP_FADEIN,
        STEP_END,
    };

public:
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;
    virtual void OnDetach(CStageBaseSequence* pSeq) override;
    virtual bool OnMove(CStageBaseSequence* pSeq) override;

private:
    STEP m_step;
    CRideCamera* m_pRideCamera;
};