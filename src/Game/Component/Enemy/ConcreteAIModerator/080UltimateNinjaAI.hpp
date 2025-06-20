#pragma once

#include "BaseAI6034.hpp"


class C080UltimateNinjaAI : public Utils6034::IBaseAIModerator
{
public:
    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_WARP = Utils6034::ORDERTYPE_MOVE_EXTEND,
        ORDERTYPE_MOVE_WARP_END,
    };

    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_AA = Utils6034::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_BBBB,
        ORDERTYPE_ATTACK_ENMAKU,
        ORDERTYPE_ATTACK_FUNDOU,
        ORDERTYPE_ATTACK_DRAGON,        
        ORDERTYPE_ATTACK_JUMP,        
    };

    enum ACTION_TYPE
    {
        ACTION_TYPE_NOTHING = 0,
        ACTION_TYPE_ATTACK_AA,
        ACTION_TYPE_ATTACK_BBBB,
        ACTION_TYPE_ATTACK_ENMAKU,
        ACTION_TYPE_ATTACK_FUNDOU,
        ACTION_TYPE_ATTACK_DRAGON,
        ACTION_TYPE_ATTACK_JUMP,
        ACTION_TYPE_WARP,
        ACTION_TYPE_DEFENCE_GUARD,
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
        SPECIAL_STATE_COUNTER_CROWD = SPECIAL_STATE_MIN,
        SPECIAL_STATE_ADDITION_ATTACK,
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
        MOVE_STATE_RUN = MOVE_STATE_MIN,

        MOVE_STATE_MAX,
    };

    enum AI_STATE
    {
        AI_STATE_THINK = 0,
        AI_STATE_ATTACK,
        AI_STATE_WAIT,
        AI_STATE_DOWN,
        AI_STATE_COUNTER_DAMAGED,
        AI_STATE_GUARD,
        AI_STATE_ATTACK_SMOKE,
        AI_STATE_ATTACK_SHURIKEN,
        AI_STATE_ATTACK_DRAGON,
        AI_STATE_WARP,
        AI_STATE_ESCAPE,

        AI_STATE_MAX,
    };

    enum AI_STATE_MOVE
    {
        AI_STATE_MOVE_RUN = 0,
        AI_STATE_MOVE_RUN_CIRCLE,

        AI_STATE_MOVE_MAX,
    };

    enum MAP_ID
    {
        MAP_ID_OTHER = 0,
        MAP_ID_PARK,
        MAP_ID_BRIDGE_1ST,
        MAP_ID_BRIDGE_2ND,
        MAP_ID_BRIDGE_PILLAR,
    };    

    class CActionTable
    {
    public:
        static int32 GetListNum(void);
        static int32 GetStateHandle(DISTANCE_STATE state);
        static int32 GetStateHandle(SPECIAL_STATE state);
        static int32 GetStateHandle(DEFENCE_STATE state);
        static int32 GetStateHandle(MOVE_STATE state);
    };

    class CMapInfo
    {
    public:
        static MAP_ID GetMapID(const CEnemyCharacter* pEnemyChr);
    };    

    class CActionDispatcher : public Utils6034::IActionDispatcher
    {
    public:
        CActionDispatcher(void);
        virtual ~CActionDispatcher(void) {};
        virtual bool DispatchNormalAttack(int32 iTargetNo, float fTargetDistanceSQ) override;
        virtual bool DispatchCrowdCounterAttack(int32 iTargetNo) override;
        virtual bool DispatchDamagedAction(int32 iTargetNo, float fTargetDistanceSQ) override;
        virtual bool DispatchDefence(void) override;
        virtual int32 CalcMoveProbability(void) override;
        virtual void OnInitialize(void) override;
        bool DispatchStunAdditionAttack(int32 iTargetNo);
        bool DispatchDragonAttack(int32 iPathID);
        bool DispatchEscape(void);
        bool DispatchAction(int32 iStateHandle, int32 iTargetNo);
        bool PushState(int32 iActionType, int32 iTargetNo);
        int32 GetAttackType(int32 iActionType) const;
        float GetWaitTime(void) const;
        int32 GetDragonPathID(void) const;

    protected:
        int32 m_iDragonPathID;
    };

    class CHpWatcher
    {
    public:
        CHpWatcher(CActionDispatcher* pActionDispatcher, MAP_ID eMapID);
        ~CHpWatcher(void) {};
        void Update(void);
        void WatchDragonAttack(void);
        void WatchEscape(void);
        void OnDragonAttackEnd(void);
        void OnEscapeEnd(void);
        bool IsHpLower(float fHpRatio) const;

    protected:
        CActionDispatcher*  m_pActionDispatcher;
        MAP_ID              m_eMapID;
        int32               m_iDragonPhase;
        bool                m_bEscapeDispatched;
    };    

    class CEscapePoint
    {
    public:
        CEscapePoint(void);
        CEscapePoint(MAP_ID eMapID);
        ~CEscapePoint(void) {};
        void Initialize(MAP_ID eMapID);
        void SetNextIndex(void);
        bool IsEndIndex(void) const;
        RwV3d GetTargetPos(void) const;

    protected:
        const RwV3d* m_avecEscapePoints;
        int32        m_iPointCurrent;
        int32        m_iPointMax;
    };

    class CThinkingState : public Utils6034::CBaseThinkingState
    {
    public:
        CThinkingState(CActionDispatcher* pActionDispatcher);
        virtual ~CThinkingState(void) {};
        virtual RUNRESULT OnRun(void) override;
        virtual bool OnAllTargetDown(void) override;
        virtual bool OnTargetAttack(int32 iTargetNo) override;
        virtual bool OnEndMove(void) override;

    protected:
        CActionDispatcher* m_pActionDispatcher;
    };

    class CSmokeAttackState : public Utils6034::IBaseAIState
    {
    public:
        CSmokeAttackState(void);
        virtual ~CSmokeAttackState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        Utils6034::CFixTimer m_timer;
    };

    class CShurikenAttackState : public Utils6034::IBaseAIState
    {
    public:
        CShurikenAttackState(void);
        virtual ~CShurikenAttackState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        float GetNextShurikenTime(void) const;

    protected:
        Utils6034::CFixTimer m_timer;
    };
    
    class CEscapeState : public Utils6034::IBaseAIState
    {
    public:
        CEscapeState(CHpWatcher* pHpWatcher);
        virtual ~CEscapeState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        bool Update43N(void);
        bool Update44N(void);

    protected:
        CHpWatcher*          m_pHpWatcher;
        Utils6034::CFixTimer m_timer;
        int32                m_step;
        MAP_ID               m_eMapID;
        int32                m_escapeStep;
        CEscapePoint         m_escapePoint;
    };

    class CDragonData
    {
    public:
        int32 iPathID;
    };

    class CDragonAttackState : public Utils6034::IBaseAIState
    {
    public:
        CDragonAttackState(CActionDispatcher* pActionDispatcher, CHpWatcher* pHpWatcher);
        virtual ~CDragonAttackState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        void OrderDragonAttack(void);
        
    protected:
        CActionDispatcher* m_pActionDispatcher;
        CHpWatcher*        m_pHpWatcher;
        int32              m_step;
    };

    class CDownState : public Utils6034::CBaseDownState
    {
    public:
        CDownState(void);
        virtual ~CDownState(void) {};
        virtual RUNRESULT OnRun(void) override;

    protected:
        float m_fWarpTriggerDistSQ;
    };

    class CWarpState : public Utils6034::IBaseAIState
    {
    public:
        CWarpState(void);
        virtual ~CWarpState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);
    static void CalcWarpPos(RwV3d* pvecWarpPos,
                            const RwV3d* pvecTargetPos,
                            const CCharacterCompositor* pChrCompositor,
                            float fDistMax);

    C080UltimateNinjaAI(CEnemyCharacter* pEnemyChr);
    virtual ~C080UltimateNinjaAI(void);
    virtual void Run(void) override;
    virtual void NotifyDamage(int32 iAmount) override;
    virtual void NotifyGuard(void) override;
    virtual void OnInitialize(void) override;

protected:
    CActionDispatcher       m_actionDispatcher;
    CHpWatcher              m_hpWatcher;
};