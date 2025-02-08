#pragma once

#include "Game/Component/Enemy/AIModerator.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"


namespace AIOT
{
    enum TYPE
    {
        Null            = 0,

        MoveTurn        = 1,
        MoveWalkLeft    = 2,
        MoveWalkRight   = 3,
        MoveWalk        = 4,
        MoveRun         = 5,        
        MoveJump        = 6,

        AttackA         = 7,
        AttackAA        = 8,
        AttackAAA       = 9,
        AttackB         = 10,
        AttackC         = 11,
        AttackRun       = 12,
        FireA           = 13,
        FireB           = 14,
        FireC           = 15,
        FireRepeatA     = 16, // FIRE_AA
        FireRepeatB     = 17, // FIRE_BB
        FireRepeatC     = 18, // FIRE_CC
        Special1        = 19,
    };

    enum
    {
        TargetNo        = (~TYPEDEF::SINT32_MAX),
    };

    int32 Set(int32 idx,        // idx - is subtype of order (for move its MoveType, for attack - AttackType and so on)
              int32 target);    // target - is flag indicator only bit, that indicates order type to player (PlayerNo field) or to point

    int32 Get(int32 type);
    int32 GetTarget(int32 type);
    void SetThinkOrder(CAIThinkOrder& rAIThinkOrder, CAIThinkOrder::ORDER eOrder, int32 eTypeIdx, float fTimer, RwV3d* pvVec = nullptr);
    void SetThinkOrder(CAIThinkOrder& rAIThinkOrder, CAIThinkOrder::ORDER eOrder, int32 eTypeIdx, float fTimer, int32 iNumber);
    void SetWaitOrder(CAIThinkOrder& rAIThinkOrder, float fTimer, RwV3d* pvVec = nullptr);
    void SetMoveOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, float fTimer, RwV3d* pvVec = nullptr);
    void SetMoveOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, float fTimer, int32 iNumber);
    void SetAttackOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, int32 iNumber);
    void SetFireOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, int32 iNumber, RwV3d* pvVec = nullptr);
    void SetDefenceOrder(CAIThinkOrder& rAIThinkOrder);
    void SetNothingOrder(CAIThinkOrder& rAIThinkOrder);
    void SetRunOrder(CAIThinkOrder& rAIThinkOrder, RwV3d* pvAt);
};


class CTarget : public CListNode<CTarget>
{
public:
    static const float HateMax;
    
    class ICalcHate : public CListNode<ICalcHate>
    {
    public:        
        struct ReferenceInfo
        {
            CAIModerator* m_pAIModerator;
            CEnemyCharacter* m_pEnemyCharacter;
        };

    public:        
        virtual ~ICalcHate(void) {};
        virtual float Importance(void) = 0;
        virtual float Get(int32 iNo, ReferenceInfo& rReferenceInfo) = 0;
    };
    
    class ICalcHateOfGeometry final : public ICalcHate
    {
    public:        
        virtual float Importance(void) override;
        virtual float Get(int32 iNo, ReferenceInfo& rReferenceInfo) override;
    };

    class ICalcHateOfCollision final : public ICalcHate
    {
    public:
        virtual float Importance(void) override;
        virtual float Get(int32 iNo, ReferenceInfo& rReferenceInfo) override;
    };

    class ICalcHateOfTargetDamaged final : public ICalcHate
    {
    public:
        virtual float Importance(void) override;
        virtual float Get(int32 iNo, ReferenceInfo& rReferenceInfo) override;
    };

    class ICalcHateOfDamageAccumulation final : public ICalcHate
    {
    public:
        virtual float Importance(void) override;
        virtual float Get(int32 iNo, ReferenceInfo& rReferenceInfo) override;
    };

    class ICalcHateOfTurtleCharacteristic final : public ICalcHate
    {
    public:
        virtual float Importance(void) override;
        virtual float Get(int32 iNo, ReferenceInfo& rReferenceInfo) override;
    };

    class ICalcHateOfTurtleStatus final : public ICalcHate
    {
    public:
        virtual float Importance(void) override;
        virtual float Get(int32 iNo, ReferenceInfo& rReferenceInfo) override;
    };

    class ICalcHateOfRandom final : public ICalcHate
    {
    public:
        virtual float Importance(void) override;
        virtual float Get(int32 iNo, ReferenceInfo& rReferenceInfo) override;
    };

public:
    CTarget(int32 iNo);
    virtual ~CTarget(void);
    virtual void Update(ICalcHate::ReferenceInfo& rReferenceInfo);
    virtual void Regist(ICalcHate* pCalcHate);
    virtual void Remove(ICalcHate* pCalcHate);
    virtual int32 GetNumber(void) const;
    virtual float GetHate(void) const;

private:
    CList<ICalcHate> m_listRoot;
    int32 m_iNumber;
    float m_fHate;
};


class CTargetCoordinator
{
public:
    CTargetCoordinator(void);
    virtual ~CTargetCoordinator(void);
    virtual void SetReferenceInfo(CTarget::ICalcHate::ReferenceInfo& rReferenceInfo);
    virtual void Update(void);
    bool RegistForHateCalculator(int32 iTarget, CTarget::ICalcHate* pCalcHate);
    int32 MaxTarget(void) const;
    CTarget* CurrentTarget() const;
    bool CanTheTargetBeChanged(void) const;
    void SetPermission(bool bState);

private:
    bool m_bPermission;
    CTarget* m_pBeforeTarget;
    CTarget* m_pCurrentTarget;
    CList<CTarget> m_listRoot;
    CTarget::ICalcHate::ReferenceInfo m_referenceInfo;
};


class CTargetInfo
{
public:
    CTargetInfo(CEnemyCharacter* pEnemyChr);
    virtual ~CTargetInfo(void);
    virtual void Update(void);
    virtual void Set(int32 iNo);
    virtual void Reset(void);
    virtual int32 Get(void) const;
    virtual float GetDistance(void) const;
    virtual float GetAngle(void) const;
    virtual CAIUtils::PLAYER_STATE_FLAG GetState(void) const;
    virtual bool TestState(CAIUtils::PLAYER_STATE_FLAG eFlag) const;

private:
    CEnemyCharacter* m_pEnemyChr;
    mutable bool m_bExist;
    mutable int32 m_iNo;
    float m_fDistance;
    float m_fAngle;
    CAIUtils::PLAYER_STATE_FLAG m_eState;
};


class CSpecialityConsider
{
public:
    enum STATE_FLAG : uint32
    {
        STATE_FLAG_NONE     = 0,
        //STATE_FLAG_0x1    = (1 << 0), // TODO unknown flag
        STATE_FLAG_DOWN     = (1 << 1),
        //STATE_FLAG_0x4    = (1 << 2), // TODO unknown flag
        STATE_FLAG_WALK     = (1 << 3),
        STATE_FLAG_RUN      = (1 << 4),
    };

    DEFINE_ENUM_FLAG_OPS(STATE_FLAG, friend);

public:
    CSpecialityConsider(void);
    virtual ~CSpecialityConsider(void);
    virtual void Update(void);
    virtual void SetEnemyCharacter(const CEnemyCharacter* pEnemyChr);
    virtual bool TestState(uint32 state);

private:
    const CEnemyCharacter* m_pEnemyChr;
    uint32 m_state;
};


class CBaseGeneralEnemyAIModerator : public CAIModerator
{
public:
    enum UNDERACTION
    {
        UNDERACTIONS_CONTINUOUS = 0,
        UNDERACTIONS_FORCE_CHANGED,
        UNDERACTIONS_THINKOVER,
    };

    enum SUITABLEDISTANCE
    {
        SUITABLEDISTANCE_OUTER = 0,
        SUITABLEDISTANCE_INNER,
        SUITABLEDISTANCE_EQUAL,
    };

public:
    static SUITABLEDISTANCE GetDistanceOfSuitable(float fDistance, float fDistanceOfSuitable);
    
    CBaseGeneralEnemyAIModerator(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseGeneralEnemyAIModerator(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    virtual CAIThinkOrder::ORDER ActionSelector(int32 iNo) = 0;
    virtual bool OnActionOfWait(int32 iNo) { return false; };
    virtual bool OnActionOfMove(int32 iNo) { return false; };
    virtual bool OnActionOfRun(int32 iNo) { return false; };
    virtual bool OnActionOfAttack(int32 iNo) { return false; };
    virtual bool OnActionOfDefence(int32 iNo) { return false; };
    virtual UNDERACTION OnUnderAllActions(void) { return UNDERACTIONS_CONTINUOUS; };
    virtual UNDERACTION OnUnderWait(void) { return UNDERACTIONS_CONTINUOUS; };
    virtual UNDERACTION OnUnderMove(void);
    virtual UNDERACTION OnUnderRun(void) { return UNDERACTIONS_CONTINUOUS; };
    virtual UNDERACTION OnUnderAttack(void) { return UNDERACTIONS_CONTINUOUS; };
    virtual UNDERACTION OnUnderDefence(void) { return UNDERACTIONS_CONTINUOUS; };
    virtual bool IsOutsidePatrolArea(void);
    virtual bool IsTakeMove(void);
    virtual bool IsMoveEndForTargetNumber(void) = 0;
    virtual bool IsMoveEndForTargetPosition(void) = 0;
    virtual AIOT::TYPE IsTakeSideWalk(void);
    UNDERACTION UnderThinking(void);
    void Thinking(void);
    bool EnableThinking(void);
    bool IsSatifyFrequency(int32 no) const;
    float GetFrequency(int32 no) const;
    int32 SearchTarget(void);
    void DebugDrawMovePoint(void);
    void DebugDrawPatrolArea(void);
    void DebugDrawInfo(void);
    void DebugDrawSuitableArea(void);
    void DebugDrawViewArea(void);

protected:
    CTargetCoordinator m_targetCoordinator;
    CTargetInfo m_targetInfo;
    CSpecialityConsider* m_pSpecialityConsider;
    float m_fThinkElapseTime;
    float m_fThinkBeforeTime;
    bool m_bActionRun;
};