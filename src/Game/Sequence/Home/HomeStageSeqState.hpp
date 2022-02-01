#pragma once

#include "Game/Sequence/Stage/StageSeqState.hpp"


class CLoadHomeStageSeqState : public CLoadStageSeqState
{
public:
    CLoadHomeStageSeqState(void);
    virtual ~CLoadHomeStageSeqState(void);
    virtual void LoadPlayers(void) override;
};


class CPlayHomeStageSeqState : public IStageSeqState
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
    void SetWarpHole(void);

private:
    STEP m_step;
};