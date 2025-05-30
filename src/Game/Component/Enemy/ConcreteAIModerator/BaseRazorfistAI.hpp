#pragma once

#include "BaseAI6045.hpp"


namespace BASERAZORFIST_AI
{
    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_A = BASEAI6045::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_B,
        ORDERTYPE_ATTACK_C,
        ORDERTYPE_ATTACK_D,
    };

    enum CATCHTRIGGER : uint32
    {
        CATCHTRIGGER_COUNTER = (1 << 0),
    };

    DEFINE_ENUM_FLAG_OPS(CATCHTRIGGER);

    namespace AIDECISIONUNITNAME
    {
        static const char* ATTACK_A = "AttackA";
        static const char* ATTACK_B = "AttackB";
        static const char* ATTACK_C = "AttackC";
        static const char* ATTACK_D = "AttackD";
    };
};


class CBaseRazorfistAI : public CBaseAI6045
{
private:
    class CDecisionUnitAttackCommon : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackCommon(const char* pszUnitName, BASERAZORFIST_AI::ORDERTYPE_ATTACK orderType);
        virtual void OnStart(void) override;
    };

    class CDecisionUnitAttackA : public CDecisionUnitAttackCommon
    {
    public:
        CDecisionUnitAttackA(void);
    };

    class CDecisionUnitAttackB : public CDecisionUnitAttackCommon
    {
    public:
        CDecisionUnitAttackB(void);
    };

    class CDecisionUnitAttackC : public CDecisionUnitAttackCommon
    {
    public:
        CDecisionUnitAttackC(void);
    };

    class CDecisionUnitAttackD : public CDecisionUnitAttackCommon
    {
    public:
        CDecisionUnitAttackD(void);
    };

    class CDecisionUnitAttackCounter : public CBaseAI6045::CDecisionUnitAttackCounter
    {
    public:
        CDecisionUnitAttackCounter(void);
        virtual bool CheckTerm(void) override;
        void ResetTarget(void);
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseRazorfistAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseRazorfistAI(void);
    virtual void CatchTriggerActionOrder(void) override;
};