#pragma once

#include "GameTypes.hpp"
#include "StageID.hpp"

#include "Game/Component/Player/PlayerID.hpp"


class CGameObjLoadInfo;


class CGameLoader
{
private:
    static const int32 LOAD_DEPTH_MAX = 2;
    
    enum MODE
    {
        MODE_NORMAL = 0,
        MODE_MAKE_INTERGRAL_LIST,
    };

private:    
    static CGameLoader& Instance(void);
    
    CGameLoader(void);
    ~CGameLoader(void);
    void loadObjectRecursive(CGameObjLoadInfo& rInfo, int32 nDepth = 0);
    
public:
    static void LoadStageCommonData(GAMETYPES::STAGEMODE stagemode);
    static void LoadStage(STAGEID::VALUE idStage);
    static void LoadPlayer(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    static void LoadRidePlayer(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    static void LoadEnbu(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);

private:
    MODE m_mode;
    bool m_bInTheBlock;
};