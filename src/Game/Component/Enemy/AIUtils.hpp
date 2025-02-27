#pragma once

#include "EnemyTypes.hpp"

#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/System/Map/WorldMap.hpp"


class CPlayerCharacter;
class CEnemyCharacter;
class CCharacterCompositor;


class CAIUtils
{
public:
    struct NEARER_PLAYERDATA
    {
        int32 no;
        float distance;
        float angle;    
    };

    enum THICKLINE_TYPE
    {
        THICKLINE_CENTER = 0,
        THICKLINE_LEFT,
        THICKLINE_RIGHT,
        
        THICKLINE_NUM,
    };

    enum OBSTACLE_TYPE
    {
        OBSTACLE_NONE = 0,
        OBSTACLE_OBJ,
        OBSTACLE_STEP,
        OBSTACLE_HOLE,
    };

    enum CHECKMOVELINE_RESULT
    {
        CHECKMOVELINE_RESULT_PERMISSION = 0,
        CHECKMOVELINE_RESULT_NEEDJUMP,
        CHECKMOVELINE_RESULT_FAILURE,
    };

    enum ESCAPEPOINT_RESULT : uint32
    {
        ESCAPEPOINT_RESULT_NONE = (1 << 0),
        ESCAPEPOINT_RESULT_LEFT = (1 << 1),
        ESCAPEPOINT_RESULT_RIGHT= (1 << 2),
        
        ESCAPEPOINT_RESULT_BOTH = ESCAPEPOINT_RESULT_LEFT
                                | ESCAPEPOINT_RESULT_RIGHT,
    };

    DEFINE_ENUM_FLAG_OPS(ESCAPEPOINT_RESULT, friend);

    enum PLAYER_STATE_FLAG : uint32
    {
        PLAYER_STATE_NULL       = 0,
        PLAYER_STATE_ENABLE     = (1 << 0),
        PLAYER_STATE_IMPOSSIBLE = (1 << 1),
        PLAYER_STATE_ATTACK     = (1 << 2),
        PLAYER_STATE_AERIAL     = (1 << 3),
        PLAYER_STATE_DOWN       = (1 << 4),
        PLAYER_STATE_UNUSUAL    = (1 << 5), // stun, bind, sleep and so on
        PLAYER_STATE_KNIFE      = (1 << 6),
        PLAYER_STATE_GUARD      = (1 << 7),
        
        PLAYER_STATE_ALL        = PLAYER_STATE_ENABLE
                                | PLAYER_STATE_IMPOSSIBLE
                                | PLAYER_STATE_ATTACK
                                | PLAYER_STATE_AERIAL
                                | PLAYER_STATE_DOWN
                                | PLAYER_STATE_UNUSUAL
                                | PLAYER_STATE_KNIFE
                                | PLAYER_STATE_GUARD,
    };

    DEFINE_ENUM_FLAG_OPS(PLAYER_STATE_FLAG, friend);

public:
    static int32 GetPlayerNum(void);
    
    static CPlayerCharacter* GetActivePlayer(int32 playerNo);

    static PLAYER_STATE_FLAG GetPlayerStateFlag(PLAYERTYPES::STATUS eStatus);

    static CPlayerCharacter* FindFirstPlayer(void);

    static CPlayerCharacter* FindNextPlayer(void);

    static int32 MakeNearerPlayerData(const CCharacterCompositor* pChrCompositor);

    static int32 MakeNearerPlayerData(const RwV3d* vecPos);

    static float GetAimTargetValueForNearerPlayerData(const CEnemyCharacter* pEnemyChr,
                                                      const NEARER_PLAYERDATA* pData,
                                                      float fJudgeWait);
                                                      
    static const NEARER_PLAYERDATA* GetNearerPlayerData(int32 dataNo);

    static int32 GetNearerPlayerDataNum();

    static float GetMapHitRadius(const CCharacterCompositor* pChrCompositor);

    static bool IsInsidePatrolArea(const RwV3d* pos,
                                   const ENEMYTYPES::FEATURE* feature);

    static OBSTACLE_TYPE CheckObstacle(const RwV3d* vecPos,
                                       float fBaseHeight,
                                       float fSafetyHeight,
                                       float* pfObstacleHeight = nullptr);

    static OBSTACLE_TYPE CheckObstacle(const CCharacterCompositor* pChrCompositor,
                                       float fCheckDist,
                                       float fSafetyHeight,
                                       float* pfObstacleHeight = nullptr);

    static bool CheckHole(const RwV3d* ptStart,
                          const RwV3d* ptEnd,
                          int32 distanceDivNum);

    static bool CheckStep(const RwV3d* ptStart,
                          const RwV3d* ptEnd,
                          float* pfHeight = nullptr,
                          RwV3d* vecStepCrossPt = nullptr,
                          RwV3d* vecStepNormal = nullptr);

    static bool CheckStep(const CCharacterCompositor* pChrCompositor,
                          float fDistance,
                          float* pfHeight = nullptr,
                          RwV3d* vecStepCrossPt = nullptr,
                          RwV3d* vecStepNormal = nullptr);

    static void GetThickLine(RwV3d* ptStart,
                             RwV3d* ptEnd,
                             const RwV3d* ptBaseStart,
                             const RwV3d* ptBaseEnd,
                             float fThickSize,
                             THICKLINE_TYPE type);

    static bool CheckMapCollisionThick(const RwV3d* ptStart,
                                       const RwV3d* ptEnd,
                                       float fThickSize);

    static bool IsObjectViewArea(const CEnemyCharacter* pEnemyChr,
                                 const RwV3d* vecPos);

    static bool IsObjectViewArea2(const CEnemyCharacter* pEnemyChr,
                                 float fDistance,
                                 float fDirection);

    static float GetViewDistance(const CEnemyCharacter* pEnemyChr);

    static void GetLineForRange(RwV3d* start,
                                RwV3d* end,
                                const RwV3d* baseStart,
                                const RwV3d* baseEnd,
                                float fMaxRange);

    static CHECKMOVELINE_RESULT CheckMoveLine(const RwV3d* ptStart,
                                              const RwV3d* ptEnd,
                                              float fMapHitRadius,
                                              int32 checkHoleDivNum,
                                              float fJumpInitSpeed,
                                              RwV3d* vecObstacleHitPoint = nullptr);

    static bool CheckMoveLine_FindHole(void);

    static float CalcMovePointEscapeObstacle(RwV3d* result,
                                             const CEnemyCharacter* pEnemyChr,
                                             const RwV3d* markPos,
                                             float fCheckSafePointInterval,
                                             float fCheckSafePointDistance);

    static float CalcMovePointEscapeObstacle(RwV3d* result,
                                             const RwV3d* vecPos,        
                                             const RwV3d* vecMarkPos,
                                             float fMapHitRadius,
                                             float fCheckSafePointInterval,
                                             float fCheckSafePointDistance);

    static ESCAPEPOINT_RESULT CalcEscapePointForHitPlane(RwV3d* avecPos,
                                                         const CWorldMap::COLLISIONRESULT* pCollisionResult,
                                                         float fMapHitRadius,
                                                         float fCheckInterval,
                                                         float fCheckDistance);
};


class CAIUtils6045
{
public:
    static bool CalcGroundPointAllPlayer(RwV3d* pvecPos);

    static bool CheckObstacleBetweenPosToPos(const RwV3d* pvecStart,
                                             const RwV3d* pvecEnd,
                                             float fThickSize = -1.0f);
    
    static bool CheckObstacleBetweenEnemyToPlayer(const CCharacterCompositor* pChrCompositor,
                                                  int32 iPlayerNo,
                                                  bool bCheckThick,
                                                  float fCheckDistance = -1.0f);
};