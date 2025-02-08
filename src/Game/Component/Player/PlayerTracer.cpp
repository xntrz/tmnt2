#include "PlayerTracer.hpp"
#include "PlayerCharacter.hpp"


CPlayerTracer::CPlayerTracer(CPlayerCharacter* pPlayerChr)
: m_pPlayerChr(pPlayerChr)
{
    ASSERT(m_pPlayerChr);
};


CPlayerTracer::~CPlayerTracer(void)
{
    ;
};


float CPlayerTracer::GetDirection(void)
{   
    return m_pPlayerChr->GetDirection();
};


void CPlayerTracer::GetPosition(RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    m_pPlayerChr->GetFootPosition(pvPosition);
};