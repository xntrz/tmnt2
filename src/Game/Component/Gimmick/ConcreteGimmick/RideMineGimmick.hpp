#pragma once

#include "RideGimmick.hpp"


class CRideMineGimmick final : public CRideGimmick
{
public:
    CRideMineGimmick(const char* pszName, void* pParam);
    virtual ~CRideMineGimmick(void);    
    virtual void PostMove(void);
    virtual void OnCatchAttack(CHitAttackData* pAttack);
    virtual void OnAttackResult(CHitCatchData* pCatch);
    void Explose(void);
    bool IsNearByPlayer(void);
};