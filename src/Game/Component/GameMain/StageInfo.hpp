#pragma once

#include "StageID.hpp"
#include "AreaID.hpp"
#include "MapID.hpp"
#include "GameTypes.hpp"

#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/System/Movie/MovieID.hpp"


class CStageInfo
{
protected:
    struct INFO;

    static const INFO m_aStageInfo[];
    
    static const INFO& Info(STAGEID::VALUE idStage);
    
public:
    enum CAMERAUPDATE
    {
        CAMERAUPDATE_NORMAL = 0,
        CAMERAUPDATE_BOSSBATTLE,
        CAMERAUPDATE_RIDE,
    };

public:
    static int32 GetFileID(STAGEID::VALUE idStage);
    static MAPID::VALUE GetMapID(STAGEID::VALUE idStage);
    static int32 GetBgmNo(STAGEID::VALUE idStage);
    static AREAID::VALUE GetAreaID(STAGEID::VALUE idStage);
    static GAMETYPES::STAGEMODE GetMode(STAGEID::VALUE idStage);
    static float GetEnemyAppearDistance(STAGEID::VALUE idStage);
    static CAMERAUPDATE GetCameraUpdateType(STAGEID::VALUE idStage);
    static float GetObjectSleepDistance(STAGEID::VALUE idStage);
    static ENEMYID::VALUE GetGaugeEnemyID(STAGEID::VALUE idStage, int32 iEnemyIndex);
    static bool IsBossGaugeNecessary(STAGEID::VALUE idStage);
    static MOVIEID::VALUE GetStageInMovieID(STAGEID::VALUE idStage);
    static bool IsPlayMovieOfStageIn(STAGEID::VALUE idStage);
    static MOVIEID::VALUE GetStageOutMovieID(STAGEID::VALUE idStage);
    static bool IsPlayMovieOfStageOut(STAGEID::VALUE idStage);
};