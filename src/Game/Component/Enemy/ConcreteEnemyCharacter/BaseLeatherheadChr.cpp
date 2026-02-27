#include "BaseLeatherheadChr.hpp"

#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/075LeatherheadAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


CBaseLeatherheadChr::CHideStatusObserver::CHideStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_APPEAR)
{
    ;
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CAppearStatusObserver::CAppearStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_THINKING)
{
    ;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseLeatherheadChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CBaseLeatherheadChr::CThinkingStatusObserver::RESULT
CBaseLeatherheadChr::CThinkingStatusObserver::Observing(void) /*override*/
{
    RESULT result = RESULT_END;

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_WAIT:
        {
            m_nextStatus = ENEMYTYPES::STATUS_IDLE;
        }
        break;

    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
            if ((moveType >= BASEAI6045::ORDERTYPE_MOVE_WALK_POS) &&
                (moveType <= BASEAI6045::ORDERTYPE_MOVE_WALK_NO))
            {
                m_nextStatus = ENEMYTYPES::STATUS_WALK;
            }
            else if ((moveType >= BASEAI6045::ORDERTYPE_MOVE_RUN_POS) &&
                     (moveType <= BASEAI6045::ORDERTYPE_MOVE_RUN_NO))
            {
                m_nextStatus = ENEMYTYPES::STATUS_RUN;
            }
            else if (moveType == BASEAI6045::ORDERTYPE_MOVE_JUMP)
            {
                m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
            }
            else if (moveType == LEATHERHEAD_AI::ORDERTYPE_MOVE_SWIM)
            {
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASELEATHERHEAD::STATUS_SWIM);
            };
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case BASEAI6045::ORDERTYPE_ATTACK_NORMAL:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_AAA;
                break;

            case BASEAI6045::ORDERTYPE_ATTACK_TO_AIR:
                m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
                break;

            case BASEAI6045::ORDERTYPE_ATTACK_THROW:
                m_nextStatus = ENEMYTYPES::STATUS_THROW;
                break;

            case BASEAI6045::ORDERTYPE_ATTACK_COUNTER:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASELEATHERHEAD::STATUS_ATTACK_COUNTER);
                break;

            case LEATHERHEAD_AI::ORDERTYPE_ATTACK_E:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASELEATHERHEAD::STATUS_ATTACK_E);
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_DEFENCE:
        {
            m_nextStatus = ENEMYTYPES::STATUS_GUARD;
        }
        break;

    default:
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            result = RESULT_CONTINUE;
        }
        break;
    };

    return result;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseLeatherheadChr::CIdleStatus::OnStart(void) /*override*/
{
	EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
	EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    if (EnemyChr().StatusSubject().StatusPrev() == BASELEATHERHEAD::STATUS_AFTER_SWIM_TOUCHDOWN)
        EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::IDLE_2, true);
    else
        EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::IDLE_1, true);

    m_fTime = 0.0f;
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CAttackAAAStatusObserver::CAttackAAAStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_AAA)
, m_nextStatus(ENEMYTYPES::STATUS_IDLE)
{
    ;
};


/*virtual*/ CBaseLeatherheadChr::CAttackAAAStatusObserver::RESULT
CBaseLeatherheadChr::CAttackAAAStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

        RwV3d vecOffset = { 0.0f, 0.5f, 2.0f };
        EnemyChr().Compositor().RotateVectorByDirection(&vecOffset, &vecOffset);

        RwV3d vecWindPressPos = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecWindPressPos, &vecMyFootPos, &vecOffset);

        uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_WINDPRESS),
                                            &vecWindPressPos,
                                            nullptr,
                                            nullptr,
                                            true);

        CMagicManager::SetScale(hMagic, 1.0f);
        CMagicManager::SetChangeStatus(hMagic, CHitAttackData::STATUS_STUN);
        CMagicManager::SetStatusTime(hMagic, 2.0f);
    };

    if (CBaseChr6045::COneShotMotionForAttackPlayerAim::Observing() == RESULT_END)
    {
        m_nextStatus = ENEMYTYPES::STATUS_IDLE;

        if (CheckChainAttack())
        {
            EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
            EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = m_orderTargetNo;

            m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
        };

        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CAttackAAAStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return m_nextStatus;
};


bool CBaseLeatherheadChr::CAttackAAAStatusObserver::CheckChainAttack(void) const
{
    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        if (!pPlrChr)
            continue;

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        float fDist = CEnemyUtils::GetDistance(&vecFootPosMe, &vecFootPosPlr);
        float fDistOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;

        if (fDist < fDistOfSuitable)
        {
            PLAYERTYPES::STATUS plrStatus = pPlrChr->GetStatus();
            CAIUtils::PLAYER_STATE_FLAG plrStateFlag = CAIUtils::GetPlayerStateFlag(plrStatus);

            /* if player was stunned */
            if (plrStateFlag & CAIUtils::PLAYER_STATE_UNUSUAL)
                return true;
        };
    };

    return false;
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CAttackBStatusObserver::CAttackBStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CAttackEStatusObserver::CAttackEStatusObserver(void)
: m_step(0)
, m_fTimer(0.0f)
, m_bAim(false)
, m_fRotSpeed(0.0f)
, m_fMoveSpeed(0.0f)
, m_bChaseHit(false)
, m_orderTargetNo(-1)
, m_nextStatus(ENEMYTYPES::STATUS_THINKING)
{
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();

    m_fRotSpeed = BASELEATHERHEAD::ATTACK_E_AIM_ANGLESPEED[difficulty];
    m_fMoveSpeed = BASELEATHERHEAD::ATTACK_E_SPEED[difficulty];
};


/*virtual*/ void CBaseLeatherheadChr::CAttackEStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_bChaseHit = false;
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::ATTACK_E1, true);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

    SetSpecialSetting();

    CGameProperty::SetCameraVibration(0.25f, 0.75f, 10);
    EntryShockwave();

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8205));
};


/*virtual*/ CBaseLeatherheadChr::CAttackEStatusObserver::RESULT
CBaseLeatherheadChr::CAttackEStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case STEP_ROAR:
        {
            CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(), m_orderTargetNo, m_fRotSpeed, MATH_DEG2RAD(3.0f));

            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            CEnemyUtils6045::ReplaceVelocityToNowDirection(&EnemyChr().Compositor(), m_fMoveSpeed);
            EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::ATTACK_E2, true);

            m_fTimer = 2.0f;
            m_bAim = true;
            m_step = STEP_CHASE;
        }
        break;

    case STEP_CHASE:
        {
            if (m_bAim)
            {
                CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(), m_orderTargetNo, m_fRotSpeed, MATH_DEG2RAD(3.0f));
                CEnemyUtils6045::ReplaceVelocityToNowDirection(&EnemyChr().Compositor(), m_fMoveSpeed);

                if (!CheckAimContinue(m_orderTargetNo))
                    m_bAim = false;
            };

            if (m_bChaseHit)
            {
                m_fTimer = 0.35f;
                m_step = STEP_CHASE_HIT;
            };

            RwV3d vecCrashWallHitPos = Math::VECTOR3_ZERO;
            if (CEnemyUtils6045::CheckCrashWall(&EnemyChr().Compositor(), MATH_DEG2RAD(63.0f), &vecCrashWallHitPos))
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
                EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::ATTACK_E3, true);

                CEffectManager::Play(EFFECTID::ID_ALL_PITAN,
                                     &vecCrashWallHitPos,
                                     EnemyChr().Compositor().GetDirection());

                CGameObjectManager::SendMessage(&EnemyChr().Compositor(),
                                                CHARACTERTYPES::MESSAGEID_RECVDMG,
                                                reinterpret_cast<void*>(40));

                CEnemyUtils6045::EntrySimpleEffect(EFFECTID::ID_ALL_HIT_B, &vecCrashWallHitPos, 1.0f);
                CGameProperty::SetCameraVibration(0.25f, 0.75f, 4);
                CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4368));

                ClrSpecialSetting();

                m_step = STEP_CHASE_CRASH;
            };
        }
        break;

    case STEP_CHASE_HIT:
        {
            const CCharacterCompositor::COLLISIONWALLINFO* pWallInfo = EnemyChr().Compositor().GetCollisionWall();
            ASSERT(pWallInfo != nullptr);

            if (pWallInfo->m_bHit)
                return RESULT_END;

            if (m_fTimer <= 0.0f)
                return RESULT_END;

            m_fTimer -= CGameProperty::GetElapsedTime();
        }
        break;

    case STEP_CHASE_CRASH:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASELEATHERHEAD::STATUS_ATTACK_E_STUN);
                return RESULT_END;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };
    
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CAttackEStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    ClrSpecialSetting();

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return m_nextStatus;
};


void CBaseLeatherheadChr::CAttackEStatusObserver::CatchAttackResult(CHitCatchData* pCatch)
{
    CHitAttackData* pAttack = pCatch->GetAttack();

    const char* pszAttackMotion = pAttack->GetMotion();
    if (!std::strcmp(pszAttackMotion, BASELEATHERHEAD::MOTIONNAMES::ATTACK_E2))
    {
        CHitAttackData::TARGET attackTarget = pAttack->GetTarget();
        if (attackTarget == CHitAttackData::TARGET_PLAYER)
            m_bChaseHit = true;
    };
};


bool CBaseLeatherheadChr::CAttackEStatusObserver::CheckAimContinue(int32 playerNo)
{
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(playerNo);
    if (!pPlrChr)
        return false;

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecFootPosPlr);

    float fDistXZ = CEnemyUtils6045::GetXZDistance(&vecFootPosMe, &vecFootPosPlr);
    if (fDistXZ < 2.75f)
        return false;

    PLAYERTYPES::STATUS plrStatus = pPlrChr->GetStatus();
    CAIUtils::PLAYER_STATE_FLAG plrStateFlag = CAIUtils::GetPlayerStateFlag(plrStatus);

    if (plrStateFlag & CAIUtils::PLAYER_STATE_UNUSUAL)
        return false;

    if (m_fTimer <= 0.0f)
        return false;

    m_fTimer -= CGameProperty::GetElapsedTime();
    return true;
};


void CBaseLeatherheadChr::CAttackEStatusObserver::EntryShockwave(void) const
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

    uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_WINDPRESS),
                                        &vecMyFootPos,
                                        nullptr,
                                        nullptr,
                                        true);

    CMagicManager::SetScale(hMagic, 2.5f);
    CMagicManager::SetDamage(hMagic, 0);
    CMagicManager::SetChangeStatus(hMagic, CHitAttackData::STATUS_STUN);
    CMagicManager::SetStatusTime(hMagic, 2.0f);
};


void CBaseLeatherheadChr::CAttackEStatusObserver::SetSpecialSetting(void)
{
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


void CBaseLeatherheadChr::CAttackEStatusObserver::ClrSpecialSetting(void)
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseLeatherheadChr::CAttackEStunStatusObserver::OnStart(void) /*override*/
{
    m_fStunTime = 2.0f;
    m_hStunEffect = 0;

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::STUN, true);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);
    vecMyFootPos.y += 2.0f;

    m_hStunEffect = CEffectManager::Play(EFFECTID::ID_ALL_PIYORI, &vecMyFootPos);
    CEffectManager::SetScale(m_hStunEffect, 1.0f);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
};


/*virtual*/ CBaseLeatherheadChr::CAttackEStunStatusObserver::RESULT
CBaseLeatherheadChr::CAttackEStunStatusObserver::Observing(void) /*override*/
{
    if (m_fStunTime <= 0.0f)
        return RESULT_END;

    m_fStunTime -= CGameProperty::GetElapsedTime();
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CAttackEStunStatusObserver::OnEnd(void) /*override*/
{
    /*
     * TODO: this status is exactly the same as CCommonEnemyObserver::CCharacterEffectStatusObserver
     *       so this status may be much easier: make OnStart & Observing empty and make OnEnd to this:
     *
     *          EnemyChr().Compositor().AttackParameter().m_fTroubleTime = 2.0f;
     *          return ENEMYTYPES::STATUS_STUN;
     *
     *       (or put code above directly to the CBaseLeatherheadChr::CAttackEStatusObserver::Observing)
     */

    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    if (m_hStunEffect)
    {
        CEffectManager::Finish(m_hStunEffect);
        m_hStunEffect = 0;
    };

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CAttackCounterStatusObserver::CAttackCounterStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr)
: m_pBaseLeatherheadChr(pBaseLeatherheadChr)
{
    ;
};


/*virtual*/ void CBaseLeatherheadChr::CAttackCounterStatusObserver::OnStart(void) /*override*/
{
    CAttackBStatusObserver::OnStart();

    if (m_pBaseLeatherheadChr->IsKnockBackCtrlActive())
        m_pBaseLeatherheadChr->SetKnockBackCtrlEnable(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CAttackCounterStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);

    if (m_pBaseLeatherheadChr->IsKnockBackCtrlActive())
        m_pBaseLeatherheadChr->SetKnockBackCtrlEnable(true);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CSwimStatusObserver::CSwimStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr)
: m_pBaseLeatherheadChr(pBaseLeatherheadChr)
, m_vecMarkPos(Math::VECTOR3_ZERO)
, m_vecCircleCenterStart(Math::VECTOR3_ZERO)
, m_vecCircleCenterEnd(Math::VECTOR3_ZERO)
, m_fCircleCenterRate(0.0f)
, m_fCircleCenterRateSpeed(0.0f)
, m_fCircleRadius(0.0f)
, m_fCircleSpeed(0.0f)
, m_fCircleAngle(0.0f)
, m_fCircleAngleSpeed(0.0f)
, m_fCircleSpeedMin(0.0f)
, m_fCircleSpeedMax(0.0f)
, m_fCenterRateSpeedMin(0.0f)
, m_fCenterRateSpeedMax(0.0f)
, m_fGravityRate(0.0f)
, m_step(-1)
, m_stageValue(-1)
, m_bJumpXZCorrectFlag(false)
, m_bJumpTouchdownFlag(false)
, m_bAttackOrderFlag(false)
, m_fAttackReadyTimer(0.0f)
, m_waterSplashCountAttackReady(-1)
{
    bool bIsNexusMap = (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X));

    m_stageValue = (bIsNexusMap ? 1 : 0);

    m_vecCircleCenterStart.x = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_X_MIN[m_stageValue];
    m_vecCircleCenterStart.y = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_Y[m_stageValue];
    m_vecCircleCenterStart.z = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_Z_MIN[m_stageValue];

    m_vecCircleCenterEnd.x = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_X_MAX[m_stageValue];
    m_vecCircleCenterEnd.y = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_Y[m_stageValue];
    m_vecCircleCenterEnd.z = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_Z_MAX[m_stageValue];

    m_fCircleCenterRate = 0.5f;
    m_fCircleRadius = BASELEATHERHEAD::SWIMMODE_CIRCLE_RADIUS[m_stageValue];
    m_fCircleAngle = 0.0f;
    m_fGravityRate = 2.0f;

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    m_fCircleSpeedMin = BASELEATHERHEAD::SWIMMODE_CIRCLESPEED_MIN[difficulty];
    m_fCircleSpeedMax = BASELEATHERHEAD::SWIMMODE_CIRCLESPEED_MAX[difficulty];
    m_fCenterRateSpeedMin = BASELEATHERHEAD::SWIMMODE_CENTERRATESPEED_MIN[difficulty];
    m_fCenterRateSpeedMax = BASELEATHERHEAD::SWIMMODE_CENTERRATESPEED_MAX[difficulty];
};


/*virtual*/ void CBaseLeatherheadChr::CSwimStatusObserver::OnStart(void) /*override*/
{
    SetSwimSpeed();

    CBaseLeatherheadChr::SWIM_PARAM* pSwimParam = static_cast<CBaseLeatherheadChr::SWIM_PARAM*>(EnemyChr().SharedData());

    bool bIsDown = (EnemyChr().StatusSubject().StatusPrev() >= ENEMYTYPES::STATUS_KNOCK) &&
                   (EnemyChr().StatusSubject().StatusPrev() <= ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK);

    if (bIsDown && pSwimParam->bNowSwimmingFlag)
    {
        EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::SWIM_2, true);
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4365));
    }
    else
    {
        SetJumpToSwimSetting();

        pSwimParam->bNowSwimmingFlag = true;

        m_bJumpXZCorrectFlag = false;
        m_bJumpTouchdownFlag = false;
        m_step = 0;

        CEnemyUtils::EntryTouchDownEffect(&EnemyChr().Compositor(), 2.25f, false);

        if (CEnemyUtils::CheckInfoWater(&EnemyChr().Compositor()))
            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4366));
        
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    };
};


/*virtual*/ CBaseLeatherheadChr::CSwimStatusObserver::RESULT
CBaseLeatherheadChr::CSwimStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            if (!ProcJumpToSwim())
                break;

            EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4365));
            CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
            CMessageManager::Request(SEGROUPID::VALUE(101));

            m_bAttackOrderFlag = false;
            ++m_step;
        }
        break;

    case 1:
        {
            if (!m_bAttackOrderFlag)
            {
                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

                if (EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK)
                {
                    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
                    InitAttackReady();
                    m_bAttackOrderFlag = true;
                };
            };

            ProcSwim();

            if (m_bAttackOrderFlag)
            {
                if (!ProcAttackReady())
                    break;

                int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
                if (!SetAttackSetting(orderTargetNo))
                    break;

                EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);
                ++m_step;
                return RESULT_END;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CSwimStatusObserver::OnEnd(void) /*override*/
{
    ClrSpecialSetting();
    CGameSound::StopSE(SDCODE_SE(4365));
    return static_cast<ENEMYTYPES::STATUS>(BASELEATHERHEAD::STATUS_AFTER_SWIM_JUMP);
};


void CBaseLeatherheadChr::CSwimStatusObserver::SetSwimSpeed(void)
{
    float fSpeedRate = 0.0f;
    
    float fHpRatio = CEnemyUtils::GetEnemyHPRate(&EnemyChr());
    if (fHpRatio <= 0.1f)
        fSpeedRate = ((1.0f / 3.0f) * 3.0f);
    else if (fHpRatio <= 0.2f)
        fSpeedRate = ((1.0f / 3.0f) * 2.0f);
    else if (fHpRatio <= 0.3f)
        fSpeedRate = ((1.0f / 3.0f) * 1.0f);
    else
        fSpeedRate = 0.0f;

    m_fCircleSpeed =
        (m_fCircleSpeedMax - m_fCircleSpeedMin) * fSpeedRate + m_fCircleSpeedMin;
    
    m_fCircleCenterRateSpeed =
        (m_fCenterRateSpeedMax - m_fCenterRateSpeedMin) * fSpeedRate + m_fCenterRateSpeedMin;
    
    m_fCircleAngleSpeed =
        m_fCircleSpeed / m_fCircleRadius * CGameProperty::GetElapsedTime();
};


bool CBaseLeatherheadChr::CSwimStatusObserver::ProcJumpToSwim(void)
{
    CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &m_vecMarkPos, MATH_PI, 0.0f);

    if (!m_bJumpXZCorrectFlag)
        CEnemyUtils::UpdateVelocityFromDirection(&EnemyChr().Compositor());

    if (!m_bJumpXZCorrectFlag)
    {
        RwV3d vecFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecFootPos);

        RwV3d vecDistToMarkPos = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDistToMarkPos, &m_vecMarkPos, &vecFootPos);
        vecDistToMarkPos.y = 0.0f;

        float fDistToMarkPos = Math::Vec3_Length(&vecDistToMarkPos);
        if (fDistToMarkPos < 0.3f)
        {
            RwV3d vecVelocity = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetVelocity(&vecVelocity);
            vecVelocity.y = 0.0f;
            EnemyChr().Compositor().SetVelocity(&vecVelocity);

            m_bJumpXZCorrectFlag = true;
        };
    };

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);

    const CCharacterCompositor::COLLISIONGROUNDINFO* pGroundInfo = EnemyChr().Compositor().GetCollisionGround();
    ASSERT(pGroundInfo != nullptr);

    bool bIsRising = (vecVelocity.y > 0.0f);
    bool bIsOnGround = (pGroundInfo->m_bHit);

    if (bIsRising || !bIsOnGround || m_bJumpTouchdownFlag)
        return false;

    m_bJumpTouchdownFlag = true;

    EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::SWIM_2, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    EnemyChr().Compositor().SetPosition(&m_vecMarkPos);
    EnemyChr().Compositor().SetDirection(m_fCircleAngle + (MATH_PI * 0.5f));

    ClrSpecialSetting();

    return true;
};


void CBaseLeatherheadChr::CSwimStatusObserver::ProcSwim(void)
{
    RwV3d vecMarkPosPrev = m_vecMarkPos;
    CalcSwimMarkPoint();

    EnemyChr().Compositor().SetPosition(&m_vecMarkPos);

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    CEnemyUtils::GetVelocityByPosDiff(&vecVelocity, &m_vecMarkPos, &vecMarkPosPrev);

    EnemyChr().Compositor().SetVelocity(&vecVelocity);
    EnemyChr().Compositor().SetDirectionFromVector(&vecVelocity);

    m_fCircleAngle += m_fCircleAngleSpeed;
    m_fCircleAngle = CEnemyUtils::RadianCorrect(m_fCircleAngle);

    float fReachCircleCenterRate = 0.5f;
    RwV3d vecAllPlrPt = Math::VECTOR3_ZERO;
    if (CAIUtils6045::CalcGroundPointAllPlayer(&vecAllPlrPt))
    {
        /* calc all players relative circle start (min) */
        Math::Vec3_Sub(&vecAllPlrPt, &vecAllPlrPt, &m_vecCircleCenterStart);

        /* calc circle move boundaries (max - min) */
        RwV3d vecCircleMoveDistMax = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecCircleMoveDistMax, &m_vecCircleCenterEnd, &m_vecCircleCenterStart);

        float d = Math::Vec3_Length(&vecCircleMoveDistMax);

        /* getting unit vector for projection */
        Math::Vec3_Normalize(&vecCircleMoveDistMax, &vecCircleMoveDistMax);

        /* project all players position (mean position) onto circle min max boundaries */
        float p = Math::Vec3_Dot(&vecCircleMoveDistMax, &vecAllPlrPt);

        /* calc circle move rate */
        if (Math::FEqual(d, 0.0f))
        {
            fReachCircleCenterRate = 0.0f;
        }
        else
        {
            fReachCircleCenterRate = (p / d);
            fReachCircleCenterRate = Clamp(fReachCircleCenterRate, 0.0f, 1.0f);
        };
    };

    float fCircleCenterRateSpeed = CGameProperty::GetElapsedTime() * m_fCircleCenterRateSpeed;
    float fCircleCenterRate = fReachCircleCenterRate - m_fCircleCenterRate;
    float fCircleCenterRateAbs = std::fabs(fCircleCenterRate);
    if (fCircleCenterRateAbs > fCircleCenterRateSpeed)
    {
        if (fCircleCenterRate <= 0.0)
            m_fCircleCenterRate = m_fCircleCenterRate - fCircleCenterRateSpeed;
        else
            m_fCircleCenterRate = fCircleCenterRateSpeed + m_fCircleCenterRate;
    }
    else
    {
        m_fCircleCenterRate = fReachCircleCenterRate;
    };
};


void CBaseLeatherheadChr::CSwimStatusObserver::SetJumpToSwimSetting(void)
{
    EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::SWIM_1, true);

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    RwV3d vecCircleCenter = Math::VECTOR3_ZERO;
    GetSwimCircleCenterPoint(&vecCircleCenter);

    /* calc dive angle */
    float fDistXZ = CEnemyUtils6045::GetXZDistance(&vecCircleCenter, &vecFootPos);
    if (fDistXZ > 0.1f && !CEnemyUtils6045::GetDirection(&m_fCircleAngle, &vecCircleCenter, &vecFootPos))
        m_fCircleAngle = 0.0f;

    float fRate = (fDistXZ / m_fCircleRadius);
    fRate = Clamp(fRate, 0.0f, 1.0f);

    float fJumpAngleCorrectMin = BASELEATHERHEAD::SWIMMODE_CIRCLE_JUMPANGLECORRECT_MIN[m_stageValue];
    float fJumpAngleCorrectMax = BASELEATHERHEAD::SWIMMODE_CIRCLE_JUMPANGLECORRECT_MAX[m_stageValue];

    float fAngle =
        fJumpAngleCorrectMin + (fRate * (fJumpAngleCorrectMax - fJumpAngleCorrectMin));

    if (m_fCircleSpeed <= 0.0f)
        fAngle = std::fabs(fAngle);

    m_fCircleAngle += fAngle;
    m_fCircleAngle = CEnemyUtils::RadianCorrect(m_fCircleAngle);

    /* calc dive pt and rotate to it */
    CalcSwimMarkPoint();

    float fDirToMarkPt = 0.0f;
    if (CEnemyUtils6045::GetDirection(&fDirToMarkPt, &vecFootPos, &m_vecMarkPos))
        EnemyChr().Compositor().SetDirection(fDirToMarkPt);

    /* calc velocity to reach dive pt */
    const float fJumpInitSpeed = 12.0f;
    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    float fGravity = CGameProperty::GetGravity();
    CEnemyUtils::GetJumpSpeedPosToPos(&vecVelocity,
                                      fJumpInitSpeed,
                                      &vecFootPos,
                                      &m_vecMarkPos,
                                      (fGravity * m_fGravityRate));

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    /* init dive state specific flags */
    SetSpecialSetting();

    CEnemyUtils6045::SetGravitySpecialSetting(&EnemyChr().Compositor(), fGravity * m_fGravityRate);
};


void CBaseLeatherheadChr::CSwimStatusObserver::GetSwimCircleCenterPoint(RwV3d* vecCircleCenterPt) const
{
    vecCircleCenterPt->x =
        m_vecCircleCenterStart.x + (m_fCircleCenterRate * (m_vecCircleCenterEnd.x - m_vecCircleCenterStart.x));

    vecCircleCenterPt->y =
        m_vecCircleCenterStart.y + (m_fCircleCenterRate * (m_vecCircleCenterEnd.y - m_vecCircleCenterStart.y));

    vecCircleCenterPt->z =
        m_vecCircleCenterStart.z + (m_fCircleCenterRate * (m_vecCircleCenterEnd.z - m_vecCircleCenterStart.z));
};


void CBaseLeatherheadChr::CSwimStatusObserver::CalcSwimMarkPoint(void)
{
    RwV3d vecCircleCenter = Math::VECTOR3_ZERO;
    GetSwimCircleCenterPoint(&vecCircleCenter);

    m_vecMarkPos.x = (std::sin(m_fCircleAngle) * m_fCircleRadius) + vecCircleCenter.x;
    m_vecMarkPos.y = (0.0f * m_fCircleRadius) + vecCircleCenter.y;
    m_vecMarkPos.z = (std::cos(m_fCircleAngle) * m_fCircleRadius) + vecCircleCenter.z;

    m_vecMarkPos.y = CWorldMap::GetMapHeight(&m_vecMarkPos);
};


void CBaseLeatherheadChr::CSwimStatusObserver::InitAttackReady(void)
{
    m_waterSplashCountAttackReady = 0;
    m_fAttackReadyTimer = 0.0f;
};


bool CBaseLeatherheadChr::CSwimStatusObserver::ProcAttackReady(void)
{
    if (m_waterSplashCountAttackReady < 3)
    {
        float fCurrentTiming = (static_cast<float>(m_waterSplashCountAttackReady) * (1.0f / 3.0f));
        if (fCurrentTiming <= m_fAttackReadyTimer)
        {
            float fScale = (fCurrentTiming * 2.0f);
            bool bPlaySound = false;
            CEnemyUtils6045::EntryWaterDobonEffect(&EnemyChr().Compositor(), fScale, bPlaySound);

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4366));

            ++m_waterSplashCountAttackReady;
        };
    };

    if (m_fAttackReadyTimer >= 1.0f)
        return true;

    m_fAttackReadyTimer += CGameProperty::GetElapsedTime();
    return false;
};


bool CBaseLeatherheadChr::CSwimStatusObserver::SetAttackSetting(int32 playerNo)
{
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(playerNo);
    if (!pPlrChr)
        return false;

    CBaseLeatherheadChr::SWIM_PARAM* pSwimParam = static_cast<CBaseLeatherheadChr::SWIM_PARAM*>(EnemyChr().SharedData());
    pSwimParam->bNowSwimmingFlag = false;

    return true;
};


void CBaseLeatherheadChr::CSwimStatusObserver::SetSpecialSetting(void)
{
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


void CBaseLeatherheadChr::CSwimStatusObserver::ClrSpecialSetting(void)
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CAfterSwimJumpStatusObserver::CAfterSwimJumpStatusObserver(void)
: m_orderTargetNo(-1)
, m_fGravityRate(3.0f)
, m_stageValue(-1)
{
    bool bIsNexusMap = (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X));

    m_stageValue = (bIsNexusMap ? 1 : 0);
};


/*virtual*/ void CBaseLeatherheadChr::CAfterSwimJumpStatusObserver::OnStart(void) /*override*/
{
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;

    EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::SWIM_3, true);
    EnemyChr().Compositor().PlayMotion();

    /* get my pos */
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyPos);

    /* get player pos */
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
    if (!pPlrChr)
    {
        int32 numPlayers = CAIUtils::GetPlayerNum();
        for (int32 i = 0; i < numPlayers; ++i)
        {
            pPlrChr = CAIUtils::GetActivePlayer(i);
            if (pPlrChr)
                break;
        };
    };

    RwV3d vecPlrPos = Math::VECTOR3_ZERO;

    if (pPlrChr)
    {
        pPlrChr->GetBodyPosition(&vecPlrPos);
    }
    else
    {
        float fMinX = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_X_MIN[m_stageValue];
        float fMaxX = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_X_MAX[m_stageValue];

        float fMinZ = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_Z_MIN[m_stageValue];
        float fMaxZ = BASELEATHERHEAD::SWIMMODE_CIRCLE_CENTER_Z_MAX[m_stageValue];

        vecPlrPos.x = ((fMaxX - fMinX) * 0.5f) + fMinX;
        vecPlrPos.y = 0.0f;
        vecPlrPos.z = ((fMaxZ - fMinZ) * 0.5f) + fMinZ;
    };

    /* correct dir */
    float fDirCorrect = 0.0f;
    if (CEnemyUtils6045::GetDirection(&fDirCorrect, &vecMyPos, &vecPlrPos))
        EnemyChr().Compositor().SetDirection(fDirCorrect);

    /* calc dist */
    RwV3d vecPos = vecMyPos;
    vecPos.y = 0.0f;

    RwV3d vecAt = vecPlrPos;
    vecAt.y = 0.0f;

    float fDist = CEnemyUtils::GetDistance(&vecPos, &vecAt);    

    /* calc jump speed */
    float fJumpSpeed = BASELEATHERHEAD::SWIMMODE_ATTACK_JUMPSPEED[0];
    float fXZSpeedCorrectRate = BASELEATHERHEAD::SWIMMODE_ATTACK_XZSPEED_CORRECTRATE[0];

    float fCircleRadiusMin = (BASELEATHERHEAD::SWIMMODE_CIRCLE_RADIUS[0] * 2.0f);
    float fCircleRadiusMax = (BASELEATHERHEAD::SWIMMODE_CIRCLE_RADIUS[1] * 2.0f);

    if ((m_stageValue == 1) && (fDist >= fCircleRadiusMin))
    {
        if (fDist <= fCircleRadiusMax)
        {
            float fDistDiff = (fDist - fCircleRadiusMin);
            float fRadiusDiff = (fCircleRadiusMax - fCircleRadiusMin);
            float fRate = (fDistDiff / fRadiusDiff);

            float fJumpSpeedMin = BASELEATHERHEAD::SWIMMODE_ATTACK_JUMPSPEED[0];
            float fJumpSpeedMax = BASELEATHERHEAD::SWIMMODE_ATTACK_JUMPSPEED[1];
            float fJumpSpeedDiff = (fJumpSpeedMax - fJumpSpeedMin);
            fJumpSpeed = fJumpSpeedMin + (fJumpSpeedDiff * fRate);
            
            float fXZSpeedCorrectRateMin = BASELEATHERHEAD::SWIMMODE_ATTACK_XZSPEED_CORRECTRATE[0];
            float fXZSpeedCorrectRateMax = BASELEATHERHEAD::SWIMMODE_ATTACK_XZSPEED_CORRECTRATE[1];            
            float fCorrectRateDiff = (fXZSpeedCorrectRateMax - fXZSpeedCorrectRateMin);
            fXZSpeedCorrectRate = fXZSpeedCorrectRateMin + (fCorrectRateDiff * fRate);
        }
        else
        {
            fJumpSpeed = BASELEATHERHEAD::SWIMMODE_ATTACK_JUMPSPEED[1];
            fXZSpeedCorrectRate = BASELEATHERHEAD::SWIMMODE_ATTACK_XZSPEED_CORRECTRATE[1];
        };
    };

    RwV3d vecJumpVelocity = Math::VECTOR3_ZERO;
    float fGravity = CGameProperty::GetGravity();
    CEnemyUtils::GetJumpSpeedPosToPos(&vecJumpVelocity, fJumpSpeed, &vecMyPos, &vecPlrPos, (fGravity * m_fGravityRate));

    vecJumpVelocity.x *= fXZSpeedCorrectRate;
    vecJumpVelocity.z *= fXZSpeedCorrectRate;

    EnemyChr().Compositor().SetVelocity(&vecJumpVelocity);

    /* init state specific flags and SE */    
    SetSpecialSetting();
    CEnemyUtils6045::EntryWaterDobonEffect(&EnemyChr().Compositor(), 2.25f, false);
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4366));
};


/*virtual*/ CBaseLeatherheadChr::CAfterSwimJumpStatusObserver::RESULT
CBaseLeatherheadChr::CAfterSwimJumpStatusObserver::Observing(void) /*override*/
{
    CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(), m_orderTargetNo, 1.0f, MATH_DEG2RAD(3.0f));

    CEnemyUtils::UpdateVelocityFromDirection(&EnemyChr().Compositor());

    const CCharacterCompositor::COLLISIONWALLINFO* pWallInfo = EnemyChr().Compositor().GetCollisionWall();
    ASSERT(pWallInfo != nullptr);

    if (pWallInfo->m_bHit)
    {
        RwV3d vecVelocity = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetVelocity(&vecVelocity);

        vecVelocity.x = 0.0f;
        vecVelocity.z = 0.0f;

        EnemyChr().Compositor().SetVelocity(&vecVelocity);
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CAfterSwimJumpStatusObserver::OnEnd(void) /*override*/
{
    ClrSpecialSetting();
    return ENEMYTYPES::STATUS_IDLE;
};


void CBaseLeatherheadChr::CAfterSwimJumpStatusObserver::SetSpecialSetting(void)
{
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    float fGravity = CGameProperty::GetGravity();
    CEnemyUtils6045::SetGravitySpecialSetting(&EnemyChr().Compositor(), fGravity * m_fGravityRate);
};


void CBaseLeatherheadChr::CAfterSwimJumpStatusObserver::ClrSpecialSetting(void)
{
    CEnemyUtils6045::ClearGravitySpecialSetting(&EnemyChr().Compositor());
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CAfterSwimTouchdownStatusObserver::CAfterSwimTouchdownStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr)
: m_pBaseLeatherheadChr(pBaseLeatherheadChr)
, m_bFirstTouchdownFlag(false)
{
    ;
};


/*virtual*/ void CBaseLeatherheadChr::CAfterSwimTouchdownStatusObserver::OnStart(void) /*override*/
{
    m_bFirstTouchdownFlag = false;

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
    EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::SWIM_4, true);

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_WINDPRESS), &vecFootPos);
    CMagicManager::SetScale(hMagic, 2.0f);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ CBaseLeatherheadChr::CAfterSwimTouchdownStatusObserver::RESULT
CBaseLeatherheadChr::CAfterSwimTouchdownStatusObserver::Observing(void) /*override*/
{
    const CCharacterCompositor::COLLISIONGROUNDINFO* pGroundInfo = EnemyChr().Compositor().GetCollisionGround();
    ASSERT(pGroundInfo != nullptr);

    if (pGroundInfo->m_bHit && !m_bFirstTouchdownFlag)
    {
        CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
        m_bFirstTouchdownFlag = true;
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CAfterSwimTouchdownStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    m_pBaseLeatherheadChr->AIModerator().SetCatchTriggerAction(LEATHERHEAD_AI::CATCHTRIGGER_SWIM_IDLE);

    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseLeatherheadChr::CThrowStatusObserver::OnStart(void) /*override*/
{
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    CCommonEnemyObserver::CBaseThrow::OnStart();
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CThrowStatusObserver::OnEnd(void) /*override*/
{
    if (IsThrowSuccess())
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        EnemyChr().Compositor().SetDirection(fDir - MATH_PI);
        EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::IDLE_1, true);
    };

    return CCommonEnemyObserver::CBaseThrow::OnEnd();
};


/*virtual*/ void CBaseLeatherheadChr::CThrowStatusObserver::OnThrowStandby(void) /*override*/
{
    CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(),
                                m_orderTargetNo,
                                EnemyChr().Feature().m_fRotateRate,
                                MATH_DEG2RAD(3.0f));
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CGuardStatusObserver::CGuardStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr)
: m_pBaseLeatherheadChr(pBaseLeatherheadChr)
{
    float fGuardTime = 0.5f;
    float fGuardTotalTime = 1.75f;
    SetParameter(fGuardTime, fGuardTotalTime);
};


/*virtual*/ CBaseLeatherheadChr::CGuardStatusObserver::RESULT
CBaseLeatherheadChr::CGuardStatusObserver::PassageGuardTotalTime(void) /*override*/
{
    m_pBaseLeatherheadChr->AIModerator().SetCatchTriggerAction(LEATHERHEAD_AI::CATCHTRIGGER_COUNTER);
    m_bCounterActionFlag = true;
    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CJumpReadyStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vecVelocity = { 0.0f, 0.0f, EnemyChr().Feature().m_fWalkMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);

    vecVelocity.y = EnemyChr().Feature().m_fJumpInitializeSpeed;

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return ENEMYTYPES::STATUS_JUMP;
};


//
// *********************************************************************************
//


/*virtual*/ CBaseLeatherheadChr::CJumpStatusObserver::RESULT
CBaseLeatherheadChr::CJumpStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK)
    {
        int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
        CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(), orderTargetNo, 0.25f, MATH_DEG2RAD(3.0f));
    };

    return CCommonEnemyObserver::CAerialStatus::Observing();
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseLeatherheadChr::CKnockStatusObserver::OnStart(void) /*override*/
{
    m_bSwimFlag = (EnemyChr().StatusSubject().StatusPrev() == BASELEATHERHEAD::STATUS_SWIM);
    if (!m_bSwimFlag)
    {
        CCommonEnemyObserver::CKnockStatus::OnStart();
        return;
    };

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::KNOCK_SWIM, true);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CKnockStatusObserver::OnEnd(void) /*override*/
{
    CCommonEnemyObserver::CKnockStatus::OnEnd();
    if (!m_bSwimFlag)
        return ENEMYTYPES::STATUS_THINKING;

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    return static_cast<ENEMYTYPES::STATUS>(BASELEATHERHEAD::STATUS_SWIM);
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CFlyawayStatusObserver::CFlyawayStatusObserver(void)
: CCommonEnemyObserver::CFlyawayStatus(false)
, m_bSwimFlag(false)
{
    ;
};


/*virtual*/ void CBaseLeatherheadChr::CFlyawayStatusObserver::OnStart(void) /*override*/
{
    m_bSwimFlag = (EnemyChr().StatusSubject().StatusPrev() == BASELEATHERHEAD::STATUS_SWIM);
    if (!m_bSwimFlag)
    {
        CCommonEnemyObserver::CFlyawayStatus::OnStart();
        return;
    };

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().ChangeMotion(BASELEATHERHEAD::MOTIONNAMES::KNOCK_SWIM, true);
};


/*virtual*/ CBaseLeatherheadChr::CFlyawayStatusObserver::RESULT
CBaseLeatherheadChr::CFlyawayStatusObserver::Observing(void) /*override*/
{
    if (!m_bSwimFlag)
        return CCommonEnemyObserver::CFlyawayStatus::Observing();

    if (EnemyChr().Compositor().IsMotionEnd())
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
        return RESULT_END;        
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseLeatherheadChr::CFlyawayStatusObserver::OnEnd(void) /*override*/
{
    if (!m_bSwimFlag)
        return CCommonEnemyObserver::CFlyawayStatus::OnEnd();

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    return static_cast<ENEMYTYPES::STATUS>(BASELEATHERHEAD::STATUS_SWIM);
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CDeathStatusObserver::CDeathStatusObserver(CBaseLeatherheadChr* pBaseLeatherheadChr)
{
    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
        SetParameter(DEATHKIND_NEXUS, nullptr, 1.0f);
    else
        SetParameter(DEATHKIND_MOTION, nullptr, 1.0f);
};


//
// *********************************************************************************
//


CBaseLeatherheadChr::CBaseLeatherheadChr(ENEMYID::VALUE enemyId)
: CBaseChr6045(enemyId)
, m_swimParam()
, m_pStatusAttackE(nullptr)
, m_hLastMagicAttacker(0)
, m_iDamageHeap(0)
, m_bEffectWaterRippleActive(false)
, m_hEffectWaterRipple(0)
, m_bEffectWaterSplashActive(false)
, m_ahEffectWaterSplash()
{
    m_ahEffectWaterSplash[0] = 0;
    m_ahEffectWaterSplash[1] = 0;

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                       new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                        new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                     new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                  new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,                 new CCommonEnemyObserver::CCounteractStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,        new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,         new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                       new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_SLEEP,                      new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_BIND,                       new CCommonEnemyObserver::CCharacterEffectStatusObserver);

    /* CBaseLeatherheadChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new CBaseLeatherheadChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                     new CBaseLeatherheadChr::CAppearStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new CBaseLeatherheadChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CBaseLeatherheadChr::CIdleStatus);
    AttachStatusObserver(BASELEATHERHEAD::STATUS_SWIM,                  new CBaseLeatherheadChr::CSwimStatusObserver(this));
    AttachStatusObserver(BASELEATHERHEAD::STATUS_AFTER_SWIM_JUMP,       new CBaseLeatherheadChr::CAfterSwimJumpStatusObserver);
    AttachStatusObserver(BASELEATHERHEAD::STATUS_AFTER_SWIM_TOUCHDOWN,  new CBaseLeatherheadChr::CAfterSwimTouchdownStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AAA,                 new CBaseLeatherheadChr::CAttackAAAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,                   new CBaseLeatherheadChr::CAttackBStatusObserver);
    AttachStatusObserver(BASELEATHERHEAD::STATUS_ATTACK_E_STUN,         new CBaseLeatherheadChr::CAttackEStunStatusObserver);
    AttachStatusObserver(BASELEATHERHEAD::STATUS_ATTACK_COUNTER,        new CBaseLeatherheadChr::CAttackCounterStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CBaseLeatherheadChr::CJumpReadyStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP,                       new CBaseLeatherheadChr::CJumpStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                      new CBaseLeatherheadChr::CGuardStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CBaseLeatherheadChr::CKnockStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CBaseLeatherheadChr::CKnockStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,              new CBaseLeatherheadChr::CFlyawayStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,               new CBaseLeatherheadChr::CFlyawayStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                      new CBaseLeatherheadChr::CDeathStatusObserver(this));
    AttachThrowStatusObserver();

    CBaseLeatherheadChr::CAttackEStatusObserver* pStatusAttackE = new CBaseLeatherheadChr::CAttackEStatusObserver;
    m_pStatusAttackE = pStatusAttackE;
    AttachStatusObserver(BASELEATHERHEAD::STATUS_ATTACK_E, pStatusAttackE);

    /* init knockback ctrl */
    m_bKnockBackCtrlActive = true;
    SetKnockBackCtrlEnable(true);

    /* init leatherhead params */
    InitSwimParameter();
    m_hLastMagicAttacker = 0;
    m_iDamageHeap = 0;
    InitWaterEffect();
};


/*virtual*/ CBaseLeatherheadChr::~CBaseLeatherheadChr(void)
{
    TermWaterEffect();
};


/*virtual*/ void CBaseLeatherheadChr::Run(void) /*override*/
{
    if (!m_swimParam.bSwimModeFlag)
    {
        float fHpRatio = CEnemyUtils::GetEnemyHPRate(this);
        if (fHpRatio <= 0.4f)
            SetSwimSetting();
    };

    ProcSwimModeMustSwimming();
    CBaseChr6045::Run();
    ProcSwimModeTimer();
    ProcWaterEffect();
};


/*virtual*/ bool CBaseLeatherheadChr::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();
    if ((statusNow == BASELEATHERHEAD::STATUS_SWIM) ||
        (statusNow == BASELEATHERHEAD::STATUS_AFTER_SWIM_JUMP) ||
        (statusNow == BASELEATHERHEAD::STATUS_ATTACK_E))
    {
        return false;
    };

    return CBaseChr6045::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ void CBaseLeatherheadChr::OnMessageTouchdown(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();

    bool bIsFlyaway = (statusNow == ENEMYTYPES::STATUS_FLYAWAY_FRONT) ||
                      (statusNow == ENEMYTYPES::STATUS_FLYAWAY_BACK);

    bool bIsDead = TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS);

    if (!bIsFlyaway || !m_swimParam.bNowSwimmingFlag || bIsDead)
    {
        CBaseChr6045::OnMessageTouchdown(fHeight);

        statusNow = GetStatus();
        if (statusNow == BASELEATHERHEAD::STATUS_AFTER_SWIM_JUMP)
            SetStatus(BASELEATHERHEAD::STATUS_AFTER_SWIM_TOUCHDOWN);
    };
};


/*virtual*/ void CBaseLeatherheadChr::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();
	if (statusNow == BASELEATHERHEAD::STATUS_ATTACK_E)
        m_pStatusAttackE->CatchAttackResult(pCatch);

    CBaseChr6045::OnMessageAttackResult(pCatch);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE CBaseLeatherheadChr::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    if (!CheckDamagePermission(rCalc))
    {
        rCalc.GetAttack().SetPower(0);
        return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
    };

    return CBaseChr6045::OnDamage(rCalc);
};


/*virtual*/ void CBaseLeatherheadChr::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    ProcSwimModeDamageHeap(iAmount);
    CBaseChr6045::OnMessageReceivedDamage(iAmount);
};


bool CBaseLeatherheadChr::CheckDamagePermission(CCharacterAttackCalculator& rCalc)
{
    if (!m_swimParam.bSwimModeFlag)
        return true;

    ENEMYTYPES::STATUS statusNow = GetStatus();

    /* handle case when we are going to swim and we are stunned (CAttackEStunStatusObserver) */
    if (m_swimParam.bGoingSwimming)
    {
        return (CEnemyUtils::IsStunState(statusNow) ||
               (statusNow == BASELEATHERHEAD::STATUS_ATTACK_E_STUN));
    };

    /* TODO: following ifdef-endif block not exist in PS2 US version */
#ifdef TARGET_PC
    if (!m_swimParam.bNowSwimmingFlag)
        return true;
    
    if (statusNow == BASELEATHERHEAD::STATUS_SWIM)
    {
        if (AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK)
            return false;
    };

    if (statusNow == ENEMYTYPES::STATUS_COUNTERACT)
        return false;
#endif /* TARGET_PC */

    /* allow damage from magic in swim mode */
    if (m_swimParam.bNowSwimmingFlag)
    {
        CGameObject* pAttacker = rCalc.GetAttacker();
        uint32 hAttacker = pAttacker->GetHandle();

        bool bIsNewAttacker = (hAttacker != m_hLastMagicAttacker);
        bool bIsEffectAttack = (pAttacker->GetType() == GAMEOBJECTTYPE::EFFECT);

        if (bIsNewAttacker && bIsEffectAttack)
        {
            CEffect* pEffect = static_cast<CEffect*>(pAttacker);
            ASSERT(pEffect->GetEffectType() == CEffect::TYPE_WITHHIT);

            CMagic* pMagic = static_cast<CMagic*>(pEffect);
            if (CMagicManager::IsChargeAttack(pMagic))
            {
                m_hLastMagicAttacker = hAttacker;
                return true;
            };
        };
    };

	return false;
};


void CBaseLeatherheadChr::InitSwimParameter(void)
{
    m_swimParam.bSwimModeFlag = false;
    m_swimParam.bNowSwimmingFlag = false;
    m_swimParam.bGoingSwimming = false;
    m_swimParam.fSwimTimer = 0.0f;

    SetSharedData(&m_swimParam);
};


void CBaseLeatherheadChr::SetSwimSetting(void)
{
    m_swimParam.bSwimModeFlag = true;
    m_swimParam.bGoingSwimming = true; /* starts swimming as soon as swim mode available */
    
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    m_swimParam.fSwimTimer = BASELEATHERHEAD::SWIMMODE_ATTACK_TIME[difficulty];

    AIModerator().SetCatchTriggerAction(LEATHERHEAD_AI::CATCHTRIGGER_SWIM_MODE);
};


void CBaseLeatherheadChr::ProcSwimModeMustSwimming(void)
{
    if (!m_swimParam.bSwimModeFlag)
        return;

    if (m_swimParam.bGoingSwimming)
    {
        ENEMYTYPES::STATUS statusNow = GetStatus();
        if (statusNow == BASELEATHERHEAD::STATUS_SWIM)
        {
            m_swimParam.bGoingSwimming = false;
            m_iDamageHeap = 0;
        }
        else
        {
            AIModerator().SetCatchTriggerAction(LEATHERHEAD_AI::CATCHTRIGGER_SWIM_MOVE);
        };
    };

    if (m_iDamageHeap >= BASELEATHERHEAD::DAMAGE_AMOUNT_RECV_TO_SWIMMODE)
    {
        m_swimParam.bGoingSwimming = true;
        AIModerator().SetCatchTriggerAction(LEATHERHEAD_AI::CATCHTRIGGER_SWIM_MOVE);
    };
};


void CBaseLeatherheadChr::ProcSwimModeTimer(void)
{
    if (m_swimParam.bSwimModeFlag &&
        m_swimParam.bNowSwimmingFlag &&
        (m_swimParam.fSwimTimer > 0.0f))
    {
        m_swimParam.fSwimTimer -= CGameProperty::GetElapsedTime();
    };
};


void CBaseLeatherheadChr::ProcSwimModeDamageHeap(int32 iAmount)
{
    if (m_swimParam.bSwimModeFlag && !m_swimParam.bNowSwimmingFlag)
        m_iDamageHeap += iAmount;
};


void CBaseLeatherheadChr::InitWaterEffect(void)
{
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    Compositor().GetPosition(&vecMyPos);

    /* init ripple effect */
    m_bEffectWaterRippleActive = false;
    m_hEffectWaterRipple = CEffectManager::Play(EFFECTID::ID_ALL_W_HAMON, &vecMyPos);
    CEffectManager::SetScale(m_hEffectWaterRipple, 2.0f);
    CEffectManager::SetActive(m_hEffectWaterRipple, m_bEffectWaterRippleActive);

    /* init splash effect */
    m_bEffectWaterSplashActive = false;
    for (int32 i = 0; i < COUNT_OF(m_ahEffectWaterSplash); ++i)
    {
        m_ahEffectWaterSplash[i] = CEffectManager::Play(EFFECTID::ID_ALL_W_SPLASH, &vecMyPos);
        CEffectManager::SetScale(m_ahEffectWaterSplash[i], 1.75f);
        CEffectManager::SetActive(m_ahEffectWaterSplash[i], m_bEffectWaterSplashActive);
    };
};


void CBaseLeatherheadChr::TermWaterEffect(void)
{
    /* terminate splash effect */
    for (int32 i = 0; i < COUNT_OF(m_ahEffectWaterSplash); ++i)
    {
        if (m_ahEffectWaterSplash[i])
        {
            CEffectManager::Finish(m_ahEffectWaterSplash[i]);
            m_ahEffectWaterSplash[i] = 0;
        };
    };

    /* terminate ripple effect */
    if (m_hEffectWaterRipple)
    {
        CEffectManager::Finish(m_hEffectWaterRipple);
        m_hEffectWaterRipple = 0;
    };
};


void CBaseLeatherheadChr::ProcWaterEffect(void)
{
    int32 iHpNow = Feature().m_iHP;
    bool bIsInWater = CEnemyUtils::CheckInfoWater(&Compositor());

    if ((iHpNow > 0) && bIsInWater)
    {
        bool bIsMoving = false;

        ENEMYTYPES::STATUS statusNow = GetStatus();
        switch (static_cast<int32>(statusNow))
        {
        case BASELEATHERHEAD::STATUS_SWIM:
            {
                const char* pszMotionNow = Compositor().GetMotionName();
                if (!std::strcmp(pszMotionNow, BASELEATHERHEAD::MOTIONNAMES::SWIM_2))
                    bIsMoving = true;
            }
            break;

        case BASELEATHERHEAD::STATUS_ATTACK_E:
            {
                const char* pszMotionNow = Compositor().GetMotionName();
                if (!std::strcmp(pszMotionNow, BASELEATHERHEAD::MOTIONNAMES::ATTACK_E2))
                    bIsMoving = true;
            }
            break;

        case ENEMYTYPES::STATUS_RUN:
            bIsMoving = true;
            break;

        default:
            break;
        };

        SetWaterSplashActive(bIsMoving ? true : false);
        SetWaterRippleActive(true);
    }
    else
    {
        SetWaterRippleActive(false);
        SetWaterSplashActive(false);
    };

    SetWaterRipplePosition();
    SetWaterSplashPosition();
};


void CBaseLeatherheadChr::SetWaterRippleActive(bool bActive)
{
    if (m_bEffectWaterRippleActive == bActive)
        return;

    m_bEffectWaterRippleActive = bActive;
    CEffectManager::SetActive(m_hEffectWaterRipple, bActive);
};


void CBaseLeatherheadChr::SetWaterSplashActive(bool bActive)
{
    if (m_bEffectWaterSplashActive == bActive)
        return;

    m_bEffectWaterSplashActive = bActive;
    for (int32 i = 0; i < COUNT_OF(m_ahEffectWaterSplash); ++i)
        CEffectManager::SetActive(m_ahEffectWaterSplash[i], bActive);
};


void CBaseLeatherheadChr::SetWaterRipplePosition(void)
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    Compositor().GetFootPosition(&vecMyFootPos);

    SetWaterEffectPosition(m_hEffectWaterRipple, &vecMyFootPos);
};


void CBaseLeatherheadChr::SetWaterSplashPosition(void)
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    Compositor().GetFootPosition(&vecMyFootPos);

    RwV3d vecOffset = m_swimParam.bNowSwimmingFlag ? BASELEATHERHEAD::EFFECT_WATERSPLASH_SWIM_OFFSET :
                                                     BASELEATHERHEAD::EFFECT_WATERSPLASH_NORMAL_OFFSET;

    Compositor().RotateVectorByDirection(&vecOffset, &vecOffset);

    for (int32 i = 0; i < COUNT_OF(m_ahEffectWaterSplash); ++i)
    {
        RwV3d vecPos = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecPos, &vecMyFootPos, &vecOffset);

        SetWaterEffectPosition(m_ahEffectWaterSplash[i], &vecPos);
    };
};


void CBaseLeatherheadChr::SetWaterEffectPosition(uint32 hEffect, const RwV3d* pvecPos)
{
    RwV3d vecPos = *pvecPos;
    CWorldMap::GetWaterSurfacePosition(&vecPos);

    CEffectManager::SetPosition(hEffect, &vecPos);
};


void CBaseLeatherheadChr::AttachThrowStatusObserver(void)
{
    m_pThrow = new CBaseLeatherheadChr::CThrowStatusObserver;

    RwV3d vecBone = { 0.0f, 0.5f, 0.0f };
    m_pThrow->SetTraceBoneData(3, &vecBone);

    RwV3d vecThrow = { 0.0f, 0.0f, -15.0f };
    m_pThrow->SetThrowVector(&vecThrow);

    AttachStatusObserver(ENEMYTYPES::STATUS_THROW, m_pThrow);
};


bool CBaseLeatherheadChr::IsSwimming(void) const
{
    if (m_swimParam.bSwimModeFlag)
        return m_swimParam.bNowSwimmingFlag;

    return false;
};


void CBaseLeatherheadChr::SetSwimTimer(float fTimer)
{
    m_swimParam.fSwimTimer = fTimer;
};


float CBaseLeatherheadChr::GetSwimTimer(void) const
{
    return m_swimParam.fSwimTimer;
};