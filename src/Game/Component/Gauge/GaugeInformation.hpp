#pragma once

#include "Game/Component/GameMain/StageID.hpp"
#include "Game/Component/Enemy/EnemyID.hpp"


class CGaugeInformation
{
public:
    enum PAUSESTATUS
    {
        PAUSESTATUS_INIT = 0,
        PAUSESTATUS_RUN,
        PAUSESTATUS_BACK,
        PAUSESTATUS_RET_WORLD,
        PAUSESTATUS_RET_TITLE,
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void DispInit(void);
    static void PausePeriod(void);
    static void PauseDraw(void);
    static PAUSESTATUS GetPauseStatus(void);
    static void MissionInfoSet(STAGEID::VALUE idStage);
    static void MissionInfoPeriod(void);
    static void MissionInfoDraw(void);
    static void SetBattleNexusInfo(void);
    static bool IsBattleNexusInfoEnd(void);
    static void DispBattleNexusInfo(void);
    static void BossGaugeEnable(bool bEnable, int32 no, ENEMYID::VALUE idEnemy);
    static void BossGaugeSet(int32 no, int32 hp);
    static void BossGaugeDraw(int32 no);
};