#pragma once

#include "BaseFlyingEnemyChr.hpp"

#include "Game/Component/Effect/MagicID.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/056KabutoAI.hpp"


class C057UtromSaucer : public CBaseFlyingEnemyChr
{
public:
    class CMoveStatusObserver : public CBaseFlyingEnemyChr::CMoveStatusObserver
    {
    public:
        virtual float AltitudeNoise(void) override;
    };

    class CFireStatusObserver : public CBaseFlyingEnemyChr::CBaseFlyingStatusObserver
    {
    public:
        CFireStatusObserver(MAGICID::VALUE magicId, int32 repeatCount, bool bRepeatChase);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void Fire(void);

    private:
        MAGICID::VALUE m_magicId;
        int32 m_repeatCount;
        int32 m_fireNo;
        int32 m_targetNo;
        int32 m_offsetNo;
        RwV3d m_vecOrderAt;
        bool m_bRepeatChase;
        bool m_bContinuationFire;
    };

    class CDeathStatusObserver : public CBaseFlyingEnemyChr::CBaseFlyingStatusObserver
    {
    public:
        CDeathStatusObserver(float fSparkPlaytime);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fSparkPlaytime;
        float m_fTimer;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C057UtromSaucer(void);
    virtual ~C057UtromSaucer(void);
    virtual float GetAltitude(void) const override;
    
private:
    CSpecialityConsider m_specialityConsider;
};