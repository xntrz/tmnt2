#pragma once

#include "GameTypes.hpp"

#include "Game/Component/Player/PlayerID.hpp"


class IGamePlayer;
class CGamePlayer;


class IGamePlayerContainer
{
public:
	virtual ~IGamePlayerContainer(void){};
    virtual void AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume) = 0;
    virtual IGamePlayer& GamePlayer(int32 no) const = 0;
    virtual int32 GetPlayerNum(void) const = 0;
};


class CGamePlayerContainer : public IGamePlayerContainer
{
public:
    CGamePlayerContainer(void);
    virtual ~CGamePlayerContainer(void);
    virtual void AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume) override;
    virtual IGamePlayer& GamePlayer(int32 no) const override;
    virtual int32 GetPlayerNum(void) const override;

private:
    CGamePlayer* getGamePlayer(int32 no) const;
    
private:
    CGamePlayer* m_pGamePlayerTable;
    int32 m_nNumPlayer;
};