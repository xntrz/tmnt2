#pragma once

#include "Game/Sequence/Stage/StageSeqState.hpp"


class IGameStageCameraUpdater;


class CPlayNexusStageSeqState : public IStageSeqState
{
private:
    enum STEP
    {
        STEP_INTRO = 0,
        STEP_PLAY,
        STEP_RESULT,
        STEP_BONUS,
        STEP_FADEIN,
        STEP_FADEWAIT,
        STEP_END,
    };

public:
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;
    virtual void OnDetach(CStageBaseSequence* pSeq) override;
    virtual bool OnMove(CStageBaseSequence* pSeq) override;
    void RecoverHP(void);
    void AwakePlayers(void);
    void SleepPlayers(void);
    void StartEnemyGenerator(void);
    void SetItemBox(void);
    void SetFailedMessage(void);
    void SetCameraUpdater(CStageBaseSequence* pSeq, IGameStageCameraUpdater* pCameraUpdater);

private:
    bool m_bPause;
    STEP m_step;
    IGameStageCameraUpdater* m_pCameraUpdater;
    float m_fTime;
};