#pragma once

#include "BaseFootNinjaChr.hpp"


class C030FootTechNinja : public CBaseFootNinjaChr
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

    class CMotionAppearStatusObserver : public CCommonEnemyObserver::CMotionAppearStatus
    {
    public:
        virtual RESULT Observing(void) override;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C030FootTechNinja(void);
    virtual ~C030FootTechNinja(void);
    virtual void OnMessageTouchdown(float fHeight) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;

private:
    CSpecialityConsider m_specialityConsider;
};