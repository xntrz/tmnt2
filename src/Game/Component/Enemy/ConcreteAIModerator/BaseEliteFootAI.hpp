#pragma once

#include "BaseEliteAI.hpp"


namespace ELITEFOOTAI
{
    static const int32 AVOID_ATTACK_REACTION_DAMAGE = 40;

    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_WARP_NO = ELITEAI::ORDERTYPE_MOVE_EXTEND,
        ORDERTYPE_MOVE_WARP_POS,
    };

    enum OREDERTYPE_ATTACk
    {
        /*  5 */ ORDERTYPE_ATTACK_SPINDOWN = ELITEAI::ORDERTYPE_ATTACK_EXTEND,
        /*  6 */ ORDERTYPE_ATTACK_SPIN,
        /*  7 */ ORDERTYPE_ATTACK_SWING,
        /*  8 */ ORDERTYPE_ATTACK_DROP,
        /*  9 */ ORDERTYPE_ATTACK_SPEAR,
        /* 99 */ ORDERTYPE_ATTACK_BUNSHIN,
    };

    enum FREQUENCY_INDEX
    {
        FREQUENCY_DASH = ELITEAI::FREQUENCY_EXTEND,
        FREQUENCY_ATTACK_SPECIAL,
        FREQUENCY_ATTACK_SPECIAL_EX,
        FREQUENCY_WAIT,
    };

    namespace AIBEHAVIORNAME
    {
        static const char* BUNSHIN      = "Bunshin";
        static const char* ATTACKSPEAR  = "AttackSpear";
        static const char* DEFEND       = "Defend";
    };
};


class CBaseEliteFootAI : public CBaseEliteAI
{
public:
    class CAIBehaviorMove : public CBaseEliteAI::CAIBehaviorMove
    {
    public:
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
    };

    class CAIBehaviorNormalAttack : public CBaseEliteAI::CAIBehaviorAttack
    {
    public:
        CAIBehaviorNormalAttack(void);
        CAIBehaviorNormalAttack(const char* pszName);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        bool m_bSpinDown;
        float m_fAttackTimeout;
    };

    class CAIBehaviorAvoidAttack : public CAIBehaviorNormalAttack
    {
    public:
        CAIBehaviorAvoidAttack(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual RESULT Update(void) override;
        virtual void OnEnd(void) override;
        void Guard(void);
        void Avoid(void);
        void AfterAvoidSet(void);

    protected:
        bool m_bAvoidOnly;
    };

    class CAIBehaviorHighAttack : public CBaseEliteAI::CAIBehaviorHighAttack
    {
    public:
        virtual void OnActive(void) override;
    };

    class CAIBehaviorSpearAttack : public CAIBehavior
    {
    public:
        CAIBehaviorSpearAttack(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        float m_fSpearTimeout;
    };

    class CAIBehaviorDefend : public CAIBehavior
    {
    public:
        CAIBehaviorDefend(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        void GetMyTerritoryCenter(RwV3d* pvecTerritoryCenter) const;

    protected:
        float m_fWarpTime;
        float m_fDefendTime;
        RwV3d m_avecWaitPosOffset[4]; // elite guard + 3 shadow dummies
    };

    class CAIBehaviorBunshin : public CAIBehavior
    {
    public:
        CAIBehaviorBunshin(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual void OnEnd(void) override;
        void Reset(void);

    protected:
        int32 m_phase;
    };

public:
    static bool IsAttackEnabled(CEnemyCharacter* pEnemyChr, int32 attackType);
    static bool IsEliteCStage(void);
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseEliteFootAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseEliteFootAI(void);
    virtual void Run(void) override;
    virtual void AttackCountDamage(int32 iAmount) override;
    virtual void CatchAttack(uint32 hAttacker) override;
    virtual void OnStatusEnd(ENEMYTYPES::STATUS statusEnd) override;

private:
    float m_fAttackCounterClearTimer;
};


class CBaseEliteFootDummyAI : public CBaseEliteAI
{
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);
    
    CBaseEliteFootDummyAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseEliteFootDummyAI(void);    
};