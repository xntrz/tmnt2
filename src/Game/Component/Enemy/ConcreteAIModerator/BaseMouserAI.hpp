#pragma once

#include "BaseAI6045.hpp"


namespace BASEMOUSER_AI
{
    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_A = BASEAI6045::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_B,
        ORDERTYPE_ATTACK_C,
    };

    namespace AIDECISIONUNITNAME
    {
        static const char* ATTACK_A = "AttackA";
        static const char* ATTACK_B = "AttackB";
        static const char* ATTACK_C = "AttackC";
    };
};


class CBaseMouserAI : public CBaseAI6045
{
public:
    class CDecisionUnitAttackCommon : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackCommon(const char* pszUnitName, BASEMOUSER_AI::ORDERTYPE_ATTACK orderType);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
        bool CheckObstacleFront(float fDistance, float fSafeHeight);
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

    class CDecisionUnitAttackC : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackC(void);
        virtual bool CheckTerm(void) override;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseMouserAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseMouserAI(void);
};