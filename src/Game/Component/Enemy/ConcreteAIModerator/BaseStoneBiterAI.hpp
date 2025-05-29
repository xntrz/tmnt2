#pragma once

#include "BaseAI6045.hpp"

#include "Game/Component/Enemy/EnemyWatcher.hpp"


namespace BASESTONEBITER_AI
{
    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_SIDE_RIGHT = BASEAI6045::ORDERTYPE_MOVE_EXTEND,
        ORDERTYPE_MOVE_SIDE_LEFT,
    };

    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_A = 0,        
        ORDERTYPE_ATTACK_B,
        ORDERTYPE_ATTACK_C,
        ORDERTYPE_ATTACK_RUN,
    };

    namespace AIDECISIONUNITNAME
    {
        static const char* ATTACK_NEAR = "NearAttack";
        static const char* ATTACK_FAR  = "FarAttack";
        static const char* MOVE_SIDE   = "Yokoaruki";
    };
};


class CBaseStoneBiterAI : public CBaseAI6045
{
public:
    class CDecisionUnitBaseBiter : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitBaseBiter(CEnemyCharacter* pEnemyChr, const char* pszUnitName, CEnemyAIDecisionUnit::TYPE type);
        virtual bool CheckTerm(void) override;
        virtual RESULT Update(void) override;
        virtual void OnStart(void) override;
        virtual void OnEnd(void) override;
        void SetAttackOrder(int32 iOrderTypeAttack);
        void SetMoveOrder(int32 iOrderTypeMove);
        bool IsSatisfyFrequency(int32 iTableNo) const;

    protected:
        CEnemyWatcher m_enemyWatcher;
    };

    class CDecisionUnitAttackFar : public CDecisionUnitBaseBiter
    {
    public:
        CDecisionUnitAttackFar(CEnemyCharacter* pEnemyChr);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
    };

    class CDecisionUnitAttackNear : public CDecisionUnitBaseBiter
    {
    public:
        CDecisionUnitAttackNear(CEnemyCharacter* pEnemyChr);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
    };

    class CDecisionUnitMoveSide : public CDecisionUnitBaseBiter
    {
    public:
        CDecisionUnitMoveSide(CEnemyCharacter* pEnemyChr);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;

    private:
        int32 m_iPrevMoveType;
    };

    class CDecisionUnitMove : public CBaseAI6045::CDecisionUnitMove2
    {
    public:
        CDecisionUnitMove(CEnemyCharacter* pEnemyChr);
        virtual bool CheckTerm(void) override;
        virtual RESULT Update(void) override;
        virtual void OnStart(void) override;

    private:
        CEnemyWatcher m_enemyWatcher;        
    };

public:
    static CAIModerator* Intsance(CEnemyCharacter* pEnemyChr);    

    CBaseStoneBiterAI(CEnemyCharacter* pEnemyChr);
    virtual ~CBaseStoneBiterAI(void);
};