#pragma once

#include "BaseAI6034.hpp"


class C090UltimateShredderAI : public Utils6034::IBaseAIModerator
{
public:
    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_TENGU = Utils6034::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_ELEC,
        ORDERTYPE_ATTACK_CHARGE,
        ORDERTYPE_ATTACK_THROW,
        ORDERTYPE_ATTACK_THUNDER,
        ORDERTYPE_ATTACK_JUMP,
    };

    enum ACTION_TYPE
    {
        ACTION_TYPE_NOTHING = 0,
        ACTION_TYPE_ATTACK_TENGU,
        ACTION_TYPE_ATTACK_CHARGE,
        ACTION_TYPE_ATTACK_ELEC,
        ACTION_TYPE_ATTACK_THROW,
        ACTION_TYPE_ATTACK_THUNDER,
        ACTION_TYPE_ATTACK_JUMP,
        ACTION_TYPE_ATTACK_CHARGE_TRIPLE,
        ACTION_TYPE_COUNTER,
        ACTION_TYPE_DEFENCE_GUARD,
        ACTION_TYPE_DEFENCE_AVOID,        
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
        SPECIAL_STATE_COUNTER_AERIAL = SPECIAL_STATE_MIN,
        SPECIAL_STATE_COUNTER_CROWD,
        SPECIAL_STATE_COUNTER_DAMAGED,

        SPECIAL_STATE_MAX,
    };

    enum DEFENCE_STATE
    {
        DEFENCE_STATE_MIN = 0,
        DEFENCE_STATE_GUARD = DEFENCE_STATE_MIN,

        DEFENCE_STATE_MAX,
    };

    enum MOVE_STATE
    {
        MOVE_STATE_MIN = 0,
        MOVE_STATE_WALK = MOVE_STATE_MIN,

        MOVE_STATE_MAX,
    };

    enum HP_STATE
    {
        HP_STATE_MIN = 0,
        HP_STATE_100PAR = HP_STATE_MIN,
        HP_STATE_50PAR,

        HP_STATE_MAX,
    };

    enum AI_STATE
    {
        AI_STATE_THINK = 0,
        AI_STATE_ATTACK,
        AI_STATE_WAIT,
        AI_STATE_COUNTER_DOWN,
        AI_STATE_COUNTER_DAMAGED,
        AI_STATE_GUARD,
        AI_STATE_ATTACK_TRIPLE,
        AI_STATE_COUNTER,
        AI_STATE_AVOID,
        AI_STATE_ELEC,
        
        AI_STATE_MAX,
    };

    enum AI_STATE_MOVE
    {
        AI_STATE_MOVE_WALK = 0,
        AI_STATE_MOVE_DASH,
        AI_STATE_MOVE_DASH_CIRCLE,

        AI_STATE_MOVE_MAX,
    };

    class CActionTable
    {
    public:
        static int32 GetListNum(void);
        static int32 GetStateHandle(DISTANCE_STATE state, HP_STATE hp);
        static int32 GetStateHandle(SPECIAL_STATE state);
        static int32 GetStateHandle(DEFENCE_STATE state);
        static int32 GetStateHandle(MOVE_STATE state);
    };

    class CActionDispatcher : public Utils6034::IActionDispatcher
    {
    protected:
        enum STAGE_TYPE
        {
            STAGE_TYPE_OTHER = 0,
            STAGE_TYPE_DECK,
            STAGE_TYPE_LAB,
        };
        
    public:
        CActionDispatcher(void);
        virtual ~CActionDispatcher(void);
        virtual bool DispatchNormalAttack(int32 iTargetNo, float fTargetDistanceSQ) override;
        virtual bool DispatchAerialCounterAttack(int32 iTargetNo) override;
        virtual bool DispatchCrowdCounterAttack(int32 iTargetNo) override;
        virtual bool DispatchDamagedAction(int32 iTargetNo, float fTargetDistanceSQ) override;
        virtual bool DispatchDefence(void) override;
        virtual int32 CalcMoveProbability(void) override;
        virtual void OnInitialize(void) override;
        bool DispatchAction(int32 iStateHandle, int32 iTargetNo);
        bool PushState(int32 iActionType, int32 iTargetNo);
        int32 GetAttackType(int32 iActionType) const;
        float GetWaitTime(void) const;
        HP_STATE GetHpState(void) const;
        DISTANCE_STATE GetDistanceState(float fTargetDistanceSQ) const;

    protected:
        STAGE_TYPE m_stageType;
    };

    class CThinkingState : public Utils6034::CBaseThinkingState
    {
    public:
        CThinkingState(void);
        virtual ~CThinkingState(void) {};
        virtual RUNRESULT OnRun(void) override;
        virtual bool OnAllTargetDown(void) override;
        virtual bool OnTargetAttack(int32 iTargetNo) override;
        virtual bool OnEndMove(void) override;
    };

    class CCounterState : public Utils6034::IBaseAIState
    {
    public:
        CCounterState(int32 iStateHandle);
        CCounterState(void);
        virtual ~CCounterState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        int32 m_step;
    };

    class CAvoidState : public CCounterState
    {
    public:
        CAvoidState(void);
        virtual ~CAvoidState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;

    protected:
        int32 m_step;
    };

    class CTripleAttackState : public Utils6034::IBaseAIState
    {
    public:
        CTripleAttackState(void);
        virtual ~CTripleAttackState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        int32 m_attackNum;
    };

    class CElecState : public Utils6034::IBaseAIState
    {
    public:
        CElecState(Utils6034::CFixTimer* pElecTimer);
        virtual ~CElecState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        Utils6034::CFixTimer* m_pElecTimer;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C090UltimateShredderAI(CEnemyCharacter* pEnemyChr);
    virtual ~C090UltimateShredderAI(void);
    virtual void Run(void) override;
    virtual void NotifyDamage(int32 iAmount) override;
    virtual void NotifyGuard(void) override;
    virtual void OnInitialize(void) override;
    float GetElecWaitTime(void) const;

protected:
    CActionDispatcher    m_actionDispatcher;
    Utils6034::CFixTimer m_timerElecAttackWait;
};
