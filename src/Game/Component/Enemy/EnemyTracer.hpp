#pragma once

#include "Game/Component/Effect/Tracer.hpp"


class CEnemyCharacter;
class CCharacterCompositor;


class CEnemyTracer : public CTracer
{
public:
    CEnemyTracer(const CCharacterCompositor* pChrCompositor);
    CEnemyTracer(const CEnemyCharacter* pEnemyChr);
    virtual ~CEnemyTracer(void);
    virtual float GetDirection(void) override;
    virtual void GetPosition(RwV3d* pvPosition) override;

protected:
    uint32 m_hObj;
};