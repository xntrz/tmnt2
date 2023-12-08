#pragma once

#include "GameTypes.hpp"

#include "Game/Component/Player/PlayerID.hpp"


class CGamePlayer;


class CGamePlayerContainer
{
public:
    CGamePlayerContainer(void);
    virtual ~CGamePlayerContainer(void);
    virtual CGamePlayer* GamePlayer(int32 no) const;
    virtual int32 GetPlayerNum(void) const;
    virtual void AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    virtual void Remove(CGamePlayer* pGamePlayer);

private:
    CGamePlayer* getGamePlayer(int32 no) const;
    
private:
    CGamePlayer* m_pGamePlayerTable;
    int32 m_nNumPlayer;
};