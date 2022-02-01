#pragma once


#include "Game/Component/GameMain/GamePlayerContainer.hpp"


class CRideGamePlayer;


class CRideGamePlayerContainer : public IGamePlayerContainer
{
public:
    CRideGamePlayerContainer(void);
    virtual ~CRideGamePlayerContainer(void);
    virtual void AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume) override;
    virtual IGamePlayer& GamePlayer(int32 no) const override;
    virtual int32 GetPlayerNum(void) const override;

private:
    CRideGamePlayer* getRideGamePlayer(int32 no) const;

private:
    CRideGamePlayer* m_pRideGamePlayerTable;
    int32 m_nNumPlayer;
};