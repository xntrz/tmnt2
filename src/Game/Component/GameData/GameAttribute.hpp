#pragma once

#include "Game/Component/GameMain/GameTypes.hpp"


class CGameAttribute
{
public:
    CGameAttribute(void);
    ~CGameAttribute(void);
    void Clear(void);
    void SetVirtualPad(int32 iController);
    int32 GetVirtualPad(void) const;
    void SetInteractive(bool bSet);
    bool IsInteractive(void) const;
    void SetGamemode(GAMETYPES::GAMEMODE mode);
    GAMETYPES::GAMEMODE GetGamemode(void) const;
    bool IsNexusMode(void) const;
    bool IsPlayDemoMode(void) const;

private:
    int32 m_iVirtualPad;
    bool m_bIsInteractive;
    GAMETYPES::GAMEMODE m_gamemode;
};