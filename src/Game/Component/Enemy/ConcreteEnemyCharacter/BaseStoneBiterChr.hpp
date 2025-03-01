#pragma once

#include "BaseChr6045.hpp"


class CBaseStoneBiterChr : public CEnemyCharacter
{
public:
    class CHideStatusObserver : public CBaseChr6045::CNothingStatusObserver
    {
    public:
        CHideStatusObserver(void);
    };

    class CAppearStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CThinkingStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CSideMoveStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_moveType;
        float m_fMoveSpeed;
        float m_fMoveAngle;
        float m_fMoveTime;
    };

    class CAttackAStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackAStatusObserver(void);
    };

    class CAttackBStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackBStatusObserver(void);
    };

    class CAttackCStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackCStatusObserver(void);
    };

    class CAttackRunStatusObserver : public CBaseChr6045::COneShotMotionForAttackPlayerAim
    {
    public:
        CAttackRunStatusObserver(void);
        virtual void OnStart(void) override;
    };

    class CDeathStatusObserver : public CMotionManagedStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    CBaseStoneBiterChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseStoneBiterChr(void);
    virtual void Run(void) override;
    virtual void OnStart(void) override;
};