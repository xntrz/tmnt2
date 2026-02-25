#pragma once

#include "StageID.hpp"
#include "AreaID.hpp"
#include "MapID.hpp"
#include "GameTypes.hpp"

#include "Game/Component/Enemy/EnemyID.hpp"


class CStageInfo
{
public:
    enum CAMERAUPDATE
    {
        CAMERAUPDATE_NORMAL = 0,
        CAMERAUPDATE_BOSSBATTLE,
        CAMERAUPDATE_RIDE,
    };

    enum IMPLEMENT
    {
        IMPLEMENT_NOIMPL = 0,
        IMPLEMENT_EXISTDATA,
        IMPLEMENT_ENABLEPLAY,
        IMPLEMENT_COMPLETED,
    };

public:
    static FNAME GetFileID(STAGEID::VALUE idStage);
    static MAPID::VALUE GetMapID(STAGEID::VALUE idStage);
    static int32 GetBgmNo(STAGEID::VALUE idStage);
    static AREAID::VALUE GetAreaID(STAGEID::VALUE idStage);
    static GAMETYPES::STAGEMODE GetMode(STAGEID::VALUE idStage);
    static float GetEnemyAppearDistance(STAGEID::VALUE idStage);
    static CAMERAUPDATE GetCameraUpdateType(STAGEID::VALUE idStage);
    static float GetObjectSleepDistance(STAGEID::VALUE idStage);
    static ENEMYID::VALUE GetGaugeEnemyID(STAGEID::VALUE idStage, int32 iEnemyIndex);
    static bool IsBossGaugeNecessary(STAGEID::VALUE idStage);
    static MVNAME GetStageInMovieID(STAGEID::VALUE idStage);
    static bool IsPlayMovieOfStageIn(STAGEID::VALUE idStage);
    static MVNAME GetStageOutMovieID(STAGEID::VALUE idStage);
    static bool IsPlayMovieOfStageOut(STAGEID::VALUE idStage);
    static IMPLEMENT GetImplementProgress(STAGEID::VALUE idStage);
};