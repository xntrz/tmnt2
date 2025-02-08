#include "BodyHitData.hpp"


CBodyHitData::CBodyHitData(void)
: m_uHitID(INVALID_HIT_ID)
, m_uHitState(0)
, m_fHitRadius(0.0f)
, m_vPos(Math::VECTOR3_ZERO)
{
    InitData(&Math::VECTOR3_ZERO, 0.5f);
    m_uHitState = 0;
};


CBodyHitData::~CBodyHitData(void)
{
    ;
};


void CBodyHitData::InitData(const RwV3d* pPos, float fHitRadius)
{
    SetCurrentPos(pPos);
    SetHitRadius(fHitRadius);
    m_uHitID = 0;
    m_uHitState = 7;
};


void CBodyHitData::SetHitID(uint32 uHitID)
{
    ASSERT(uHitID != INVALID_HIT_ID);
    
    m_uHitID = uHitID;
};


uint32 CBodyHitData::GetHitID(void) const
{
    return m_uHitID;
};


void CBodyHitData::SetState(STATE state, bool bEnable)
{
    if (bEnable)
        m_uHitState |= state;
    else
        m_uHitState &= (~state);
};


bool CBodyHitData::IsEnableState(STATE state) const
{
    return ((m_uHitState & state) == state);
};


void CBodyHitData::SetCurrentPos(const RwV3d* pPos)
{
    ASSERT(pPos);

    m_vPos = *pPos;
};


const RwV3d* CBodyHitData::GetCurrentPos(void) const
{
    return &m_vPos;
};


void CBodyHitData::SetHitRadius(float fHitRadius)
{
    ASSERT(fHitRadius > 0.0f);
    
    m_fHitRadius = fHitRadius;
};


float CBodyHitData::GetHitRadius(void) const
{
    return m_fHitRadius;
};