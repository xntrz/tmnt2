#pragma once

#include "BaseAI6034.hpp"


class C088ShredderAI : public Utils6034::IBaseAIModerator
{
public:
    enum ACTION_TYPE
    {
        ACTION_TYPE_NOTHING = 0,
        ACTION_TYPE_ATTACK_ELEC,
        ACTION_TYPE_DEFENCE_GUARD,
    };

    enum ATTACK_STATE
    {
        ATTACK_STATE_MIN = 0,
        ATTACK_STATE_ELEC = ATTACK_STATE_MIN,

        ATTACK_STATE_MAX,
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
        AI_STATE_DOWN,
        AI_STATE_GUARD,

        AI_STATE_MAX,
    };

    enum AI_STATE_MOVE
    {
        AI_STATE_MOVE_WALK = 0,

        AI_STATE_MOVE_MAX,
    };

    class CActionTable
    {
    public:
        static int32 GetListNum(void);
        static int32 GetStateHandle(ATTACK_STATE state);
        static int32 GetStateHandle(DEFENCE_STATE state);
    };

    class CActionDispatcher : public Utils6034::IActionDispatcher
    {
    public:
        CActionDispatcher(void);
        virtual ~CActionDispatcher(void);
        virtual bool DispatchNormalAttack(int32 iTargetNo, float fTargetDistanceSQ) override;
        virtual bool DispatchDefence(void) override;
        virtual int32 CalcMoveProbability(void) override;
        virtual void OnInitialize(void) override;
        bool DispatchAction(int32 iStateHandle, int32 iTargetNo);
        bool PushState(int32 iActionType, int32 iTargetNo);
    };

    class CThinkingState : public Utils6034::IBaseAIState
    {
    protected:
        enum STEP
        {
            STEP_WAIT = 0,
            STEP_MOVE,
            STEP_MOVE_ENDED,
        };

    public:
        CThinkingState(void);
        virtual ~CThinkingState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        bool DispatchMove(void);
        float GetPositionParamT(float fTargetPosX) const;
        void GetMovePosition(RwV3d* pvecMovePos, float t) const;
        float GetToMovePosDistSQ(const RwV3d* pvecPos) const;
        float GetBeforeWaitTime(void) const;

    protected:
        Utils6034::CFixTimer m_timer;
        int32 m_step;
        RwV3d m_vecMovePos;
    };

    class CDownState : public Utils6034::IBaseAIState
    {
    public:
        CDownState(void);
        virtual ~CDownState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
    };

    class CAttackState : public Utils6034::CNormalAttackState
    {
    public:
        CAttackState(void);
        virtual ~CAttackState(void){};
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C088ShredderAI(CEnemyCharacter* pEnemyChr);
    virtual ~C088ShredderAI(void);
    virtual void Run(void) override;
    virtual void NotifyDamage(int32 iAmount) override;
    virtual void NotifyGuard(void) override;
    virtual void OnInitialize(void) override;

protected:
    CActionDispatcher m_actionDispatcher;
};
