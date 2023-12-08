#pragma once

#include "GameTypes.hpp"
#include "StageID.hpp"
#include "MapID.hpp"

#include "Game/Component/Effect/EffectID.hpp"
#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CGameObjLoadInfo;


class CGameLoader
{
public:    
    enum MODE
    {
        MODE_NORMAL = 0,
        MODE_MAKE_INTERGRAL_LIST,
    };

private:    
    static const int32 LOAD_DEPTH_MAX = 2;
    
    static CGameLoader& Instance(void);
    
    CGameLoader(void);
    ~CGameLoader(void);
    void loadObjectRecursive(CGameObjLoadInfo& rInfo, int32 nDepth = 0);
    
public:
    static void SetMode(MODE mode);
    static void LoadStageCommonData(GAMETYPES::STAGEMODE stagemode);
    static void LoadStage(STAGEID::VALUE idStage);
    static void LoadPlayer(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    static void LoadRidePlayer(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    static void LoadEnbu(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    static void LoadEnemy(ENEMYID::VALUE idEnemy);
    static void LoadEffect(EFFECTID::VALUE idEffect);
    static void LoadMap(MAPID::VALUE idMap);

private:
    MODE m_mode;
    bool m_bInTheBlock;
};