#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"

#include "Game/Component/Player/PlayerID.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CPlayerLoadInfo final : public CGameObjLoadInfo
{
private:
    static const char* LISTFILE_PATH;
    
public:
    CPlayerLoadInfo(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    virtual ~CPlayerLoadInfo(void);
    virtual int32 GetFileID(void) const override;
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    PLAYERID::VALUE m_idPlayer;
    GAMETYPES::COSTUME m_costume;
};