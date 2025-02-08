#pragma once

#include "BaseGeneralEnemyChr.hpp"

#include "Game/Component/Effect/EffectGeneric.hpp"


class CBaseFlyingEnemyChr : public CBaseGeneralEnemyChr
{
public:
    static const float BASE_ALTITUDE;

    class CBaseFlyingStatusObserver : public CMotionManagedStatusObserver
    {
    public:
        CBaseFlyingStatusObserver(void);
        CBaseFlyingStatusObserver(float fAltitude);
        virtual void OnStart(void) override;
        virtual float GetAltitude(void) const;
        virtual float GetAttackAltitude(void) const;
        virtual void DoStoppingVelocity(void);
        virtual float CalcAltitude(float fHeight, float HeightAt, float fSpeed) const;

    private:
        float m_fAltitude;
    };

    class CAppearStatusObserver : public CBaseFlyingStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CAppearFlyStatusObserver : public CBaseFlyingStatusObserver
    {
    public:
        CAppearFlyStatusObserver(float fAppearOffsetY);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual float AltitudeNoise(void);

    private:
        float m_fTimer;
        float m_fMoveSpeed;
        float m_fRotRate;
        float m_fDistanceOfSuitable;
        float m_fAppearOffsetY;
        RwV3d m_vecStartPos;
    };

    class CIdleStatusObserver : public CBaseFlyingStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

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

    class CAttackAStatusObserver : public CBaseFlyingStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CDeathStatusObserver : public CBaseFlyingStatusObserver
    {
    public:
        CDeathStatusObserver(EFFECT_GENERIC::DEATHTYPE deathType, float fEffectScale);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float                     m_fTimer;
        float                     m_fEffectScale;
        EFFECT_GENERIC::DEATHTYPE m_deathType;
        uint32                    m_hEffect;
    };

    class CBindStatusObserver : public CCommonEnemyObserver::CCharacterEffectStatusObserver
    {
    public:
        virtual void OnStart(void) override;
    };

    class CStunStatusObserver : public CCommonEnemyObserver::CCharacterEffectStatusObserver
    {
    public:
        virtual void OnStart(void) override;
    };

public:
    CBaseFlyingEnemyChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseFlyingEnemyChr(void);
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual void OnMessageDoChangingAerial(void) override;
    virtual float GetAltitude(void) const;
    virtual float GetAttackAltitude(void) const;
    virtual void SetAttackAltitude(float fAltitude);

private:
    float m_fAttackAltitude;
};