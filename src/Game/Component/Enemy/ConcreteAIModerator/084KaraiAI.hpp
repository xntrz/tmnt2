#pragma once

#include "BaseAI6045.hpp"


namespace KARAI_AI
{
    enum ORDERTYPE_MOVE
    {
        ORDERTYPE_MOVE_BACKROUND = BASEAI6045::ORDERTYPE_MOVE_EXTEND,
        ORDERTYPE_MOVE_JUMP_ESCAPE,
    };

    enum ORDERTYPE_ATTACK
    {
        ORDERTYPE_ATTACK_A = BASEAI6045::ORDERTYPE_ATTACK_EXTEND,
        ORDERTYPE_ATTACK_AA,
        ORDERTYPE_ATTACK_AAA,
        ORDERTYPE_ATTACK_AAB,
        ORDERTYPE_ATTACK_AAC,
        ORDERTYPE_ATTACK_B,
        ORDERTYPE_ATTACK_BB,
        ORDERTYPE_ATTACK_C1,
        ORDERTYPE_ATTACK_C2,
        ORDERTYPE_ATTACK_D,
        ORDERTYPE_ATTACK_E,
        ORDERTYPE_ATTACK_F,
        ORDERTYPE_ATTACK_SPECIAL,
    };

    enum CATCHTRIGGER : uint32
    {
        CATCHTRIGGER_COUNTER        = (1 << 0),
        CATCHTRIGGER_MODE_WARNING   = (1 << 1),
        CATCHTRIGGER_MODE_FINAL     = (1 << 2),
    };
    
    DEFINE_ENUM_FLAG_OPS(CATCHTRIGGER);

    namespace AIDECISIONUNITNAME
    {
        static const char* MOVE_ESCAPE                      = "MoveEscape";
        static const char* ATTACK_A                         = "AttackA";
        static const char* ATTACK_A_FORUNUSUALSTATUS        = "AttackAUnusual";
        static const char* ATTACK_B                         = "AttackB";
        static const char* ATTACK_B_FORUNUSUALSTATUS        = "AttackBUnusual";
        static const char* ATTACK_C                         = "AttackC";
        static const char* ATTACK_C2                        = "AttackC2";
        static const char* ATTACK_D                         = "AttackD";
        static const char* ATTACK_E                         = "AttackE";
        static const char* ATTACK_F                         = "AttackF";
        static const char* ATTACK_SPECIAL                   = "AttackSpecial";
        static const char* ATTACK_SPECIAL_FORUNUSUALSTATUS  = "AttackSpecialUnusual";
    };

    enum FREQUENCY_INDEX
    {
        /*
         *  _B - base mode
         *  _W - warning mode (red stars aura)
         *  _F - final mode (red circle aura)
         */

        /* 0 */  FREQUENCY_ATTACK_A = 0,
        /* 1 */  FREQUENCY_ATTACK_B,
        /* 2 */  FREQUENCY_ATTACK_C,
        /* 3 */  FREQUENCY_ATTACK_D,
        /* 4 */  FREQUENCY_ATTACK_E,
        /* 5 */  FREQUENCY_ATTACK_F,
        /* 6 */  FREQUENCY_ATTACK_SPECIAL, // counts as _B
        /* 7 */  FREQUENCY_ATTACK_A_UNUSUAL,
        /* 8 */  FREQUENCY_ATTACK_B_UNUSUAL,
        /* 9 */  FREQUENCY_ATTACK_SPECIAL_UNUSUAL,
        /* 10 */ FREQUENCY_ATTACK_AA,
        /* 11 */ FREQUENCY_ATTACK_AAA,
        /* 12 */ FREQUENCY_ATTACK_BB,
        /* 13 */ FREQUENCY_ATTACK_AA_W,
        /* 14 */ FREQUENCY_ATTACK_AAA_W,
        /* 15 */ FREQUENCY_ATTACK_BB_W,
        /* 16 */ FREQUENCY_ATTACK_SPECIAL_W,
        /* 17 */ FREQUENCY_ATTACK_SPECIAL_F,
        /* 18 */ FREQUENCY_GUARD_B,
        /* 19 */ FREQUENCY_GUARD_W,
        /* 20 */ FREQUENCY_GUARD_F,
        /* 21 */ FREQUENCY_MOVE_ESCAPE,

        /* 22 */ FREQUENCY_MAX,
    };

    static float ATTACK_INTERVAL_FORWARNINGMODE[] = { 0.4f, 0.25f, 0.125f };
    static_assert(COUNT_OF(ATTACK_INTERVAL_FORWARNINGMODE) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_INTERVAL[] = { 0.8f, 0.5f, 0.3f };
    static_assert(COUNT_OF(ATTACK_INTERVAL) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float ATTACK_WAITTIME_ALLPLAYERDOWN[] = { 1.0f, 0.75f, 0.5f };
    static_assert(COUNT_OF(ATTACK_WAITTIME_ALLPLAYERDOWN) == GAMETYPES::DIFFICULTY_NUM, "table incorrect");

    static float MOVE_INTERVAL_BASE = 0.4f;
    static float MOVE_INTERVAL_WARNING = 0.125f;
    static float MOVE_ESCAPE_CONDITION_DISTANCE = 6.0f;
    static float ATTACK_SPECIAL_UNUSUAL_CONDITION_DISTANCE = 4.0f;
    static float ATTACK_C_CONDITION_DISTANCE = 4.0f;
    static float ATTACK_D_CONDITION_DISTANCE = 6.0f;
    static float ATTACK_E_CONDITION_DISTANCE = 1.5f;
    static float ATTACK_F_CONDITION_DISTANCE = 2.0f;
    static float ATTACK_SPECIAL_CONDITION_DISTANCE = 4.0f;
    static float MOVE_TIME = 0.125f;
    static float MOVE_CONDITION_DISTANCE = 3.5f;
    static float MOVE_STOP_DISTANCE = 3.0f;
    static float MOVE_STOP_DISTANCE_WARNING = 6.5f;
    static int32 MOVE_ESCAPE_PERMIT_INALLTYPEHISTORYNUM = 2;
    static int32 ATTACK_C_PERMIT_INATTACKHISTORYNUM = 2;
};


class CBaseKaraiChr;


class C084KaraiAI : public CBaseAI6045
{
public:
    class CDecisionUnitMoveEscape : public CEnemyAIDecisionUnit
    {
    public:
        CDecisionUnitMoveEscape(CBaseKaraiChr* pBaseKaraiChr);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
        bool ProcEscapeSetting(const RwV3d* pvecPlayerPos);
        bool CheckTermForGuardReaction(void);

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
    };

    class CDecisionUnitAttackSpecial : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:        
        CDecisionUnitAttackSpecial(CBaseKaraiChr* pBaseKaraiChr);
        bool CheckTermForGuardReaction(void);

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
    };

    class CDecisionUnitAttackSpecialForUnusualStatus : public CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus
    {
    public:
        CDecisionUnitAttackSpecialForUnusualStatus(void);
    };

    class CDecisionUnitAttackF : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackF(void);
        bool CheckTermForGuardReaction(void);
    };

    class CDecisionUnitAttackD : public CBaseAI6045::CDecisionUnitAttackOverInterval
    {
    public:
        CDecisionUnitAttackD(void);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
    };

    class CDecisionUnitAttackCounter : public CBaseAI6045::CDecisionUnitAttackCounter
    {
    public:
        CDecisionUnitAttackCounter(void);
        virtual bool CheckTerm(void) override;
    };

    class CDecisionUnitAttackE : public CBaseAI6045::CDecisionUnitAttackCounter
    {
    public:
        CDecisionUnitAttackE(void);
    };
    
    class CDecisionUnitAttackC : public CBaseAI6045::CDecisionUnitAttackOverInterval
    {
    public:
        CDecisionUnitAttackC(void);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
    };

    class CDecisionUnitAttackC2 : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackC2(void);
        virtual bool CheckTerm(void) override;
        virtual void OnStart(void) override;
    };

    class CDecisionUnitAttackB : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackB(CBaseKaraiChr* pBaseKaraiChr);
        virtual void OnStart(void) override;

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
    };

    class CDecisionUnitAttackBForUnusualStatus : public CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus
    {
    public:
        CDecisionUnitAttackBForUnusualStatus(CBaseKaraiChr* pBaseKaraiChr);
        virtual void OnStart(void) override;

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
    };

    class CDecisionUnitAttackA : public CBaseAI6045::CDecisionUnitAttackNormal
    {
    public:
        CDecisionUnitAttackA(CBaseKaraiChr* pBaseKaraiChr);
        virtual void OnStart(void) override;

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
    };

    class CDecisionUnitAttackAForUnusualStatus : public CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus
    {
    public:
        CDecisionUnitAttackAForUnusualStatus(CBaseKaraiChr* pBaseKaraiChr);
        virtual void OnStart(void) override;

    private:
        CBaseKaraiChr* m_pBaseKaraiChr;
    };

public:
    static CAIModerator* Instance(CEnemyCharacter* pEnemyChr);
    static void SetAttackTypeA(CEnemyAIDecisionUnit& rDecisionUnit, int32 freqTableAA, int32 freqTableAAA);
    static void SetAttackTypeB(CEnemyAIDecisionUnit& rDecisionUnit, int32 freqTableBB);

    C084KaraiAI(CEnemyCharacter* pEnemyChr);
    virtual ~C084KaraiAI(void);
    virtual void CatchTriggerActionOrder(void) override;
};