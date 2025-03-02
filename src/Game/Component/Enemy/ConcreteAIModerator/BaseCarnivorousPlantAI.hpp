#pragma once

#include "BaseAI6045.hpp"


namespace BASECARNIVOROUSPLANT_AI
{
    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_A = BASEAI6045::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_B,
    }; /* enum ORDERTYPE_ATTACK */

    namespace AIDECISIONUNITNAME
    {
        static const char* ATTACK_A = "AttackA";
        static const char* ATTACK_B = "AttackB";        
    }; /* namespace AIDECISIONUNITNAME */
}; /* namespace BASECARNIVOROUSPLANT_AI */


class CBaseCarnivorousPlantAI : public CBaseAI6045
{
public:
    class CDecisionUnitAttackA : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackA(void);
    };

    class CDecisionUnitAttackB : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackB(void);
    };
    
public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    CBaseCarnivorousPlantAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseCarnivorousPlantAI(void);
};