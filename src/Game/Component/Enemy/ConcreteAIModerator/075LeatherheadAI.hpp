#pragma once

#include "BaseAI6045.hpp"


class CBaseLeatherheadChr;


namespace LEATHERHEAD_AI
{
    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_SWIM = BASEAI6045::ORDERTYPE_MOVE_EXTEND,
    }; /* enum ORDERTYPE_MOVE */

    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_E = BASEAI6045::ORDERTYPE_ATTACK_EXTEND,
    }; /* enum ORDERTYPE_ATTACK */

    enum CATCHTRIGGER : uint32
    {
        CATCHTRIGGER_COUNTER   = (1 << 0),
        CATCHTRIGGER_SWIM_IDLE = (1 << 1),
        CATCHTRIGGER_SWIM_MOVE = (1 << 2),
        CATCHTRIGGER_SWIM_MODE = (1 << 3),
    }; /* enum CATCHTRIGGER */

    DEFINE_ENUM_FLAG_OPS(CATCHTRIGGER);

    namespace AIDECISIONUNITNAME
    {
        static const char* ATTACK_TACKLE = "AttackTackle";
        static const char* SWIM_MOVE     = "SwimMove";
        static const char* SWIM_ATTACK   = "SwimAttack";
        static const char* SWIM_IDLE     = "SwimIdle";
    }; /* namespace AIDECISIONUNITNAME */

    enum FREQUENCY_INDEX
    {
        /* 0 */ FREQUENCY_ATTACK_AAA = 0,
        /* 1 */ FREQUENCY_ATTACK_THROW,
        /* 2 */ FREQUENCY_ATTACK_TACKLE,
        /* 3 */ FREQUENCY_ATTACK_TO_AIR,
        /* 4 */ FREQUENCY_4, // TODO unknown (unused) freq
        /* 5 */ FREQUENCY_GUARD_N,  // normal mode guard freq
        /* 6 */ FREQUENCY_GUARD_S,  // swim mode (when hp < 40%) guard freq
        
        /* 7 */ FREQUENCY_MAX,
    }; /* enum FREQUENCY_INDEX */

    static float ATTACK_INTERVAL[] = { 2.0f, 1.5f, 0.75f };
    static_assert(COUNT_OF(ATTACK_INTERVAL) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_INTERVAL_FORSWIMMODE[] = { 1.0f, 0.75f, 0.25f };
    static_assert(COUNT_OF(ATTACK_INTERVAL_FORSWIMMODE) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_WAITTIME_ALLPLAYERDOWN[] = { 2.0f, 1.0f, 0.75f };
    static_assert(COUNT_OF(ATTACK_WAITTIME_ALLPLAYERDOWN) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float SWIMMODE_ATTACK_TIME[] = { 7.0f, 7.0f, 8.0f };
    static_assert(COUNT_OF(SWIMMODE_ATTACK_TIME) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float SWIMMODE_ATTACK_TIME_RANDOMSIZE[] = { 1.0f, 1.0f, 4.0f };
    static_assert(COUNT_OF(SWIMMODE_ATTACK_TIME_RANDOMSIZE) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float SWIMMODE_AFTERSWIMATTACK_IDLETIME[] = { 3.0f, 2.0f, 1.5f };
    static_assert(COUNT_OF(SWIMMODE_AFTERSWIMATTACK_IDLETIME) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_TO_AIR_HEIGHT_LOW = 1.0f;
    static float ATTACK_TACKLE_CONDITION_DISTANCE = 8.0f;
    static float MOVE_INTERVAL_BASE = 0.5f;
    static float MOVE_STOP_DINSTANCE = 2.85f;
    static float MOVE_TIME = 0.125f;
    static float MOVE_CONDITION_DISTANCE = 5.5f;
}; /* namespace LEATHERHEAD_AI */


class C075LeatherheadAI : public CBaseAI6045
{
public:    
    class CDecisionUnitSwimAttack : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitSwimAttack(CBaseLeatherheadChr* pBaseLeatherheadChr);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;

    private:
        CBaseLeatherheadChr* m_pBaseLeatherheadChr;
    };

    class CDecisionUnitSwimMove : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitSwimMove(CBaseLeatherheadChr* pBaseLeatherheadChr);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;

    private:
        CBaseLeatherheadChr* m_pBaseLeatherheadChr;
    };

    class CDecisionUnitSwimIdle : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitSwimIdle(void);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
    };

    class CDecisionUnitAttackToAir : public CBaseAI6045::CDecisionUnitAttackToAir
    {
    public:
        virtual bool CheckTerm(void) override;
    };

    class CDecisionUnitAttackTackle : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackTackle(void);
        virtual bool CheckTerm(void) override;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);

    C075LeatherheadAI(CEnemyCharacter* pEnemyChr);
    virtual ~C075LeatherheadAI(void);
    virtual void CatchTriggerActionOrder(void) override;
};