#pragma once

#include "Game/Component/GameMain/GameStage.hpp"
#include "System/Common/Process/Sequence.hpp"


class IStageSeqState;


class CStageBaseSequence : public CSequence
{
protected:
    enum STATE
    {
        STATE_NONE = 0,
        
        STATEGENERAL_START,
        STATE_LOAD = STATEGENERAL_START,
        STATE_INTRO,
        STATE_PLAY,
        
        STAESPECIFIC_START,
        STATE_END = 0xF,
        
        STATEMAX,
    };

private:    
    struct STATENODE
    {
        bool m_bAutoDelete;
        IStageSeqState* m_pStateObj;
    };

public:
    CStageBaseSequence(void);
    virtual ~CStageBaseSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* param) override;
    virtual void OnDraw(void) const override;
    virtual void OnStateDetached(STATE state);    
    void RegisterStateObject(STATE state, IStageSeqState* pStateObj, bool bAutoDelete);
    void ChangeState(STATE state, const void* pParam = nullptr);
    STATE GetState(void) const;
    CGameStage& Stage(void);
    const CGameStage& Stage(void) const;

private:
    void setupState(void);
    void cleanupState(void);
    IStageSeqState* getStateObj(STATE state);
    bool runState(void);

private:
    CGameStage m_GameStage;
    STATE m_state;
    STATE m_stateRequest;
    const void* m_pAttachStateParam;
    IStageSeqState* m_pCurrentStateObj;
    STATENODE m_aStateNode[STATEMAX];
};