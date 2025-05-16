#pragma once


#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CommonEnemyObserver.hpp"
#include "Game/Component/Effect/Tracer.hpp"
#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class C087Slashuur : public CEnemyCharacter
{
public:
    enum FREQUENCY
    {
        FREQUENCY_THINKING = 0,
        FREQUENCY_HPSTEAL,
    };
    
    enum STATUS
    {
        STATUS_ATTACK_A = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_AA,
        STATUS_ATTACK_AAB,
        STATUS_ATTACK_AAC,
        STATUS_ATTACK_B,
        STATUS_ESCAPE_READY,
        STATUS_TELEPORT_START,
        STATUS_TELEPORT_ATTACK,
        STATUS_TELEPORT_SCYTHE,
        STATUS_TELEPORT_DRAIN,
        STATUS_SHOT,
        STATUS_RUN_ATTACK,
    };

    struct STATE
    {
        uint32  ahMagic[GAMETYPES::PLAYERS_MAX];
        float   afDrainTime[GAMETYPES::PLAYERS_MAX];
        bool    abPlayerSleeping[GAMETYPES::PLAYERS_MAX];
        bool    abPlayerDrained[GAMETYPES::PLAYERS_MAX];
        bool    bDrain;
        bool    bEscape;
        RwV3d   vecJumpPos;
        int32   iTeleportPlayerNo;
        float   fMapMaxZ;
        int32   iThrowPlayerNo;
    };

    class CBodyTracer : public CTracer
    {
    public:
        CBodyTracer(C087Slashuur* pSlashuur);
        virtual float GetDirection(void) override;
        virtual void GetPosition(RwV3d* pvPosition) override;

    private:
        uint32 m_hObj;
    };

    class CScytheTracer : public CTracer
    {
    public:
        CScytheTracer(C087Slashuur* pSlashuur);
        virtual float GetDirection(void) override;
        virtual void GetPosition(RwV3d* pvPosition) override;

    private:
        uint32 m_hObj;
    };

    class CHideStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CThinkingStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        CThinkingStatusObserver();
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CDeathStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CDashStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fDashTime;
        int32 m_step;
    };

    class CAerialStatusObserver : public CCommonEnemyObserver::CAerialStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bAttackFlag;
        bool m_bShotFlag;
    };

    class CTouchdownStatusObserver : public CCommonEnemyObserver::CTouchdownStatus
    {
    public:
        virtual void OnStart(void) override;
    };

    class CGuardStatusObserver : public CCommonEnemyObserver::CGuardStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;

    private:
        int32 m_step;
    };

    class CNormalAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        ENEMYTYPES::STATUS m_statusNext;
        const char* m_pszMotionName;
        bool m_bCheckChain;
    };

    class CRunAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CEscapeReadyStatusObserver : public CCommonEnemyObserver::CJumpReadyStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CTeleportationStartStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_orderType;
        int32 m_step;
        float m_fTime;
    };

    class CTeleportationAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        float m_fTime;
    };

    class CTeleportationScytheStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void FadeScythe(void);

    private:
        int32 m_step;
        float m_fTime;
        RwV3d m_avecPlayerBodyPos[GAMETYPES::PLAYERS_MAX];
        float m_fFadeTiming;
        bool m_bFadeScythe;
        uint32 m_ahScythe[GAMETYPES::PLAYERS_MAX];
    };

    class CTeleportationDrainStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        float m_fTime;
        float m_fEffectScale;
        bool m_bDrain;
        bool m_bDrainProc;
        uint32 m_hEffect;
    };

    class CShotStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_orderTargetNo;
    };

    class CThrowStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
    };

public:
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_WARP_START,
                            EFFECTID::ID_WARP_OUT,
                            EFFECTID::ID_HPSTEAL_LIGHT,
                            EFFECTID::ID_SICKLE_WARP,
                            EFFECTID::ID_SLA_BARRIER,
                            EFFECTID::ID_SLA_SHADOW,
                            EFFECTID::ID_HPSTEAL,
                            EFFECTID::ID_SLABALL,
                            EFFECTID::ID_SLABALL_END });

    DEFINE_EXTEND_STATUS_FN(STATUS, CEnemyCharacter);

    C087Slashuur(void);
    virtual ~C087Slashuur(void);
    virtual void Run(void) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;
    void InvokeBarrier(CHitCatchData* pCatch) const;
    void RegistBarrierCatch(void);
    void TeleportDrainProc(void);
    void InitState(void);

private:
    STATE m_state;
};