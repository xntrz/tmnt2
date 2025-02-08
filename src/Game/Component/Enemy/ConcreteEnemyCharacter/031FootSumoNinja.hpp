#pragma once

#include "BaseGrapplerEnemyChr.hpp"


class C031FootSumoNinja : public CBaseGrapplerEnemyChr
{
private:
    class CWalkStatusObserver : public CBaseGrapplerEnemyChr::CWalkStatusObserver
    {
    public:
        virtual RESULT Observing(void) override;
    };

    class CRunStatusObserver : public CBaseGrapplerEnemyChr::CRunStatusObserver
    {
    public:
        virtual RESULT Observing(void) override;
    };

    class CRunAttackStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        RwV3d  m_vStartPos;
        float  m_fTimer;
        uint32 m_hEffect;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C031FootSumoNinja(void);
    virtual ~C031FootSumoNinja(void);
    virtual void OnMessageTouchdown(float fHeight) override;
    virtual CHARACTERTYPES::DEFENCERSTATUSFLAG CheckDefenceStatusFlag(void) override;

private:
    CSpecialityConsider m_specialityConsider;
};