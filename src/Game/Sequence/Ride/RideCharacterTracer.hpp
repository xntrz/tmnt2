#pragma once

#include "Game/Component/Effect/Tracer.hpp"


class CRideCharacter;


class CRideCharacterTracer : public CTracer
{
public:
    CRideCharacterTracer(CRideCharacter* pRideChr);
    virtual ~CRideCharacterTracer(void);
    virtual float GetDirection(void) override;
    virtual void GetPosition(RwV3d* pvPosition) override;

private:
    CRideCharacter* m_pRideChr;
};