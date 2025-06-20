#pragma once

#include "BaseEliteAI.hpp"


namespace USAGIAI
{
    static const int32 COUNTER_ATTACK_REACTION_DAMAGE = 40;

    enum ORDERTYPE_ATTACK
    {
        /* 10 */ ORDERTYPE_ATTACK_DASH = ELITEAI::ORDERTYPE_ATTACK_EXTEND,
        /* 11 */ ORDERTYPE_ATTACK_DASH_PLAYER_1,
        /* 12 */ ORDERTYPE_ATTACK_DASH_PLAYER_2_3,
        /* 13 */ ORDERTYPE_ATTACK_DASH_PLAYER_4,
        /* 14 */ ORDERTYPE_ATTACK_KICK,
        /* 15 */ ORDERTYPE_ATTACK_RUN,
        /* 16 */ ORDERTYPE_ATTACK_AVOID,
        /* 17 */ ORDERTYPE_ATTACK_COUNTER,
        /* 18 */ ORDERTYPE_ATTACK_MUTEKI,
    };

    enum FREQUENCY_INDEX
    {
        FREQUENCY_ATTACK_NORMAL = ELITEAI::FREQUENCY_EXTEND,
        FREQUENCY_ATTACK_NORMALA,
        FREQUENCY_ATTACK_NORMALB,
        FREQUENCY_ATTACK_KICK,
        FREQUENCY_ATTACK_RUN_SIDE,
        FREQUENCY_MOVE_CONTINUE,
        FREQUENCY_ATTACK_ALL_PLR,
        FREQUENCY_ATTACK_STAB,
    };

    namespace AIBEHAVIORNAME
    {
        static const char* AVOID            = "Avoid";
        static const char* ATTACK_KICK      = "AttackKick";
        static const char* ATTACK_RUN       = "AttackRun";
        static const char* ATTACK_COUNTER   = "AttackCounter";
        static const char* ATTACK_MUTEKI    = "AttackMuteki";
        static const char* ATTACK_ALLPLAYER = "AttackAllPlayer";
        static const char* ATTACK_STAB      = "AttackStab";
    };
};


class C085MiyamotoUsagiAI : public CBaseEliteAI
{
public:
    class CAIBehaviorMove : public CBaseEliteAI::CAIBehaviorMove 
    {
    public:
        virtual void OnActive(void) override;
    };

    class CAIBehaviorAvoid : public CAIBehavior 
    {
    public:
        CAIBehaviorAvoid(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
    };

    class CAIBehaviorAttack : public CBaseEliteAI::CAIBehaviorAttack 
    {
    public:
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual RESULT Update(void) override;
    };

    class CAIBehaviorKickAttack : public CAIBehavior 
    {
    public:
        CAIBehaviorKickAttack(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        float m_fKickTimeout;
    };

    class CAIBehaviorRunAttack : public CAIBehavior 
    {
    public:
        CAIBehaviorRunAttack(void);
        virtual void Initialize(void) override;
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;

    protected:
        float m_fRunAttackTimeout;
    };

    class CAIBehaviorAvoidAttack : public CAIBehavior 
    {
    public:
        CAIBehaviorAvoidAttack(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual void OnEnd(void) override;
    };

    class CAIBehaviorCounterAttack : public CAIBehavior 
    {
    public:
        CAIBehaviorCounterAttack(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual void OnEnd(void) override;
    };

    class CAIBehaviorMutekiGuard : public CAIBehavior 
    {
    public:
        CAIBehaviorMutekiGuard(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual RESULT Update(void) override;
        virtual void OnEnd(void) override;

    protected:
        float m_fMutekiDuration;
    };

    class CAIBehaviorAllPlayerAttack : public CAIBehavior 
    {
    public:
        CAIBehaviorAllPlayerAttack(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
    };

    class CAIBehaviorStabAttack : public CAIBehavior 
    {
    public:
        CAIBehaviorStabAttack(void);
        virtual bool IsRunnable(void) override;
        virtual void OnActive(void) override;
        virtual RESULT Update(void) override;
        virtual void OnEnd(void) override;

    protected:
        float m_fStabAttackDuration;
        int32 m_numRollBack;
    };

    class CAIBehaviorHighRotateAttack : public CBaseEliteAI::CAIBehaviorHighAttack 
    {
    public:
        virtual void OnActive(void) override;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C085MiyamotoUsagiAI(CEnemyCharacter* pEnemyChr);
    virtual ~C085MiyamotoUsagiAI(void);
    virtual void AttackCountInc(void) override;
    virtual void AttackCountDamage(int32 iAmount) override;
    virtual void CatchAttack(uint32 hObj, GAMEOBJECTTYPE::VALUE objType) override;
    virtual void OnStatusEnd(ENEMYTYPES::STATUS statusEnd) override;
};