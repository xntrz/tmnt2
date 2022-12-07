#include "BodyHitData.hpp"


/*static*/ float CBodyHitData::m_fDefaultHitRadius = 0.5f;


CBodyHitData::CBodyHitData(void)
: m_uHitID(INVALID_HIT_ID)
, m_uHitState(STATE_DEFAULT)
, m_fHitRadius(m_fDefaultHitRadius)
, m_vPos(Math::VECTOR3_ZERO)
{
    ;
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
    m_uHitState = STATE_DEFAULT;
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


void CBodyHitData::SetState(uint32 uState, bool bEnable)
{
    FLAG_CHANGE(m_uHitState, uState, bEnable);
};


bool CBodyHitData::IsEnableState(uint32 uState) const
{
    return FLAG_TEST(m_uHitState, uState);
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