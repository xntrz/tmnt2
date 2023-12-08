#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CDialog;
class CAreaWorkPool;


class CNexusMenuSequence final : public CAnim2DSequence
{
private:
    enum NEXUSMENU
    {
        NEXUSMENU_NONE = 0,
        NEXUSMENU_KITTY_OPEN,
        NEXUSMENU_MONSTER_OPEN,
        NEXUSMENU_FOOT_COMBAT,
        NEXUSMENU_BATTLE_NEXUS,
        NEXUSMENU_EXIT,
    };

public:
    static CProcess* Instance(void);

    CNexusMenuSequence(void);
    virtual ~CNexusMenuSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void BeginFadein(void) override;

private:
    void replaceText(void);
    void replaceTexture(void);
    bool menuMessageProc(void);
    void pressFlashKeyByNexusState(GAMETYPES::NEXUSID idNexus);

private:
    NEXUSMENU m_nexusmenu;
};