#pragma once

#include "BaseFlyingEnemyChr.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/055KurageAI.hpp"


class C055Kurage : public CBaseFlyingEnemyChr
{
public:
    class CMoveStatusObserver : public CBaseFlyingEnemyChr::CBaseFlyingStatusObserver
    {
    private:
        enum MOTIONID
        {
            MOTIONID_A1 = 0,
            MOTIONID_A2,
            MOTIONID_A3,
        };
        
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual bool OnMotionChange(int32 idOld, int32 idNew) override;
        virtual float AltitudeNoise(void);
        void UpdateMotion(void);
        void UpdateDirection(void);
        void UpdateVelocity(void);

    private:
        CAIThinkOrder::ORDER m_order;
        int32 m_orderTarget;
        float m_fMoveSpeed;
        float m_fRotRate;
        float m_fDistanceOfSuitable;
        float m_fAltitude;
        float m_fDirection;
        bool m_bTargetChanged;
    };

    class CAttackBombingStatusObserver : public CBaseFlyingEnemyChr::CBaseFlyingStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C055Kurage(void);
    virtual ~C055Kurage(void);
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;

private:
    CSpecialityConsider m_specialityConsider;
    int32 m_hitCount;
};