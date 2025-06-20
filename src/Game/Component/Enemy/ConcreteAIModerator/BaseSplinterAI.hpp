#pragma once

#include "BaseEliteAI.hpp"


namespace SPLINTERAI
{
    static const int32 ATTACK_COUNT_FOR_AVOID = 5;
    
    enum ORDERTYPE_ATTACK
    {
        /* 14 */ ORDERTYPE_ATTACK_KICK = ELITEAI::ORDERTYPE_ATTACK_EXTEND,
        /* 15 */ ORDERTYPE_ATTACK_RUN,
        /* 19 */ ORDERTYPE_ATTACK_B,
        /* 20 */ ORDERTYPE_ATTACK_B_CHARGE,
        /* 21 */ ORDERTYPE_ATTACK_SHOT,
        /* 22 */ ORDERTYPE_ATTACK_SHOT_JUMP,
        /* 23 */ ORDERTYPE_ATTACK_THROW,
    };

    enum FREQUENCY_INDEX
    {
        FREQUENCY_ATTACK_NORMAL = ELITEAI::FREQUENCY_EXTEND,
        FREQUENCY_ATTACK_NORMALA,
        FREQUENCY_ATTACK_NORMALB,
        FREQUENCY_ATTACK_B,
        FREQUENCY_ATTACK_KICK,
        FREQUENCY_ATTACK_RUN,
        FREQUENCY_ATTACK_B_CHARGE,
        FREQUENCY_ATTACK_THROW,
        FREQUENCY_ATTACK_SHOT,
        FREQUENCY_ATTACK_SHOT_JUMP,
        FREQUENCY_WAIT,
    };

    namespace AIBEHAVIORNAME
    {
        static const char* THROW        = "Throw";
        static const char* ATTACK_KICK  = "AttackKick";
        static const char* ATTACK_A     = "AttackA";
        static const char* ATTACK_B     = "AttackB";
        static const char* SHOT         = "Shot";
    };
};


class CBaseSplinterAI : public CBaseEliteAI
{
public:
    class CAIBehaviorAvoid : public CAIBehavior
    {
    public:
        CAIBehaviorAvoid(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual void OnEnd(void) override;
    };

    class CAIBehaviorEnclose : public CBaseEliteAI::CAIBehaviorAvoidEnclose
    {
    public:
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;        
    };

    class CAIBehaviorHighKickAttack : public CBaseEliteAI::CAIBehaviorHighAttack
    {
    public:
        virtual void OnActive(void) override;
    };

    class CAIBehaviorThrowAttack : public CAIBehavior
    {
    public:
        CAIBehaviorThrowAttack(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        float m_fThrowTimeout;
    };

    class CAIBehaviorAntiGuard : public CAIBehavior
    {
    public:
        CAIBehaviorAntiGuard(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        float m_fAntiGuardTimeout;
    };

    class CAIBehaviorNearAttack : public CAIBehavior
    {
    public:        
        CAIBehaviorNearAttack(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        float m_fNearAttackTimeout;
    };

    class CAIBehaviorFarAttack : public CBaseEliteAI::CAIBehaviorAttack
    {
    protected:
        enum ATTACKTYPE
        {
            ATTACKTYPE_NONE = 0,
            ATTACKTYPE_A,
            ATTACKTYPE_B,
        };

    public:
        CAIBehaviorFarAttack(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        float m_fFarAttackTimeout;
        int32 m_attackType;
    };

    class CAIBehaviorShot : public CAIBehavior
    {
    public:
        CAIBehaviorShot(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        float m_fShotTimeout;
    };

    class CAIBehaviorMove : public CBaseEliteAI::CAIBehaviorMove
    {
    public:        
        virtual void OnActive(void) override;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseSplinterAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseSplinterAI(void);
    virtual void AttackCountInc(void) override;
    virtual void CatchAttack(uint32 hObj, GAMEOBJECTTYPE::VALUE objType) override;
};