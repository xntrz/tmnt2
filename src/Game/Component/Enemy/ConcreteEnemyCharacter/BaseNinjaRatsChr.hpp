#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CommonEnemyObserver.hpp"


class CBaseNinjaRatsChr : public CEnemyCharacter
{
public:
    enum FREQUENCY
    {
        FREQUENCY_VIEW_ANGLE = 0,
    };

    enum STATUS
    {
        STATUS_ATTACK_CHAIN = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_SINGLE,
        STATUS_ATTACK_STUN,
        STATUS_BACKAWAY,
        STATUS_HOP,
    };

    struct STATE
    {
        int32 hp;
        float fMapHeight;
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

    class CRunStatusObserver : public CCommonEnemyObserver::CMoveStatus
    {
    public:
        virtual RESULT Observing(void) override;
    };

    class CJumpReadyStatusObserver : public CCommonEnemyObserver::CJumpReadyStatus
    {
    public:
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CHopStatusObserver : public CCommonEnemyObserver::CJumpReadyStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        RwV3d m_vecHopPos;
        float m_fHopSpeed;
    };

    class CAerialStatusObserver : public CCommonEnemyObserver::CAerialStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CJump2ndStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CJumpShotStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fMotionPreT;
    };

    class CTouchdownStatusObserver : public CCommonEnemyObserver::CTouchdownStatus
    {
    public:
        virtual void OnStart(void) override;
    };

    class CGetupStatusObserver : public CCommonEnemyObserver::CGetupStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        bool IsLoopMotionEnd(void) const;

    private:
        bool m_bDown;
    };

    class CAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void SetVelocityProc(void);
        void SqueakSoundProc(void);

    private:
        const char* m_pszMotionName;
        int32 m_squeakCnt;
    };

    class CBackawayStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

public:
    DEFINE_EXTEND_STATUS_FN(STATUS, CEnemyCharacter);
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_WARP_START });
    
    CBaseNinjaRatsChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseNinjaRatsChr(void);
    virtual void Run(void) override;
    virtual void OnMessageCatchAttack(CHitAttackData* pAttack) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;
    void InitState(void);
    void SqueakSoundProc(void);
    void JumpCheckProc(void);
    void BroadcastReceivedDamage(int32 iAmount) const;

private:
    static STATE m_state;
    float m_fSqueakTime;
    bool m_bJump2nd;
};