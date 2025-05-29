#include "BaseNinjaRatsChr.hpp"

#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseNinjaRatsAI.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


#define JUMP_SPEED_XZ (9.0f)


namespace MOTIONNAMES
{
    static const char* JUMP_2ND      = "JDouble";
    static const char* JUMP_SHOT     = "JSyuriken";

    static const char* ATTACK_CHAIN  = "AAA";
    static const char* ATTACK_SINGLE = "D";
    static const char* ATTACK_STUN   = "B";

    static const char* BACKAWAY      = "Backaway";
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CHideStatusObserver::OnStart(void) /*override*/
{
    ;
};


/*virtual*/ CBaseNinjaRatsChr::CHideStatusObserver::RESULT
CBaseNinjaRatsChr::CHideStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CHideStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


CBaseNinjaRatsChr::CThinkingStatusObserver::CThinkingStatusObserver()
: m_nextStatus(ENEMYTYPES::STATUS_IDLE)
{
    ;
};


/*virtual*/ void CBaseNinjaRatsChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    ENEMYTYPES::STATUS nextStatus = m_nextStatus;

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_WAIT:
        {
            int32 waitType = EnemyChr().AIThinkOrder().OrderWait().m_iWaitType;
            switch (waitType)
            {
            case CBaseNinjaRatsAI::ORDERTYPE::WAIT_IDLE:
                nextStatus = ENEMYTYPES::STATUS_IDLE;
                break;

            case CBaseNinjaRatsAI::ORDERTYPE::WAIT_TURN_NO:
            case CBaseNinjaRatsAI::ORDERTYPE::WAIT_TURN_POS:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(CBaseNinjaRatsChr::STATUS_HOP);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
            switch (moveType)
            {
            case CBaseNinjaRatsAI::ORDERTYPE::MOVE_RUN_POS:
            case CBaseNinjaRatsAI::ORDERTYPE::MOVE_RUN_NO:
                nextStatus = ENEMYTYPES::STATUS_RUN;
                break;

            case CBaseNinjaRatsAI::ORDERTYPE::MOVE_JUMP:
                nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
                break;

            case CBaseNinjaRatsAI::ORDERTYPE::MOVE_DASH:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(CBaseNinjaRatsChr::STATUS_BACKAWAY);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case CBaseNinjaRatsAI::ORDERTYPE::ATTACK_CHAIN:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(CBaseNinjaRatsChr::STATUS_ATTACK_CHAIN);
                break;

            case CBaseNinjaRatsAI::ORDERTYPE::ATTACK_SINGLE:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(CBaseNinjaRatsChr::STATUS_ATTACK_SINGLE);
                break;

            case CBaseNinjaRatsAI::ORDERTYPE::ATTACK_STUN:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(CBaseNinjaRatsChr::STATUS_ATTACK_STUN);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    default:
        nextStatus = ENEMYTYPES::STATUS_IDLE;
        break;
    };

    bool bResult = EnemyChr().SetStatus(nextStatus);
    ASSERT(bResult);
};


/*virtual*/ CBaseNinjaRatsChr::CThinkingStatusObserver::RESULT
CBaseNinjaRatsChr::CThinkingStatusObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CDeathStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);
    EnemyChr().Compositor().SetEnableCatchHit(false);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
};


/*virtual*/ CBaseNinjaRatsChr::CDeathStatusObserver::RESULT
CBaseNinjaRatsChr::CDeathStatusObserver::Observing(void) /*override*/
{
    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CDeathStatusObserver::OnEnd(void) /*override*/
{
    CEnemyUtils::EntryNexusDeathEffect(&EnemyChr(), 1.5f);
    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


/*virtual*/ CBaseNinjaRatsChr::CRunStatusObserver::RESULT
CBaseNinjaRatsChr::CRunStatusObserver::Observing(void) /*override*/
{
    /* check for order changed */
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_MOVE)
    {
        m_nextStatus = ENEMYTYPES::STATUS_THINKING;
        return RESULT_END;
    };

    /* check for order timeout */
    float fMoveTime = EnemyChr().AIThinkOrder().OrderMove().m_fMoveTimer;
    if (m_fMoveTime < fMoveTime)
    {
        m_fMoveTime += CGameProperty::GetElapsedTime();
    }
    else
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    };

    /* check for valid order move type */
    if ((EnemyChr().AIThinkOrder().OrderMove().m_iMoveType < m_moveTypeJudgeBegin) ||
        (EnemyChr().AIThinkOrder().OrderMove().m_iMoveType > m_moveTypeJudgeEnd))
    {
        m_nextStatus = ENEMYTYPES::STATUS_THINKING;
        return RESULT_END;
    };

    /* get order position */
    RwV3d vMarkPos = Math::VECTOR3_ZERO;
    if (EnemyChr().AIThinkOrder().OrderMove().m_iMoveType == m_moveTypeJudgeMovePos)
    {
        vMarkPos = EnemyChr().AIThinkOrder().OrderMove().m_vAt;
    }
    else
    {
        int32 iMoveTarget = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
        
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iMoveTarget);
        if (!pPlayerChr)
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            return RESULT_CONTINUE;
        };

        pPlayerChr->GetFootPosition(&vMarkPos);
    };

    float fRotSpeed = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
    CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &vMarkPos, fRotSpeed, MATH_DEG2RAD(3.0f));

    RwV3d vVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vVelocity, &vVelocity);
    EnemyChr().Compositor().SetVelocity(&vVelocity);

    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CJumpReadyStatusObserver::OnEnd(void) /*override*/
{
    float fDir = EnemyChr().Compositor().GetDirection();

    int32 targetPlayerNo = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
    if (targetPlayerNo >= 0)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(targetPlayerNo);

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);
        vecFootPosPlr.y = 0.0f;

        RwV3d vecPosMe = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecPosMe);
        vecPosMe.y = 0.0f;

        if (!Math::Vec3_IsEqual(&vecFootPosPlr, &vecPosMe))
        {
            fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecFootPosPlr);
            EnemyChr().Compositor().SetDirection(fDir);
        };
    };

    float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
    RwV3d vecVelocity = { std::sin(fDir) * JUMP_SPEED_XZ,
                          fJumpInitSpeed,
                          std::cos(fDir) * JUMP_SPEED_XZ };
    
    EnemyChr().Compositor().SetVelocity(&vecVelocity);
    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CHopStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CJumpReadyStatus::OnStart();

    int32 waitType = EnemyChr().AIThinkOrder().OrderWait().m_iWaitType;
    switch (waitType)
    {
    case CBaseNinjaRatsAI::ORDERTYPE::WAIT_TURN_POS:
        {
            m_vecHopPos = EnemyChr().AIThinkOrder().OrderWait().m_vAt;
            m_fHopSpeed = 3.0f;
        }
        break;

    case CBaseNinjaRatsAI::ORDERTYPE::WAIT_TURN_NO:
        {
            int32 targetPlayerNo = EnemyChr().AIThinkOrder().OrderWait().m_iPlayerNo;            
            CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(targetPlayerNo);
            ASSERT(pPlrChr != nullptr);

            RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
            pPlrChr->GetFootPosition(&vecFootPosPlr);

            m_vecHopPos = vecFootPosPlr;
            m_fHopSpeed = 0.0f;
        }
        break;

    default:
        {
            ASSERT(false);

            m_vecHopPos = Math::VECTOR3_ZERO;
            m_fHopSpeed = 0.0f;
        }
        break;
    };
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CHopStatusObserver::OnEnd(void) /*override*/
{
    float fHopDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &m_vecHopPos);
    EnemyChr().Compositor().SetDirection(fHopDir);

    RwV3d vecVelocity = { (std::sin(fHopDir) * m_fHopSpeed),
                          5.0f,
                          (std::cos(fHopDir) * m_fHopSpeed), };

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CAerialStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_TOUCHDOWN;

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::JUMP, true);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    ENEMYTYPES::STATUS prevStatus = EnemyChr().StatusSubject().StatusPrev();
    if (prevStatus == ENEMYTYPES::STATUS_JUMP_READY)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4370));
    else if (prevStatus == static_cast<ENEMYTYPES::STATUS>(CBaseNinjaRatsChr::STATUS_HOP))
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8204));
};


/*virtual*/ CBaseNinjaRatsChr::CAerialStatusObserver::RESULT
CBaseNinjaRatsChr::CAerialStatusObserver::Observing(void) /*override*/
{
    CCommonEnemyObserver::CAerialStatus::Observing();
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_ATTACK)
        return RESULT_CONTINUE;

    m_nextStatus = ENEMYTYPES::STATUS_JUMP_ATTACK;
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CAerialStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CJump2ndStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::JUMP_2ND, true);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    float fMyDir = EnemyChr().Compositor().GetDirection();
    float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;

    RwV3d vecVelocity = { (std::sin(fMyDir) * JUMP_SPEED_XZ),
                          fJumpInitSpeed,
                          (std::cos(fMyDir) * JUMP_SPEED_XZ) };
    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


/*virtual*/ CBaseNinjaRatsChr::CJump2ndStatusObserver::RESULT
CBaseNinjaRatsChr::CJump2ndStatusObserver::Observing(void) /*override*/
{
    float fJumpInitSpeed = EnemyChr().CharacterParameter().m_feature.m_fJumpInitializeSpeed;
    CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), fJumpInitSpeed);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CJump2ndStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    return ENEMYTYPES::STATUS_TOUCHDOWN;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CJumpShotStatusObserver::OnStart(void) /*override*/
{
    m_fMotionPreT = 0.0f;

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::JUMP_SHOT, true);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8204));
};


/*virtual*/ CBaseNinjaRatsChr::CJumpShotStatusObserver::RESULT
CBaseNinjaRatsChr::CJumpShotStatusObserver::Observing(void) /*override*/
{
    float afMotionQuarterTiming[] =
    {
        0.167f,
        0.3f,
        0.433f,
        0.566f,
    };

    float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
    float fMotionPreT = (fMotionNowT - CGameProperty::GetElapsedTime());
    int32 quarterNo = -1;

    for (int32 i = 0; i < COUNT_OF(afMotionQuarterTiming); ++i)
    {
        if (fMotionPreT <  afMotionQuarterTiming[i] &&
            fMotionNowT >= afMotionQuarterTiming[i])
        {
            quarterNo = i;
            break;
        };
    };

    if (quarterNo == -1)
        return RESULT_CONTINUE;

    ASSERT(quarterNo < 4);

    static const int32 SHOT_QUARTER_SHOT_NUM = 4;
    static const float SHOT_QUARTER_DIR_STEP = (90.0f / static_cast<float>(SHOT_QUARTER_SHOT_NUM));

    float fBaseDir = static_cast<float>(quarterNo) * ((MATH_PI * 2.0f) / static_cast<float>(SHOT_QUARTER_SHOT_NUM));
    float fMyDir = EnemyChr().Compositor().GetDirection();
    float fShotDir = fBaseDir + fMyDir;

    CBaseNinjaRatsChr::STATE* pState = static_cast<CBaseNinjaRatsChr::STATE*>(EnemyChr().SharedData());
    float fMapHeight = pState->fMapHeight;

    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

    for (int32 i = 0; i < SHOT_QUARTER_SHOT_NUM; ++i)
    {
        RwV3d vecShotDir = { (std::sin(fShotDir) * 12.0f) - vecMyFootPos.x,
                             (fMapHeight - vecMyFootPos.y),
                             (std::cos(fShotDir) * 12.0f) - vecMyFootPos.z, };

        uint32 hShot = CShotManager::Shot(SHOTID::ID_KNIFE_ENEMY,   /* shot id */
                                          &vecMyFootPos,            /* shot start pos */
                                          &vecShotDir,              /* shot direction */
                                          &EnemyChr().Compositor(), /* shot owner */
                                          MATH_DEG2RAD(15.0f),      /* shot angle for direction correction */
                                          5.0f);                    /* shot lifetime */

        fShotDir += SHOT_QUARTER_DIR_STEP;
    };

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4128));

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CJumpShotStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    return ENEMYTYPES::STATUS_TOUCHDOWN;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CTouchdownStatusObserver::OnStart(void) /*override*/
{
    RwV3d vecVel = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVel);

    CCommonEnemyObserver::CTouchdownStatus::OnStart();

    vecVel.x *= 0.75f;
    vecVel.y  = 0.0f;
    vecVel.z *= 0.75f;
    EnemyChr().Compositor().SetVelocity(&vecVel);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CGetupStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CGetupStatus::OnStart();
    m_bDown = true;
};


/*virtual*/ CBaseNinjaRatsChr::CGetupStatusObserver::RESULT
CBaseNinjaRatsChr::CGetupStatusObserver::Observing(void) /*override*/
{
    if (m_bDown)
    {
        bool bIsMotionEnd = IsLoopMotionEnd();
        bool bIsRandProc = ((Math::Rand() % 2) > 0);
        
        if (bIsMotionEnd && bIsRandProc)
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_GETUP, true);
            m_bDown = false;
        };

		return RESULT_CONTINUE;
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


bool CBaseNinjaRatsChr::CGetupStatusObserver::IsLoopMotionEnd(void) const
{
    float fEndMotionT = EnemyChr().Compositor().GetMotionEndTime();
    float fNowMotionT = EnemyChr().Compositor().GetMotionTime();
    float dt = CGameProperty::GetElapsedTime();

    return ((fNowMotionT + dt) > fEndMotionT);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CAttackStatusObserver::OnStart(void) /*override*/
{
	m_squeakCnt = 0;

    ENEMYTYPES::STATUS statusNow = Status();
    switch (static_cast<int32>(statusNow))
    {
    case CBaseNinjaRatsChr::STATUS_ATTACK_CHAIN:  m_pszMotionName = MOTIONNAMES::ATTACK_CHAIN;  break;
    case CBaseNinjaRatsChr::STATUS_ATTACK_STUN:   m_pszMotionName = MOTIONNAMES::ATTACK_STUN;   break;
    case CBaseNinjaRatsChr::STATUS_ATTACK_SINGLE: m_pszMotionName = MOTIONNAMES::ATTACK_SINGLE; break;
    default: ASSERT(false); break;
    };

    EnemyChr().Compositor().ChangeMotion(m_pszMotionName, true);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    int32 targetPlayerNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    ASSERT(targetPlayerNo >= 0);

    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(targetPlayerNo);

    RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecFootPosPlr);
    vecFootPosPlr.y = 0.0f;

    RwV3d vecPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPosMe);
    vecPosMe.y = 0.0f;

    float fDir = 0.0f;
    if (!Math::Vec3_IsEqual(&vecFootPosPlr, &vecPosMe))
    {
        fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecFootPosPlr);
        EnemyChr().Compositor().SetDirection(fDir);
    };

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);

    float fMoveSpeed = Math::Vec3_Length(&vecVelocity);

    vecVelocity = { (std::sin(fDir) * fMoveSpeed) * 0.5f,
                     0.0f,
                    (std::cos(fDir) * fMoveSpeed) * 0.5f };
    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


/*virtual*/ CBaseNinjaRatsChr::CAttackStatusObserver::RESULT
CBaseNinjaRatsChr::CAttackStatusObserver::Observing(void) /*override*/
{
    SetVelocityProc();
    SqueakSoundProc();

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CAttackStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


void CBaseNinjaRatsChr::CAttackStatusObserver::SetVelocityProc(void)
{
    RwV3d vecVel = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVel);

    float fMoveSpeed = Math::Vec3_Length(&vecVel);
    fMoveSpeed -= (CGameProperty::GetElapsedTime() * 9.0f);
    if (fMoveSpeed < 0.0f)
        fMoveSpeed = 0.0f;

    Math::Vec3_Normalize(&vecVel, &vecVel);
    Math::Vec3_Scale(&vecVel, &vecVel, fMoveSpeed);

    EnemyChr().Compositor().SetVelocity(&vecVel);
};


void CBaseNinjaRatsChr::CAttackStatusObserver::SqueakSoundProc(void)
{
    static float s_afSqueakTimingsAAA[] =
    {
        0.166f,
        0.4f,
        0.7f,
        TYPEDEF::FLOAT_MAX,
    };

    static float s_afSqueakTimingsB[] =
    {
        0.166f,
        TYPEDEF::FLOAT_MAX,
    };

    static float s_afSqueakTimingsD[] =
    {
        0.33f,
        TYPEDEF::FLOAT_MAX,
    };

    float* afSqueakTimings = nullptr;

    if (!std::strcmp(m_pszMotionName, MOTIONNAMES::ATTACK_CHAIN))
    {
        afSqueakTimings = s_afSqueakTimingsAAA;
    }
    else if (!std::strcmp(m_pszMotionName, MOTIONNAMES::ATTACK_STUN))
    {
        afSqueakTimings = s_afSqueakTimingsB;
    }
    else if (!std::strcmp(m_pszMotionName, MOTIONNAMES::ATTACK_SINGLE))
    {
        afSqueakTimings = s_afSqueakTimingsD;
    }
    else
    {
        ASSERT(false);
    };

    if (afSqueakTimings)
    {
        float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
        if (fMotionNowT >= afSqueakTimings[m_squeakCnt])
        {
            ++m_squeakCnt;
            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8204));
        };
    };    
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseNinjaRatsChr::CBackawayStatusObserver::OnStart(void) /*override*/
{
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4369));
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::BACKAWAY, true);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    int32 targetPlayerNo = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
    ASSERT(targetPlayerNo >= 0);

    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(targetPlayerNo);

    RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecFootPosPlr);
    vecFootPosPlr.y = 0.0f;

    RwV3d vecPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPosMe);
    vecPosMe.y = 0.0f;

    float fDir = 0.0f;
    if (!Math::Vec3_IsEqual(&vecFootPosPlr, &vecPosMe))
    {
        fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecFootPosPlr);
        EnemyChr().Compositor().SetDirection(fDir);
    };

    RwV3d vecVelocity = { (std::sin(fDir) * -9.0f),
                          0.0f,
                          (std::cos(fDir) * -9.0f) };
    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


/*virtual*/ CBaseNinjaRatsChr::CBackawayStatusObserver::RESULT
CBaseNinjaRatsChr::CBackawayStatusObserver::Observing(void) /*override*/
{
    RwV3d vecVel = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVel);

    float fMoveSpeed = Math::Vec3_Length(&vecVel);
    fMoveSpeed -= ((9.0f * CGameProperty::GetElapsedTime()) / 0.5f);
    if (fMoveSpeed < 0.0f)
        fMoveSpeed = 0.0f;

    Math::Vec3_Normalize(&vecVel, &vecVel);
    Math::Vec3_Scale(&vecVel, &vecVel, fMoveSpeed);

    EnemyChr().Compositor().SetVelocity(&vecVel);

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseNinjaRatsChr::CBackawayStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*static*/ CBaseNinjaRatsChr::STATE CBaseNinjaRatsChr::m_state;


CBaseNinjaRatsChr::CBaseNinjaRatsChr(ENEMYID::VALUE enemyId)
: CEnemyCharacter(enemyId)
, m_fSqueakTime(0.0f)
, m_bJump2nd(false)
{
    /* init shared state */
    InitState();

    /* CBaseNinjaRatsChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new CBaseNinjaRatsChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new CBaseNinjaRatsChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                      new CBaseNinjaRatsChr::CDeathStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                        new CBaseNinjaRatsChr::CRunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CBaseNinjaRatsChr::CJumpReadyStatusObserver);
    AttachStatusObserver(CBaseNinjaRatsChr::STATUS_HOP,                 new CBaseNinjaRatsChr::CHopStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                     new CBaseNinjaRatsChr::CAerialStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_2ND,                   new CBaseNinjaRatsChr::CJump2ndStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_ATTACK,                new CBaseNinjaRatsChr::CJumpShotStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                  new CBaseNinjaRatsChr::CTouchdownStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CBaseNinjaRatsChr::CGetupStatusObserver); // NOTE: guard present but AI not handled it
    AttachStatusObserver(CBaseNinjaRatsChr::STATUS_ATTACK_CHAIN,        new CBaseNinjaRatsChr::CAttackStatusObserver);
    AttachStatusObserver(CBaseNinjaRatsChr::STATUS_ATTACK_SINGLE,       new CBaseNinjaRatsChr::CAttackStatusObserver);
    AttachStatusObserver(CBaseNinjaRatsChr::STATUS_ATTACK_STUN,         new CBaseNinjaRatsChr::CAttackStatusObserver);
    AttachStatusObserver(CBaseNinjaRatsChr::STATUS_BACKAWAY,            new CBaseNinjaRatsChr::CBackawayStatusObserver);

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                      new CCommonEnemyObserver::CGuardStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,              new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,               new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,        new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,         new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FRONT,            new CCommonEnemyObserver::CCrashWallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_BACK,             new CCommonEnemyObserver::CCrashWallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT,       new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK,        new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT,  new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK,   new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                       new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,                 new CCommonEnemyObserver::CCounteractStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = CBaseNinjaRatsAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 1.0f;
    parameter.m_feature.m_fRunMoveSpeed         = 9.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 1.0f;
    parameter.m_feature.m_fDashMoveTime         = 1.0f;
    parameter.m_feature.m_fRotateRate           = 2.0f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 10.0f;
    parameter.m_feature.m_fAerialMoveSpeed      = JUMP_SPEED_XZ;
    parameter.m_feature.m_fAerialAcceleration   = 10.0f;
    parameter.m_feature.m_iHPMax                = 300;
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
        /* FREQUENCY_VIEW_ANGLE   0 */  { 30, 45, 90 },
    };
    
    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

	bool bReplace = false;
	Initialize(&parameter, bReplace);
};


/*virtual*/ CBaseNinjaRatsChr::~CBaseNinjaRatsChr(void)
{
    ;
};


/*virtual*/ void CBaseNinjaRatsChr::Run(void) /*override*/
{
    CEnemyCharacter::Run();
    SqueakSoundProc();
    JumpCheckProc();
};


/*virtual*/ void CBaseNinjaRatsChr::OnMessageCatchAttack(CHitAttackData* pAttack) /*override*/
{
    CEnemyCharacter::OnMessageCatchAttack(pAttack);
    CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(8204));
};


/*virtual*/ void CBaseNinjaRatsChr::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);

    bool bIsAttackerChr = (pCatch->GetObject()->GetType() == GAMEOBJECTTYPE::CHARACTER);
    bool bIsDead = TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS);

    if (m_bJump2nd && bIsAttackerChr && !bIsDead)
    {
        SetStatus(ENEMYTYPES::STATUS_JUMP_2ND);
        CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(8204));
    };
};


/*virtual*/ void CBaseNinjaRatsChr::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    if (iAmount <= 0)
        return;

    if (TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS))
        return;

    if ((m_state.hp == Feature().m_iHP) || (m_state.hp < 0))
    {
        CEnemyCharacter::OnMessageReceivedDamage(iAmount);
        ASSERT(Feature().m_iHP >= 0);

        m_state.hp = Feature().m_iHP;
        BroadcastReceivedDamage(iAmount);
    }
    else
    {
        CEnemyCharacter::OnMessageReceivedDamage(iAmount);
    };
};


void CBaseNinjaRatsChr::InitState(void)
{
    m_state.hp = -1;

    RwV3d vecPoint = { 0.0f, 10.0f, 0.0f };
    m_state.fMapHeight = CWorldMap::GetMapHeight(&vecPoint);

    SetSharedData(&m_state);
};


void CBaseNinjaRatsChr::SqueakSoundProc(void)
{
    m_fSqueakTime -= CGameProperty::GetElapsedTime();
    if (m_fSqueakTime <= 0.0f)
    {
        m_fSqueakTime = (static_cast<float>(Math::Rand() % 80) * 0.1f);

        ENEMYTYPES::STATUS statusNow = GetStatus();
        bool bIsValidStatus = (statusNow != ENEMYTYPES::STATUS_DEATH) &&
                              (statusNow != ENEMYTYPES::STATUS_IDLE);
        if (bIsValidStatus)
            CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(8204));
    };
};


void CBaseNinjaRatsChr::JumpCheckProc(void)
{
    ENEMYTYPES::STATUS statusNow = GetStatus();
    if (statusNow != ENEMYTYPES::STATUS_AERIAL)
        m_bJump2nd = (statusNow == ENEMYTYPES::STATUS_JUMP_READY);
};


void CBaseNinjaRatsChr::BroadcastReceivedDamage(int32 iAmount) const
{
    CGameObject* pGameObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::CHARACTER);
    while (pGameObj)
    {
        CCharacter* pChr = static_cast<CCharacter*>(pGameObj);
        if (pChr->GetCharacterType() == CCharacter::TYPE_ENEMY)
        {
            CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pChr);

            bool bIsRat = (pChrCompositor->GetID() >= ENEMYID::ID_NINJA_RATS_A) &&
                          (pChrCompositor->GetID() <= ENEMYID::ID_NINJA_RATS_D);

            bool bIsSelf = (pChrCompositor->GetID() == GetID());

            if (bIsRat && !bIsSelf)
            {
                CGameObjectManager::SendMessage(pChrCompositor,
                                                CHARACTERTYPES::MESSAGEID_RECVDMG,
                                                reinterpret_cast<void*>(iAmount));
            };
        };

        pGameObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::CHARACTER, pGameObj);
    };
};