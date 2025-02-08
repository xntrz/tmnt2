#include "RideCharacterTracer.hpp"
#include "RideCharacter.hpp"


CRideCharacterTracer::CRideCharacterTracer(CRideCharacter* pRideChr)
: m_pRideChr(pRideChr)
{
    ASSERT(m_pRideChr);
};


CRideCharacterTracer::~CRideCharacterTracer(void)
{
    ;
};


float CRideCharacterTracer::GetDirection(void)
{
    return 0.0f;
};


void CRideCharacterTracer::GetPosition(RwV3d* pvPosition)
{
    if (m_pRideChr->IsEnableMove())
        m_pRideChr->GetBodyPosition(pvPosition);
};