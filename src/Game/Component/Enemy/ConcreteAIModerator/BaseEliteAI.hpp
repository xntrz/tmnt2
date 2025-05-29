#pragma once

#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/PlayerWatcher.hpp"


namespace ELITEAI
{
    enum PRIORITY
    {
        PRIORITY_NONE   = -1,
        PRIORITY_ALL    = PRIORITY_NONE,

        PRIORITY_HIGH   = 0,
        PRIORITY_NORMAL = 128,
        PRIORITY_LOW    = 255,
        PRIORITY_ABSLOW = 256,
    };

    enum ORDERTYPE_WAIT
    {
        ORDERTYPE_WAIT_IDLE = 0,
        ORDERTYPE_WAIT_TURN,
        ORDERTYPE_WAIT_TIMEOUT,
    };

    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_WALK = 0,
        ORDERTYPE_MOVE_RUN,
        ORDERTYPE_MOVE_DASH,
        ORDERTYPE_MOVE_DASH_LEFT,
        ORDERTYPE_MOVE_DASH_RIGHT,
        ORDERTYPE_MOVE_JUMP,
        ORDERTYPE_MOVE_BACK,
        
        ORDERTYPE_MOVE_EXTEND,
    };

    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_NORMAL = 0,
        ORDERTYPE_ATTACK_NORMALA,
        ORDERTYPE_ATTACK_NORMALB,
        ORDERTYPE_ATTACK_NORMALC,
        ORDERTYPE_ATTACK_HIGH,

        ORDERTYPE_ATTACK_EXTEND,    
    };

    enum ORDERTYPE_DEFENCE
    {
        ORDERTYPE_DEFENCE_NEAR = 0,
        ORDERTYPE_DEFENCE_MIDDLE,
        ORDERTYPE_DEFENCE_LONG,

        ORDERTYPE_DEFENCE_EXTEND,
    };

    enum FREQUENCY_INDEX
    {
        FREQUENCY_GUARD = 0,
        FREQUENCY_ATTACK_HIGH,
        FREQUENCY_ATTACK_AVOID,

        FREQUENCY_EXTEND,
    };

    enum ATTACK_RESULT
    {
        ATTACK_RESULT_NONE = 0,
        ATTACK_RESULT_STAB,
        ATTACK_RESULT_MISS,
        ATTACK_RESULT_GUARD,
    };

    namespace AIBEHAVIORNAME
    {
        static const char* DEBUG         = "Debug";
        static const char* IDLE          = "Idle";
        static const char* JUMP          = "Jump";
        static const char* MOVE          = "Move";
        static const char* GUARD         = "Guard";
        static const char* ATTACK        = "Attack";
        static const char* ATTACK_HIGH   = "HighAttack";
        static const char* AVOID_ENCLOSE = "AvoidEnclose";
        static const char* AVOID_ATTACK  = "AvoidAttack";
    };
};


class CBaseEliteAI;


class CEliteAIParam
{
protected:
    static const int32 THINK_MAX = 10;

    class CThinkStack : public CListNode<CThinkStack>
    {
    public:
        ELITEAI::PRIORITY priority;
        CAIThinkOrder     thinkOrder;

        CThinkStack(void);        
    };

public:
    CEliteAIParam(CEnemyCharacter* pEnemyChr);
    ~CEliteAIParam(void);
    void Update(void);
    void DebugDrawPatrolAreaInfo(void);
    void DebugDrawInfo(void);
    void UpdateMe(void);
    bool Think(void);
    bool SetThinkOrderStack(CAIThinkOrder thinkOrder, ELITEAI::PRIORITY priority);
    void ThinkOrderSetFromStack(CAIThinkOrder& thinkOrder);
    void ThinkOrderDeleteFront(ELITEAI::PRIORITY priority);
    void ThinkOrderDeleteRequest(ELITEAI::PRIORITY priority);
    void ThinkOrderDelete(ELITEAI::PRIORITY priority);
    bool IsMovePermission(void) const;
    bool IsActivityPermission(void) const;
    void SetActionPermissionTimeout(float fWaitTime);
    bool IsActionPermission(void) const;
    void SetAttackResult(ELITEAI::ATTACK_RESULT attackResult);
    ELITEAI::ATTACK_RESULT GetAttackResult(void) const;
    bool RegistExecTimer(float* pfTimer, const char* pszName);
    void ClearAllExecTimer(void);
    void ClearElapsedTimer(void);
    void UpdateTimers(void);
    float GetElapsedTime(void) const;
    void SetOwnTerritory(const RwV3d* pvecTerritoryCenter, float fTerritoryRadius);
    bool IsStatusChanged(void) const;
    ENEMYTYPES::STATUS GetStatus(void) const;
    bool IsThinkStackEmpty(void) const;
    void SetCounterAttack(const char* pszCounterBehaviorName);
    void ClearCounterAttack(void);
    bool IsCounterAttack(void) const;
    const char* GetCounterBehaviorName(void) const;
    void AddAttackCount(void);
    void ClearAttackCount(void);
    int32 GetAttackCount(void) const;
    void AddAttackCountDamage(int32 iAmount);
    void ClearAttackCountDamage(void);
    int32 GetAttackCountDamage(void) const;
    float GetTerritoryDist(const RwV3d* pvecPos = nullptr) const;
    float GetTerritoryRadius(void) const;
    void GetTerritoryCenter(RwV3d* pvecResult) const;
    bool IsInsideTerritory(const RwV3d* pvecPos = nullptr) const;
    void ClearEliteDummyInfo(void);
    void SetEliteDummyNo(int32 no);
    void SetEliteDummyHandle(int32 handle);
    int32 GetEliteDummyNo(void) const;
    uint32 GetEliteDummyHandle(void) const;
    CPlayerWatcher& PlayerWatcher(void);
    const CPlayerWatcher& PlayerWatcher(void) const;
    CEnemyCharacter& EnemyChr(void);
    const CEnemyCharacter& EnemyChr(void) const;

protected:
    RwV3d m_vecTerriotiryCenter;
    float m_fTerritoryRadius;
    int32 m_eliteDummyNo;
    uint32 m_eliteDummyHandle;
    ENEMYTYPES::STATUS m_enemyStatus;
    ENEMYTYPES::STATUS m_enemyStatusOld;
    ELITEAI::ATTACK_RESULT m_attackResult;
    bool m_bCounter;
    const char* m_pszCounterName;
    float m_fRatioOfActivity;
    float m_fRatioOfMove;
    float m_fThinkTimer;
    bool m_bThink;
    float m_fThinkWaitTimer;
    float m_fElapsedTime;
    int32 m_attackCount;
    int32 m_attackCountDamage;
    CEnemyCharacter* m_pEnemyChr;
    CPlayerWatcher m_playerWatcher;
    CEliteAIParam::CThinkStack m_thinkStack[THINK_MAX];
    CList<CThinkStack> m_thinkList;
    float* m_apfExecTimer[THINK_MAX];
    char m_aszExecTimerName[THINK_MAX][32];
};


class CAIBehavior
{
public:
    enum RESULT
    {
        RESULT_END = 0,
        RESULT_CONTINUE,
        RESULT_FORCEEND,
    };

public:
    static void SetInitParam(CEnemyCharacter* pEnemyChr, CBaseEliteAI* pAIModerator, CEliteAIParam* pAIParam);

    CAIBehavior(const char* pszName);
    virtual ~CAIBehavior(void);
    virtual void Initialize(void) {};
    virtual bool IsRunnable(void) { return true; };
    virtual void OnActive(void) {};
    virtual RESULT Update(void) { return RESULT_END; };
    virtual void OnEnd(void) {};

    void OrderWait(int32 waitType, float fWaitTime = 0.0f);
    void OrderMove(int32 moveType, float fMoveTime = 0.0f);
    void OrderAttack(int32 attackType);
    void OrderDefence(int32 defenceType);
    void OrderPush(ELITEAI::PRIORITY priority);
    bool IsSatisfyFreq(int32 freqTableNo) const;
    float GetFreqRatio(int32 freqTableNo) const;
    bool IsActive(void) const;
    void SetActive(bool bActive);    
    const char* GetName(void) const;
    CPlayerWatcher& PlayerWatcher(void);
    const CPlayerWatcher& PlayerWatcher(void) const;
    CEliteAIParam& AIParam(void);
    const CEliteAIParam& AIParam(void) const;
    CBaseEliteAI& AIModerator(void);
    const CBaseEliteAI& AIModerator(void) const;
    CEnemyCharacter& EnemyChr(void);

protected:
    static CEnemyCharacter* m_pInitEnemyChr;
    static CBaseEliteAI*    m_pInitAIModerator;
    static CEliteAIParam*   m_pInitAIParam;
    bool                    m_bActive;
    CEnemyCharacter*        m_pEnemyChr;
    CBaseEliteAI*           m_pAIModerator;
    const char*             m_pszName;
    CEliteAIParam*          m_pAIParam;
    CAIThinkOrder           m_AIThinkOrder;
};


class CAIBehaviorManager
{
protected:
    class CAIBehaviorObj : public CListNode<CAIBehaviorObj>
    {
    public:
        CAIBehavior*        behavior;
        ELITEAI::PRIORITY   priority;
    };

public:
    CAIBehaviorManager(CEliteAIParam* pAIParam);
    ~CAIBehaviorManager(void);
    CAIBehavior::RESULT Update(void);
    void DebugDrawOrderInfo(void);
    void Stride(void);
    bool Attach(CAIBehavior* pAIBehavior, ELITEAI::PRIORITY priority);
    void Change(const char* pszName);
    void Change(CAIBehaviorObj* pNewBehaviorObj);
    CAIBehavior* GetBehavior(const char* pszName);
    CAIBehaviorObj* Find(const char* pszName);
    CAIBehaviorObj* Find(void);
    bool CounterAttack(void);
    CEliteAIParam& AIParam(void);

protected:
    bool                  m_bAllOff;
    int32                 m_no;
    CAIBehaviorObj        m_aBehaviorObj[16];
    CList<CAIBehaviorObj> m_listBehavior;
    CAIBehaviorObj*       m_pNowBehaviorObj;
    CEliteAIParam*        m_pAIParam;
};


class CBaseEliteAI : public CAIModerator
{
public:
    class CAIBehaviorDebug : public CAIBehavior
    {
    public:
        CAIBehaviorDebug(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual RESULT Update(void) override;
        virtual void OnEnd(void) override;

    protected:
        float m_afTimers[4];
        int32 m_pattern;
        int32 m_orderType;
    };
    
    class CAIBehaviorIdle : public CAIBehavior
    {
    public:
        CAIBehaviorIdle(void);
        virtual void OnActive(void) override;
    };

    class CAIBehaviorJump : public CAIBehavior
    {
    public:
        CAIBehaviorJump(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
    };

    class CAIBehaviorMove : public CAIBehavior
    {
    public:
        CAIBehaviorMove(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        bool IsDisablePlayer(void);

    protected:
        float m_fDashTimeout;
        float m_fReachLimit;
        bool  m_bEnablePlayer;
        bool  m_bAfraidOfDonBarrier;
        float m_fDonBarrierTimer;
    };

    class CAIBehaviorGuard : public CAIBehavior
    {
    protected:
        enum GUARDTYPE
        {
            GUARDTYPE_SHORT_RANGE = 0,
            GUARDTYPE_LONG_RANGE,
        };

    public:
        CAIBehaviorGuard(bool bAvoidAttack);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual RESULT Update(void) override;
        virtual void OnEnd(void) override;
        void SetAvoidAttack(bool bAvoidAttack);

    protected:
        GUARDTYPE m_guardType;
        float     m_fGuardLimitTime;
        bool      m_bAvoidAttack;
        float     m_fGuardTimeout;        
    };

    class CAIBehaviorAttack : public CAIBehavior
    {
    public:
        CAIBehaviorAttack(void);
        CAIBehaviorAttack(const char* pszName);
        virtual bool IsRunnable(void) override;
    };

    class CAIBehaviorHighAttack : public CAIBehavior
    {
    public:
        CAIBehaviorHighAttack(void);
        virtual bool IsRunnable(void) override;
    };

    class CAIBehaviorAvoidEnclose : public CAIBehavior
    {
    public:
        CAIBehaviorAvoidEnclose(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
    };

public:
    CBaseEliteAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseEliteAI(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    virtual void AttackCountInc(void);
    virtual void AttackCountDamage(int32 iAmount);
    virtual void CatchAttack(uint32 hAttacker);
    virtual void OnStatusEnd(ENEMYTYPES::STATUS statusEnd);
    void SetTerritory(ENEMYID::VALUE enemyId);
    bool WaitAIStartProc(void);
    void UpdateStatus(void);
    void UpdateAI(void);
    void UpdateChrFlags(void);
    void Attach(CAIBehavior* pBehavior, ELITEAI::PRIORITY pri);
    CEliteAIParam& AIParam(void);
    const CEliteAIParam& AIParam(void) const;
    CAIBehaviorManager& AIBehaviorMgr(void);
    const CAIBehaviorManager& AIBehaviorMgr(void) const;
    CEnemyCharacter& EnemyChr(void);
    const CEnemyCharacter& EnemyChr(void) const;

protected:
    CAIBehaviorManager* m_pAIBehaviorMgr;
    CEliteAIParam* m_pAIParam;
    float m_fAIStartTime;
};