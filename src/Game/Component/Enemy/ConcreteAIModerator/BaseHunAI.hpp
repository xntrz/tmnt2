#pragma once

#include "BaseAI6045.hpp"


class CBaseHunChr;


namespace BASEHUN_AI
{
    enum ORDERTYPE_ATTACK
    {  
        ORDERTYPE_ATTACK_A = BASEAI6045::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_B,
        ORDERTYPE_ATTACK_C,
        ORDERTYPE_ATTACK_D,
        ORDERTYPE_ATTACK_F,
        ORDERTYPE_ATTACK_F_RUN,
        ORDERTYPE_ATTACK_F_TALL,
    };

    enum CATCHTRIGGER : uint32
    {
        CATCHTRIGGER_COUNTER = (1 << 0),
        CATCHTRIGGER_BURST   = (1 << 1),
    };

    DEFINE_ENUM_FLAG_OPS(CATCHTRIGGER);

    namespace AIDECISIONUNITNAME
    {
        static const char* ATTACK_A                  = "AttackA";
        static const char* ATTACK_B                  = "AttackB";
        static const char* ATTACK_C                  = "AttackC";
        static const char* ATTACK_D                  = "AttackD";
        static const char* ATTACK_D_FOR_ESCAPE       = "AttackDForEscape";
        static const char* ATTACK_F                  = "AttackF";
        static const char* ATTACK_F_CONNECT_FROM_RUN = "AttackFRun";
        static const char* ATTACK_F_ON_TALL_OBJ      = "AttackFOnTallObj";
        static const char* ATTACK_REVENGE            = "AttackRevenge";
    };
};


class CBaseHunAI : public CBaseAI6045
{
public:
    class CDecisionUnitMove : public CBaseAI6045::CDecisionUnitMoveBoss
    {
    public:
        CDecisionUnitMove(CEnemyCharacter* pEnemyChr);
        virtual RESULT Update(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
    };

    class CDecisionUnitAttackA : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackA(void);
    };

    class CDecisionUnitAttackB : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackB(CEnemyCharacter* pEnemyChr);
        virtual bool CheckTerm(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
    };

    class CDecisionUnitAttackC : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackC(void);
    };

    class CDecisionUnitAttackD : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackD(void);
    };

    class CDecisionUnitAttackDForEscape : public CBaseAI6045::CDecisionUnitAttackCounter
    {
    public:
        CDecisionUnitAttackDForEscape(void);
    };

    class CDecisionUnitAttackF : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackF(CEnemyCharacter* pEnemyChr);
        virtual bool CheckTerm(void) override;

    private:
        CBaseHunChr* m_pBaseHunChr;
    };

    class CDecisionUnitAttackFConnectFromRun : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackFConnectFromRun(void);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
    };

    class CDecisionUnitAttackFOnTallObject : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackFOnTallObject(void);
        virtual bool CheckTerm(void) override;
    };

    class CDecisionUnitAttackRevenge : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackRevenge(CEnemyCharacter* pEnemyChr);
        virtual bool CheckTerm(void) override;
        virtual void SetAttackTypeNearRange(void);
        virtual void SetAttackTypeMiddleRange(void);
        virtual void SetAttackTypeFarRange(void);

    private:
        CBaseHunChr* m_pBaseHunChr;
    };

    class CDecisionUnitAttackCounter : public CBaseAI6045::CDecisionUnitAttackCounter
    {
    public:
        CDecisionUnitAttackCounter(void);
        virtual bool CheckTerm(void) override;
    };

public:
    CBaseHunAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseHunAI(void);
    virtual void CatchTriggerActionOrder(void) override;
};