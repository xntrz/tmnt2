#pragma once

#include "Game/Component/Effect/EffectID.hpp"
#include "Game/Component/Effect/MagicID.hpp"
#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/PlayerWatcher.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CommonEnemyObserver.hpp"
#include "Game/System/Character/CharacterTypes.hpp"
#include "System/Common/Memory.hpp"


class CEnemyCharacter;
class CCharacterCompositor;
class CPlayerStateWatcher;


namespace Utils6034
{
    class IBaseAIModerator;


    /****************************************************************
     *                                                              *
     *                              Common                          *
     *                                                              *
     ****************************************************************/

    enum ORDERTYPE_WAIT
    {
        ORDERTYPE_WAIT_IDLE = 0,
        ORDERTYPE_WAIT_TURN_POS,
        ORDERTYPE_WAIT_TURN_TAR,

        ORDERTYPE_WAIT_EXTEND,
    };

    
    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_WALK_POS = 0,
        ORDERTYPE_MOVE_WALK_TAR,
        ORDERTYPE_MOVE_RUN_POS,
        ORDERTYPE_MOVE_RUN_TAR,
        ORDERTYPE_MOVE_TODO_4, // TODO 4
        ORDERTYPE_MOVE_DASH,
        ORDERTYPE_MOVE_DASH_POS,
        ORDERTYPE_MOVE_DASH_TAR,
        ORDERTYPE_MOVE_DASH_DIR,
        ORDERTYPE_MOVE_JUMP,
        ORDERTYPE_MOVE_JUMP_POS,
        ORDERTYPE_MOVE_JUMP_TAR,
        ORDERTYPE_MOVE_JUMP_DIR,

        ORDERTYPE_MOVE_EXTEND,
    };

    
    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_EXTEND = 0,        
    };


    bool IsDownState(ENEMYTYPES::STATUS status);
    bool IsFlyawayState(ENEMYTYPES::STATUS status);
    bool IsParallelVector(const RwV3d* vecA, const RwV3d* vecB);
    void MakeXByDirection(RwV3d* pvecAxisX, const RwV3d* pvecDireciton, const RwV3d* pvecUp);
    void MakeYByDirection(RwV3d* pvecAxisY, const RwV3d* pvecDireciton, const RwV3d* pvecUp);
    void MakeZByDirection(RwV3d* pvecAxisZ, const RwV3d* pvecDireciton);
    void MakeAxisByDirection(RwV3d* pvecAxisX, RwV3d* pvecAxisY, RwV3d* pvecAxisZ, const RwV3d* pvecDirection, const RwV3d* pvecUp);
    void GetCharacterDirectionVector(const CCharacterCompositor* pChrCompositor, RwV3d* pvecDirection);
    RwV3d GetPositionByThinkOrder(CAIThinkOrder* pAIThinkOrder);
    void GetWorldBonePosition(RwV3d* pvecWorldPos, const CCharacterCompositor* pChrCompositor, int32 iBoneID, const RwV3d* pvecOffset = nullptr);
    float CalcRotationRate(float fRate, float fSpeed);
    void CalcRotationPoint(RwV3d* pvecRotPt, const RwV3d* pvecPos, const RwV3d* pvecAt, float fMoveSpeed, float fMoveDist, bool bRotRight);
    void GetMovePosByWorld(RwV3d* pvecMovePos, const RwV3d* pvecPos, const RwV3d* pvecAt);
    

    class CSmallMemoryManager : public CMemory
    {
    public:
        static const int32 CHECK = 0x33EE44AA;

        struct MEMBLOCK : public CListNode<MEMBLOCK>
        {
#ifdef _DEBUG
            int32 check;
#endif /* _DEBUG */
            uint8 memory[];
        };

    public:
        CSmallMemoryManager(int32 iNodeSize, int32 iNodeMax);
        virtual ~CSmallMemoryManager(void);
        virtual void* RepAlloc(size_t size, const char* fname, int fline) override;
        virtual void RepFree(void* mem) override;
        virtual void* RepRwAlloc(size_t size, uint32 hint) override;
        virtual void RepRwFree(void* mem) override;
        virtual void* RepRwRealloc(void* mem, size_t size, uint32 hint) override;
        virtual void* RepRwCalloc(size_t objSize, size_t size, uint32 hint) override;
        virtual size_t AllocatedSize(void) override;
        void PushGlobal(void);
        void PopGlobal(void);

    protected:
        int32           m_iNodeSize;
        int32           m_iNodeMax;
        int32           m_iNodeAllocated;
        uint8*          m_pMemory;
        CList<MEMBLOCK> m_listMemBlockFree;
        CList<MEMBLOCK> m_listMemBlockAlloc;
        CMemory*        m_pPrev;
    };
    

    class CFixTimer
    {
    public:
        CFixTimer(void);
        CFixTimer(float fAlarmTime);
        ~CFixTimer(void);
        void Update(void);
        void UnPause(void);
        void Start(void);
        void SetElapseTime(float fTime);
        void SetCounterTime(float fTime);
        void AddCounterTime(float fAddTime);
        void SetAlarmTime(float fTime);
        void Reset(void);
        void Pause(void);
        bool IsPaused(void) const;
        bool IsCounterPaused(void) const;
        bool IsAlarm(void) const;
        float GetSystemTime(void) const;
        float GetRemainCounterTime(void) const;
        float GetElapseTime(void) const;

    private:
        bool  m_bPause;
        float m_fStartTime;
        float m_fElapseTime;
        float m_fAlarmTime;
        float m_fRemainCounterTime;
    };


    class CFlagController
    {
    public:
        CFlagController(void);
        ~CFlagController(void);
        void Update(void);
        void Start(void);
        void SetTime(float fTime);
        void SetCounter(int32 iCounter);
        bool IsEnableFlag(void) const;
        void IncCounter(void);
        void DecCounter(void);
        void Clear(void);
        void AddTime(float fTime);
        int32 GetCounter(void) const;
        const CFixTimer& GetTimer(void) const;
        
    private:
        CFixTimer m_timer;
        int32     m_iCounter;
    };


    class CDamageCounter
    {
    public:
        CDamageCounter(void);
        ~CDamageCounter(void);
        void Update(void);
        void Start(void);
        void SetOverflowData(int32 iOverflowCount, int32 iOverflowQuantity, float fAlaramTime);
        void Reset(void);
        bool IsOverflowDamage(void) const;
        void AddDamage(int32 iAmount);

    private:
        int32       m_iOverflowCount;
        int32       m_iOverflowQuantity;
        int32       m_iCurrentCount;
        int32       m_iCurrentQuantity;
        CFixTimer   m_resetTimer;
    };


    class COrderController
    {
    public:
        COrderController(void);
        ~COrderController(void);
        void Update(void);
        void SetTargetOrder(CAIThinkOrder* pAIThinkOrder);
        void ClearOrder(CAIThinkOrder* pAIThinkOrder) const;
        bool IsSendOrder(void) const;
        bool IsAcceptOrder(void) const;
        bool IsRefused(void) const;
        CAIThinkOrder& Lock(void);
        void Unlock(void);
        bool IsLocked(void) const;

    private:
        bool            m_bSendOrder;
        bool            m_bLocked;
        CAIThinkOrder*  m_pCurrentOrder;
        CAIThinkOrder   m_nextOrder;
    };


    /****************************************************************
     *                                                              *
     *                      Position tracer                         *
     *                                                              *
     ****************************************************************/

    class ITracer
    {
    public:
        ITracer(void);
        virtual ~ITracer(void);
        virtual const RwV3d& GetPosition(void) const = 0;
        virtual const RwV3d& GetDirection(void) const = 0;
        virtual void OnUpdate(void) = 0;
        void SetEnableDirection(bool bEnable);
        bool IsEnabledDirection(void) const;
        void SetEnableManualUpdate(bool bEnable);
        bool IsEnabledManualUpdate(void) const;

    protected:
        bool m_bEnableDirection;
        bool m_bEnableManualUpdate;
    };


    class CBoneTracer : public ITracer
    {
    public:
        CBoneTracer(void);
        virtual ~CBoneTracer(void);
        virtual const RwV3d& GetPosition(void) const override;
        virtual const RwV3d& GetDirection(void) const override;
        virtual void OnUpdate(void) override;
        void Initialize(const CCharacterCompositor* pChrCompositor, int32 iBoneID);
        void SetDirection(const RwV3d* pvecDirection);
        void SetOffset(const RwV3d* pvecOffset);

    protected:
        const CCharacterCompositor* m_pChrCompositor;
        int32 m_iBoneID;
        RwV3d m_vecOffset;
        RwV3d m_vecDirection;
        RwV3d m_vecPosition;
        RwV3d m_direction;
    };


    class CPositionTracer : public ITracer
    {
    public:
        CPositionTracer(void);
        virtual ~CPositionTracer(void);
        virtual const RwV3d& GetPosition(void) const override;
        virtual const RwV3d& GetDirection(void) const override;
        virtual void OnUpdate(void) override;
        void SetPosition(const RwV3d* pvecPosition);

    protected:
        RwV3d m_vecPosition;
        RwV3d m_vecDirection;
    };


    class CPlayerFootTracer : public ITracer
    {
    public:
        CPlayerFootTracer(void);
        virtual ~CPlayerFootTracer(void);
        virtual const RwV3d& GetPosition(void) const override;
        virtual const RwV3d& GetDirection(void) const override;
        virtual void OnUpdate(void) override;
        void Initialize(int32 iPlayerID);

    protected:
        int32 m_iPlayerID;
        RwV3d m_vecPosition;
    };

    
    /****************************************************************
     *                                                              *
     *                      Effect & magic trace                    *
     *                                                              *
     ****************************************************************/

    class ITraceEffect
    {
    public:
        ITraceEffect(void);
        virtual ~ITraceEffect(void);
        virtual void Start(void) = 0;
        virtual void Stop(void) = 0;
        virtual void Update(void) = 0;
        void Initialize(ITracer* pTracer);
        void SetScale(float fScale);
        void SetResetTime(float fTime);

    protected:
        ITracer*  m_pTracer;
        CFixTimer m_effectTimer;
        float     m_fScale;
    };


    class CTraceEffect : public ITraceEffect
    {
    public:
        CTraceEffect(void);
        CTraceEffect(EFFECTID::VALUE effectId, ITracer* pTracer);
        virtual ~CTraceEffect(void);
        virtual void Start(void) override;
        virtual void Stop(void) override;
        virtual void Update(void) override;
        void Initialize(EFFECTID::VALUE effectId, ITracer* pTracer);
        void Restart(void);

    protected:
        uint32          m_hEffect;
        EFFECTID::VALUE m_effectId;
    };


    class CTraceMagic : public ITraceEffect
    {
    public:
        CTraceMagic(void);
        CTraceMagic(MAGICID::VALUE magicId, ITracer* pTracer);
        virtual ~CTraceMagic(void);
        virtual void Start(void) override;
        virtual void Stop(void) override;
        virtual void Update(void) override;
        void Initialize(MAGICID::VALUE magicId, ITracer* pTracer);
        void Restart(void);

    protected:
        uint32         m_hMagic;
        MAGICID::VALUE m_magicId;
    };


    class CBodyElecEffectManager
    {
    public:
        CBodyElecEffectManager(void);
        ~CBodyElecEffectManager(void);
        void Update(void);
        void UpdateHeadToFoot(void);
        void UpdateFootToHead(void);
        void StartUpdate(void);
        void SwitchAllEffectOn(bool bOn);
        void SetUpdateStepTime(float fStepTime);
        void SetDispType(bool bToOn, bool bFootToHead);
        void SetCharacter(const CCharacterCompositor* pChrCompositor);
        void SetEffect(int32 iEffectIndex);

    protected:
        CTraceEffect m_aTraceEffect[10];
        CBoneTracer  m_aBoneTracer[10];
        bool         m_abSetFlag[10];
        float        m_fStepTime;
        float        m_fStartTime;
        float        m_fCurrentTime;
        bool         m_bToOn;
        bool         m_bFootToHead;
    };


    class CElecBallManager
    {
    public:
        CElecBallManager(void);
        ~CElecBallManager(void);
        void InitEffectCheckPos(const CCharacterCompositor* pChrCompositor);
        void ClearTargetPlayerList(void);
        void SetTargetPlayerList(const RwV3d* pvecCurrentPos);
        void SetTargetPlayer(const CPlayerCharacter* pTarget);
        void ShotMagic(const CCharacterCompositor* pChrCompositor, int32 iBoneHandL, int32 iBoneHandR);
        void PlayMagic(const RwV3d* pvecPosition, const RwV3d* pvecDirection, const CPlayerCharacter* pTarget);

    protected:
        const CPlayerCharacter* m_apPlrChr[GAMETYPES::PLAYERS_MAX];
        RwV3d m_aEffectCheckPos[GAMETYPES::PLAYERS_MAX];
    };


    /****************************************************************
     *                                                              *
     *                      Template mess                           *
     *                                                              *
     ****************************************************************/

    template<class T>
    class IMessageReceiver
    {
    public:
        virtual ~IMessageReceiver(void) {};
        virtual void ReceiveMessage(const T& mesage) = 0;
    };

    
    template<class T>
    class CMessageSender
    {
    public:
        inline CMessageSender(void) : CMessageSender(nullptr) {};
        inline CMessageSender(IMessageReceiver<T>* pReceiver) : m_pReceiver(nullptr) { SetReceiver(pReceiver); };
        inline void SetReceiver(IMessageReceiver<T>* pReceiver) { m_pReceiver = pReceiver; };
        inline void SendMessage(const T& message) const { m_pReceiver->ReceiveMessage(message); };

    private:
        IMessageReceiver<T>* m_pReceiver;
    };


    class IPolymorphMessage
    {
    public:
        inline virtual ~IPolymorphMessage(void) {};
        virtual void SendMessage(void) = 0;
    };


    template<class T>
    class CPolymorphCapsel : public IPolymorphMessage
    {
    public:
        inline CPolymorphCapsel(CMessageSender<T>& rSender, const T& data) : m_data(data), m_rSender(rSender) {};
        inline virtual ~CPolymorphCapsel(void) {};
        inline virtual void SendMessage(void) override { m_rSender.SendMessage(m_data); };

    private:
        T m_data;
        CMessageSender<T>& m_rSender;
    };


    template<class T>
    class TStack
    {
    public:
        inline TStack(void)
        : m_aStack()
        , m_iIndexCurrent(0)
        , m_iIndexMax(COUNT_OF(m_aStack))
        {};

        inline void Push(T data)
        {
            ASSERT(m_iIndexCurrent < m_iIndexMax);
            if (m_iIndexCurrent != m_iIndexMax)
                m_aStack[m_iIndexCurrent++] = data;
        };

        inline void Pop(void)
        {
            --m_iIndexCurrent;
        };

        inline T Peek(void)
        {
            ASSERT(m_iIndexCurrent > 0);
            return m_aStack[m_iIndexCurrent - 1];
        };

        inline bool IsEmpty(void) const
        {
            return (m_iIndexCurrent == 0);
        };        

    private:
        T    m_aStack[16];
        int8 m_iIndexCurrent;
        int8 m_iIndexMax;
    };

    
    /****************************************************************
     *                                                              *
     *                      AI contollers                           *
     *                                                              *
     ****************************************************************/

    class CProbabilityRecord
    {
    public:
        int8 iActionType;
        int8 aProbability[GAMETYPES::DIFFICULTY_NUM];
    };


    class CProbabilityTable
    {
    public:
        class CListData
        {
        public:
            int32               iListIndex;
            CProbabilityRecord* aRecords;
            int32               iRecordsNum;
        };

        class CProbabilityList
        {
        public:
            CProbabilityList(void);
            ~CProbabilityList(void);
            int8 CalcProbability(int32 iRecordIndex);
            void CopyListDataArray(CProbabilityRecord* aRecord, int32 iRecordNum);
            void Cleanup(void);

        protected:
            int32               m_iRecordNum;
            CProbabilityRecord* m_paRecordArray;
        };

    public:
        CProbabilityTable(int32 iListNum);
        ~CProbabilityTable(void);
        int8 CalcProbability(int32 iListIndex, int32 iRecordIndex);
        void CopyListDataArray(CListData** apListData, int32 iListNum);
        CProbabilityList* FindList(int32 iListIndex) const;

    protected:
        int32               m_iListNum;
        CProbabilityList*   m_paListArray;
    };


    class ICharacterController
    {
    public:
        enum MOVETYPE
        {
            MOVETYPE_MIN = 0,
            MOVETYPE_NORMAL,
            MOVETYPE_POSITION,
            MOVETYPE_DIRECTION,
            MOVETYPE_TARGET,

            MOVETYPE_MAX,
        };

    public:
        ICharacterController(void);
        virtual ~ICharacterController(void);
        virtual bool IsEnableChangeAction(void);
        virtual void DispatchAttack(int32 iAttackType, int32 iTargetNo);
        virtual void DispatchAttack(int32 iAttackType, const RwV3d* pvecTargetPos);
        virtual void DispatchMove(int32 iMoveType, int32 iTargetNo);
        virtual void DispatchMove(int32 iMoveType, const RwV3d* pvecTargetPos);
        virtual void DispatchTurnPos(const RwV3d* pvecTargetPos);
        virtual void DispatchTurnDirection(const RwV3d* pvecTargetDirection);
        virtual void DispatchTurnTarget(int32 iTargetNo);
        virtual void DispatchWalk(void);
        virtual void DispatchWalkPos(const RwV3d* pvecTargetPos);
        virtual void DispatchWalkDirection(const RwV3d* pvecTargetDirection);
        virtual void DispatchWalkTarget(int32 iTargetNo);
        virtual void DispatchRun(void);
        virtual void DispatchRunPos(const RwV3d* pvecTargetPos);
        virtual void DispatchRunDirection(const RwV3d* pvecTargetDirection);
        virtual void DispatchRunTarget(int32 iTargetNo);
        virtual void DispatchJump(void);
        virtual void DispatchJumpPos(const RwV3d* pvecTargetPos);
        virtual void DispatchJumpDirection(const RwV3d* pvecTargetDirection);
        virtual void DispatchJumpTarget(int32 iTargetNo);
        virtual void DispatchDash(void);
        virtual void DispatchDashPos(const RwV3d* pvecTargetPos);
        virtual void DispatchDashDirection(const RwV3d* pvecTargetDirection);
        virtual void DispatchDashTarget(int32 iTargetNo);
        virtual void DispatchGuard(void);
        virtual void OnUpdate(void);
        void Update(void);
        void Initialize(CEnemyCharacter* pEnemyChr, CAIThinkOrder* pAIThinkOrder, IBaseAIModerator* pBaseAIModerator);
        void StoreEnemyFlag(void);
        void SetEnemyFlag(void);
        void RestoreEnemyFlag(void);
        void SetOverflowData(int32 iOverflowCount, int32 iOverflowQuantity, float fAlarmTime);
        void Death(void);
        void SetMoveOrderTurn(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos = nullptr);
        void SetMoveOrderWalk(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos = nullptr);
        void SetMoveOrderRun(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos = nullptr);
        void SetMoveOrderJump(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos = nullptr);
        void SetMoveOrderDash(MOVETYPE eMoveType, int32 iTargetNo, const RwV3d* pvecTargetPos = nullptr);
        void SetGuardOrder(void);
        void SetAttackOrder(int32 iAttackType, int32 iTargetNo);
        void SetAttackOrder(int32 iAttackType, const RwV3d* pvecTargetPos);
        void SetMoveOrder(int32 iMoveType, int32 iTargetNo);
        void SetMoveOrder(int32 iMoveType, const RwV3d* pvecTargetPos = nullptr);
        void SendAIMessage(CHARACTERTYPES::ATTACKRESULTTYPE eAttackResultType, int32 iDamage = 0);
        bool IsValidMoveType(MOVETYPE eMoveType) const;
        bool IsResultKnockDamaged(CHARACTERTYPES::ATTACKRESULTTYPE eAttackResultType) const;
        bool IsFlyawayState(void) const;
        bool IsDownState(void) const;
        RwV3d GetAtForDirection(const RwV3d* pvecTargetPos) const;
        RwV3d GetAtForNormal(void) const;

        inline CFlagController& FlagNoReaction(void) { return m_flagNoReaction; };
        inline CFlagController& FlagInvincibility(void) { return m_flagInvincibility; };
        inline CDamageCounter& DamageCounter(void) { return m_damageCounter; };

        inline CEnemyCharacter& EnemyChr(void) { return *m_pEnemyChr; };
        inline const CEnemyCharacter& EnemyChr(void) const { return *m_pEnemyChr; };

        inline IBaseAIModerator& AIModerator(void) { return *m_pBaseAIModerator; };
        inline const IBaseAIModerator& AIModerator(void) const { return *m_pBaseAIModerator; };

        inline const COrderController& OrderController(void) const { return m_orderController; };
        
    protected:
        CEnemyCharacter*  m_pEnemyChr;
        IBaseAIModerator* m_pBaseAIModerator;
        COrderController  m_orderController;
        CDamageCounter    m_damageCounter;
        CFlagController   m_flagInvincibility;
        CFlagController   m_flagNoReaction;
        bool              m_bFlagStateInvinsibility;
        bool              m_bFlagStateNoReaction;
    };


    class ITargetManager
    {
    public:
        struct DISTANCE_DATA
        {
            int32 iPlayerNo;
            float fDistance;
        };

    public:
        ITargetManager(const CEnemyCharacter* pEnemyChr);
        virtual ~ITargetManager(void);
        virtual void OnStart(void) = 0;
        virtual void OnUpdate(void) = 0;
        void Update(void);
        void Start(void);
        void SortPlayerByDistance(void);
        bool IsTargetStun(void) const;
        bool IsTargetFlyaway(void) const;
        bool IsTargetAttack(void) const;
        bool IsTargetAttackTrigger(void) const;
        bool IsTargetSeeMe(void) const;
        bool GetNearestActivePlayer(int32* piPlayerNo, float* pfDist) const;
        RwV3d GetMyPos(void) const;
        CPlayerCharacter* GetTarget(void) const;
        RwV3d GetTargetPos(void) const;
        int32 GetTargetNo(void) const;
        float GetTargetDistance(void) const;
        float GetTargetDistanceSQ(void) const;
        float GetCurrentTargetDistance(void) const;
        const CPlayerStateWatcher& PlayerStateWatcher(void) const;

    protected:
        CFixTimer               m_timer;
        const CEnemyCharacter*  m_pEnemyChr;
        float                   m_fMyHitRadius;
        int32                   m_iTargetNo;
        float                   m_fTargetDistance;
        float                   m_fTargetHitRadius;
        float                   m_fTargetMaxDistForAttack;
        DISTANCE_DATA           m_aDistanceData[GAMETYPES::PLAYERS_MAX];
        CPlayerStateWatcher     m_playerStateWatcher;
    };


    class CNearestTargetSelector : public ITargetManager
    {
    public:
        CNearestTargetSelector(const CEnemyCharacter* pEnemyChr);
        virtual ~CNearestTargetSelector(void);
        virtual void OnStart(void) override;
        virtual void OnUpdate(void) override;        
    };


    class IAIState
    {
    public:
        static const int32 INVALID_STATE_HANDLE = -1;
        static const int32 FROMSTACK_STATE_HANDLE = INVALID_STATE_HANDLE;

        enum STARTRESULT
        {
            STARTRESULT_END = 0,
            STARTRESULT_IMMEDIATE_CHANGE,            
        };

        enum RUNRESULT
        {
            RUNRESULT_END = 0,
            RUNRESULT_CONTINUE,
        };

        enum RESULT_END
        {
            RESULT_END_FORCE = -2,
            RESULT_END_STRIDE,
        };

    public:
        IAIState(int32 iHandle);
        virtual ~IAIState(void);
        virtual STARTRESULT OnStart(void) = 0;
        virtual int32 OnEnd(void) = 0;
        virtual RUNRESULT OnRun(void) = 0;
        int32 GetHandle(void) const;

    private:
        int32 m_iHandle;
    };


    class CAIStateContext
    {
    public:
        CAIStateContext(int32 iStateMax);
        virtual ~CAIStateContext(void);
        virtual int32 OnGetNextState(int32 iEndResult);
        void RegistAIState(IAIState* pAIState);
        void SetDefaultState(int32 iStateHandle);
        void ChangeState(int32 iNextStateHandle);
        void ChangeStateForce(int32 iNextStateHandle);        
        IAIState::RUNRESULT RunState(void);
        void StartState(int32 iStateHandle);
        bool StartState_Sub(int32 iStateHandle);
        void StartDefaultState(void);
        int32 EndState(void);
        bool IsContainState(int32 iStateHandle) const;
        int32 GetDefaultState(void) const;
        int32 GetCurrentStateHandle(void) const;
        int32 GetForceChangeStateHandle(void) const;
        IAIState* GetCurrentState(void) const;
        int32 FindStateIndex(int32 iStateHandle) const;

    protected:
        int32       m_iCurrentStateNum;
        int32       m_iStateMax;
        IAIState**  m_apAIState;
        int32       m_iCurrentStateIndex;
        int32       m_iDefaultStateHandle;
        int32       m_iForceStateHandle;
        int32       m_iStateResult;
    };


    class CNormalAttackData
    {
    public:
        int32 iAttackType;
        int32 iTargetNo;
    };


    class CWaitData
    {
    public:
        float fWaitTime;
    };


    class IMessageStackAIStateContext : public CAIStateContext
    {
    private:
        union MEM_NODE
        {
            CPolymorphCapsel<CWaitData> wait;
            CPolymorphCapsel<CNormalAttackData> attack;

            inline ~MEM_NODE(void) {};
        };

        struct STACK_DATA
        {
            inline STACK_DATA(void)
                : iStateHandle(IAIState::INVALID_STATE_HANDLE), pMessage(nullptr) {};

            inline STACK_DATA(int32 _iStateHandle, IPolymorphMessage* _pMessage)
                : iStateHandle(_iStateHandle), pMessage(_pMessage) {};

            int32 iStateHandle;
            IPolymorphMessage* pMessage;
        };

    public:
        IMessageStackAIStateContext(int32 iStateMax);
        virtual ~IMessageStackAIStateContext(void);
        virtual int32 OnGetNextState(int32 iEndResult) override;
        void PushMessage(int32 iStateHandle, IPolymorphMessage* pMessage = nullptr);
        void PushMessage(int32 iStateHandle, const CNormalAttackData& data);
        void PushMessage(int32 iStateHandle, const CWaitData& data);
        void Cleanup(void);
        void SetReceiver(IMessageReceiver<CWaitData>* pReceiver);
        void SetReceiver(IMessageReceiver<CNormalAttackData>* pReceiver);

    private:
        CSmallMemoryManager                 m_memoryManager;
        TStack<STACK_DATA>                  m_messageStack;
        CMessageSender<CWaitData>           m_msgSenderWait;
        CMessageSender<CNormalAttackData>   m_msgSenderNormalAttack;
    };


    class IActionDispatcher
    {
    public:
        IActionDispatcher(int32 iProbabilityListNum);
        virtual ~IActionDispatcher(void);
        virtual bool DispatchNormalAttack(int32 iTargetNo, float fTargetDistanceSQ);
        virtual bool DispatchAerialCounterAttack(int32 iTargetNo);
        virtual bool DispatchCrowdCounterAttack(int32 iTargetNo);
        virtual bool DispatchDamagedAction(int32 iTargetNo, float fTargetDistanceSQ);
        virtual bool DispatchDefence(void);
        virtual int32 CalcMoveProbability(void) = 0; // returns state id of move dispatcher (see CBaseThinkingState::OnStart)
        virtual void OnInitialize(void) = 0;
        void Initialize(IMessageStackAIStateContext* pAIStateContext, ICharacterController* pCharacterController);

        inline ICharacterController& CharacterController(void) { return *m_pCharacterController; };
        inline const ICharacterController& CharacterController(void) const { return *m_pCharacterController; };

        inline IMessageStackAIStateContext& MessageStackAIStateContext(void) { return *m_pAIStateContext; };
        inline const IMessageStackAIStateContext& MessageStackAIStateContext(void) const { return *m_pAIStateContext; };

    protected:
        CProbabilityTable            m_probabilityTable;
        IMessageStackAIStateContext* m_pAIStateContext;
        ICharacterController*        m_pCharacterController;
    };


    class IBaseAIModerator : public CAIModerator
    {
    public:
        IBaseAIModerator(CEnemyCharacter* pEnemyChr, IActionDispatcher* pActionDispatcher, int32 iAIStateMax);
        virtual ~IBaseAIModerator(void);
        virtual void Draw(void) override;
        virtual void NotifyDamage(int32 iAmount);
        virtual void NotifyGuard(void);
        virtual void OnInitialize(void) = 0;
        void Initialize(ICharacterController* pCharacterController);

        inline ITargetManager& TargetManager(void) { return m_targetManager; };
        inline const ITargetManager& TargetManager(void) const { return m_targetManager; };

        inline IActionDispatcher& ActionDispatcher(void) { return *m_pActionDispatcher; };
        inline const IActionDispatcher& ActionDispatcher(void) const { return *m_pActionDispatcher; };

        inline ICharacterController& CharacterController(void) { return *m_pCharacterController; };
        inline const ICharacterController& CharacterController(void) const { return *m_pCharacterController; };

        inline IMessageStackAIStateContext& MessageStackAIStateContext(void) { return m_aiStateContext; };
        inline const IMessageStackAIStateContext& MessageStackAIStateContext(void) const { return m_aiStateContext; };

    protected:
        IMessageStackAIStateContext m_aiStateContext;
        CNearestTargetSelector      m_targetManager;
        IActionDispatcher*          m_pActionDispatcher;
        ICharacterController*       m_pCharacterController;
    };

    
    /****************************************************************
     *                                                              *
     *                          AI state                            *
     *                                                              *
     ****************************************************************/

    class IBaseAIState : public IAIState
    {
    public:
        static void SetInitParam(IBaseAIModerator* pBaseAIModerator);
        
        inline IBaseAIState(int32 iHandle) : IAIState(iHandle), m_pBaseAIModerator(m_pInitBaseAIModerator) {};
        inline virtual ~IBaseAIState(void) {};

        inline ITargetManager& TargetManager(void) { return AIModerator().TargetManager(); };
        inline const ITargetManager& TargetManager(void) const { return AIModerator().TargetManager(); };

        inline IActionDispatcher& ActionDispatcher(void) { return AIModerator().ActionDispatcher(); };
        inline const IActionDispatcher& ActionDispatcher(void) const { return AIModerator().ActionDispatcher(); };

        inline ICharacterController& CharacterController(void) { return AIModerator().CharacterController(); };
        inline const ICharacterController& CharacterController(void) const { return AIModerator().CharacterController(); };

        inline IMessageStackAIStateContext& MessageStackAIStateContext(void) { return AIModerator().MessageStackAIStateContext(); };
        inline const IMessageStackAIStateContext& MessageStackAIStateContext(void) const { return AIModerator().MessageStackAIStateContext(); };

        inline IBaseAIModerator& AIModerator(void) { return *m_pBaseAIModerator; };
        inline const IBaseAIModerator& AIModerator(void) const { return *m_pBaseAIModerator; };

    protected:
        static IBaseAIModerator* m_pInitBaseAIModerator;
        IBaseAIModerator*        m_pBaseAIModerator;
    };

    
    class CWaitState : public IBaseAIState,
                       public IMessageReceiver<CWaitData>
    {
    public:        
        CWaitState(int32 iHandle);
        virtual ~CWaitState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        virtual void ReceiveMessage(const CWaitData& data) override;

    protected:
        CFixTimer m_waitTimer;
    };

    
    class CNormalAttackState : public IBaseAIState,
                               public IMessageReceiver<CNormalAttackData>
    {
    public:
        CNormalAttackState(int32 iHandle);
        virtual ~CNormalAttackState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        virtual void ReceiveMessage(const CNormalAttackData& data) override;

    protected:
        int32 m_iAttackType;
        int32 m_iTargetNo;
    };


    class CBaseThinkingState : public IBaseAIState
    {
    protected:
        enum MOVESTATE
        {
            MOVESTATE_WALK = 0,
            MOVESTATE_RUN,

            MOVESTATENUM,
        };

    public:
        CBaseThinkingState(int32 iHandle, int32 iCtxStateMax);
        virtual ~CBaseThinkingState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        virtual bool OnAllTargetDown(void);
        virtual bool OnTargetAttack(int32 iTargetNo);
        virtual bool OnEndMove(void);

    protected:
        CAIStateContext m_moveStateCtx;
        bool m_bUpdateTargetMgr;
    };


    class CBaseGuardState : public IBaseAIState
    {
    public:
        CBaseGuardState(int32 iHandle);
        CBaseGuardState(int32 iHandle, float fGuardTime, float fGuardTimeMax);
        virtual ~CBaseGuardState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        int32     m_step;
        CFixTimer m_timerGuard;
        CFixTimer m_timerGuardTotal;
    };


    class CBaseDownState : public IBaseAIState
    {
    public:
        CBaseDownState(int32 iHandle, float fNoReactionAddTime);
        virtual ~CBaseDownState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        float m_fNoReactionAddTime;
    };

    
    class CBaseDamagedCounterState : public IBaseAIState
    {
    public:
        CBaseDamagedCounterState(int32 iHandle);
        virtual ~CBaseDamagedCounterState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override; 
    };

    
    class CBaseCounterDownState : public CBaseDownState
    {
    public:
        CBaseCounterDownState(int32 iHandle);
        CBaseCounterDownState(int32 iHandle, float fCounterMaxTargetDistSQ);
        CBaseCounterDownState(int32 iHandle, float fNoReactionAddTime, float fCounterMaxTargetDistSQ);
        virtual ~CBaseCounterDownState(void) {};
        virtual RUNRESULT OnRun(void) override;

    protected:
        float m_fCounterMaxTargetDistSQ;
    };


    class IBaseMoveState : public IBaseAIState
    {
    public:
        IBaseMoveState(int32 iHandle);
        virtual ~IBaseMoveState(void) {};

    protected:
        CFixTimer m_moveTimer;
        float     m_fMinTargetDistSQ;
        bool      m_bPauseTimer;
    };


    class CWalkDispatcher : public IBaseMoveState
    {
    public:
        CWalkDispatcher(int32 iHandle);
        virtual ~CWalkDispatcher(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
    };


    class CRunDispatcher : public IBaseMoveState
    {
    public:
        CRunDispatcher(int32 iHandle);
        virtual ~CRunDispatcher(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
    };


    class CCircleRunDispatcher : public IBaseMoveState
    {
    public:
        CCircleRunDispatcher(int32 iHandle);
        CCircleRunDispatcher(int32 iHandle, float fMoveTime, float fMinTargetDist, float fInRangeTime);
        virtual ~CCircleRunDispatcher(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        void OrderRunPos(void);
        void CalcRunPos(RwV3d* pvecRunPos, const RwV3d* pvecCurrentPos, float fRunMoveSpeed) const;

    protected:
        bool      m_bFlagDirection;
        CFixTimer m_timerInRange;
    };

    
    class CDashDispatcher : public IBaseMoveState
    {
    public:
        CDashDispatcher(int32 iHandle);
        virtual ~CDashDispatcher(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
    };


    class CCircleDashDispatcher : public IBaseMoveState
    {
    public:
        CCircleDashDispatcher(int32 iHandle);
        virtual ~CCircleDashDispatcher(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;
        bool OrderDash(void);
        void CalcDashPos(RwV3d* pvecDashPos, const RwV3d* pvecCurrentPos, float fDashMoveSpeed) const;

    protected:
        bool m_bFlagDirection;
    };


    class CShredderElecState : public IBaseAIState
    {
    public:
        CShredderElecState(int32 iHandle, int32 iAttackTypeDefault, CFixTimer* pElecTimer);
        virtual ~CShredderElecState(void) {};
        virtual STARTRESULT OnStart(void) override;
        virtual int32 OnEnd(void) override;
        virtual RUNRESULT OnRun(void) override;

    protected:
        CFixTimer*  m_pElecTimer;
        int32       m_iAttackTypeDefault;
    };


    /****************************************************************
     *                                                              *
     *                          Chr state                           *
     *                                                              *
     ****************************************************************/

    class CBaseHideObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual ~CBaseHideObserver(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };


    class CBaseDeathObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual ~CBaseDeathObserver(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        bool m_bIsNexusMap;
    };


    class CBaseThinkingObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual ~CBaseThinkingObserver(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual int32 OnGetOriginalWaitStatus(int32 iWaitType);
        virtual int32 OnGetOriginalMoveStatus(int32 iMoveType);
        virtual int32 OnGetOriginalAttackStatus(int32 iAttackType);

    protected:
        ENEMYTYPES::STATUS m_nextStatus;
    };


    class CBaseJumpReadyObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual ~CBaseJumpReadyObserver(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };


    class CBaseThrowObserver : public CCommonEnemyObserver::CBaseThrow
    {
    protected:
        static const int32 INVALID_SE = SDCODE_SE(4096);

    public:
        CBaseThrowObserver(ICharacterController* pCharacterController);
        CBaseThrowObserver(int32 seID, ICharacterController* pCharacterController);
        virtual ~CBaseThrowObserver(void) {};
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void OnThrowStandby(void) override;
        virtual void OnLift(void) override;
        virtual void OnThrowHit(void) override;
        void SetSE(int32 seID);

    protected:
        int32                 m_orderTargetNo;
        int32                 m_seID;
        ICharacterController* m_pCharacterController;
        bool                  m_bThrowing;
    };


    class CGuardObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual ~CGuardObserver(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        int32 m_step;
    };


    class CBaseOneShotAttack : public CCommonEnemyObserver::COneShotMotion
    {
    public:
        CBaseOneShotAttack(const char* pszMotion);
        virtual ~CBaseOneShotAttack(void) {};
        virtual RESULT Observing(void) override;
    };


    class CBaseOneShotTurnAttack : public CBaseOneShotAttack
    {
    public:
        CBaseOneShotTurnAttack(const char* pszMotion);
        virtual ~CBaseOneShotTurnAttack(void) {};
        virtual void OnStart(void) override;
    };


    class CElecAttackObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        CElecAttackObserver(const CCharacterCompositor* pChrCompositor,
                            const char* pszMotionName,
                            ICharacterController* pChrController);
        virtual ~CElecAttackObserver(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void ShotElball(void) const;
        void ShotElballMagic(const RwV3d* pvecPosition, const RwV3d* pvecDirection) const;
        void UpdateTiming(void);
        void UpdateRotation(void);
        void UpdateHandEffect(void);
        void UpdateBodyEffectMgr(void);

    protected:
        int32                   m_stepTiming;
        int32                   m_stepBodyEffect;
        CBodyElecEffectManager  m_elecEffectMgr;
        CTraceEffect            m_aHandTraceEffect[2];
        CBoneTracer             m_aHandBoneTracer[2];
        CElecBallManager        m_elecBallMgr;
        CFixTimer               m_timerRotation;
        CFixTimer               m_timerBodyEffect;
        CFixTimer               m_timerHandEffect;
        const char*             m_pszMotionName;
        ICharacterController*   m_pChrController;
        bool                    m_bFlagNoReactionChanged;
    };


    class CTenguAttackObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        CTenguAttackObserver(void);
        virtual ~CTenguAttackObserver(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void ShotTenguSword(void) const;
        void ShotTenguSwordMagic(const RwV3d* pvecPosition, const RwV3d* pvecDirection) const;

    protected:
        int32 m_step;
    };

    
    class CChargeAttackObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        CChargeAttackObserver(ICharacterController* pChrController);
        virtual ~CChargeAttackObserver(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        int32 m_step;
        ICharacterController* m_pChrController;
        RwV3d m_vecVelocity;
        float m_fMoveTimer;
    };

    
    class CDashStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual ~CDashStatus(void) {};
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        float m_fDashTime;
    };
};