#pragma once

#include "Game/Component/Effect/Tracer.hpp"


class CPlayerCharacter;


class CPlayerTracer : public CTracer
{
public:
    CPlayerTracer(CPlayerCharacter* pPlayerChr);
    virtual ~CPlayerTracer(void);
    virtual float GetDirection(void) override;
    virtual void GetPosition(RwV3d* pvPosition) override;

private:
    CPlayerCharacter* m_pPlayerChr;
};


class CPlayerTargetTracer : public CTracer
{
public:
    CPlayerTargetTracer(const CPlayerCharacter* pPlayerChr);
    virtual ~CPlayerTargetTracer(void);
    virtual float GetDirection(void) override;
    virtual void GetPosition(RwV3d* pvPosition) override;

private:
    const CPlayerCharacter* m_pPlayerChr;
};