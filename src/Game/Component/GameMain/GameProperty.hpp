#pragma once

#include "GameTypes.hpp"
#include "GameEvent.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CMapCamera;
class IGamePlayer;
class CGameGimmick;
class CGameEnemy;
class CEnemy;
class CGimmick;

class CGameProperty
{
public:
	static void Initialize(void);
	static void Terminate(void);
	static void Period(void);
	static GAMETYPES::DIFFICULTY GetDifficulty(void);
	static float GetElapsedTime(void);
	static float GetTotalElapsedTime(void);
	static float GetGravity(void);
	static void SetCurrentRwCamera(RwCamera* pRwCamera);
	static void SetMapCamera(CMapCamera* pMapCamera);
	static CMapCamera* GetMapCamera(void);
	static void GetCameraViewMatrix(RwMatrix* pMatrix);
	static void GetCameraFrameMatrix(RwMatrix* pMatrix);
	static bool AdjustWithinMoveBoundary(RwV3d* pvOut, RwV3d* pvIn);
	static void SetCameraVibration(float fPower, float fTime, int32 nFreq);
	static void StopCameraVibration(void);
	static void AddPlayerCharacter(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
	static IGamePlayer* Player(int32 no); // does not increment ref count
    static IGamePlayer* GetGamePlayer(int32 no); // increments ref count (require manually call Release())    
    static int32 GetPlayerNum(void);
	static CEnemy* GetEnemy(int32 no); // may return nullptr if not alive
	static int32 GetEnemyMax(void);
	static CGimmick* GetGimmick(int32 no); // may return nullptr if not alive
    static int32 GetGimmickMax(void);
    static CGameGimmick* GetGameGimmick(int32 no);
    static int32 GetGameGimmickNum(void);
    static CGameEnemy* GetGameEnemy(int32 no);
    static int32 GetGameEnemyNum(void);
};