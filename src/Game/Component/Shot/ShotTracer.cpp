#include "ShotTracer.hpp"
#include "Shot.hpp"


CShotTracer::CShotTracer(CShot* pShot)
: m_pShot(pShot)
{
    ASSERT(m_pShot);
};


CShotTracer::~CShotTracer(void)
{
    ;
};


float CShotTracer::GetDirection(void)
{
    return m_pShot->GetDirection();
};


void CShotTracer::GetPosition(RwV3d* pvPosition)
{
    m_pShot->GetPosition(pvPosition);
};