#include "AIUtils.hpp"
#include "CharacterCompositor.hpp"
#include "EnemyCharacter.hpp"
#include "EnemyUtils.hpp"

#include "Game/Component/Player/Player.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"

#ifdef _DEBUG
#include "Game/System/Misc/DebugShape.hpp"
#endif /* _DEBUG */


static int32 s_iNearerPlayerDataNum = 0;
static CAIUtils::NEARER_PLAYERDATA s_aPlayerDataList[GAMETYPES::PLAYERS_MAX];
static CAIUtils::NEARER_PLAYERDATA* s_apNearerPlayerDataList[GAMETYPES::PLAYERS_MAX];
static bool s_bCheckMoveHole = false;
static CGameObject* s_pGameObjectFind = nullptr;


/*static*/ int32 CAIUtils::GetPlayerNum(void)
{
    return CGameProperty::GetPlayerNum();
};


/*static*/ CPlayerCharacter* CAIUtils::GetActivePlayer(int32 playerNo)
{
    if (playerNo < 0)
        return nullptr;

    if (playerNo >= GetPlayerNum())
        return nullptr;

    CPlayerCharacter* pPlayerChr = CGameProperty::Player(playerNo)->GetCurrentCharacter();
    if (!pPlayerChr)
        return nullptr;

    if (!pPlayerChr->IsActive())
        return nullptr;

    return pPlayerChr;
};


/*static*/ CAIUtils::PLAYER_STATE_FLAG
CAIUtils::GetPlayerStateFlag(PLAYERTYPES::STATUS eStatus)
{
    PLAYER_STATE_FLAG flag = PLAYER_STATE_NULL;

    if (((eStatus >= PLAYERTYPES::STATUS_ATTACK_A) && (eStatus <= PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP)) ||
        ((eStatus >= PLAYERTYPES::STATUS_THROW)    && (eStatus <= PLAYERTYPES::STATUS_THROW_COMBINATION)) ||
         (eStatus == PLAYERTYPES::STATUS_THROWN_COMBINATION))
    {
        flag = PLAYER_STATE_ATTACK;
    };

    if ((eStatus == PLAYERTYPES::STATUS_ATTACK_KNIFE) || (eStatus == PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP))
    {
        flag |= PLAYER_STATE_KNIFE;
    };

    if (((eStatus >= PLAYERTYPES::STATUS_JUMP_READY) && (eStatus <= PLAYERTYPES::STATUS_JUMP_READY)) ||
         (eStatus == PLAYERTYPES::STATUS_ATTACK_JUMP))
    {
        flag |= PLAYER_STATE_AERIAL;
    };

    if (((eStatus >= PLAYERTYPES::STATUS_FLYAWAY_FRONT) && (eStatus <= PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_BACK)) ||
        ((eStatus >= PLAYERTYPES::STATUS_DOWN_FRONT)    && (eStatus <= PLAYERTYPES::STATUS_TUMBLER_BACK)))
    {
        flag |= PLAYER_STATE_DOWN;
    };

    if ((eStatus >= PLAYERTYPES::STATUS_DINDLE) && (eStatus <= PLAYERTYPES::STATUS_FREEZE))
    {
        flag |= PLAYER_STATE_UNUSUAL;
    };        
    
    if ((eStatus >= PLAYERTYPES::STATUS_GUARD_READY) && (eStatus <= PLAYERTYPES::STATUS_GUARD_FINISH))
    {
        flag |= PLAYER_STATE_GUARD;
    };

    if ((flag & PLAYER_STATE_DOWN) ||
        ((eStatus >= PLAYERTYPES::STATUS_CAUGHT) && (eStatus <= PLAYERTYPES::STATUS_THROWN_COMBINATION_END)))
    {
        flag |= PLAYER_STATE_IMPOSSIBLE;
    }
    else
    {
        flag |= PLAYER_STATE_ENABLE;
    };

    return flag;
};


/*static*/ CPlayerCharacter* CAIUtils::FindFirstPlayer(void)
{
    s_pGameObjectFind = nullptr;
    return FindNextPlayer();
};


/*static*/ CPlayerCharacter* CAIUtils::FindNextPlayer(void)
{
    s_pGameObjectFind = CGameObjectManager::GetNext(GAMEOBJECTTYPE::PLAYER, s_pGameObjectFind);
    if (!s_pGameObjectFind)
        return nullptr;
    
    ASSERT(s_pGameObjectFind->GetType() == GAMEOBJECTTYPE::PLAYER);
    CPlayer* pPlayer = static_cast<CPlayer*>(s_pGameObjectFind);

    return pPlayer->GetCurrentCharacter();
};


/*static*/ int32 CAIUtils::MakeNearerPlayerData(const CCharacterCompositor* pChrCompositor)
{
    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&vecFootPos);

    return MakeNearerPlayerData(&vecFootPos);
};


/*static*/ int32 CAIUtils::MakeNearerPlayerData(const RwV3d* vecPos)
{
    CPlayerCharacter* apPlayerChr[GAMETYPES::PLAYERS_MAX];
    int32 numActivePlayer = 0;

    int32 playerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        IGamePlayer* pGameplayer = CGameProperty::Player(i);

        CPlayerCharacter* pPlayerChr = pGameplayer->GetCurrentCharacter();
        if (!pPlayerChr)
            continue;

        if (pPlayerChr->IsActive())
            apPlayerChr[numActivePlayer++] = pPlayerChr;
    };

    s_iNearerPlayerDataNum = 0;
    for (int32 i = 0; i < numActivePlayer; ++i)
    {
        RwV3d vecPlayerPos = Math::VECTOR3_ZERO;
        apPlayerChr[i]->GetFootPosition(&vecPlayerPos);

        float distance = CEnemyUtils::GetDistance(vecPos, &vecPlayerPos);

        float angle = 0.0f;
        vecPlayerPos.y = vecPos->y;
        if (!Math::Vec3_IsEqual(vecPos, &vecPlayerPos))
            angle = CEnemyUtils::GetDirection(vecPos, &vecPlayerPos);

        NEARER_PLAYERDATA* pData = &s_aPlayerDataList[s_iNearerPlayerDataNum];
        pData->no       = i;
        pData->distance = distance;
        pData->angle    = angle;

        s_apNearerPlayerDataList[s_iNearerPlayerDataNum++] = pData;
    };

    NEARER_PLAYERDATA** itBegin = std::begin(s_apNearerPlayerDataList);
    NEARER_PLAYERDATA** itEnd   = std::begin(s_apNearerPlayerDataList) + s_iNearerPlayerDataNum;

    std::sort(itBegin, itEnd, [](const NEARER_PLAYERDATA* a, const NEARER_PLAYERDATA* b) {
        return (a->distance < b->distance);
    });
    
    return s_iNearerPlayerDataNum;
};


/*static*/ float CAIUtils::GetAimTargetValueForNearerPlayerData(const CEnemyCharacter* pEnemyChr,
                                                                const NEARER_PLAYERDATA* pData,
                                                                float fJudgeWait)
{
    float fDirMe = pEnemyChr->Compositor().GetDirection();

    float fDirDiff = pData->angle - fDirMe;
    fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
    fDirDiff = std::fabs(fDirDiff);

    float fDirDiffRatio = (fDirDiff * MATH_INV_PI);

    float fRatioOfRearView = pEnemyChr->AICharacteristic().m_fRatioOfRearView;
    float fRatioOfFrontView = pEnemyChr->AICharacteristic().m_fRatioOfFrontView;

    float fViewRatio = fRatioOfFrontView + ((fRatioOfRearView - fRatioOfFrontView) * fDirDiffRatio);

    float fRadiusOfAction = pEnemyChr->AICharacteristic().m_fRadiusOfAction;
    float fDistanceOfSuitable = pEnemyChr->AICharacteristic().m_fDistanceOfSuitable;

    float fViewDistance = (fRadiusOfAction - fDistanceOfSuitable) * fViewRatio;

    float fDist = (fDistanceOfSuitable + fViewDistance);

    if (fJudgeWait == 0.0f)
        return fDirDiffRatio;

    return (pData->distance / (fDist * fJudgeWait * 0.2f)) + fDirDiffRatio;
};


/*static*/ const CAIUtils::NEARER_PLAYERDATA*
CAIUtils::GetNearerPlayerData(int32 dataNo)
{
    ASSERT(dataNo >= 0);
    ASSERT(dataNo < s_iNearerPlayerDataNum);

    return s_apNearerPlayerDataList[dataNo];
};


/*static*/ int32 CAIUtils::GetNearerPlayerDataNum()
{
    return s_iNearerPlayerDataNum;
};


/*static*/ float CAIUtils::GetMapHitRadius(const CCharacterCompositor* pChrCompositor)
{
    return pChrCompositor->GetCollisionParameter().m_fRadius;
};


/*static*/ bool CAIUtils::IsInsidePatrolArea(const RwV3d* pos,
                                             const ENEMYTYPES::FEATURE* feature)
{

    RwV3d vecPos = *pos;
    vecPos.y = 0.0f;

    RwV3d vecAt = feature->m_vPatrolOrigin;
    vecAt.y = 0.0f;

    if (Math::FEqual(feature->m_fPatrolRadius, 0.0f))
        return true;

    float fDist = CEnemyUtils::GetDistance(&vecPos, &vecAt);
    return (fDist < feature->m_fPatrolRadius);
};


/*static*/ CAIUtils::OBSTACLE_TYPE
CAIUtils::CheckObstacle(const RwV3d* vecPos,
                        float fBaseHeight,
                        float fSafetyHeight,
                        float* pfObstacleHeight /*= nullptr*/)
{

    float fMapHeight = CWorldMap::GetMapHeight(vecPos);
    MAPTYPES::ATTRIBUTE attribute = CWorldMap::GetCollisionResultAttribute();

    if ((fMapHeight == CWorldMap::INVALID_MAP_HEIGHT) || (attribute == MAPTYPES::ATTRIBUTE_DEATH))
    {
        if (pfObstacleHeight)
            *pfObstacleHeight = CWorldMap::INVALID_MAP_HEIGHT;

        return OBSTACLE_HOLE;
    };

    float fToMapHeight = (fMapHeight - fBaseHeight);

    if (pfObstacleHeight)
        *pfObstacleHeight = fToMapHeight;

    if (fToMapHeight > fSafetyHeight)
        return OBSTACLE_OBJ;

    if (fToMapHeight < -fSafetyHeight)
        return OBSTACLE_STEP;

    return OBSTACLE_NONE;
};


/*static*/ CAIUtils::OBSTACLE_TYPE
CAIUtils::CheckObstacle(const CCharacterCompositor* pChrCompositor,
                        float fCheckDist,
                        float fSafetyHeight,
                        float* pfObstacleHeight /*= nullptr*/)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&vecPos);

    float fBaseHeight = vecPos.y;

    RwV3d vecStep = { 0.0f, 0.0f, fCheckDist };
    pChrCompositor->RotateVectorByDirection(&vecStep, &vecStep);

    Math::Vec3_Add(&vecPos, &vecPos, &vecStep);

    return CheckObstacle(&vecPos, fBaseHeight, fSafetyHeight, pfObstacleHeight);
};


/*static*/ bool CAIUtils::CheckHole(const RwV3d* ptStart,
                                    const RwV3d* ptEnd,
                                    int32 distanceDivNum)
{

    for (int32 i = 0; i < distanceDivNum; ++i)
    {
        float t = static_cast<float>(i + 1) / static_cast<float>(distanceDivNum);

        RwV3d pt = Math::VECTOR3_ZERO;
        Math::Vec3_Lerp(&pt, ptStart, ptEnd, t);

        float fHeight = CWorldMap::GetMapHeight(&pt);

        if (fHeight == CWorldMap::INVALID_MAP_HEIGHT)
            return true;

        if (CWorldMap::GetCollisionResultAttribute() == MAPTYPES::ATTRIBUTE_DEATH)
            return true;
    };

    return false;
};


/*static*/ bool CAIUtils::CheckStep(const RwV3d* ptStart,
                                    const RwV3d* ptEnd,
                                    float* pfHeight /*= nullptr*/,
                                    RwV3d* stepCrossPt /*= nullptr*/,
                                    RwV3d* stepNormal /*= nullptr*/)
{

    if (!CWorldMap::CheckCollisionLine(ptStart, ptEnd))
        return false;

    const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
    ASSERT(pCollisionResult != nullptr);

    if (pCollisionResult->m_vNormal.y > 0.5f)
        return false;

    if (stepCrossPt)
        *stepCrossPt = pCollisionResult->m_vClosestPt;

    if (stepNormal)
        *stepNormal = pCollisionResult->m_vNormal;

    RwV3d vPt = pCollisionResult->m_vClosestPt;
    vPt.y += 100.0f;

    if (!CWorldMap::CheckCollisionHeight(&vPt, 1.0f, (CWorldMap::INVALID_MAP_HEIGHT * 2.0f)))
        return false;

    pCollisionResult = CWorldMap::GetCollisionResult();
    ASSERT(pCollisionResult != nullptr);

    if (pfHeight)
        *pfHeight = pCollisionResult->m_vClosestPt.y;

    return true;
};


/*static*/ bool CAIUtils::CheckStep(const CCharacterCompositor* pChrCompositor,
                                    float fDistance,
                                    float* pfHeight /*= nullptr*/,
                                    RwV3d* stepCrossPt /*= nullptr*/,
                                    RwV3d* stepNormal /*= nullptr*/)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&vecPos);
    vecPos.y += 0.2f;

    RwV3d vecStep = { 0.0f, 0.0f, fDistance };
    pChrCompositor->RotateVectorByDirection(&vecStep, &vecStep);
    Math::Vec3_Add(&vecStep, &vecStep, &vecPos);

    RwV3d ptStart = vecPos;
    RwV3d ptEnd   = vecStep;

    return CheckStep(&ptStart, &ptEnd, pfHeight, stepCrossPt, stepNormal);
};


/*static*/ void CAIUtils::GetThickLine(RwV3d* ptStart,
                                       RwV3d* ptEnd,
                                       const RwV3d* ptBaseStart,
                                       const RwV3d* ptBaseEnd,
                                       float fThickSize,
                                       THICKLINE_TYPE type)
{
    switch (type)
    {
    case THICKLINE_LEFT:
    case THICKLINE_RIGHT:
        {
            float fAngle = (type == THICKLINE_RIGHT ? -MATH_DEG2RAD(90.0f) : MATH_DEG2RAD(90.0f));

            RwV3d vecDir = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecDir, ptBaseEnd, ptBaseStart);

            RwV3d vecBuf = Math::VECTOR3_ZERO;
            Math::Vec3_Normalize(&vecBuf, &vecDir);

            RwMatrix matBuf;
            RwMatrixSetIdentityMacro(&matBuf);
            Math::Matrix_RotateY(&matBuf, fAngle);

            RwV3d vecCorrect = Math::VECTOR3_ZERO;
            RwV3dTransformVector(&vecCorrect, &vecBuf, &matBuf);
            Math::Vec3_Scale(&vecCorrect, &vecCorrect, (fThickSize * 0.5f));

            Math::Vec3_Add(ptStart, ptBaseStart, &vecCorrect);

            Math::Vec3_Add(ptEnd, ptStart, &vecDir);
        }
        break;

    case THICKLINE_CENTER:
        {
            *ptStart = *ptBaseStart;
            *ptEnd   = *ptBaseEnd;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


/*static*/ bool CAIUtils::CheckMapCollisionThick(const RwV3d* ptStart,
                                                 const RwV3d* ptEnd,
                                                 float fThickSize)
{
    RwV3d vecStart = Math::VECTOR3_ZERO;
    RwV3d vecEnd   = Math::VECTOR3_ZERO;

    for (int32 i = 0; i < THICKLINE_NUM; ++i)
    {
        GetThickLine(&vecStart, &vecEnd, ptStart, ptEnd, fThickSize, static_cast<THICKLINE_TYPE>(i));

        if (CWorldMap::CheckCollisionLine(&vecStart, &vecEnd))
            return true;
    };

    return false;
};


/*static*/ bool CAIUtils::IsObjectViewArea(const CEnemyCharacter* pEnemyChr,
                                           const RwV3d* vecPos)
{
    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    pEnemyChr->Compositor().GetFootPosition(&vecFootPos);

    RwV3d vecAt = *vecPos;
    vecAt.y = vecFootPos.y;

    float fDist = CEnemyUtils::GetDistance(&vecFootPos, &vecAt);

    float fDir = 0.0f;
	if (!Math::Vec3_IsEqual(&vecFootPos, &vecAt))
		fDir = CEnemyUtils::GetDirection(&vecFootPos, &vecAt);

    return IsObjectViewArea2(pEnemyChr, fDist, fDir);
};


/*static*/ bool CAIUtils::IsObjectViewArea2(const CEnemyCharacter* pEnemyChr,
                                            float fDistance,
                                            float fDirection)
{
    float fDirMe = pEnemyChr->Compositor().GetDirection();

    float fDirDiff = fDirection - fDirMe;
    fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
    fDirDiff = std::fabs(fDirDiff);

    float fDirDiffRatio = (fDirDiff * MATH_INV_PI);

    float fRatioOfRearView  = pEnemyChr->AICharacteristic().m_fRatioOfRearView;
    float fRatioOfFrontView = pEnemyChr->AICharacteristic().m_fRatioOfFrontView;

    float fViewRatio = fRatioOfFrontView + ((fRatioOfRearView - fRatioOfFrontView) * fDirDiffRatio);

    float fRadiusOfAction = pEnemyChr->AICharacteristic().m_fRadiusOfAction;
    float fDistanceOfSuitable = pEnemyChr->AICharacteristic().m_fDistanceOfSuitable;

    float fViewDistance = (fRadiusOfAction - fDistanceOfSuitable) * fViewRatio;

    return ((fDistanceOfSuitable + fViewDistance) > fDistance);
};


/*static*/ void CAIUtils::GetLineForRange(RwV3d* ptStart,
                                          RwV3d* ptEnd,
                                          const RwV3d* ptBaseStart,
                                          const RwV3d* ptBaseEnd,
                                          float fMaxRange)
{
    RwV3d vecBuf = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecBuf, ptBaseEnd, ptBaseStart);

    float fDist = Math::Vec3_Length(&vecBuf);
    if (fDist > fMaxRange)
    {
        Math::Vec3_Normalize(&vecBuf, &vecBuf);
        Math::Vec3_Scale(&vecBuf, &vecBuf, fMaxRange);
    };

    *ptStart = *ptBaseStart;
    
    *ptEnd = *ptStart;
    Math::Vec3_Add(ptEnd, ptEnd, &vecBuf);
};


/*static*/ CAIUtils::CHECKMOVELINE_RESULT
CAIUtils::CheckMoveLine(const RwV3d* ptStart,
                        const RwV3d* ptEnd,
                        float fMapHitRadius,
                        int32 checkHoleDivNum,
                        float fJumpInitSpeed,
                        RwV3d* vecObstacleHitPoint /*= nullptr*/)
{
    s_bCheckMoveHole = false;

    float fObstacleHeight = 0.0f;
    RwV3d vecNormal = Math::VECTOR3_ZERO;
    if (CheckStep(ptStart, ptEnd, &fObstacleHeight, vecObstacleHitPoint, &vecNormal))
    {
        float fAngle =
            (std::atan2(ptEnd->x - ptStart->x, ptEnd->z - ptStart->z) - std::atan2(vecNormal.x, vecNormal.z));

        fAngle = CEnemyUtils::RadianCorrect(fAngle);
        fAngle = std::fabs(fAngle);

        if (fAngle > MATH_DEG2RAD(112.5f))
        {
            float fMapHeight = CWorldMap::GetMapHeight(ptStart);

            if (!Math::FEqual(fObstacleHeight, fMapHeight))
            {
                float fToObstacleHeight = (fObstacleHeight - fMapHeight);

                float fGravity = CGameProperty::GetGravity();
                float fJumpHeight = CEnemyUtils::GetJumpableHeight(fJumpInitSpeed, fGravity);
                if (fJumpHeight >= fToObstacleHeight)
                {
                    RwV3d vecStart = *ptStart;
                    vecStart.y = (fObstacleHeight + 0.1f);

                    RwV3d vecEnd = *ptEnd;
                    vecEnd.y = vecStart.y;

                    if (!CheckMapCollisionThick(&vecStart, &vecEnd, fMapHitRadius))
                        return CHECKMOVELINE_RESULT_NEEDJUMP;
                };				
            };

            return CHECKMOVELINE_RESULT_FAILURE;
        };
    };

    if (CheckHole(ptStart, ptEnd, checkHoleDivNum))
    {
        s_bCheckMoveHole = true;
        return CHECKMOVELINE_RESULT_FAILURE;
    };

    return CHECKMOVELINE_RESULT_PERMISSION;
};


/*static*/ bool CAIUtils::CheckMoveLine_FindHole(void)
{
    return s_bCheckMoveHole;
};


/*static*/ float CAIUtils::CalcMovePointEscapeObstacle(RwV3d* result,
                                                       const CEnemyCharacter* pEnemyChr,
                                                       const RwV3d* vecMarkPos,
                                                       float fCheckSafePointInterval,
                                                       float fCheckSafePointDistance)
{

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    pEnemyChr->Compositor().GetFootPosition(&vecFootPos);

    float fMapHeight = CWorldMap::GetMapHeight(&vecFootPos);

    RwV3d vecPos = vecFootPos;
    vecPos.y = (fMapHeight + 0.2f);

    RwV3d vecAt = *vecMarkPos;
    vecAt.y = vecPos.y;

    float fHitRadius = GetMapHitRadius(&pEnemyChr->Compositor());

    return CalcMovePointEscapeObstacle(result,
                                       &vecPos,
                                       &vecAt,
                                       fHitRadius,
                                       fCheckSafePointInterval,
                                       fCheckSafePointDistance);
};


/*static*/ float CAIUtils::CalcMovePointEscapeObstacle(RwV3d* result,
                                                       const RwV3d* vecPos,
                                                       const RwV3d* vecMarkPos,
                                                       float fMapHitRadius,
                                                       float fCheckSafePointInterval,
                                                       float fCheckSafePointDistance)
{
    RwV3d avecCollisionResultNormal[THICKLINE_NUM]  = { Math::VECTOR3_ZERO };
    RwV3d avecCollisionResultCrossPt[THICKLINE_NUM] = { Math::VECTOR3_ZERO };
    int32 collisionResultNum = 0;

    for (int32 i = 0; i < THICKLINE_NUM; ++i)
    {
        RwV3d vecStart = Math::VECTOR3_ZERO;
        RwV3d vecEnd   = Math::VECTOR3_ZERO;
        GetThickLine(&vecStart, &vecEnd, vecPos, vecMarkPos, fMapHitRadius, static_cast<THICKLINE_TYPE>(i));

        if (CWorldMap::CheckCollisionLine(&vecStart, &vecEnd))
        {
            bool bAlreadyEntryFlag = false;

            const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
            ASSERT(pCollisionResult != nullptr);

            for (int32 j = 0; j < collisionResultNum; ++j)
            {
                if (Math::Vec3_IsEqual(&avecCollisionResultNormal[j], &pCollisionResult->m_vNormal))
                {
                    bAlreadyEntryFlag = true;
                    break;
                };
            };

            if (!bAlreadyEntryFlag)
            {
                avecCollisionResultCrossPt[collisionResultNum] = pCollisionResult->m_vClosestPt;
                avecCollisionResultNormal[collisionResultNum]  = pCollisionResult->m_vNormal;
                ++collisionResultNum;
            };
        };
    };

    if (!collisionResultNum)
    {
        *result = *vecMarkPos;

        RwV3d vecBuff = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecBuff, vecMarkPos, vecPos);

        float fDist = Math::Vec3_Length(&vecBuff);
        return fDist;
    };

    enum SAFEPOINT
    {
        SAFEPOINT_INVALID = -1,
        SAFEPOINT_LEFT = 0,
        SAFEPOINT_RIGHT,

        SAFEPOINTNUM,
    };

    RwV3d avecSafePoint[THICKLINE_NUM][SAFEPOINTNUM] = { Math::VECTOR3_ZERO };
    int32 type = SAFEPOINT_INVALID;
    int32 escapePointNo = -1;
    float fDistMin = 99999.0f;

    CWorldMap::COLLISIONRESULT collisionResultBuff;
    std::memset(&collisionResultBuff, 0, sizeof(collisionResultBuff));

    for (int32 i = 0; i < collisionResultNum; ++i)
    {
        collisionResultBuff.m_vClosestPt = avecCollisionResultCrossPt[i];
        collisionResultBuff.m_vNormal    = avecCollisionResultNormal[i];

        ESCAPEPOINT_RESULT escapePointResult = CalcEscapePointForHitPlane(avecSafePoint[i],
                                                                          &collisionResultBuff,
                                                                          fMapHitRadius,
                                                                          fCheckSafePointInterval,
                                                                          fCheckSafePointDistance);
        if (escapePointResult & ESCAPEPOINT_RESULT_LEFT)
        {
            RwV3d vecBuff = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecBuff, vecMarkPos, &avecSafePoint[i][SAFEPOINT_LEFT]);

            float fDist = Math::Vec3_Length(&vecBuff);
            if (fDist < fDistMin)
            {
                fDistMin      = fDist;
                escapePointNo = i;
                type          = SAFEPOINT_LEFT;
            };
        };

        if (escapePointResult & ESCAPEPOINT_RESULT_RIGHT)
        {
            RwV3d vecBuff = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecBuff, vecMarkPos, &avecSafePoint[i][SAFEPOINT_RIGHT]);

            float fDist = Math::Vec3_Length(&vecBuff);
            if (fDist < fDistMin)
            {
                fDistMin      = fDist;
                escapePointNo = i;
                type          = SAFEPOINT_RIGHT;
            };
        };
    };

    if (escapePointNo >= 0)
    {
        *result = avecSafePoint[escapePointNo][type];

        RwV3d vecBuff = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecBuff, result, vecPos);

        float fDist = Math::Vec3_Length(&vecBuff);
        return (fDist + fDistMin);
    };

    return -1.0f;
};


/*static*/ CAIUtils::ESCAPEPOINT_RESULT
CAIUtils::CalcEscapePointForHitPlane(RwV3d* avecPos,
                                     const CWorldMap::COLLISIONRESULT* pCollisionResult,
                                     float fMapHitRadius,
                                     float fCheckInterval,
                                     float fCheckDistance)
{
    RwV3d vecNormal = pCollisionResult->m_vNormal;
    vecNormal.y = 0.0f;
    Math::Vec3_Normalize(&vecNormal, &vecNormal);

    if (Math::Vec3_IsEqual(&vecNormal, &Math::VECTOR3_ZERO))
        return ESCAPEPOINT_RESULT_NONE;

    RwV3d vecNormalNegate = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecNormalNegate, &vecNormal, -1.0f);

    RwV3d vecCheckBasePos = pCollisionResult->m_vClosestPt;
    Math::Vec3_Scale(&vecNormal, &vecNormal, (fMapHitRadius * 1.05f));
    Math::Vec3_Add(&vecCheckBasePos, &vecCheckBasePos, &vecNormal);
    vecCheckBasePos.y = (CWorldMap::GetMapHeight(&vecCheckBasePos) + 0.2f);

    ASSERT(!Math::FEqual(fCheckInterval, 0.0f));
    int32 checkNum = static_cast<int32>(fCheckDistance / fCheckInterval);

    RwMatrix matBuf;
    RwMatrixSetIdentityMacro(&matBuf);

    RwV3d avecDirBase[2] = { Math::VECTOR3_ZERO };

    Math::Matrix_RotateY(&matBuf, MATH_DEG2RAD(90.0f));
    RwV3dTransformVector(&avecDirBase[0], &vecNormalNegate, &matBuf);

    Math::Matrix_RotateY(&matBuf, -MATH_DEG2RAD(90.0f));
    RwV3dTransformVector(&avecDirBase[1], &vecNormalNegate, &matBuf);

    float afSafePointDistance[2] = { -1.0f, -1.0f };

    for (int32 i = 0; i < 2; ++i)
    {
        float fCheckDistanceNow = 0.0f;

        for (int32 j = 0; j < checkNum; ++j)
        {
            fCheckDistanceNow += fCheckInterval;

            RwV3d vecStart = Math::VECTOR3_ZERO;
            Math::Vec3_Scale(&vecStart, &avecDirBase[i], fCheckDistanceNow);
            Math::Vec3_Add(&vecStart, &vecStart, &vecCheckBasePos);

            RwV3d vecEnd = Math::VECTOR3_ZERO;
            Math::Vec3_Scale(&vecEnd, &vecNormalNegate, (fMapHitRadius * 1.5f));
            Math::Vec3_Add(&vecEnd, &vecEnd, &vecStart);

            if (!CheckMapCollisionThick(&vecStart, &vecEnd, fMapHitRadius))
            {
                vecEnd   = vecStart;
                vecStart = vecCheckBasePos;

                if (!CheckMapCollisionThick(&vecStart, &vecEnd, fMapHitRadius) &&
                    !CheckHole(&vecStart, &vecEnd, j + 1))
                {
                    avecPos[i] = vecEnd;
                    afSafePointDistance[i] = fCheckDistanceNow;
                    break;
                };
            };
        };
    };

    ESCAPEPOINT_RESULT result = ESCAPEPOINT_RESULT_NONE;

    if (afSafePointDistance[0] > 0.0f)
        result |= ESCAPEPOINT_RESULT_LEFT;

    if (afSafePointDistance[1] > 0.0f)
        result |= ESCAPEPOINT_RESULT_RIGHT;

    return result;
};


//
// *********************************************************************************
//


/*static*/ bool CAIUtils6045::CalcGroundPointAllPlayer(RwV3d* pvecPos)
{
    ASSERT(pvecPos != nullptr);

    RwV3d vecBuf = Math::VECTOR3_ZERO;
    int32 numPos = 0;

    int32 playersNum = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < playersNum; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (!pPlayerChr)
            continue;

        RwV3d vecFootPos = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPos);

        Math::Vec3_Add(&vecBuf, &vecBuf, &vecFootPos);

        ++numPos;
    };

    if (numPos)
    {
        Math::Vec3_Scale(pvecPos, &vecBuf, (1.0f / static_cast<float>(numPos)));
        return true;
    };

    return false;
};


/*static*/ bool CAIUtils6045::CheckObstacleBetweenPosToPos(const RwV3d* pvecStart,
                                                           const RwV3d* pvecEnd,
                                                           float fThickSize /*= -1.0f*/)
{
    ASSERT(pvecStart != nullptr);
    ASSERT(pvecEnd != nullptr);

    if (fThickSize <= 0.0f)
    {
        RwV3d vecStart = *pvecStart;
        RwV3d vecEnd = *pvecEnd;

        if (!CWorldMap::CheckCollisionLine(&vecStart, &vecEnd))
            return false;
    }
    else
    {
        bool bFound = false;

        for (int32 i = 0; i < CAIUtils::THICKLINE_NUM; ++i)
        {
            RwV3d vecStart = Math::VECTOR3_ZERO;
            RwV3d vecEnd = Math::VECTOR3_ZERO;
            
            CAIUtils::GetThickLine(&vecStart,
                                   &vecEnd,
                                   pvecStart,
                                   pvecEnd,
                                   fThickSize,
                                   static_cast<CAIUtils::THICKLINE_TYPE>(i));

            bFound = CWorldMap::CheckCollisionLine(&vecStart, &vecEnd);
            if (bFound)
                break;
        };

        if (!bFound)
            return false;
    };

    const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
    ASSERT(pCollisionResult != nullptr);

    float fDistToPos = CEnemyUtils::GetDistance(pvecStart, pvecEnd);
    float fDistToMap = CEnemyUtils::GetDistance(pvecStart, &pCollisionResult->m_vClosestPt);

    return (fDistToMap < fDistToPos);
};


/*static*/ bool CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(const CCharacterCompositor* pChrCompositor,
                                                                int32 iPlayerNo,
                                                                bool bCheckThick,
                                                                float fCheckDistance /*= -1.0f*/)
{
    ASSERT(pChrCompositor != nullptr);

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerNo);
    if (!pPlayerChr)
        return true;
    
    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);
    vecFootPosPlayer.y += 0.2f;

    RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&vecFootPosEnemy);
    vecFootPosEnemy.y += 0.2f;

    if (fCheckDistance >= 0.0f)
    {
        RwV3d vecBuf = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecBuf, &vecFootPosPlayer, &vecFootPosEnemy);
        Math::Vec3_Normalize(&vecBuf, &vecBuf);
        Math::Vec3_Scale(&vecBuf, &vecBuf, fCheckDistance);

        Math::Vec3_Add(&vecFootPosPlayer, &vecFootPosEnemy, &vecBuf);
    };

    if (bCheckThick)
    {
        float fHitRadius = CAIUtils::GetMapHitRadius(pChrCompositor);
        fHitRadius *= 2.0f;
        
        return CheckObstacleBetweenPosToPos(&vecFootPosEnemy, &vecFootPosPlayer, fHitRadius);
    };
    
    return CheckObstacleBetweenPosToPos(&vecFootPosEnemy, &vecFootPosPlayer);
};


//
// *********************************************************************************
//


CRandNormal::CRandNormal(void)
: m_generate(true)
, m_r(0.0f)
, m_theta(0.0f)
{
    ;
};


float CRandNormal::Generate(void)
{
    /*
     *  Box–Muller transform (basic form)
     */

    if (m_generate)
    {
        m_generate = false;

        float u = Math::RandFloat();
        ASSERT(u >= 0.0f);
        ASSERT(u <= 1.0f);

        m_r = std::sqrt(-2.0f * std::logf(1.0f - u));
        m_theta = Math::RandFloat() * MATH_PI2;
    }
    else
    {
        m_generate = true;
    };

    if (m_generate)
        return std::sin(m_theta) * m_r;
    else
        return std::cos(m_theta) * m_r;
};