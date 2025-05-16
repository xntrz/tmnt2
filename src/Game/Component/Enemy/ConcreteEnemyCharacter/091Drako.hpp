#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CommonEnemyObserver.hpp"
#include "Game/Component/Effect/Tracer.hpp"
#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class C091Drako : public CEnemyCharacter
{
public:
    enum FREQUENCY
    {
        FREQUENCY_THINKING = 0,
    };

    enum STATUS
    {
        STATUS_ATTACK_ENERGY_BALL = ENEMYTYPES::STATUS_SPECIAL,
        STATUS_ATTACK_TAIL,
        STATUS_ATTACK_SOMERSAULT,
        STATUS_ATTACK_RUSH,
        STATUS_ATTACK_ANTI_AIR,
        STATUS_ATTACK_BEAM,
        STATUS_ATTACK_QUAKE,
    };

    class CRodTracer : public CTracer
    {
    public:
        CRodTracer(CEnemyCharacter* pEnmChr);
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

    class CTouchdownStatusObserver : public CCommonEnemyObserver::CTouchdownStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;

    private:
        bool m_bQuake;
    };

    class CGuardStatusObserver : public CCommonEnemyObserver::CGuardStatus
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;

    private:
        int32 m_step;
    };

    class CEnergyBallStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        uint32 m_hEffect;
        uint32 m_hMagic;
    };

    class CTailStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CSomersaultStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CRushStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
    };

    class CAntiAirStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        uint32 m_hMagic;
        int32 m_stopCnt;
        float m_fStopTime;
    };

    class CBeamStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        bool IsLoopMotionEnd(void) const;
        void EndEffect(void);

    private:
        int32 m_step;
        int32 m_loopCnt;
        int32 m_playerNo;
        uint32 m_hEffect;
    };

    class CQuakeStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CJumpAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bSoundPlay;
    };

    class CThrowStatusObserver : public CCommonEnemyObserver::CBaseThrow
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bSoundPlay;
    };

public:
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_ROD_FLASH,
                            EFFECTID::ID_DRKBALL_START,
                            EFFECTID::ID_DRKBALL,
                            EFFECTID::ID_DRKBALL_END,
                            EFFECTID::ID_DRK_PILLAR,
                            EFFECTID::ID_ROD_BEAM,
                            EFFECTID::ID_ROD_BEAM_HIT });

    DEFINE_EXTEND_STATUS_FN(STATUS, CEnemyCharacter);

    C091Drako(void);
    virtual ~C091Drako(void);
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    void AttachThrowStatusObserver(void);

private:
    CCommonEnemyObserver::CBaseThrow* m_pThrow;
};