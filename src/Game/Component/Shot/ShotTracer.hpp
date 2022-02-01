#pragma once

#include "Game/Component/Effect/Tracer.hpp"


class CShot;


class CShotTracer : public CTracer
{
public:
    CShotTracer(CShot* pShot);
    virtual ~CShotTracer(void);
    virtual float GetDirection(void) override;
    virtual void GetPosition(RwV3d* pvPosition) override;

private:
    CShot* m_pShot;
};