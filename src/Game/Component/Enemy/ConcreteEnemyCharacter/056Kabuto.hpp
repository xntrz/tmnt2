#pragma once

#include "BaseFlyingEnemyChr.hpp"

#include "Game/Component/Enemy/ConcreteAIModerator/056KabutoAI.hpp"


class C056Kabuto : public CBaseFlyingEnemyChr
{
public:
    class CMoveStatusObserver : public CBaseFlyingEnemyChr::CMoveStatusObserver
    {
    public:
        virtual float AltitudeNoise(void) override;
    };

public:
    static EFFECTID::VALUE GetNeededEffect(int32 no);

    C056Kabuto(void);
    virtual ~C056Kabuto(void);

private:
    CSpecialityConsider m_specialityConsider;
};