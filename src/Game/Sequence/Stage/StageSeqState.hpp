#pragma once

#include "Game/Component/GameMain/StageID.hpp"


class CStageBaseSequence;


class IStageSeqState
{
public:
    virtual ~IStageSeqState(void) {};
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) = 0;
    virtual void OnDetach(CStageBaseSequence* pSeq) = 0;
    virtual bool OnMove(CStageBaseSequence* pSeq) = 0;
    virtual void OnDraw(const CStageBaseSequence* pSeq) const {};
};


class CDummyStageState final : public IStageSeqState
{
public:
    static CDummyStageState* Instance(void);
    
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override {};
    virtual void OnDetach(CStageBaseSequence* pSeq) override {};
    virtual bool OnMove(CStageBaseSequence* pSeq) override { return true; };
};


class CLoadStageSeqState : public IStageSeqState
{
public:
    enum STEP
    {
        STEP_LOAD_SOUND = 0,
        STEP_LOAD_DATA,
        STEP_END,
    };
    
public:
    CLoadStageSeqState(STAGEID::VALUE idStage);
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;
    virtual void OnDetach(CStageBaseSequence* pSeq) override;
    virtual bool OnMove(CStageBaseSequence* pSeq) override;
    virtual void LoadPlayers(void);
    void LoadData(void);
    void LoadStageCommon(void);
    void LoadStage(void);

private:
    STAGEID::VALUE m_idStage;
    STEP m_step;
};


class CIntroStageSeqState : public IStageSeqState
{
public:
    enum STEP
    {
        STEP_INTRO = 0,
        STEP_DISPINFO,
        STEP_FADEOUT,
        STEP_END,
    };
    
public:
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;
    virtual void OnDetach(CStageBaseSequence* pSeq) override;
    virtual bool OnMove(CStageBaseSequence* pSeq) override;

private:
    float m_fTime;
    STEP m_step;
};


class CPlayStageSeqState : public IStageSeqState
{
public:
    enum STEP
    {
        STEP_PLAYING = 0,
        STEP_RESULT,
        STEP_PLAYEND,
        STEP_FADEOUT,
        STEP_FADEOUT_WAIT,
        STEP_END,
    };
    
public:
    virtual void OnAttach(CStageBaseSequence* pSeq, const void* pParam) override;
    virtual void OnDetach(CStageBaseSequence* pSeq) override;
    virtual bool OnMove(CStageBaseSequence* pSeq) override;

private:
    float m_fTime;
    STEP m_step;
    bool m_bPause;
};