#pragma once

#include "RideTypes.hpp"

#include "Game/Component/Player/Player.hpp"


class CRideCharacter;


class CRidePlayer : public CPlayer
{
public:
    static CRidePlayer* New(int32 nNo, int32 nControllerNo);
    static void Delete(CRidePlayer* pPlayer);

    CRidePlayer(const char* pszName, int32 nNo, int32 nControllerNo);
    virtual ~CRidePlayer(void);
    virtual void SetCurrentCharacter(PLAYERID::VALUE idChr, bool bPlayChangeEffect) override;
    void AddCharacter(PLAYERID::VALUE idChr, GAMETYPES::COSTUME costume);
    int32 GetScore(RIDETYPES::SCOREKIND scorekind);

private:
    CRideCharacter* GetRideCharacter(int32 no) const;
};