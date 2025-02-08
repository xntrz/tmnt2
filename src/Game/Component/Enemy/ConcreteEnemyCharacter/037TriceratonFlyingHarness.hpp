#pragma once

#include "BaseFlyingEnemyChr.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/037TriceratonFlyingHarnessAI.hpp"


class C037TriceratonFlyingHarness : public CBaseFlyingEnemyChr
{
public:
    class CMoveStatusObserver : public CBaseFlyingStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual float AltitudeNoise(void);
        void Direction(const RwV3d* vecPos, const RwV3d* vecAt);
        void Velocity(const RwV3d* vecPos, const RwV3d* vecAt);

    protected:
        CAIThinkOrder::ORDER m_order;
        float m_fMoveSpeed;
        float m_fRotRate;
        float m_fDistanceOfSuitable;
        float m_fAltitude;
        float m_fDirection;
    };

    class CFireStatusObserver : public CBaseFlyingStatusObserver
    {
    private:
        enum MOTIONID
        {
            MOTIONID_A = 0,
            MOTIONID_IDLE,
        };

    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual bool OnMotionChange(int32 idOld, int32 idNew) override;

    private:
        int32 m_targetNo;
        int32 m_offsetNo;
        float m_fTimer;
    };

    class CDeathStatusObserver : public CBaseFlyingStatusObserver
    {
    private:
        enum MOTIONID
        {
            MOTIONID_DEATH_1 = 0,
            MOTIONID_DEATH_2,
        };

    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fFallWaitTimer;
        uint32 m_hSmokeEffect;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C037TriceratonFlyingHarness(void);
    virtual ~C037TriceratonFlyingHarness(void);
    virtual void Run(void) override;
    virtual float GetAltitude(void) const override;

private:
    CSpecialityConsider m_specialityConsider;
    uint32 m_hEngineEffect;
};