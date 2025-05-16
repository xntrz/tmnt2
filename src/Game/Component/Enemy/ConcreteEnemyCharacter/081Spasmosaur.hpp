#pragma once

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CommonEnemyObserver.hpp"
#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class C081Spasmosaur : public CEnemyCharacter
{
public:
    enum FREQUENCY
    {
        FREQUENCY_THINKING = 0,
        FREQUENCY_CRASH_DIR_DIST,
    };
    
    enum STATUS
    {
        STATUS_SHRINK = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_OPEN_A,
        STATUS_ATTACK_CLOSE_A,
        STATUS_ATTACK_CLOSE_B,
        STATUS_ATTACK_CLOSE_C,
        STATUS_SPIT_OPEN_BIND,
        STATUS_SPIT_CLOSE_BIND,
        STATUS_SPIT_OPEN_SLEEP,
        STATUS_SPIT_CLOSE_SLEEP,
    };

    enum PHASE
    {
        PHASE_NORMAL = 0,   // normal state
        PHASE_MAD,          // mad state (after head hit)
    };

    enum RUNTYPE
    {
        RUNTYPE_CHASE = 0,  // chase last attacker
        RUNTYPE_RUSH,       // rush into the closest by direction wall ignoring the players
    };

    enum DAMAGESOURCE
    {
        DAMAGESOURCE_NONE = 0,
        DAMAGESOURCE_HEAD,
        DAMAGESOURCE_TENTACLE,
        DAMAGESOURCE_WALL_CRASH,
    };

    struct THROW_TARGET
    {
        bool bThrown;
        bool bLeft; // boss side
    };

    struct PLAYER_DATA
    {
        RwV3d vecFootPos;
        float fDirection;
    };

    struct PLAYER_INFO
    {
        float fDistance;
        float fDirection;
        bool bIsTrouble;
    };

    struct STATE
    {
        CCharacterCompositor* pCompositor;
        bool         bHelpShow;
        bool         bHelpSkip;
        float        fHelpTimeout;
        bool         bMadFristMsg;
        bool         bMouthOpen;
        DAMAGESOURCE damageSource;
        PHASE        phase;
        int32        numHitsUntilShrink;
        int32        targetPlayerNo;
        RUNTYPE      runType;
        int32        numWallMiss;
        THROW_TARGET aThrowTarget[GAMETYPES::PLAYERS_MAX];
        uint32       ahEffectDrool[4];
        uint32       hEffectSmoke;
        float        fColorChangeTimer;
        float        fTimeoutOnDamage;
        float        fThinkTimer;
        PLAYER_DATA  aPlayerData[GAMETYPES::PLAYERS_MAX];
        PLAYER_INFO  aPlayerInfo[GAMETYPES::PLAYERS_MAX];
        int32        numPlayers;

        void CheckPlayerStatus(void);
        ENEMYTYPES::STATUS GetNextAttackStatus(void) const;
        int32 GetFrontNearestPlayerNo(void) const;
        ENEMYTYPES::STATUS GetNextAttackStatusOnMouthOpen(int32 playerNo) const;
        ENEMYTYPES::STATUS GetNextAttackStatusOnMouthClose(int32 playerNo) const;
        bool IsLoopMotionEnd(void) const;
        bool IsResponseTime(void) const;
    };

    class CHideStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CIdleStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bThinkResponse;
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CWalkStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        bool CheckDistance(const RwV3d* pvecDstPos) const;
        void SetDirection(const RwV3d* pvecDstPos);
        void SetVelocity(void);

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CRunStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        bool IsWallHit(void) const;
        bool CheckDistance(RwV3d* pvecRunPos) const;
        bool CheckDirection(bool* pbDirectionFree) const;
        void SetDirection(const RwV3d* pvecRunPos);
        void SetVelocity(void);

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CShrinkStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        int32 m_loopCnt;
    };

    class CAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CThrowStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void ThrowPlayers(RwV3d* pvecThrowDirection, bool bMiss = false);

    private:
        bool m_bThrowing;
    };

    class CSpitStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CCrashWallStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        int32 m_loopCnt;
    };

    class CDeathStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        int32 m_loopCnt;
        float m_fTimer;
    };

public:
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_SPSM_YODAR2,
                            EFFECTID::ID_SPSM_YODAR1,
                            EFFECTID::ID_SPSM_KABECRASH,
                            EFFECTID::ID_SPSM_RUNSMOKE,
                            EFFECTID::ID_SPSM_TBSHOT,
                            EFFECTID::ID_SLEEPGAS });

    DEFINE_EXTEND_STATUS_FN(STATUS, CEnemyCharacter);

    C081Spasmosaur(void);
    virtual ~C081Spasmosaur(void);
    virtual void Run(void) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;
    float GetRandNormal(void);
    void InitState(void);
    void InitEffect(void);
    void TermEffect(void);
    void RunAI(void);
    void UpdateEffectMouthDrool(void);
    void UpdateEffectRunSmoke(void);
    void UpdateBodyColorChange(void);
    void UpdateHelpMessage(void);
    int32 GetAttackerPlayerNo(CHitAttackData* pAttack) const;

private:
    CRandNormal m_rand;
    STATE m_state;
};