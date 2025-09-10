#include "081Spasmosaur.hpp"

#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"

#ifdef _DEBUG
#include "Game/Component/Enemy/EnemyDebug.hpp"
#endif /*_DEBUG */


#define DEFAULT_THINK_TIMER     (1.5f)
#define HIT_NUM_UNTIL_SHRINK    (2)
#define COLOR_CHANGE_TIME       (0.5f)
#define TIMEOUT_FOR_HIT_HELP    (3.0f)
#define TIMEOUT_FOR_DAMAGE      (1.0f)
#define NUM_HIT_MISS_UNTIL_RUSH (8)


static const RwV3d MOUTH_OFFSET = { 0.0f, -1.0f, -0.85f };


namespace MOTIONNAMES
{
    static const char* OPEN_IDLE          = "Idle1";
    static const char* OPEN_SPIT          = "E1";
    static const char* OPEN_SHRINK_START  = "YSyokusyu1A";
    static const char* OPEN_SHRINK_LOOP   = "YSyokusyu1B";
    static const char* OPEN_SHRINK_END    = "YSyokusyu1C";
    static const char* OPEN_ATTACK_A      = "A1";

    static const char* CLOSE_IDLE         = "Idle2";
    static const char* CLOSE_SPIT         = "E2";
    static const char* CLOSE_SHRINK_START = "YSyokusyu2A";
    static const char* CLOSE_SHRINK_LOOP  = "YSyokusyu2B";
    static const char* CLOSE_SHRINK_END   = "YSyokusyu2C";
    static const char* CLOSE_ATTACK_A     = "A2";
    static const char* CLOSE_ATTACK_B     = "B2";
    static const char* CLOSE_ATTACK_C     = "C2";
    
    static const char* WALL_START         = "YarareA";
    static const char* WALL_LOOP          = "YarareB";
    static const char* WALL_END           = "YarareC";

    static const char* THROW              = "D";
}; /* namespace MOTIONNAMES */


static inline void MoveFootStepsSoundProc(const CEnemyCharacter* pEnemyChr)
{
    float fMotionNowT = pEnemyChr->Compositor().GetMotionTime();
    float fMotionPreT = (fMotionNowT - CGameProperty::GetElapsedTime());
    float fMotionEndT = pEnemyChr->Compositor().GetMotionEndTime();

    const float fPrecision = 256.0f;

    float fNowT = (fMotionNowT * fPrecision);
    float fPreT = (fMotionPreT * fPrecision);
    float fMidT = (fMotionEndT * (fPrecision * 0.5f));
    float fEndT = (fMotionEndT * fPrecision);

    bool bIsHalfOfMotion = (fPreT < fMidT) &&
                           (fMidT <= fNowT);

    bool bIsEndOfMotion = (fPreT < fEndT) &&
                          (fEndT <= fNowT);

    if (bIsHalfOfMotion || bIsEndOfMotion)
        CGameSound::PlayObjectSE(&pEnemyChr->Compositor(), SDCODE_SE(4342));
};


//
// *********************************************************************************
//


void C081Spasmosaur::STATE::CheckPlayerStatus(void)
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    pCompositor->GetFootPosition(&vecMyFootPos);

    float fMyDir = pCompositor->GetDirection();

    numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        if (!pPlrChr)
            continue;            

        /* init player data */
        RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecPlrFootPos);

        aPlayerData[i].vecFootPos = vecPlrFootPos;
        aPlayerData[i].fDirection = pPlrChr->GetDirection();

        /* init player info */
        RwV3d vecDistance = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDistance, &vecPlrFootPos, &vecMyFootPos);
        vecDistance.y = 0.0f;

        aPlayerInfo[i].fDistance = std::sqrt(vecDistance.x * vecDistance.x +
                                             vecDistance.z * vecDistance.z);

        if (!Math::Vec3_Equal(&vecDistance, &Math::VECTOR3_ZERO))
            aPlayerInfo[i].fDirection = std::atan2(vecDistance.x, vecDistance.z);

        float fDirDiff = (aPlayerInfo[i].fDirection - fMyDir);
        fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);

        aPlayerInfo[i].fDirection = fDirDiff;

        aPlayerInfo[i].bIsTrouble = ((pPlrChr->GetStatus() >= PLAYERTYPES::STATUS_STUN) &&
                                     (pPlrChr->GetStatus() <= PLAYERTYPES::STATUS_BIND));
    };
};


ENEMYTYPES::STATUS
C081Spasmosaur::STATE::GetNextAttackStatus(void) const
{
    int32 playerNo = GetFrontNearestPlayerNo();
    if (playerNo < 0)
        return ENEMYTYPES::STATUS_IDLE;

    if (bMouthOpen)
        return GetNextAttackStatusOnMouthOpen(playerNo);
    else
        return GetNextAttackStatusOnMouthClose(playerNo);
};


int32 C081Spasmosaur::STATE::GetFrontNearestPlayerNo(void) const
{
    int32 nearestPlayerNo = -1;
    float fNearestDist = TYPEDEF::FLOAT_MAX;
    for (int32 i = 0; i < numPlayers; ++i)
    {
        float fDir = aPlayerInfo[i].fDirection;
        fDir = std::fabs(fDir);

        if (fDir <= MATH_DEG2RAD(45.0f) && (aPlayerInfo[i].fDistance < fNearestDist))
        {
            nearestPlayerNo = i;
            fNearestDist = aPlayerInfo[i].fDistance;
        };
    };

    return nearestPlayerNo;
};


ENEMYTYPES::STATUS
C081Spasmosaur::STATE::GetNextAttackStatusOnMouthOpen(int32 playerNo) const
{
    const PLAYER_INFO* pPlayerInfo = &aPlayerInfo[playerNo];

    if (pPlayerInfo->fDistance < 10.0f)
        return static_cast<ENEMYTYPES::STATUS>(C081Spasmosaur::STATUS_SPIT_OPEN_SLEEP);

    if (pPlayerInfo->fDistance < 21.0f)
    {
        float fDir = pPlayerInfo->fDirection;
        fDir = std::fabs(fDir);

        if (fDir >= MATH_DEG2RAD(3.0f))
            return static_cast<ENEMYTYPES::STATUS>(STATUS_ATTACK_OPEN_A);
        else
            return static_cast<ENEMYTYPES::STATUS>(STATUS_SPIT_OPEN_BIND);
    };

    return ENEMYTYPES::STATUS_IDLE;
};


ENEMYTYPES::STATUS
C081Spasmosaur::STATE::GetNextAttackStatusOnMouthClose(int32 playerNo) const
{
    const PLAYER_INFO* pPlayerInfo = &aPlayerInfo[playerNo];

    if (pPlayerInfo->bIsTrouble)
    {
        if (pPlayerInfo->fDistance > 12.0f)
            return static_cast<ENEMYTYPES::STATUS>(STATUS_ATTACK_CLOSE_A);
        else
            return static_cast<ENEMYTYPES::STATUS>(STATUS_ATTACK_CLOSE_C);
    };

    if (pPlayerInfo->fDistance >= 21.0f)
        return ENEMYTYPES::STATUS_IDLE;

    if (pPlayerInfo->fDistance >= 10.0f)
    {
        float fDir = pPlayerInfo->fDirection;
        fDir = std::fabs(fDir);

        if (fDir >= MATH_DEG2RAD(3.0f))
        {
            if (pPlayerInfo->fDistance >= 15.0f)
                return ENEMYTYPES::STATUS_THROW;
            else
                return static_cast<ENEMYTYPES::STATUS>(STATUS_ATTACK_CLOSE_B);
        }
        else
        {
            return static_cast<ENEMYTYPES::STATUS>(STATUS_SPIT_CLOSE_BIND);
        };
    };

    return static_cast<ENEMYTYPES::STATUS>(STATUS_SPIT_CLOSE_SLEEP);
};


bool C081Spasmosaur::STATE::IsLoopMotionEnd(void) const
{
    float fMotionEndT = pCompositor->GetMotionEndTime();
    float fMotionNowT = pCompositor->GetMotionTime();
    float dt = CGameProperty::GetElapsedTime();

    return ((fMotionNowT + dt) > fMotionEndT);
};


bool C081Spasmosaur::STATE::IsResponseTime(void) const
{
    return (fThinkTimer <= 0.0f);
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CHideStatusObserver::OnStart(void) /*override*/
{
    ;
};


/*virtual*/ C081Spasmosaur::CHideStatusObserver::RESULT
C081Spasmosaur::CHideStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CHideStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CIdleStatusObserver::OnStart(void) /*override*/
{
    m_bThinkResponse = false;

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    if (pState->bMouthOpen)
        EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::OPEN_IDLE, true);
    else
        EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::CLOSE_IDLE, true);
};


/*virtual*/ C081Spasmosaur::CIdleStatusObserver::RESULT
C081Spasmosaur::CIdleStatusObserver::Observing(void) /*override*/
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    if (pState->IsResponseTime())
    {
        if (pState->GetNextAttackStatus() == ENEMYTYPES::STATUS_IDLE)
        {
            m_bThinkResponse = false;
        }
        else
        {
            if (m_bThinkResponse)
            {
                m_bThinkResponse = false;
                m_nextStatus = pState->GetNextAttackStatus();
                return RESULT_END;
            };

            uint8 freq = EnemyChr().FrequencyParameter(C081Spasmosaur::FREQUENCY_THINKING);
            pState->fThinkTimer = static_cast<float>(freq) / 60.0f;

            m_bThinkResponse = true;
        };
    };

    if (!m_bThinkResponse)
    {
        int32 playerNo = pState->GetFrontNearestPlayerNo();
        if (playerNo < 0)
            playerNo = pState->targetPlayerNo;

        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(playerNo);
        ASSERT(pPlrChr != nullptr);

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        float fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecFootPosPlr);
        EnemyChr().Compositor().RotateDirection(fDir, 0.2f);
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CIdleStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CWalkStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::WALK, true);

    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    CEffectManager::SetActive(pState->hEffectSmoke, true);
    pState->phase = PHASE_NORMAL;
    pState->fColorChangeTimer = COLOR_CHANGE_TIME;
};


/*virtual*/ C081Spasmosaur::CWalkStatusObserver::RESULT
C081Spasmosaur::CWalkStatusObserver::Observing(void) /*override*/
{
    RwV3d vecFootPosMy = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMy);

    RwV3d vecDstPos = Math::VECTOR3_ZERO; // center of the map
    vecDstPos.y = vecFootPosMy.y;

    if (CheckDistance(&vecDstPos))
    {
        C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
        pState->bMouthOpen = true;        
        m_nextStatus = ENEMYTYPES::STATUS_IDLE;
        return RESULT_END;
    };

    SetDirection(&vecDstPos);
    SetVelocity();
    MoveFootStepsSoundProc(&EnemyChr());

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CWalkStatusObserver::OnEnd(void) /*override*/
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    CEffectManager::SetActive(pState->hEffectSmoke, false);

    return m_nextStatus;
};


bool C081Spasmosaur::CWalkStatusObserver::CheckDistance(const RwV3d* pvecDstPos) const
{
    RwV3d vecFootPosMy = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMy);

    if (CEnemyUtils::GetDistance(&vecFootPosMy, pvecDstPos) > 1.0f)
        return false;

    return true;
};


void C081Spasmosaur::CWalkStatusObserver::SetDirection(const RwV3d* pvecDstPos)
{
    float fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), pvecDstPos);
    float fRotRate = 0.75f;
    EnemyChr().Compositor().RotateDirection(fDir, fRotRate);
};


void C081Spasmosaur::CWalkStatusObserver::SetVelocity(void)
{
    float fWalkMoveSpeed = EnemyChr().Feature().m_fWalkMoveSpeed;

    RwV3d vecVelocity = { 0.0f, 0.0f, fWalkMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);

    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CRunStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::RUN, true);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8197));

    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    CEffectManager::SetActive(pState->hEffectSmoke, true);

    if (pState->bMadFristMsg)
    {
        pState->bMadFristMsg = false;
        CMessageManager::Request(SEGROUPID::VALUE(81));
    }
    else
    {
        CMessageManager::Request(SEGROUPID::VALUE(84));
    };
};


/*virtual*/ C081Spasmosaur::CRunStatusObserver::RESULT
C081Spasmosaur::CRunStatusObserver::Observing(void) /*override*/
{
    MoveFootStepsSoundProc(&EnemyChr());

    if (IsWallHit())
    {
        m_nextStatus = ENEMYTYPES::STATUS_CRASHWALL_FRONT;
        return RESULT_END;
    };

    RwV3d vecRunPos = Math::VECTOR3_ZERO;
    if (!CheckDistance(&vecRunPos))
    {
        m_nextStatus = ENEMYTYPES::STATUS_WALK;
        return RESULT_END;
    };

    bool bDirectionFree = true;
    if (!CheckDirection(&bDirectionFree))
    {
        m_nextStatus = ENEMYTYPES::STATUS_WALK;
        return RESULT_END;
    };

    if (bDirectionFree)
        SetDirection(&vecRunPos);
    
    SetVelocity();

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CRunStatusObserver::OnEnd(void) /*override*/
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    CEffectManager::SetActive(pState->hEffectSmoke, false);

    if (m_nextStatus == ENEMYTYPES::STATUS_CRASHWALL_FRONT)
        pState->numWallMiss = 0;
    else
        ++pState->numWallMiss;
    
    pState->runType = (pState->numWallMiss >= NUM_HIT_MISS_UNTIL_RUSH ? RUNTYPE_RUSH :
                                                                        RUNTYPE_CHASE);
    return m_nextStatus;
};


bool C081Spasmosaur::CRunStatusObserver::IsWallHit(void) const
{
    const CCharacterCompositor::COLLISIONWALLINFO* pWallInfo = EnemyChr().Compositor().GetCollisionWall();
    ASSERT(pWallInfo != nullptr);

    return pWallInfo->m_bHit;
};


bool C081Spasmosaur::CRunStatusObserver::CheckDistance(RwV3d* pvecRunPos) const
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    if (pState->runType == RUNTYPE_RUSH)
    {
        float fDirWallSide = MATH_DEG2RAD(120.0f);
        float fDirMe = EnemyChr().Compositor().GetDirection();

        if ((fDirMe < 0.0f) && (fDirMe >= -fDirWallSide))
        {
            /* right */
            *pvecRunPos = { -34.0f, 0.0f, 20.0f };
        }
        else if ((fDirMe >= 0.0f) && (fDirMe < fDirWallSide))
        {
            /* left */
            *pvecRunPos = { 34.0f, 0.0f, 20.0f };
        }
        else
        {
            /* center */
            *pvecRunPos = { 0.0f, 0.0f, -40.0f };
        };
    }
    else
    {
        ASSERT(pState->runType == RUNTYPE_CHASE);

        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(pState->targetPlayerNo);
        if (!pPlrChr)
            return false;

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        float fDistToPlr = CEnemyUtils::GetDistance(&EnemyChr().Compositor(), &vecFootPosPlr);
        float fDistToMid = CEnemyUtils::GetDistance(&EnemyChr().Compositor(), &Math::VECTOR3_ZERO);

        if ((fDistToPlr > 20.0f) && (fDistToMid > 5.0f))
            return false;

        *pvecRunPos = vecFootPosPlr;
    };

    return true;
};


bool C081Spasmosaur::CRunStatusObserver::CheckDirection(bool* pbDirectionFree) const
{
	RwV3d vecFootPosMy = Math::VECTOR3_ZERO;
	EnemyChr().Compositor().GetFootPosition(&vecFootPosMy);
	vecFootPosMy.y = 0.0f;

	float fDirMe = (Math::Vec3_Equal(&vecFootPosMy, &Math::VECTOR3_ZERO) ? 0.0f : CEnemyUtils::GetDirection(&vecFootPosMy));
    float fDirWallSide = MATH_DEG2RAD(120.0f);

    RwV3d vecWallNormal = Math::VECTOR3_ZERO;
    float fDirWall = 0.0f;

    if ((fDirMe < 0.0f) && (fDirMe >= -fDirWallSide))
        vecWallNormal = {  0.86602539f, 0.0f,  0.5f };  /* right */
    else if ((fDirMe >= 0.0f) && (fDirMe < fDirWallSide))
        vecWallNormal = { -0.86602539f, 0.0f,  0.5f };  /* left */
    else
        vecWallNormal = { 0.0f, 0.0f, -1.0f };          /* center */

    fDirWall = std::atan2(vecWallNormal.x, vecWallNormal.z);

    uint8 crashDistFreq = EnemyChr().FrequencyParameter(C081Spasmosaur::FREQUENCY_CRASH_DIR_DIST);
    float fCrashDistSQ = static_cast<float>(crashDistFreq);

    float fDistSQ = Math::Vec3_Dot(&vecFootPosMy, &vecWallNormal);
    float fDistWallSQ = 40.0f;
    float fDistToWallSQ = fDistWallSQ - fDistSQ;

    if (fDistToWallSQ >= fCrashDistSQ)
    {
        *pbDirectionFree = true;
        return true;
    };

    fDirMe = EnemyChr().Compositor().GetDirection();
    float fDirDiff = fDirWall - fDirMe;
    fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
    fDirDiff = std::fabs(fDirDiff);

    if (fDirDiff >= MATH_DEG2RAD(30.0f))
    {
        *pbDirectionFree = true;
        return false;
    };

    *pbDirectionFree = false;
    return true;
};


void C081Spasmosaur::CRunStatusObserver::SetDirection(const RwV3d* pvecRunPos)
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());

    float fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), pvecRunPos);
    float fRotRate = (pState->runType == RUNTYPE_RUSH) ? 0.05f : 0.9f;

    EnemyChr().Compositor().RotateDirection(fDir, fRotRate);
};


void C081Spasmosaur::CRunStatusObserver::SetVelocity(void)
{
    float fRunMoveSpeed = EnemyChr().Feature().m_fRunMoveSpeed;

    RwV3d vecVelocity = { 0.0f, 0.0f, fRunMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);

    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CShrinkStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_loopCnt = 0;

    const char* pszNowMotion = EnemyChr().Compositor().GetMotionName();
    if (!std::strcmp(pszNowMotion, MOTIONNAMES::CLOSE_SHRINK_LOOP))
    {
        EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::OPEN_SHRINK_LOOP, true);
        m_step = 1;
    }
    else
    {
        C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());

        const char* pszNewMotion = (pState->bMouthOpen ? MOTIONNAMES::OPEN_SHRINK_START :
                                                         MOTIONNAMES::CLOSE_SHRINK_START);
        EnemyChr().Compositor().ChangeMotion(pszNewMotion, true);

        m_step = 0;
    };

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
};


/*virtual*/ C081Spasmosaur::CShrinkStatusObserver::RESULT
C081Spasmosaur::CShrinkStatusObserver::Observing(void) /*override*/
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());

    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;
            
            const char* pszNewMotion = (pState->bMouthOpen ? MOTIONNAMES::OPEN_SHRINK_LOOP :
                                                             MOTIONNAMES::CLOSE_SHRINK_LOOP);
            EnemyChr().Compositor().ChangeMotion(pszNewMotion, true);

            ++m_step;
        }
        break;

    case 1:
        {
            if (!pState->IsLoopMotionEnd())
                break;

            if (++m_loopCnt < 3)
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WALL_END, true);

            const char* pszNewMotion = (pState->bMouthOpen ? MOTIONNAMES::OPEN_SHRINK_END :
                                                             MOTIONNAMES::CLOSE_SHRINK_END);
            EnemyChr().Compositor().ChangeMotion(pszNewMotion, true);

            ++m_step;
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CShrinkStatusObserver::OnEnd(void) /*override*/
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());

    return ((pState->phase == PHASE_NORMAL) ? ENEMYTYPES::STATUS_IDLE : ENEMYTYPES::STATUS_RUN);
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CAttackStatusObserver::OnStart(void) /*override*/
{
    const char* pszMotion = nullptr;

    switch (static_cast<int32>(Status()))
    {
    case C081Spasmosaur::STATUS_ATTACK_OPEN_A:  pszMotion = MOTIONNAMES::OPEN_ATTACK_A; break;
    case C081Spasmosaur::STATUS_ATTACK_CLOSE_A: pszMotion = MOTIONNAMES::CLOSE_ATTACK_A; break;
    case C081Spasmosaur::STATUS_ATTACK_CLOSE_B: pszMotion = MOTIONNAMES::CLOSE_ATTACK_B; break;
    case C081Spasmosaur::STATUS_ATTACK_CLOSE_C: pszMotion = MOTIONNAMES::CLOSE_ATTACK_C; break;
    default: ASSERT(false); break;
    };

    EnemyChr().Compositor().ChangeMotion(pszMotion, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ C081Spasmosaur::CAttackStatusObserver::RESULT
C081Spasmosaur::CAttackStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CAttackStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CThrowStatusObserver::OnStart(void) /*override*/
{
	m_bThrowing = true;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::THROW, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ C081Spasmosaur::CThrowStatusObserver::RESULT
C081Spasmosaur::CThrowStatusObserver::Observing(void) /*override*/
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        m_bThrowing = false;

        float fDir = EnemyChr().Compositor().GetDirection();
        float fThrowSpeed = 15.0f;

        RwV3d vecThrowDirection = { std::sin(fDir + MATH_PI) * fThrowSpeed,
                                    fThrowSpeed,
                                    std::cos(fDir + MATH_PI) * fThrowSpeed, };
        ThrowPlayers(&vecThrowDirection);
    };

    if (m_bThrowing)
    {
        float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
        if (fMotionNowT > 0.3f)
        {
            float fMotionNewT = (fMotionNowT + (CGameProperty::GetElapsedTime() * 2.0f));
            EnemyChr().Compositor().SetMotionTime(fMotionNewT);
            EnemyChr().Compositor().UpdateMatrices();

            for (int32 i = 0; i < pState->numPlayers; ++i)
            {
                THROW_TARGET* pThrowTarget = &pState->aThrowTarget[i];
                if (!pThrowTarget->bThrown)
                    continue;

                CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
                if (!pPlrChr)
                    continue;

                RwV3d vecLiftPos = Math::VECTOR3_ZERO;            
                if (pThrowTarget->bLeft)
                {
                    RwV3d vecOfs = { 0.0f,  1.0f, 0.0f };
                    EnemyChr().Compositor().GetBonePosition(&vecLiftPos, 21, &vecOfs);
                }
                else
                {
                    RwV3d vecOfs = { 0.0f, -1.0f, 0.0f };
                    EnemyChr().Compositor().GetBonePosition(&vecLiftPos, 18, &vecOfs);
                };

                RwV3d vecMyPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetPosition(&vecMyPos);

                Math::Vec3_Add(&vecLiftPos, &vecLiftPos, &vecMyPos);
                vecLiftPos.y += 0.5f;
                
                static CCharacter::MSG_LIFT_INFO s_liftInfo;
                s_liftInfo.m_vPosition = vecLiftPos;
                s_liftInfo.m_fDirection = EnemyChr().Compositor().GetDirection();
                s_liftInfo.m_iStatus = 0;

                CGameObjectManager::SendMessage(pPlrChr, CHARACTERTYPES::MESSAGEID_LIFT, &s_liftInfo);
            };

            EnemyChr().Compositor().SetMotionTime(fMotionNowT);
            EnemyChr().Compositor().UpdateMatrices();
        };
    };
    
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CThrowStatusObserver::OnEnd(void) /*override*/
{
    /* release remain players */
    RwV3d vecFall = Math::VECTOR3_ZERO;
    ThrowPlayers(&vecFall, true);

    return ENEMYTYPES::STATUS_IDLE;
};


void C081Spasmosaur::CThrowStatusObserver::ThrowPlayers(RwV3d* pvecThrowDirection, bool bMiss /*= false*/)
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    for (int32 i = 0; i < pState->numPlayers; ++i)
    {
        if (!pState->aThrowTarget[i].bThrown)
            continue;

        pState->aThrowTarget[i].bThrown = false;

        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        if (!pPlrChr)
            continue;

        int32 msgId = (bMiss ? CHARACTERTYPES::MESSAGEID_MISSTHROW :
                               CHARACTERTYPES::MESSAGEID_THROW);
        CGameObjectManager::SendMessage(pPlrChr, msgId, pvecThrowDirection);
    };
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CSpitStatusObserver::OnStart(void) /*override*/
{
    const char* pszMotion = nullptr;

    switch (static_cast<int32>(Status()))
    {
    case C081Spasmosaur::STATUS_SPIT_OPEN_BIND:
    case C081Spasmosaur::STATUS_SPIT_OPEN_SLEEP:
        pszMotion = MOTIONNAMES::OPEN_SPIT;
        break;

    case C081Spasmosaur::STATUS_SPIT_CLOSE_BIND:
    case C081Spasmosaur::STATUS_SPIT_CLOSE_SLEEP:
        pszMotion = MOTIONNAMES::CLOSE_SPIT;
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().Compositor().ChangeMotion(pszMotion, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
};


/*virtual*/ C081Spasmosaur::CSpitStatusObserver::RESULT
C081Spasmosaur::CSpitStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        
        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecMyPos);

        RwV3d vecMouthPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetBonePosition(&vecMouthPos, 5, &MOUTH_OFFSET);
        Math::Vec3_Add(&vecMouthPos, &vecMouthPos, &vecMyPos);

        bool bSpitBind = (Status() == C081Spasmosaur::STATUS_SPIT_OPEN_BIND) ||
                         (Status() == C081Spasmosaur::STATUS_SPIT_CLOSE_BIND);
        if (bSpitBind)
        {
            RwV3d vecDir = { std::sin(fDir), 0.15f, std::cos(fDir) };

            CMagicManager::CParameter param;
            param.SetPositon(&vecMouthPos);
            param.SetDirection(&vecDir);
            param.SetObject(&EnemyChr().Compositor());

            uint32 hMagic = CMagicManager::Play(MAGICID::ID_SPSM_TBSHOT, &param);
            CMagicManager::SetScale(hMagic, 2.0f);
            CMagicManager::SetSpeed(hMagic, 30.0f);

            CEffectManager::Play(EFFECTID::ID_SPSM_YODAR2, &vecMouthPos, fDir);

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4344));
        }
        else
        {
            float fSpeed = 3.5f;
            RwV3d vecSpeed = { (std::sin(fDir) * fSpeed), 0.0f, (std::cos(fDir) * fSpeed) };

            CMagicManager::CParameter param;
            param.SetPositon(&vecMouthPos);
            param.SetDirection(fDir);
            param.SetObject(&EnemyChr().Compositor());

            MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                        | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                        | MAGICTYPES::FEATURE_LIVETIME
                                        | MAGICTYPES::FEATURE_ATTACK;

            uint32 hMagic = CMagicManager::Play(MAGICID::ID_SLEEPGAS_1, &param);
            CMagicManager::SetHitAttack(hMagic, feature);
            CMagicManager::SetScale(hMagic, 3.0f);
            CMagicManager::SetSpeed(hMagic, &vecSpeed);

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4343));
        };
    };

    if (EnemyChr().Compositor().IsMotionEnd())
    {
        C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
        pState->bMouthOpen = false;

        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CSpitStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CCrashWallStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_loopCnt = 0;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WALL_START, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);

    CEffectManager::Play(EFFECTID::ID_SPSM_KABECRASH, &vecMyPos);

    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    pState->damageSource = DAMAGESOURCE_WALL_CRASH;

    int32 hpMax = EnemyChr().Feature().m_iHPMax;
    int32 damageAmount = ((hpMax / 4) + 1);

    CGameObjectManager::SendMessage(&EnemyChr().Compositor(),
                                    CHARACTERTYPES::MESSAGEID_RECVDMG,
                                    reinterpret_cast<void*>(damageAmount));

    CGameProperty::SetCameraVibration(1.0f, 1.0f, 5);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4345));
};


/*virtual*/ C081Spasmosaur::CCrashWallStatusObserver::RESULT
C081Spasmosaur::CCrashWallStatusObserver::Observing(void) /*override*/
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());

    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WALL_LOOP, true);
                ++m_step;
            };
        }
        break;

    case 1:
        {
            if (!pState->IsLoopMotionEnd())
                break;

            if (++m_loopCnt < 4)
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WALL_END, true);
            ++m_step;
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CCrashWallStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_WALK;
};


//
// *********************************************************************************
//


/*virtual*/ void C081Spasmosaur::CDeathStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_loopCnt = 0;
    m_fTimer = 0.0f;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WALL_START, true);

    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());
    for (int32 i = 0; i < COUNT_OF(STATE::ahEffectDrool); ++i)
        CEffectManager::SetActive(pState->ahEffectDrool[i], false);
};


/*virtual*/ C081Spasmosaur::CDeathStatusObserver::RESULT
C081Spasmosaur::CDeathStatusObserver::Observing(void) /*override*/
{
    C081Spasmosaur::STATE* pState = static_cast<C081Spasmosaur::STATE*>(EnemyChr().SharedData());

    switch (m_step)
    {
    case 0:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WALL_LOOP, true);
                ++m_step;
            };            
        }
        break;

    case 1:
        {
            if (!pState->IsLoopMotionEnd())
                break;

            if (++m_loopCnt < 4)
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WALL_END, true);
            ++m_step;
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DEATH, true);
                ++m_step;
            };            
        }
        break;

    case 3:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer > 2.0f)
                ++m_step;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C081Spasmosaur::CDeathStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


C081Spasmosaur::C081Spasmosaur(void)
: CEnemyCharacter(ENEMYID::ID_SPASMOSAUR)
, m_rand()
, m_state()
{
    /* init shared state */
    InitState();

    /* C081Spasmosaur status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new C081Spasmosaur::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new C081Spasmosaur::CIdleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new C081Spasmosaur::CWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                    new C081Spasmosaur::CRunStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_SHRINK,             new C081Spasmosaur::CShrinkStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_ATTACK_OPEN_A,      new C081Spasmosaur::CAttackStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_ATTACK_CLOSE_A,     new C081Spasmosaur::CAttackStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_ATTACK_CLOSE_B,     new C081Spasmosaur::CAttackStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_ATTACK_CLOSE_C,     new C081Spasmosaur::CAttackStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THROW,                  new C081Spasmosaur::CThrowStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_SPIT_OPEN_BIND,     new C081Spasmosaur::CSpitStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_SPIT_CLOSE_BIND,    new C081Spasmosaur::CSpitStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_SPIT_OPEN_SLEEP,    new C081Spasmosaur::CSpitStatusObserver);
    AttachStatusObserver(C081Spasmosaur::STATUS_SPIT_CLOSE_SLEEP,   new C081Spasmosaur::CSpitStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FRONT,        new C081Spasmosaur::CCrashWallStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new C081Spasmosaur::CDeathStatusObserver);
    
    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = nullptr;
    parameter.m_bToon           = false;
    parameter.m_fShadowRadius   = 16.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 5.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 8.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 1.0f;
    parameter.m_feature.m_fDashMoveTime         = 1.0f;
    parameter.m_feature.m_fRotateRate           = 1.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 1.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = 1.0f;
    parameter.m_feature.m_fAerialAcceleration   = 1.0f;
    parameter.m_feature.m_iHPMax                = 1000;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 1.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 10.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 20.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_THINKING           0   */  { 60, 30, 10 },
        /* FREQUENCY_CRASH_DIR_DIST     1   */  { 15, 13, 11 }, // crash wall angle distance condition
    };

    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

	bool bReplace = false;
    Initialize(&parameter, bReplace);

    /* init light */
    CWorldMap::SetAmbientLightColor({ 0.9f, 0.9f, 0.9f, 1.0f });
    CEnemyUtils::SetAmbientLightEnable(&Compositor(), false);

    /* init body collision */
    Compositor().SetEnableBodyHitSelfToOther(false);
};


/*virtual*/ C081Spasmosaur::~C081Spasmosaur(void)
{
    TermEffect();
};


/*virtual*/ void C081Spasmosaur::Run(void) /*override*/
{
    RunAI();
    CEnemyCharacter::Run();
    UpdateEffectMouthDrool();
    UpdateEffectRunSmoke();
    UpdateBodyColorChange();
    UpdateHelpMessage();

    if (m_state.fTimeoutOnDamage > 0.0f)
        m_state.fTimeoutOnDamage -= CGameProperty::GetElapsedTime();
};


/*virtual*/ void C081Spasmosaur::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);

    if ((pCatch->GetResult() == CHitCatchData::RESULT_THROWFRONT) ||
        (pCatch->GetResult() == CHitCatchData::RESULT_THROWBACK))
    {
        ASSERT(pCatch->GetObject()->GetType() == GAMEOBJECTTYPE::CHARACTER);
        ASSERT(static_cast<CCharacter*>(pCatch->GetObject())->GetCharacterType() == CCharacter::TYPE_PLAYER);

        CPlayerCharacter* pPlrChr = static_cast<CPlayerCharacter*>(pCatch->GetObject());
        int32 playerNo = pPlrChr->GetPlayerNo();
        if (!m_state.aThrowTarget[playerNo].bThrown)
        {
            m_state.aThrowTarget[playerNo].bThrown = true;

            CHitAttackData* pAttack = pCatch->GetAttack();
            ASSERT(pAttack->GetAttackNo() <= 7);

            m_state.aThrowTarget[playerNo].bLeft = (pAttack->GetAttackNo() >= 4);

            CGameObjectManager::SendMessage(pPlrChr, CHARACTERTYPES::MESSAGEID_CATCH);

            pPlrChr->RequestDamage(pAttack->GetPower());
        };
    };
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C081Spasmosaur::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    if (TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS))
        return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;

    ENEMYTYPES::STATUS statusNow = GetStatus();
    bool bIsInvalidStatus =  (statusNow == ENEMYTYPES::STATUS_RUN)
                          || (statusNow == ENEMYTYPES::STATUS_WALK)
                          || (statusNow == ENEMYTYPES::STATUS_CRASHWALL_FRONT);

    if (bIsInvalidStatus)
        return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;

    CHitAttackData* pAttack = &rCalc.GetAttack();
    if (pAttack->GetStatus() == CHitAttackData::STATUS_THROW)
        return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;

    CHitCatchData* pCatch = pAttack->GetCatch();
    if (pCatch->GetCatchNo() == 0)
    {
        /* head hit */
        if (m_state.fTimeoutOnDamage > 0.0f)
            return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
        
        if (!std::strstr(pAttack->GetMotion(), "JAttack")) /* check for any jump attacks starts with JAttack */
            return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;

        pAttack->SetPower(100); /* damaging on the head should deals a bit of damage?
                                   but it's not calculated for this damage source in retail game
                                   (see C081Spasmosaur::OnMessageReceivedDamage) */

        pCatch->SetResult(CHitCatchData::RESULT_HIT);

        m_state.bHelpSkip = true;
        m_state.damageSource = DAMAGESOURCE_HEAD;
        m_state.fTimeoutOnDamage = TIMEOUT_FOR_DAMAGE;
        m_state.targetPlayerNo = GetAttackerPlayerNo(pAttack);

        return CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK;
    }
    else if (pCatch->GetCatchNo() < 21)
    {
        /* tentacles hit */
        if (statusNow == static_cast<ENEMYTYPES::STATUS>(C081Spasmosaur::STATUS_SHRINK))
            return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;

        pCatch->SetResult(CHitCatchData::RESULT_HIT);

        m_state.damageSource = DAMAGESOURCE_TENTACLE;
        m_state.targetPlayerNo = GetAttackerPlayerNo(pAttack);

        return CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK;
    }
    else
    {
        /* body hit */
        if (!m_state.bHelpShow)
        {
            m_state.bHelpShow = true;
            m_state.fHelpTimeout = TIMEOUT_FOR_HIT_HELP;
        };
    };

    return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
};


/*virtual*/ void C081Spasmosaur::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    switch (m_state.damageSource)
    {
    case DAMAGESOURCE_HEAD:
        {
            m_state.phase = PHASE_MAD;
            m_state.bMouthOpen = true;
            m_state.fColorChangeTimer = COLOR_CHANGE_TIME;

            CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(8197));
            SetStatus(C081Spasmosaur::STATUS_SHRINK);
        }
        break;

    case DAMAGESOURCE_TENTACLE:
        {
            if (--m_state.numHitsUntilShrink <= 0)
            {
                m_state.numHitsUntilShrink = HIT_NUM_UNTIL_SHRINK;

                CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(8197));
                SetStatus(C081Spasmosaur::STATUS_SHRINK);
            };
        }
        break;

    case DAMAGESOURCE_WALL_CRASH:
        {
            CEnemyCharacter::OnMessageReceivedDamage(iAmount);
            m_state.fColorChangeTimer = COLOR_CHANGE_TIME;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


float C081Spasmosaur::GetRandNormal(void)
{
    return m_rand.Generate();
};


void C081Spasmosaur::InitState(void)
{
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    Compositor().GetPosition(&vecMyPos);

    m_state.pCompositor = &Compositor();
    m_state.bHelpShow = false;
    m_state.bHelpSkip = false;
    m_state.fHelpTimeout = 0.0f;
    m_state.bMadFristMsg = true;
    m_state.bMouthOpen = false;
    m_state.damageSource = DAMAGESOURCE_NONE;
    m_state.phase = PHASE_NORMAL;
    m_state.numHitsUntilShrink = HIT_NUM_UNTIL_SHRINK;
    m_state.targetPlayerNo = 0;
    m_state.runType = RUNTYPE_RUSH;
    m_state.numWallMiss = 0;

    for (int32 i = 0; i < GAMETYPES::PLAYERS_MAX; ++i)
    {
        m_state.aThrowTarget[i].bLeft = false;
        m_state.aThrowTarget[i].bThrown = false;
    };

    InitEffect();

    m_state.fColorChangeTimer = 0.0f;
    m_state.fTimeoutOnDamage = 0.0f;
    m_state.fThinkTimer = DEFAULT_THINK_TIMER;

    std::memset(m_state.aPlayerData, 0, sizeof(m_state.aPlayerData));
    std::memset(m_state.aPlayerInfo, 0, sizeof(m_state.aPlayerInfo));
    m_state.numPlayers = 0;

    SetSharedData(&m_state);
};


void C081Spasmosaur::InitEffect(void)
{
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    Compositor().GetPosition(&vecMyPos);

    for (int32 i = 0; i < COUNT_OF(STATE::ahEffectDrool); ++i)
        m_state.ahEffectDrool[i] = CEffectManager::Play(EFFECTID::ID_SPSM_YODAR1, &vecMyPos);

    m_state.hEffectSmoke = CEffectManager::Play(EFFECTID::ID_SPSM_RUNSMOKE, &vecMyPos);
    CEffectManager::SetActive(m_state.hEffectSmoke, false);
};


void C081Spasmosaur::TermEffect(void)
{
    if (m_state.hEffectSmoke)
    {
        CEffectManager::Finish(m_state.hEffectSmoke);
        m_state.hEffectSmoke = 0;
    };

    for (int32 i = 0; i < COUNT_OF(STATE::ahEffectDrool); ++i)
    {
        if (m_state.ahEffectDrool[i])
        {
            CEffectManager::Finish(m_state.ahEffectDrool[i]);
            m_state.ahEffectDrool[i] = 0;
        };
    };
};


void C081Spasmosaur::RunAI(void)
{
    if (!IsRunningAI())
        return;

    if (m_state.fThinkTimer > 0.0f)
    {
        m_state.fThinkTimer -= CGameProperty::GetElapsedTime();
    }
    else
    {
        uint8 thinkFreq = FrequencyParameter(FREQUENCY_THINKING);
        m_state.fThinkTimer = static_cast<float>(thinkFreq) / 60.0f;
        m_state.fThinkTimer *= (GetRandNormal() + 1.0f);
        m_state.fThinkTimer = std::fabs(m_state.fThinkTimer);
    };

    m_state.CheckPlayerStatus();
};


void C081Spasmosaur::UpdateEffectMouthDrool(void)
{
    static const RwV3d s_avecDroolMouthOffset[] =
    {
        {  3.1f, -3.0f, -1.2f },
        {  2.5f, -4.0f, -1.2f },
        { -2.5f, -4.0f, -1.2f },
        { -3.1f, -3.0f, -1.2f },
    };

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    Compositor().GetPosition(&vecMyPos);

    for (int32 i = 0; i < COUNT_OF(s_avecDroolMouthOffset); ++i)
    {
        RwV3d vecMouthDroolPos = Math::VECTOR3_ZERO;
        Compositor().GetBonePosition(&vecMouthDroolPos, 5, &s_avecDroolMouthOffset[i]);
        Math::Vec3_Add(&vecMouthDroolPos, &vecMouthDroolPos, &vecMyPos);

        float fDir = CEnemyUtils::GetDirection(&vecMyPos, &vecMouthDroolPos);

        CEffectManager::SetPosition(m_state.ahEffectDrool[i], &vecMouthDroolPos);
        CEffectManager::SetDirection(m_state.ahEffectDrool[i], fDir);
    };
};


void C081Spasmosaur::UpdateEffectRunSmoke(void)
{
    float fMyDir = Compositor().GetDirection();

    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    Compositor().GetFootPosition(&vecMyFootPos);

    CEffectManager::SetPosition(m_state.hEffectSmoke, &vecMyFootPos);
    CEffectManager::SetDirection(m_state.hEffectSmoke, fMyDir);
};


void C081Spasmosaur::UpdateBodyColorChange(void)
{
    CModel* pModel = Compositor().GetModel();
    if (!pModel)
        return;
    
    RwRGBA color;
    color.red   = 255;
    color.green = (255 - (m_state.phase == PHASE_NORMAL ? 0 : 50));
    color.blue  = (255 - (m_state.phase == PHASE_NORMAL ? 0 : 50));
    color.alpha = 255;

    if (m_state.fColorChangeTimer > 0.0f)
    {
        color.blue  -= static_cast<RwUInt8>((m_state.fColorChangeTimer * 2.0f) * 50.0f);
        color.green -= static_cast<RwUInt8>((m_state.fColorChangeTimer * 2.0f) * 50.0f);

        m_state.fColorChangeTimer -= CGameProperty::GetElapsedTime();
    };

    pModel->SetColor(color);
};


void C081Spasmosaur::UpdateHelpMessage(void)
{
    if (!m_state.bHelpSkip && m_state.bHelpShow && (m_state.fHelpTimeout > 0.0f))
    {
        m_state.fHelpTimeout -= CGameProperty::GetElapsedTime();
        if (m_state.fHelpTimeout < 0.0f)
            CMessageManager::Request(SEGROUPID::VALUE(77));
    };
};


int32 C081Spasmosaur::GetAttackerPlayerNo(CHitAttackData* pAttack) const
{
    CGameObject* pAttacker = pAttack->GetObject();

    /* getting real attacker (for example if it's shot or magic it will returns owner of this attack) */    
    uint32 hAttacker = CEnemyUtils::GetHandleFromHitObj(pAttacker);
    ASSERT(hAttacker != 0);
    if (hAttacker == 0)
        return -1;

    /* owner of attacks on the boss always players (i hope) */
    pAttacker = CGameObjectManager::GetObject(hAttacker);
    ASSERT(pAttacker != nullptr);
    if (pAttacker == nullptr)
        return -1;
    
    ASSERT(pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER);
    ASSERT(static_cast<CCharacter*>(pAttacker)->GetCharacterType() == CCharacter::TYPE_PLAYER);

    CPlayerCharacter* pPlrChr = static_cast<CPlayerCharacter*>(pAttacker);
    return pPlrChr->GetPlayerNo();
};