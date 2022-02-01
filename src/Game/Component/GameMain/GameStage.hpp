#pragma once

#include "Game/Component/GameData/GamePlayResult.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/StageID.hpp"


class CEnemy;
class CMapCamera;
class CGameObject;
class IGameStageExGauge;
class IGameStagePause;
class IGameStageCameraUpdater;


class CGameStage final
{
public:
    static const int32 EXGAUGE_MAX = 5;

    enum SYSTEMSTATE
    {
        SYSTEMSTATE_NONE = 0,
        SYSTEMSTATE_NORMAL,
        SYSTEMSTATE_DEMO,        
    };
    
    enum PAUSETYPE
    {
        PAUSETYPE_MENU = 0,
        PAUSETYPE_TUTORIAL,        
    };

    enum RESULT
    {
        RESULT_NONE = 0,
        RESULT_GAMECLEAR,
        RESULT_GAMEOVER,
        RESULT_PLAYERDED,
        RESULT_RET_TITLE,
        RESULT_RET_STAGESEL,
    };

public:
    static CGameStage* GetCurrent(void);

    CGameStage(void);
    ~CGameStage(void);
    void Initialize(void);
    void Terminate(void);
    void Start(void);
    void Stop(void);
    void Period(void);
    void Draw(void) const;
    void AddGauge(void);
    void AddStageObjects(void);
    void AddPlayers(bool bProtect = true);
    void StartPlay(void);
    bool CheckPauseMenu(void) const;
    void StartPause(PAUSETYPE pausetype, void* param);
    bool IsPaused(void) const;
    void BeginDemo(void);
    void EndDemo(void);
    void BeginPlayerNegateDamage(void);
    void EndPlayerNegateDamager(void);
    bool SetResult(RESULT result);
    void NotifyGameClear(CGamePlayResult::CLEARSUB clearsub);
    void NotifyGameOver(void);
    void NotifyPlayerDead(int32 nPlayerNo);
    int32 GetDeadPlayer(void) const;
    void NotifyEnemyDead(CEnemy* pEnemy);
    void NotifyEnemyDamaged(CEnemy* pEnemy, int32 nAmount);
    RESULT GetResult(void) const;
    CMapCamera* GetMapCamera(void) const;
    void SetCameraUpdater(CStageInfo::CAMERAUPDATE cameraupdtype);
    void SetCameraUpdater(IGameStageCameraUpdater* pCameraUpdater);
    
private:
    void pause(void);
    void resume(void);
    void updatePlayData(void);
    void updateResultData(void);
    void createCamera(RpWorld* pWorld);
    void destroyCamera(void);
    void createExGauge(void);
    void destroyExGauge(void);
    void startExGauge(void);
    void stopExGauge(void);
    void updateExGauge(void);
    void changeSystemState(SYSTEMSTATE sysstate);

private:
    static CGameStage* m_pCurrent;    
    STAGEID::VALUE m_idStage;
    int32 m_nPauseLevel;
    SYSTEMSTATE m_systemstate;
    bool m_bCreatedGauge;
    bool m_bCreatedGimmick;
    bool m_bCreatedRadar;
    bool m_bPlayStarted;
    float m_fTimer;
    RESULT m_result;
    int32 m_resultParam;
    bool m_bMultipleBoss;
    RpWorld* m_pWorld;
    CMapCamera* m_pMapCamera;
    CGameObject* m_pRecoveryObj;
    IGameStageCameraUpdater* m_pCameraUpdater;
    IGameStagePause* m_pPauseHandler;
    IGameStageExGauge* m_apExGauge[EXGAUGE_MAX];
};