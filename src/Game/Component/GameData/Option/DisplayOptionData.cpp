#include "DisplayOptionData.hpp"


CDisplayOptionData::CDisplayOptionData(void)
{
    ;
};


CDisplayOptionData::~CDisplayOptionData(void)
{
    ;
};


void CDisplayOptionData::Initialize(void)
{
    m_bFontEffectFlag = true;
    m_bPlayerMarkerFlag = true;
};


void CDisplayOptionData::Terminate(void)
{
    ;
};


void CDisplayOptionData::SetDefault(void)
{
    m_bFontEffectFlag = true;
    m_bPlayerMarkerFlag = true;
};


void CDisplayOptionData::Apply(void)
{
    ;
};


bool CDisplayOptionData::IsValid(void) const
{
    return true;
};


void CDisplayOptionData::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_bFontEffectFlag = m_bFontEffectFlag;
    rRawData.m_bPlayerMarkerFlag = m_bPlayerMarkerFlag;
};


void CDisplayOptionData::Restore(const RAWDATA& rRawData)
{
    m_bFontEffectFlag = rRawData.m_bFontEffectFlag;
    m_bPlayerMarkerFlag = rRawData.m_bPlayerMarkerFlag;
};


void CDisplayOptionData::SetEnableFontEffect(bool bSet)
{
    m_bFontEffectFlag = bSet;
};


void CDisplayOptionData::SetEnablePlayerMarker(bool bSet)
{
    m_bPlayerMarkerFlag = bSet;
};


bool CDisplayOptionData::IsEnabledFontEffect(void) const
{
    return m_bFontEffectFlag;
};


bool CDisplayOptionData::IsEnabledPlayerMarker(void) const
{
    return m_bPlayerMarkerFlag;
};

