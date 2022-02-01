#pragma once

#include "Game/Component/Effect/Tracer.hpp"


class CCharacter;


class CEnemyTracer : public CTracer
{
public:
    CEnemyTracer(CCharacter* pCharacter);
    virtual ~CEnemyTracer(void);
    virtual float GetDirection(void) override;
    virtual void GetPosition(RwV3d* pvPosition) override;

private:
    uint32 m_hObj;
};