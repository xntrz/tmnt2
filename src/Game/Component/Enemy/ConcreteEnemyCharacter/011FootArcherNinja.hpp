#pragma once

#include "BaseGunnerEnemyChr.hpp"


class C011FootArcherNinja : public CBaseGunnerEnemyChr
{
private:
    class CFireArrowStatusObserver : public CFireStatusObserver
    {
    public:
        CFireArrowStatusObserver(const PARAM* pParam);
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual bool OnMotionChange(int32 idOld, int32 idNew) override;
        void SetArrowActive(bool bActive);
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C011FootArcherNinja(void);
    virtual ~C011FootArcherNinja(void);
    virtual void SetArrowActive(bool bActive) const;

private:
    CSpecialityConsider m_specialityConsider;
    uint32 m_hArrowEffect;
};