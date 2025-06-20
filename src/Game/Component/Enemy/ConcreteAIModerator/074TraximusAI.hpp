#pragma once

#include "BaseAI6034.hpp"


class C074TraximusAI : public Utils6034::IBaseAIModerator
{
public:
    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_BACK_STEP = Utils6034::ORDERTYPE_MOVE_EXTEND,        
    };

    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_AA = Utils6034::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_B,
        ORDERTYPE_ATTACK_THROW,
        ORDERTYPE_ATTACK_JUMP,
    };

    enum ACTION_TYPE
    {
        ACTION_TYPE_NOTHING = 0,
        ACTION_TYPE_ATTACK_AA,
        ACTION_TYPE_ATTACK_B,
        ACTION_TYPE_ATTACK_THROW,
        ACTION_TYPE_ATTACK_JUMP,
        ACTION_TYPE_BACKSTEP,
        ACTION_TYPE_BACKSTEP_COUNTER,
        ACTION_TYPE_ATTACK_RUN,
        ACTION_TYPE_DEFENCE_GUARD,
        ACTION_TYPE_ATTACK_MOVE,
    };

    enum DISTANCE_STATE
    {
        DISTANCE_STATE_MIN = 0,
        DISTANCE_STATE_NEAR = DISTANCE_STATE_MIN,
        DISTANCE_STATE_MIDDLE,
        DISTANCE_STATE_LONG,

        DISTANCE_STATE_MAX,
    };

    enum SPECIAL_STATE
    {
        SPECIAL_STATE_MIN = 0,
        SPECIAL_STATE_CROWD_COUNTER = SPECIAL_STATE_MIN,
        SPECIAL_STATE_ADDITION,
        SPECIAL_STATE_DAMAGED_COUNTER,
        SPECIAL_STATE_BACK_STEP,

        SPECIAL_STATE_MAX,
    };

    enum DEFENCE_STATE
    {
        DEFENCE_STATE_MIN = 0,
        DEFENCE_STATE_GUARRD = DEFENCE_STATE_MIN,

        DEFENCE_STATE_MAX,
    };

    enum AI_STATE
    {
        AI_STATE_THINK = 0,
        AI_STATE_ATTACK,
        AI_STATE_WAIT,
        AI_STATE_COUNTER_DOWN,
        AI_STATE_COUNTER_DAMAGED,
        AI_STATE_GUARD,
        AI_STATE_RUN_ATTACK,
        AI_STATE_RUN_MOVE,
        AI_STATE_BACK_STEP,
        AI_STATE_BACK_STEP_COUNTER,

        AI_STATE_MAX,
    };

    enum AI_STATE_MOVE
    {
        AI_STATE_MOVE_WALK = 0,
        AI_STATE_MOVE_RUN,

        AI_STATE_MOVE_MAX,
    };

    class CActionTable
    {
    public:
        static int32 GetListNum(void);
        static int32 GetStateHandle(DISTANCE_STATE state);
        static int32 GetStateHandle(SPECIAL_STATE state);
        static int32 GetStateHandle(DEFENCE_STATE state);
    };

    class CActionDispatcher : public Utils6034::IActionDispatcher
    {
    public:
        CActionDispatcher(void);
        virtual ~CActionDispatcher(void);
        virtual bool DispatchNormalAttack(int32 iTargetNo, float fTargetDistanceSQ) override;
        virtual bool DispatchCrowdCounterAttack(int32 iTargetNo) override;
        virtual bool DispatchDamagedAction(int32 iTargetNo, float fTargetDistanceSQ) override;
        virtual bool DispatchDefence(void) override;
        virtual int32 CalcMoveProbability(void) override;
        virtual void OnInitialize(void) override;
        virtual bool DispatchStunAdditionAttack(int32 iTargetNo);
        virtual bool DispatchBackStepAttack(int32 iTargetNo);
        bool DispatchAction(int32 iStateHandle, int32 iTargetNo);
        bool PushState(int32 iActionType, int32 iTargetNo);
        int32 GetAttackType(int32 iActionType) const;
        float GetWaitTime(void) const;
    };

    class CThinkingState : public Utils6034::CBaseThinkingState
    {
    public:
        CThinkingState(void);
        virtual ~CThinkingState(void) {};
        virtual bool OnAllTargetDown(void) override;
        virtual bool OnTargetAttack(int32 iTargetNo) override;
        virtual bool OnEndMove(void) override;
    };

    class CWaitState : public Utils6034::CWaitState
    {
    public:
        CWaitState(CActionDispatcher* pActionDispatcher);
        virtual ~CWaitState(void) {};
        virtual RUNRESULT OnRun(void) override;

    protected:
        CActionDispatcher* m_pActionDispatcher;
    };

    class CRunAttackState : public Utils6034::IBaseAIState
    {
    public:
        CRunAttackState(void);
        virtual ~CRunAttackState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        Utils6034::CFixTimer m_runTimer;
        float m_fRunTimeTarget;
        float m_fRunTimeContinue;
        float m_fRunTargetDistSQ;
        int32 m_step;
    };

    class CRunMoveState : public Utils6034::IBaseAIState
    {
    public:        
        CRunMoveState(void);
        virtual ~CRunMoveState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
    };

    class CBackStepJumpAttackState : public Utils6034::IBaseAIState
    {
    public:
        CBackStepJumpAttackState(void);
        CBackStepJumpAttackState(int32 iStateHandle);
        virtual ~CBackStepJumpAttackState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        int32 m_step;
        bool m_bFlagInvinsibilityChanged;
        bool m_bFlagNoReationChanged;
    };

    class CBackStepCounterState : public CBackStepJumpAttackState
    {
    public:
        CBackStepCounterState(CActionDispatcher* pActionDispatcher);
        virtual ~CBackStepCounterState(void) {};
        virtual RUNRESULT OnRun(void) override;

    protected:
        CActionDispatcher* m_pActionDispatcher;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C074TraximusAI(CEnemyCharacter* pEnemyChr);
    virtual ~C074TraximusAI(void);
    virtual void Run(void) override;
    virtual void NotifyDamage(int32 iAmount) override;
    virtual void NotifyGuard(void) override;
    virtual void OnInitialize(void) override;

protected:
    CActionDispatcher m_actionDispatcher;
};