#pragma once

#include "BaseAI6034.hpp"


class C089FeudalJapanShredderAI : public Utils6034::IBaseAIModerator
{
public:
    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_TENGU = Utils6034::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_ELEC,
        ORDERTYPE_ATTACK_CHARGE,
        ORDERTYPE_ATTACK_THROW,
    };

    enum ACTION_TYPE
    {
        ACTION_TYPE_NOTHING = 0,
        ACTION_TYPE_ATTACK_TENGU,
        ACTION_TYPE_ATTACK_CHARGE,
        ACTION_TYPE_ATTACK_ELEC,
        ACTION_TYPE_ATTACK_THROW,
        ACTION_TYPE_DEFENCE_GUARD,
        ACTION_TYPE_DEFENCE_AVOID,
        ACTION_TYPE_ESCAPE,
    };

    enum DISTANCE_STATE
    {        
        DISTANCE_STATE_MIN = 0,
        DISTANCE_STATE_NEAR = DISTANCE_STATE_MIN,
        DISTANCE_STATE_MIDDLE,

        DISTANCE_STATE_MAX,
    };

    enum SPECIAL_STATE
    {
        SPECIAL_STATE_MIN = 0,
        SPECIAL_STATE_AVOID = SPECIAL_STATE_MIN,

        SPECIAL_STATE_MAX,
    };

    enum DEFENCE_STATE
    {
        DEFENCE_STATE_MIN = 0,
        DEFENCE_STATE_GUARD = DEFENCE_STATE_MIN,
        DEFENCE_STATE_ESCAPE,

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
        AI_STATE_ESCAPE,
        AI_STATE_AVOID,
        AI_STATE_ELEC,

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
        virtual bool DispatchDamagedAction(int32 iTargetNo, float fTargetDistanceSQ) override;
        virtual bool DispatchDefence(void) override;
        virtual int32 CalcMoveProbability(void) override;
        virtual void OnInitialize(void) override;
        bool DispatchEscape(void);
        bool DispatchAction(int32 iStateHandle, int32 iTargetNo);
        bool PushState(int32 iActionType, int32 iTargetNo);
        int32 GetAttackType(int32 iActionType) const;
        float GetWaitTime(void) const;
    };

    class CWalkDispatcher : public Utils6034::CWalkDispatcher
    {
    public:
        CWalkDispatcher(int32 iStateHandle, C089FeudalJapanShredderAI* pShredderAI);
        virtual ~CWalkDispatcher(void) {};
        virtual RUNRESULT OnRun(void) override;

    protected:
        C089FeudalJapanShredderAI* m_pShredderAI;
    };

    class CThinkingState : public Utils6034::CBaseThinkingState
    {
    public:
        CThinkingState(C089FeudalJapanShredderAI* pShredderAI, CActionDispatcher* pActionDispatcher);
        virtual ~CThinkingState(void) {};
        virtual RUNRESULT OnRun(void) override;
        virtual bool OnAllTargetDown(void) override;
        virtual bool OnTargetAttack(int32 iTargetNo) override;
        virtual bool OnEndMove(void) override;
        bool CheckFenceAttack(void);
        float GetFenceWaitTime(void) const;

    protected:
        C089FeudalJapanShredderAI* m_pShredderAI;
        Utils6034::CFixTimer m_timerFenceWait;
        CActionDispatcher* m_pActionDispatcher;
    };

    class CAvoidState : public Utils6034::IBaseAIState
    {
    public:
        CAvoidState(void);
        virtual ~CAvoidState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        int32 m_step;
    };

    class CEscapeState : public Utils6034::IBaseAIState
    {
    public:
        CEscapeState(C089FeudalJapanShredderAI* pShredderAI);
        virtual ~CEscapeState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        C089FeudalJapanShredderAI* m_pShredderAI;
        int32 m_step;
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

    C089FeudalJapanShredderAI(CEnemyCharacter* pEnemyChr);
    virtual ~C089FeudalJapanShredderAI(void);
    virtual void Run(void) override;
    virtual void NotifyDamage(int32 iAmount) override;
    virtual void NotifyGuard(void) override;
    virtual void OnInitialize(void) override;
    bool IsOutsideDefenceRadius(void) const;
    bool IsOutsideDefenceRadius(const RwV3d* pvecPos) const;
    bool IsEscapeReady(void) const;
    void SetEscapeReady(bool bInside);
    float GetElecWaitTime(void) const;

protected:
    CActionDispatcher    m_actionDispatcher;
    Utils6034::CFixTimer m_timerDefenceRadiusAdvance;
    float                m_fCurrentDefenceRadiusSQ;
    Utils6034::CFixTimer m_timerElecAttackWait;
    bool                 m_bEscapeReady;
};
