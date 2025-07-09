#pragma once

#include "RideGimmick.hpp"


class CRideRockGimmick : public CRideGimmick
{
public:
    CRideRockGimmick(const char* pszName, void* pParam);
    virtual ~CRideRockGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    virtual void OnAttackResult(CHitCatchData* pCatch) override;

protected:
    static RwRGBA   m_damageColor;
    int32           m_iLife;
    RwRGBA          m_defaultColor;
    float           m_fBlendRate;
};