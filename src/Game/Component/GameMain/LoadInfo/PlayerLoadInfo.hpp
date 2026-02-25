#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"

#include "Game/Component/Player/PlayerID.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CPlayerCommonLoadInfo final : public CGameObjLoadInfo
{
public:
    CPlayerCommonLoadInfo(void) {};
    virtual ~CPlayerCommonLoadInfo(void) {};
    virtual bool MakeFileName(char* pszFilenameBuff) const override;
};


class CPlayerLoadInfo final : public CGameObjLoadInfo
{
   
public:
    CPlayerLoadInfo(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    virtual ~CPlayerLoadInfo(void);
    virtual FNAME GetFileID(void) const override;
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    PLAYERID::VALUE m_idPlayer;
    GAMETYPES::COSTUME m_costume;
};