#include "TimingParameter.hpp"


CTimingParameter::CTimingParameter(INIT_PARAMETER* pParam)
: m_kind(TIMING_ATTACK)
, m_fTime(0.0f)
, m_nParam(0)
{
    Initialize(pParam);
};


CTimingParameter::~CTimingParameter(void)
{
    ;
};


void CTimingParameter::Initialize(INIT_PARAMETER* pParam)
{
    ASSERT(pParam);
    ASSERT(pParam->m_kind >= 0);
    ASSERT(pParam->m_kind < TIMING_KIND_MAX);

    m_kind   = pParam->m_kind;
    m_fTime  = pParam->m_fTime;
    m_nParam = pParam->m_nParam;
};


CTimingParameter::TIMING_KIND CTimingParameter::GetKind(void) const
{
    ASSERT(m_kind >= 0);
    ASSERT(m_kind < TIMING_KIND_MAX);

    return m_kind;
};


float CTimingParameter::GetTime(void) const
{
    ASSERT(m_fTime >= 0.0f);
    
    return m_fTime;
};


int32 CTimingParameter::GetParam(void) const
{
    return m_nParam;
};


bool CTimingParameter::IsLocus(void) const
{
    ASSERT(m_kind >= 0);
    ASSERT(m_kind < TIMING_KIND_MAX);

    return (m_kind == TIMING_LOCUS_ON) ||
           (m_kind == TIMING_LOCUS_OFF);
};


bool CTimingParameter::IsAtomic(void) const
{
    ASSERT(m_kind >= 0);
    ASSERT(m_kind < TIMING_KIND_MAX);

    return (m_kind == TIMING_ATOMIC_ON) ||
           (m_kind == TIMING_ATOMIC_OFF);
};


bool CTimingParameter::IsAttack(void) const
{
    ASSERT(m_kind >= 0);
    ASSERT(m_kind < TIMING_KIND_MAX);

    return (m_kind == TIMING_ATTACK) ||
           (m_kind == TIMING_ATTACK_2);
};


bool CTimingParameter::IsInvincible(void) const
{
    ASSERT(m_kind >= 0);
    ASSERT(m_kind < TIMING_KIND_MAX);

    return (m_kind == TIMING_INVINCIBLE_ON) ||
           (m_kind == TIMING_INVINCIBLE_OFF);
};