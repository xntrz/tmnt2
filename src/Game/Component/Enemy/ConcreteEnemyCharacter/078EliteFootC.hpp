#pragma once

#include "BaseEliteFootChr.hpp"


class C078EliteFootC : public CBaseEliteFootChr
{
public:
    class CSpearStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void YariShot(const RwV3d* pvecYariPos);

    protected:
        RwV3d m_vecSpearPos;
    };

public:
    C078EliteFootC(void);
    virtual ~C078EliteFootC(void);
};


class C078EliteFootDummyC : public CBaseEliteFootChr
{
public:
    C078EliteFootDummyC(void);
    virtual ~C078EliteFootDummyC(void);
};