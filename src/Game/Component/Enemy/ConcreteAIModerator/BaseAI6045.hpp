#pragma once

#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/PlayerWatcher.hpp"


namespace BASEAI6045
{
    /* CAIThinkOrder wait order type */
    enum ORDERTYPE_WAIT
    {
        ORDERTYPE_WAIT_IDLE = 0,
        ORDERTYPE_WAIT_TURN_POS,
        ORDERTYPE_WAIT_TURN_NO,
    };

    /* CAIThinkOrder move order type */
    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_WALK_POS = 0,
        ORDERTYPE_MOVE_WALK_NO,
        ORDERTYPE_MOVE_RUN_POS,
        ORDERTYPE_MOVE_RUN_NO,
        ORDERTYPE_MOVE_JUMP,

        ORDERTYPE_MOVE_EXTEND = 9,    
    };

    /* CAIThinkOrder attack order type */
    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_NONE = 0,
        ORDERTYPE_ATTACK_TO_AIR,
        ORDERTYPE_ATTACK_THROW,
        ORDERTYPE_ATTACK_COUNTER,

        ORDERTYPE_ATTACK_EXTEND,
    };

    enum SPECIAL_FLAG
    {
        FLAG_COUNTERATTACK              = 0x1,
        FLAG_CHANGEDECISION             = 0x2,
        FLAG_MOVE_TURN                  = 0x4,
        FLAG_MOVE_BACK                  = 0x8,
        FLAG_MOVE_HITANDAWAY            = 0x10,
        FLAG_TRIGGER_MOVERUN_ENABLE     = 0x20,
        FLAG_TRIGGER_ATTACK_ENABLE      = 0x40,
        FLAG_TRIGGER_ESCAPE_ENABLE      = 0x80,
        FLAG_TRIGGER_REDUCTIONHP_ENABLE = 0x100,
        FLAG_TRIGGER_MOVERUN_PERMIT     = 0x200,
        FLAG_TRIGGER_ATTACK_PERMIT      = 0x400,
        FLAG_TRIGGER_ESCAPE_PERMIT      = 0x800,
        FLAG_TRIGGER_REDUCTIONHP_PERMIT = 0x1000,

        FLAG_DEFAULT                    = 0,

        FLAG_MOVE_ALL                   = FLAG_MOVE_TURN
                                        | FLAG_MOVE_BACK
                                        | FLAG_MOVE_HITANDAWAY,

        FLAG_TRIGGER_ENABLE_ALL         = FLAG_TRIGGER_MOVERUN_ENABLE
                                        | FLAG_TRIGGER_ATTACK_ENABLE
                                        | FLAG_TRIGGER_ESCAPE_ENABLE
                                        | FLAG_TRIGGER_REDUCTIONHP_ENABLE,

        FLAG_TRIGGER_PERMIT_ALL         = FLAG_TRIGGER_MOVERUN_PERMIT
                                        | FLAG_TRIGGER_ATTACK_PERMIT
                                        | FLAG_TRIGGER_ESCAPE_PERMIT
                                        | FLAG_TRIGGER_REDUCTIONHP_PERMIT,
    }; /* enum SPECIAL_FLAG */

    DEFINE_ENUM_FLAG_OPS(SPECIAL_FLAG);

    namespace AIDECISIONUNITNAME
    {
        static const char* TRIGGER             = "Trigger";
        static const char* IDLE                = "Idle";
        static const char* MOVE                = "Move";
        static const char* MOVE_PATROLORIGIN   = "MovePatrolOrigin";
        static const char* MOVE_OBSTACLEJUMP   = "MoveObstacleJump";
        static const char* GUARD               = "Guard";
        static const char* ATTACK_COUNTER      = "AttackCounter";
        static const char* ATTACK_NORMAL       = "AttackNormal";
        static const char* ATTACK_TOAIR        = "AttackToAir";
        static const char* ATTACK_THROW        = "AttackThrow";
    }; /* namespace AIDECISIONUNITNAME */
}; /* namespace BASEAI6045 */


class CEnemyAIDecisionUnitCommonParameter;


class CEnemyAIDecisionUnit
{
public:
    enum TYPE
    {
        TYPE_WAIT = 0,
        TYPE_MOVE,
        TYPE_ATTACK,
        TYPE_GUARD,
        TYPE_ETC,

        TYPENUM,
    };

    enum RESULT
    {
        RESULT_CONTINUE = 0,
        RESULT_END, // unit may be reattached only after another unit
        RESULT_STOP, // unit may be reattached if there is no ready unit
    };

public:
    static void SetInitParam(CEnemyCharacter* pEnemyChr, CAIModerator* pAIModerator);

    CEnemyAIDecisionUnit(const char* pszUnitName, TYPE type);
    inline virtual ~CEnemyAIDecisionUnit(void) {};
    inline virtual bool CheckTerm(void) { return true; };
    inline virtual RESULT Update(void) { return RESULT_STOP; };
    inline virtual void OnStart(void) {}
    inline virtual void OnEnd(void) {}

    bool IsSatisfyFrequency(int32 iTableNo) const;
    bool IsSatisfyAngleCondition(const CAIUtils::NEARER_PLAYERDATA* pNearerPlayerData) const;
    int32 GetDistanceConditionViewAreaPlayerNum(void) const;
    int32 GetDistanceConditionEnablePlayerNum(void) const;
    void SetFreqTable(int32 tableNo);
    void SetConditionDistance(float fDist);
    void SetConditionAngle(float fAngle);
    TYPE GetType(void) const;
    int32 GetID(void) const;
    const char* GetName(void) const;
    CEnemyCharacter& EnemyChr(void);
    const CEnemyCharacter& EnemyChr(void) const;
    CEnemyAIDecisionUnitCommonParameter& DecisionUnitCommonParameter(void);
    const CEnemyAIDecisionUnitCommonParameter& DecisionUnitCommonParameter(void) const;

protected:
    friend class CEnemyAIDecisionUnitManager;
    
    static CEnemyCharacter* m_pInitEnemyChr;
    static CAIModerator* m_pInitAIModerator;
    CEnemyCharacter* m_pEnemyChr;
    CAIModerator* m_pAIModerator;
    bool m_bFrequencyEnable;
    int32 m_frequencyTableNo;
    float m_fDistanceCondition;
    float m_fAngleCondition;
    int32 m_orderType;
    int32 m_orderTargetNo;
    RwV3d m_vecOrderPos;
    char m_szUnitName[32];
    CEnemyAIDecisionUnitCommonParameter* m_pAIDecisionUnitCommonParameter;
    TYPE m_type;
    int32 m_id;
};


/* NOTE: trigger is used only by NON-bosses enemies (mouser for example)
         so standard frequency index is used here */
class CEnemyAIDecisionUnitTrigger : public CEnemyAIDecisionUnit
{
public:
    CEnemyAIDecisionUnitTrigger(void);
    virtual bool CheckTerm(void) override;
    virtual void InitTrigger(void);
    virtual void ProcTrigger(void);
    bool IsPlayerSuitableDitance(void);
    bool IsDamaged(void);
};


class CEnemyAIDecisionUnitCommonParameter
{
public:
    CEnemyAIDecisionUnitCommonParameter(CEnemyCharacter* pEnemyChr);
    ~CEnemyAIDecisionUnitCommonParameter(void);
    void UpdateEveryTime(void);
    void UpdatePlayerPositionData(void);
    void UpdatePlayerStatusData(void);
    void UpdateMyParameter(void);
    void SetHistory(CEnemyAIDecisionUnit* pDecisionUnit);
    void ClearHistory(void);
    void SetAllTypeHistory(CEnemyAIDecisionUnit* pDecisionUnit);
    int32 CheckAllTypeHistory(CEnemyAIDecisionUnit* pDecisionUnit);
    void SetAttackHistory(CEnemyAIDecisionUnit* pDecisionUnit);
    int32 CheckAttackHistory(CEnemyAIDecisionUnit* pDecisionUnit);
    bool CalcGroundPointViewAreaPlayer(RwV3d* vecGroundPt) const;
    void SetHistoryProcess(CEnemyAIDecisionUnit* pDecisionUnit, int32* aHistory, int32 numHistory);
    int32 CheckHistoryProcess(CEnemyAIDecisionUnit* pDecisionUnit, int32* aHistory, int32 numHistory);
    void ClearHistoryProcess(int32* aHistory, int32 numHistory);
    void SetChangeDicisionUnit(const char* pszDecisionUnitName);
    const char* GetChangeDicisionUnit(void) const;
    void ChangeDicisionUnit(const char* pszUnitName);
    void SetSpecialFlag(BASEAI6045::SPECIAL_FLAG flag);
    void ClearSpecialFlag(BASEAI6045::SPECIAL_FLAG flag);
    bool TestSpecialFlag(BASEAI6045::SPECIAL_FLAG flag) const;
    bool CheckSpecialFlag(BASEAI6045::SPECIAL_FLAG flag) const;
    void SetAttackInterval(float fInterval);
    void SetMoveInterval(float fInterval);
    void SetAttackTimer(float fTimer);
    void SetMoveTimer(float fTimer);
    void SetMoveWaitTime(float fTimer);
    void SetAttackWaitTime(float fTimer);
    void SetWaitTimerAllPlayerDown(float fTimer);
    bool IsAttackTime(void) const;
    bool IsAttack(void) const;
    bool IsMoveTime(void) const;
    bool IsMove(void) const;
    float GetAttackInterval(void) const;
    float GetRatioOfActivity(void) const;
    float GetRatioOfMove(void) const;
    float GetMoveWaitTime(void) const;
    float GetAttackWaitTime(void) const;
    CPlayerWatcher& PlayerWatcher(void);
    bool IsViewDataValid(void) const;
    int32 GetViewDataNum(void) const;
    const CAIUtils::NEARER_PLAYERDATA& GetViewData(int32 index);
    const CAIUtils::NEARER_PLAYERDATA& GetViewData(int32 index) const;
    bool IsEnableDataValid(void) const;
    int32 GetEnableDataNum(void) const;
    const CAIUtils::NEARER_PLAYERDATA& GetEnableData(int32 index);
    const CAIUtils::NEARER_PLAYERDATA& GetEnableData(int32 index) const;

private:
    int32 m_numPlayerEnable;
    const CAIUtils::NEARER_PLAYERDATA* m_apDataViewArea[4];
    int32 m_numDataViewArea;
    float m_fRatioOfActivity;
    float m_fRatioOfMove;
    float m_fAttackTimer;
    float m_fAttackInterval;
    float m_fMoveTimer;
    float m_fMoveInterval;
    float m_fMoveWaitTimer;
    float m_fAttackWaitTimer;
    float m_fAttackWaitTimerAllPlayerDown;
    BASEAI6045::SPECIAL_FLAG m_specialFlag;
    const char* m_pszChangeDecisionUnitName;
    int32 m_aAIDecisionUnitAllTypeHistory[4];
    int32 m_aAIDecisionUnitAttackHistory[4];
    CPlayerWatcher m_playerWatcher;
    CEnemyCharacter* m_pEnemyChr;
};


class CEnemyAIDecisionUnitManager
{
public:
    CEnemyAIDecisionUnitManager(CEnemyCharacter* pEnemyCHr, CEnemyAIDecisionUnitCommonParameter* pAIDecisionUnitCommonParameter);
    ~CEnemyAIDecisionUnitManager(void);
    void Run(void);
    void Draw(void);
    void Attach(CEnemyAIDecisionUnit* pDecisionUnit);
    void SetNowDecisionUnit(CEnemyAIDecisionUnit* pDecisionUnit);
    bool SetNowDecisionUnit(const char* pszDecisionUnitName);
    CEnemyAIDecisionUnit* SearchDecisionUnit(const char* pszDecisionUnitName);
    CEnemyAIDecisionUnit* CheckRunnableUnit(void);
    bool ProcThinkingFrequency(void);
    bool ProcAIThinkOrderResult(void);
    void ProcUpdateUnit(void);
    CEnemyCharacter& EnemyChr();
    void DebugDrawInfo(void);
    void DebugDrawPatrolArea(void);
    void DebugDrawSuitableArea(void);
    
private:
    CEnemyAIDecisionUnit* m_apAIDecisionUnit[18];
    int32 m_numAIDicisionUnit;
    CEnemyAIDecisionUnit* m_pNowDecisionUnit;
    CEnemyAIDecisionUnitCommonParameter* m_pAIDecisionUnitCommonParameter;
    CEnemyCharacter* m_pEnemyChr;
    float m_fThinkTimer;
    CEnemyAIDecisionUnitTrigger* m_pAIDecisionUnitTrigger;
};


class CBaseAI6045 : public CAIModerator
{
public:
    class CDecisionUnitIdle : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitIdle(void);
        virtual bool CheckTerm(void) override;
        virtual RESULT Update(void) override;
        virtual void OnStart(void) override;
        void SetIdleAIThinkOrderParameter(void);
    };

    class CDecisionUnitMove : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitMove(void);
        CDecisionUnitMove(const char* pszUnitName);
        virtual bool CheckTerm(void) override;
        virtual RESULT Update(void) override;
        virtual void OnStart(void) override;
        virtual void OnEnd(void) override;
        virtual bool IsDoing(void);
        virtual bool ThinkMoveOrder(void);
        void SetMoveOrder(void);
        bool IsMoveToRun(float fDistance);
        void SetMoveTime(float fTime);
        void SetMoveStopDistance(float fDistance);
        void SetPatrolAreaEnable(bool bEnable);        

    protected:
        float m_fMoveTime;
        float m_fMoveStopDistance;
        bool m_bPatrolAreaEnable;
    };

    class CDecisionUnitMove2 : public CDecisionUnitMove
    {
    public:
        CDecisionUnitMove2(void);
        CDecisionUnitMove2(const char* pszUnitName);
        virtual bool IsDoing(void) override;
        virtual bool ThinkMoveOrder(void) override;
        bool ThinkMoveOrderBack(void);
        bool ThinkMoveOrderHitAndAway(void);
        bool ThinkMoveOrderRunaway(void);
        bool ThinkMoveOrderStraightLine(void);
        bool ThinkMoveOrderTurn(void);
        void ClearTurnSetting(void);
        void ClearHitAndAwaySetting(void);
        void ClearBackSetting(void);
        bool GetCheckObstacleLine(int32 iPlayerNo, RwV3d* pvecStart, RwV3d* pvecEnd);
        bool CheckMapCollisionAndHole(RwV3d* pvecStart, RwV3d* pvecEnd);
        bool CalcRunawayPoint(RwV3d* pvecRunawayPoint, float fRunawayDistance);
        void SetCheckObstacleDistance(float fDist);
        void SetCheckObstacleDistanceDivNum(int32 iDivNum);
        void SetCheckJump(bool bState);

    protected:
        float m_fCheckObstacleDistane;
        int32 m_iCheckObstacleDistanceDivNum;
        bool m_bCheckJump;
    };

    class CDecisionUnitMoveBoss : public CDecisionUnitMove2
    {
    public:
        virtual bool IsDoing(void) override;
        virtual bool ThinkMoveOrder(void) override;
        bool ThinkMoveOrderStraightLineForBoss(void);
    };

    class CDecisionUnitMovePatrolOrigin : public CDecisionUnitMove2
    {
    public:
        CDecisionUnitMovePatrolOrigin(void);
        virtual bool CheckTerm(void) override;
        virtual RESULT Update(void) override;
        virtual void OnEnd(void) override;
        virtual bool IsDoing(void) override;
        bool ThinkMoveOrderPatrolOrigin(void);
        bool IsPlayerPatrolArea(void);
        bool IsReachPatrolOrigin(float* pfDist = nullptr);
        void SetWaitTimer(float fTimer);

    private:
        float m_fTimer;
        float m_fWaitTimer;
    };

    class CDecisionUnitMoveObstacleJump : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitMoveObstacleJump(void);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
        void SetCheckObstacleDistance(float fDist);

    private:
        float m_fCheckObstacleDistance;
    };

    class CDecisionUnitGuard : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitGuard(void);
        virtual bool CheckTerm(void) override;
        virtual RESULT Update(void) override;
        virtual void OnStart(void) override;
        virtual void OnEnd(void) override;
        bool IsDoing(void);
    };

    class CDecisionUnitGuard2 : public CDecisionUnitGuard
    {
    public:
        virtual bool CheckTerm(void) override;
    };

    class CDecisionUnitAttackNormal : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitAttackNormal(const char* pszUnitName);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
    };

    class CDecisionUnitAttackNormalForUnusualStatus : public CDecisionUnitAttackNormal
    {
    public:        
        CDecisionUnitAttackNormalForUnusualStatus(const char* pszUnitName);
        virtual bool CheckTerm(void) override;
    };
    
    class CDecisionUnitAttackToAir : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitAttackToAir(void);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;

    private:
        float m_fHeightLow;
        float m_fHeightHigh;
    };

    class CDecisionUnitAttackOverInterval : public CEnemyAIDecisionUnit
    {
    public:        
        CDecisionUnitAttackOverInterval(const char* pszUnitName);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;

    private:
        int32 m_attackPermissionPlayerNum;
    };

    class CDecisionUnitAttackCounter : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitAttackCounter(const char* pszUnitName);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;

    private:
        int32 m_attackPermissionPlayerNum;
    };

public:
    CBaseAI6045(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseAI6045(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    virtual void CatchTriggerActionOrder(void);
    void Attach(CEnemyAIDecisionUnit* pDecisionUnit);
    CEnemyAIDecisionUnit* GetDicisionUnitForName(const char* pszDecisionUnitName);
    void SetCatchTriggerAction(uint32 catchTriggerAction);
    CEnemyAIDecisionUnitManager& DecisionUnitManager(void);
    const CEnemyAIDecisionUnitManager& DecisionUnitManager(void) const;
    CEnemyAIDecisionUnitCommonParameter& DecisionUnitCommonParameter(void);
    const CEnemyAIDecisionUnitCommonParameter& DecisionUnitCommonParameter(void) const;

protected:
    CEnemyAIDecisionUnitManager* m_pAIDecisionUnitManager;
    CEnemyAIDecisionUnitCommonParameter* m_pAIDecisionUnitCommonParameter;
    uint32 m_catchTriggerAction;
};