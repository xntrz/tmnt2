#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"

#include "Game/Component/Player/PlayerID.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CRidePlayerLoadInfo final : public CGameObjLoadInfo
{
public:
    CRidePlayerLoadInfo(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    virtual ~CRidePlayerLoadInfo(void);
    virtual FNAME GetFileID(void) const override;
    virtual bool MakeFileName(char* pszFilenameBuff) const override;
    bool IsSpaceStage(void) const;

private:
    PLAYERID::VALUE m_idPlayer;
    GAMETYPES::COSTUME m_costume;
};