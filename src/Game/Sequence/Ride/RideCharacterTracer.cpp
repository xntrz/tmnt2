#include "RideCharacterTracer.hpp"
#include "RideCharacter.hpp"


CRideCharacterTracer::CRideCharacterTracer(CRideCharacter* pRideChr)
: m_pRideChr(pRideChr)
, m_vPosition(Math::VECTOR3_ZERO)
{
    ASSERT(m_pRideChr);
    m_pRideChr->GetPosition(&m_vPosition);
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
    ASSERT(pvPosition);
    
    if (m_pRideChr->IsEnableMove())
        m_pRideChr->GetPosition(&m_vPosition);

    *pvPosition = m_vPosition;
};