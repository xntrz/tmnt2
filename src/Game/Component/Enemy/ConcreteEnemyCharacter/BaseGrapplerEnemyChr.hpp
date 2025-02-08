#pragma once

#include "BaseGeneralEnemyChr.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/BaseGrapplerAI.hpp"


class CBaseGrapplerEnemyChr : public CBaseGeneralEnemyChr
{
public:
    class CMoveStatusObserver : public CStatusObserver
    {
    public:
        CMoveStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        CAIThinkOrder::ORDER m_order;
        ENEMYTYPES::STATUS m_nextStatus;
        RwV3d m_vMovePos;
        float m_fAttackRange;
        float m_fMoveSpeed;
        float m_fRotateRate;
        float m_fDistanceOfSuitable;        
        const char* m_pszMotion;
    };

    class CWalkStatusObserver : public CMoveStatusObserver
    {
    public:
        virtual void OnStart(void) override;
    };

    class CRunStatusObserver : public CMoveStatusObserver
    {
    public:
        virtual void OnStart(void) override;
    };

    class CDeathStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CAttackStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CAttackASeriesStatusObserver : public CMotionManagedStatusObserver
    {
    private:
        enum MOTIONID
        {
            MOTIONID_ATTACK_A = 0,
            MOTIONID_ATTACK_AA,
            MOTIONID_ATTACK_AAA,
        };

    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual bool OnMotionChange(int32 idOld, int32 idNew) override;
    };

public:
    CBaseGrapplerEnemyChr(ENEMYID::VALUE idEnemy);
    virtual ~CBaseGrapplerEnemyChr(void);
};