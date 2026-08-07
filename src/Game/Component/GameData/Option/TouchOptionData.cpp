#include "TouchOptionData.hpp"

#include "Game/System/Misc/Gamepad.hpp"


CTouchOptionData::CTouchOptionData(void)
{
    ;
};


CTouchOptionData::~CTouchOptionData(void)
{
    ;
};


void CTouchOptionData::Initialize(int32 port)
{
    CGamepadOptionData::Initialize(port);
    SetDefault();
};


void CTouchOptionData::SetDefault(void)
{
    CGamepadOptionData::SetDefault();

    m_bLeftHanded = false;
    m_bFloating = true;
    m_bDynamic = true;
};


void CTouchOptionData::Apply(void)
{
    CGamepadOptionData::Apply();
};


void CTouchOptionData::Snapshot(RAWDATA& rRawData) const
{
    CGamepadOptionData::Snapshot(rRawData.m_gamepad);

    rRawData.m_bLeftHanded = m_bLeftHanded;
    rRawData.m_bFloating = m_bFloating;
    rRawData.m_bDynamic = m_bDynamic;
};


void CTouchOptionData::Restore(const RAWDATA& rRawData)
{
    CGamepadOptionData::Restore(rRawData.m_gamepad);

    m_bLeftHanded = rRawData.m_bLeftHanded;
    m_bFloating = rRawData.m_bFloating;
    m_bDynamic = rRawData.m_bDynamic;
};


void CTouchOptionData::SetLeftHanded(bool bSet)
{
    m_bLeftHanded = bSet;
};


bool CTouchOptionData::IsLeftHanded(void) const
{
    return m_bLeftHanded;
};


void CTouchOptionData::SetFloating(bool bSet)
{
    m_bFloating = bSet;
};


bool CTouchOptionData::IsFloating(void) const
{
    return m_bFloating;
};


void CTouchOptionData::SetDynamic(bool bSet)
{
    m_bDynamic = bSet;
};


bool CTouchOptionData::IsDynamic(void) const
{
    return m_bDynamic;
};