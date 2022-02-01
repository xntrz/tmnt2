#include "LocusParameter.hpp"


CLocusParameter::CLocusParameter(INIT_PARAMETER* pParam)
: m_nBoneID(0)
, m_vPositionHigh(Math::VECTOR3_ZERO)
, m_vPositionLow(Math::VECTOR3_ZERO)
, m_Color({ 0xFF, 0xFF, 0xFF, 0x80 })
{
    Initialize(pParam);
};


CLocusParameter::~CLocusParameter(void)
{
    ;
};


void CLocusParameter::Initialize(INIT_PARAMETER* pParam)
{
    ASSERT(pParam);

    m_nBoneID       = pParam->m_nBoneID;
    m_vPositionHigh = pParam->m_vPositionHigh;
    m_vPositionLow  = pParam->m_vPositionLow;
    m_Color         = pParam->m_Color;
};


int32 CLocusParameter::GetBoneID(void) const
{
    return m_nBoneID;
};


const RwV3d* CLocusParameter::GetPosHigh(void) const
{
    return &m_vPositionHigh;
};


const RwV3d* CLocusParameter::GetPosLow(void) const
{
    return &m_vPositionLow;
};


RwRGBA CLocusParameter::GetColor(void) const
{
    return m_Color;
};