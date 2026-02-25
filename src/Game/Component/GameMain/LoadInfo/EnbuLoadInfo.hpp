#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CEnbuLoadInfo final : public CGameObjLoadInfo
{
public:
    CEnbuLoadInfo(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    virtual ~CEnbuLoadInfo(void);
    virtual FNAME GetFileID(void) const override;
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    PLAYERID::VALUE m_idPlayer;
    GAMETYPES::COSTUME m_costume;
};