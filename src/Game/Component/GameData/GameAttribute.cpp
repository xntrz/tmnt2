#include "GameAttribute.hpp"

#include "System/Common/Controller.hpp"


CGameAttribute::CGameAttribute(void)
{
    Clear();
};


CGameAttribute::~CGameAttribute(void)
{
    ;
};


void CGameAttribute::Clear(void)
{
    m_iVirtualPad = CController::CONTROLLER_UNLOCKED_ON_VIRTUAL;
    m_bIsInteractive = false;
};


void CGameAttribute::SetVirtualPad(int32 iController)
{
    ASSERT( iController == CController::CONTROLLER_LOCKED_ON_VIRTUAL ||
            iController == CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);
    
    m_iVirtualPad = iController;
};


int32 CGameAttribute::GetVirtualPad(void) const
{
    return m_iVirtualPad;
};


void CGameAttribute::SetInteractive(bool bSet)
{
    m_bIsInteractive = bSet;
};


bool CGameAttribute::IsInteractive(void) const
{
    return m_bIsInteractive;
};


void CGameAttribute::SetGamemode(GAMETYPES::GAMEMODE mode)
{
    m_gamemode = mode;
};


GAMETYPES::GAMEMODE CGameAttribute::GetGamemode(void) const
{
    return m_gamemode;
};


bool CGameAttribute::IsNexusMode(void) const
{
    return (m_gamemode == GAMETYPES::GAMEMODE_NEXUS);
};


bool CGameAttribute::IsPlayDemoMode(void) const
{
    return (m_gamemode == GAMETYPES::GAMEMODE_DEMO);
};

