#pragma once

#include "GameTypes.hpp"
#include "GameEvent.hpp"
#include "Game/Component/Player/PlayerID.hpp"


class CMapCamera;
class IGamePlayer;
class CGameEnemy;
class CGameGimmick;


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
	static void SetCameraVibration(float fPower, float fTime, uint32 uFreq);
	static int32 GetPlayerNum(void);
	static int32 GetPlayerMax(void);
	static void AddPlayerCharacter(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
	static IGamePlayer& Player(int32 no);
	static int32 GetEnemyMax(void);
	static CGameEnemy& Enemy(int32 no);
	static int32 GetGimmickMax(void);
	static CGameGimmick& Gimmick(int32 no);
};