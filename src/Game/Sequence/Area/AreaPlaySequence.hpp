#pragma once

#include "System/Common/Process/Sequence.hpp"

#include "Game/Component/GameMain/StageID.hpp"
#include "Game/System/Movie/MovieID.hpp"


class CAreaPlaySequence final : public CSequence
{
private:
    enum STEP
    {
        STEP_NONE = 0,
        STEP_STAGESTART,
        STEP_STAGERUN,
        STEP_STAGEEND,
        STEP_RESULT,
        STEP_ENBU,
        STEP_UNLOCK,
        STEP_AREAMOVIEOUT,
        STEP_AUTOSAVE,
        STEP_EOL,
    };
    
public:
    static CProcess* Instance(void);

    CAreaPlaySequence(void);
    virtual ~CAreaPlaySequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

private:
    void changeStep(STEP step);
    void initResultType(void);
    void callStageSequence(STAGEID::VALUE idStage);
    void callResultSequence(void);
    void callEnbuSequence(void);
    void callUnlockSequence(void);
    void callMovieSequence(MOVIEID::VALUE idMovie);

private:
    STAGEID::VALUE m_idLastStage;
    STEP m_step;
    int32 m_substep;
#ifdef _DEBUG
    bool m_bSkipAllMovies;
#endif
};