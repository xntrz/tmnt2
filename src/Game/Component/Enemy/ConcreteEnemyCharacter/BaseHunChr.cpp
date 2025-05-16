#include "BaseHunChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseHunAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/FallRubbleGimmickManager.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"

#ifdef _DEBUG
#include "Game/System/GameObject/GameObjectManager.hpp"
#endif /* _DEBUG */


CBaseHunChr::CHideStatusObserver::CHideStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_APPEAR)
{
    ;
};


//
// *********************************************************************************
//


CBaseHunChr::CAppearStatusObserver::CAppearStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_THINKING)
{
    ;
};


//
// *********************************************************************************
//


CBaseHunChr::CThinkingStatusObserver::CThinkingStatusObserver(CBaseHunChr* pBaseHunChr)
: m_pBaseHunChr(pBaseHunChr)
, m_bBurstEffectActive(false)
, m_hBurstEffect(0)
, m_fBurstThinkWaitTimer(0.0f)
, m_nextStatus(ENEMYTYPES::STATUS_IDLE)
{
    RwV3d vecTraceOffset = { 0.0f, 1.0f, 0.0f };
    m_hBurstEffect = CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_WHITESMOKE,
                                                       &pBaseHunChr->Compositor(),
                                                       2.0f,
                                                       CEnemyUtils6045::TRACE_BODY,
                                                       false,
                                                       &vecTraceOffset);
    CEffectManager::SetActive(m_hBurstEffect, false);
};


/*virtual*/ void CBaseHunChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_fBurstThinkWaitTimer = 0.0f;
    m_bBurstEffectActive = false;
    CEffectManager::SetActive(m_hBurstEffect, false);
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CBaseHunChr::CThinkingStatusObserver::RESULT
CBaseHunChr::CThinkingStatusObserver::Observing(void) /*override*/
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
            };
#ifdef _DEBUG
            /* for test map avoid obstacles */
            if (moveType == BASEAI6045::ORDERTYPE_MOVE_JUMP)
                m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
#endif /* _DEBUG */
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            if (!BurstAttackThinkWaitProc())
                return RESULT_CONTINUE;

            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case BASEAI6045::ORDERTYPE_ATTACK_THROW:
                m_nextStatus = ENEMYTYPES::STATUS_THROW;
                break;

            case BASEAI6045::ORDERTYPE_ATTACK_COUNTER:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASEHUN::STATUS_ATTACK_D);
                break;

            case BASEHUN_AI::ORDERTYPE_ATTACK_A:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case BASEHUN_AI::ORDERTYPE_ATTACK_B:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case BASEHUN_AI::ORDERTYPE_ATTACK_C:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
                break;

            case BASEHUN_AI::ORDERTYPE_ATTACK_D:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASEHUN::STATUS_ATTACK_D);
                break;

            case BASEHUN_AI::ORDERTYPE_ATTACK_F:
            case BASEHUN_AI::ORDERTYPE_ATTACK_F_TALL:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASEHUN::STATUS_ATTACK_F);
                break;
                
            case BASEHUN_AI::ORDERTYPE_ATTACK_F_RUN:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASEHUN::STATUS_ATTACK_F_CONNECT);
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


/*virtual*/ ENEMYTYPES::STATUS CBaseHunChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    if (m_bBurstEffectActive)
        CEffectManager::SetActive(m_hBurstEffect, false);

    return m_nextStatus;
};


bool CBaseHunChr::CThinkingStatusObserver::BurstAttackThinkWaitProc(void)
{
    if (!m_pBaseHunChr->IsInBurstMode())
        return true;

    if (!m_bBurstEffectActive)
    {
        m_bBurstEffectActive = true;
        CEffectManager::SetActive(m_hBurstEffect, m_bBurstEffectActive);

        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
        EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    };

    int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;

    CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(),
                                orderTargetNo,
                                EnemyChr().Feature().m_fRotateRate,
                                MATH_DEG2RAD(3.0f));

    if (m_fBurstThinkWaitTimer < 0.5f)
    {
        m_fBurstThinkWaitTimer += CGameProperty::GetElapsedTime();
        return false;
    };

    return true;
};


//
// *********************************************************************************
//


/*virtual*/ CBaseHunChr::CRunStatusObserver::RESULT
CBaseHunChr::CRunStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_ATTACK)
        return CCommonEnemyObserver::CMoveStatus::Observing();

    if (EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType != BASEHUN_AI::ORDERTYPE_ATTACK_F_RUN)
        return CCommonEnemyObserver::CMoveStatus::Observing();

    m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASEHUN::STATUS_ATTACK_F);
    return RESULT_END;
};


//
// *********************************************************************************
//


CBaseHunChr::CAttackAStatusObserver::CAttackAStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_A)
{
    ;
};


//
// *********************************************************************************
//


CBaseHunChr::CAttackBStatusObserver::CAttackBStatusObserver(CBaseHunChr* pBaseHunChr)
: CBaseChr6045::COneShotMotionForAttack(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
, m_pBaseHunChr(pBaseHunChr)
, m_pszFallRubbleMgrObjName(nullptr)
, m_vecOffset(Math::VECTOR3_ZERO)
, m_fScale(0.0f)
{
    int32 numRubbleCreate = 0;
    float fRubbleBaseHeight = 0.0f;
    float fRubbleFallWaitTime = 0.0f;

    if (m_pBaseHunChr->GetID() == ENEMYID::ID_HUN_A)
    {
        m_pszFallRubbleMgrObjName = BASEHUN::ATTACK_B_GIMMICK_NAME_HUN_1;
        m_vecOffset = { 0.0f, 0.0f, 0.5f };
        m_fScale = 0.85f;

        numRubbleCreate = 4;
        fRubbleBaseHeight = 7.0f;
        fRubbleFallWaitTime = 0.125f;
    }
    else
    {
        m_pszFallRubbleMgrObjName = BASEHUN::ATTACK_B_GIMMICK_NAME_HUN_2;
        m_vecOffset = { 0.0f, 0.0f, 0.9f };
        m_fScale = 1.0f;

        numRubbleCreate = 6;
        fRubbleBaseHeight = 9.0f;
        fRubbleFallWaitTime = 0.2f;
    };

    CGimmick* pGimmick = CGimmickManager::Find(m_pszFallRubbleMgrObjName);

#ifdef _DEBUG
    /* find rubble mgr on the test map */
    if (pGimmick == nullptr)
    {
        CGameObject* pGameObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK);
        while (pGameObj)
        {
            if (static_cast<CGimmick*>(pGameObj)->GetID() == GIMMICKID::ID_K_M28OBRM)
            {
                pGimmick = static_cast<CGimmick*>(pGameObj);
                m_pszFallRubbleMgrObjName = pGameObj->GetName();
                break;
            };

            pGameObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK, pGameObj);
        };
    };
#endif /* _DEBUG */
    
    if (pGimmick)
    {
        CFallRubbleGimmickManager* pFallRubbleGimmickMgr = static_cast<CFallRubbleGimmickManager*>(pGimmick);
        pFallRubbleGimmickMgr->SetRubbleCreateParam(numRubbleCreate, fRubbleBaseHeight, fRubbleFallWaitTime);
    };
};


/*virtual*/ void CBaseHunChr::CAttackBStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttack::OnStart();
    CMessageManager::DirectCall(SDCODE_VOICE(16424));
};


/*virtual*/ CBaseHunChr::CAttackBStatusObserver::RESULT
CBaseHunChr::CAttackBStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

        RwV3d vecOffset = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().RotateVectorByDirection(&vecOffset, &m_vecOffset);

        RwV3d vecWindPressPos = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecWindPressPos, &vecMyFootPos, &vecOffset);

        uint32 hMagic = CEnemyUtils6045::EntrySimpleMagic(MAGICID::ID_WINDPRESS, &vecWindPressPos, m_fScale);
        CMagicManager::SetChangeStatus(hMagic, CHitAttackData::STATUS_FLYAWAY);

        CGimmickManager::PostEvent(m_pszFallRubbleMgrObjName,
                                   EnemyChr().Compositor().GetName(),
                                   GIMMICKTYPES::EVENTTYPE_ACTIVATE);

        CGameProperty::SetCameraVibration(0.75f, 1.0f, 8);
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4407));
        
        m_bMotionEnd = true;
    };

    return CBaseChr6045::COneShotMotionForAttack::Observing();
};


/*virtual*/ ENEMYTYPES::STATUS CBaseHunChr::CAttackBStatusObserver::OnEnd(void) /*override*/
{
    if (m_bMotionEnd)
    {
        if (m_pBaseHunChr->IsInRevengeMode())
            m_pBaseHunChr->ClearRevengeMode();
    };

    return CBaseChr6045::COneShotMotionForAttack::OnEnd();
};


//
// *********************************************************************************
//


CBaseHunChr::CAttackCStatusObserver::CAttackCStatusObserver(CBaseHunChr* pBaseHunChr)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_C)
, m_pBaseHunChr(pBaseHunChr)
, m_vecAimPos(Math::VECTOR3_ZERO)
, m_fTimer(0.0f)
, m_vecEffectPos(Math::VECTOR3_ZERO)
, m_hEffect(0)
, m_bEffectActive(false)
{
    ;
};


/*virtual*/ void CBaseHunChr::CAttackCStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart();
    m_fTimer = 0.0f;

    InitWaterSplashEffect();
};


/*virtual*/ CBaseHunChr::CAttackCStatusObserver::RESULT
CBaseHunChr::CAttackCStatusObserver::Observing(void) /*override*/
{
    if (m_fTimer < 0.35f)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
        if (pPlrChr)
        {
            RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

            RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
            pPlrChr->GetFootPosition(&vecFootPosPlr);

            float fDist = CEnemyUtils::GetDistance(&vecFootPosMe, &vecFootPosPlr);

            RwV3d vecVelocityPlr = Math::VECTOR3_ZERO;
            pPlrChr->GetVelocity(&vecVelocityPlr);

            Math::Vec3_Scale(&vecVelocityPlr, &vecFootPosPlr, fDist * 0.06f);
            Math::Vec3_Add(&m_vecAimPos, &vecFootPosPlr, &vecVelocityPlr);

            CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &m_vecAimPos, 1.0f, 0.0f);
        };

        m_fTimer += CGameProperty::GetElapsedTime();
    };

    if (EnemyChr().Compositor().IsMotionEnd())
    {
        m_bMotionEnd = true;
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseHunChr::CAttackCStatusObserver::OnEnd(void) /*override*/
{
    TermWaterSplashEffect();

    if (m_bMotionEnd)
    {
        if (m_pBaseHunChr->IsInRevengeMode())
            m_pBaseHunChr->ClearRevengeMode();

        m_pBaseHunChr->AIModerator().DecisionUnitCommonParameter().SetAttackWaitTime(0.25f);
    };
    
    return CBaseChr6045::COneShotMotionForAttackPlayerAim::OnEnd();
};


void CBaseHunChr::CAttackCStatusObserver::InitWaterSplashEffect(void)
{
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);

    m_hEffect = CEffectManager::Play(EFFECTID::ID_ALL_W_SPLASH, &vecMyPos);

    float fScale = (EnemyChr().CharacterParameter().m_fShadowRadius / 1.5f);
    CEffectManager::SetScale(m_hEffect, fScale);

    m_bEffectActive = false;
    CEffectManager::SetActive(m_hEffect, m_bEffectActive);

    EnemyChr().Compositor().GetFootPosition(&m_vecEffectPos);
};


void CBaseHunChr::CAttackCStatusObserver::TermWaterSplashEffect(void)
{
    if (m_hEffect)
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;
    };
};


void CBaseHunChr::CAttackCStatusObserver::ProcWaterSplashEffect(void)
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

    RwV3d vecPosFrameDiff = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecPosFrameDiff, &vecMyFootPos, &m_vecEffectPos);


    bool bEffectActive = false;

    if (CEnemyUtils::CheckInfoWater(&EnemyChr().Compositor()))
    {
        RwV3d vecPosNow = vecMyFootPos;
        RwV3d vecPosPrev = m_vecEffectPos;
        RwV3d vecVelocity = Math::VECTOR3_ZERO;
        CEnemyUtils::GetVelocityByPosDiff(&vecVelocity, &vecPosNow, &vecPosPrev);
        
        float fMoveSpeed = Math::Vec3_Length(&vecVelocity);
        if (fMoveSpeed > 6.0f)
            bEffectActive = true;
    };

    m_vecEffectPos = vecMyFootPos;

    if (m_bEffectActive != bEffectActive)
    {
        CEffectManager::SetActive(m_hEffect, bEffectActive);
        m_bEffectActive = bEffectActive;
    };
    
    CWorldMap::GetWaterSurfacePosition(&vecMyFootPos);
    CEffectManager::SetPosition(m_hEffect, &vecMyFootPos);
};


//
// *********************************************************************************
//


CBaseHunChr::CAttackDStatusObserver::CAttackDStatusObserver(CBaseHunChr* pBaseHunChr)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(BASEHUN::MOTIONNAMES::ATTACK_D)
, m_pBaseHunChr(pBaseHunChr)
{
    ;
};


/*virtual*/ void CBaseHunChr::CAttackDStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart();
    CMessageManager::DirectCall(SDCODE_VOICE(16426));
};


/*virtual*/ ENEMYTYPES::STATUS CBaseHunChr::CAttackDStatusObserver::OnEnd(void) /*override*/
{
    if (m_bMotionEnd)
    {
        if (m_pBaseHunChr->IsInRevengeMode())
            m_pBaseHunChr->ClearRevengeMode();
    };

    return CBaseChr6045::COneShotMotionForAttackPlayerAim::OnEnd();
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseHunChr::CThrowStatusObserver::OnStart(void) /*override*/
{
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    CCommonEnemyObserver::CBaseThrow::OnStart();
    CMessageManager::DirectCall(SDCODE_VOICE(16425));
};


/*virtual*/ ENEMYTYPES::STATUS CBaseHunChr::CThrowStatusObserver::OnEnd(void) /*override*/
{
    if (IsThrowSuccess())
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        EnemyChr().Compositor().SetDirection(fDir - MATH_PI);
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
    };

    return CCommonEnemyObserver::CBaseThrow::OnEnd();
};


/*virtual*/ void CBaseHunChr::CThrowStatusObserver::OnThrowStandby(void) /*override*/
{
    CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(),
                                m_orderTargetNo,
                                EnemyChr().Feature().m_fRotateRate,
                                MATH_DEG2RAD(3.0f));
};


//
// *********************************************************************************
//


CBaseHunChr::CAttackFStatusObserver::CAttackFStatusObserver(void)
: m_orderTargetNo(-1)
{
    SetParameter(BASEHUN::MOTIONNAMES::ATTACK_F1,
                 BASEHUN::MOTIONNAMES::ATTACK_F2,
                 BASEHUN::MOTIONNAMES::ATTACK_F3,
                 16.0f);
};


/*virtual*/ void CBaseHunChr::CAttackFStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::CJumpActionStatusObserver::OnStart();
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
};


/*virtual*/ CBaseHunChr::CAttackFStatusObserver::RESULT
CBaseHunChr::CAttackFStatusObserver::Observing(void) /*override*/
{
    if (m_step == STEP_JUMP_READY)
    {
        CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(),
                                    m_orderTargetNo,
                                    EnemyChr().Feature().m_fRotateRate,
                                    MATH_DEG2RAD(3.0f));
    };

    return CBaseChr6045::CJumpActionStatusObserver::Observing();
};


/*virtual*/ ENEMYTYPES::STATUS CBaseHunChr::CAttackFStatusObserver::OnEnd(void) /*override*/
{
    CEnemyUtils6045::ClearGravitySpecialSetting(&EnemyChr().Compositor());
    return CBaseChr6045::CJumpActionStatusObserver::OnEnd();
};


/*virtual*/ void CBaseHunChr::CAttackFStatusObserver::SetJumpSetting(void) /*override*/
{
    CPlayerCharacter* pPlrchr = CAIUtils::GetActivePlayer(m_orderTargetNo);
    if (pPlrchr)
    {
        RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrchr->GetFootPosition(&vecFootPosPlr);

        RwV3d vecDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDir, &vecFootPosPlr, &vecFootPosMe);
        Math::Vec3_Normalize(&vecDir, &vecDir);

        RwV3d vecDirDeflection = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vecDirDeflection, &vecDir, 0.8f);

        RwV3d vecJumpPos = Math::VECTOR3_ZERO;
        Math::Vec3_Add(&vecJumpPos, &vecFootPosPlr, &vecDirDeflection);

        float fGravity = CGameProperty::GetGravity();
        RwV3d vecVelocity = Math::VECTOR3_ZERO;
        CEnemyUtils::GetJumpSpeedPosToPos(&vecVelocity,
                                          m_fJumpInitSpeed,
                                          &vecFootPosMe,
                                          &vecJumpPos,
                                          (fGravity * 4.0f));

        EnemyChr().Compositor().SetVelocity(&vecVelocity);
        EnemyChr().Compositor().SetDirectionFromVector(&vecVelocity);
    }
    else
    {
        RwV3d vecVelocity = { 0.0f, 0.0f, 11.0f };
        EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
        vecVelocity.y = m_fJumpInitSpeed;

        EnemyChr().Compositor().SetVelocity(&vecVelocity);
    };

    float fGravity = CGameProperty::GetGravity();
    CEnemyUtils6045::SetGravitySpecialSetting(&EnemyChr().Compositor(), (fGravity * 4.0f));
};


/*virtual*/ void CBaseHunChr::CAttackFStatusObserver::SetTouchdownSetting(void) /*override*/
{
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    CEnemyUtils::EntryFlyawayBoundEffectAndSE(&EnemyChr());
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseHunChr::CAttackFConnectFromRunStatusStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(m_pszMotionJumpReady, true);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    m_step = STEP_JUMP_READY;
};


/*virtual*/ CBaseHunChr::CAttackFConnectFromRunStatusStatusObserver::RESULT
CBaseHunChr::CAttackFConnectFromRunStatusStatusObserver::Observing(void) /*override*/
{
    return CBaseChr6045::CJumpActionStatusObserver::Observing();
};


/*virtual*/ void CBaseHunChr::CAttackFConnectFromRunStatusStatusObserver::SetJumpSetting(void) /*override*/
{
    RwV3d vecVel = { 0.0f, 0.0f, 11.0f };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVel, &vecVel);

    vecVel.y = 16.0f;

    EnemyChr().Compositor().SetVelocity(&vecVel);

    float fGravity = CGameProperty::GetGravity();
    CEnemyUtils6045::SetGravitySpecialSetting(&EnemyChr().Compositor(), (fGravity * 4.0f));
};


//
// *********************************************************************************
//


CBaseHunChr::CGuardStatusObserver::CGuardStatusObserver(CBaseHunChr* pBaseHunChr)
: m_pBaseHunChr(pBaseHunChr)
{
    float fGuardTime = 0.5f;
    float fGuardTotalTime = 1.5f;
    SetParameter(fGuardTime, fGuardTotalTime);
};


/*virtual*/ CBaseHunChr::CGuardStatusObserver::RESULT
CBaseHunChr::CGuardStatusObserver::PassageGuardTotalTime(void) /*override*/
{
    m_pBaseHunChr->AIModerator().SetCatchTriggerAction(BASEHUN_AI::CATCHTRIGGER_COUNTER);
    m_bCounterActionFlag = true;
    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


CBaseHunChr::CFlyawayForBurstStatusObserver::CFlyawayForBurstStatusObserver(CBaseHunChr* pBaseHunChr)
: CCommonEnemyObserver::CFlyawayStatus(false)
, m_pBaseHunChr(pBaseHunChr)
{
    ;
};


/*virtual*/ void CBaseHunChr::CFlyawayForBurstStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_FRONT, true);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    RwV3d vAttackDir = EnemyChr().Compositor().AttackParameter().m_vDirection;
    EnemyChr().Compositor().SetDirectionFromVector(&vAttackDir);

    RwV3d vAttackVel = EnemyChr().Compositor().AttackParameter().m_vVelocity;
    EnemyChr().Compositor().SetVelocity(&vAttackVel);

    float fSpeed = std::fabs(vAttackVel.y);

    m_fJumpInitSpeed = fSpeed * 0.9f;
    m_nextStatus     = ENEMYTYPES::STATUS_IDLE;

    if (EnemyChr().Compositor().AttackParameter().m_direction == CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK)
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        EnemyChr().Compositor().SetDirection(fDir + MATH_PI);
    };

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    m_pBaseHunChr->ClearRevengeMode();
};


/*virtual*/ ENEMYTYPES::STATUS CBaseHunChr::CFlyawayForBurstStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    return CCommonEnemyObserver::CFlyawayStatus::OnEnd();
};


//
// *********************************************************************************
//


CBaseHunChr::CFlyawayBoundForBurstStatusObserver::CFlyawayBoundForBurstStatusObserver(CBaseHunChr* pBaseHunChr)
: m_pBaseHunChr(pBaseHunChr)
{
    ;
};


/*virtual*/ void CBaseHunChr::CFlyawayBoundForBurstStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_BOUND_FRONT, true);

    RwV3d vecVel = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVel);

    float fMotionEndT = EnemyChr().Compositor().GetMotionEndTime();
    RwV3d vecAccel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecAccel, &vecVel, -(1.0f / fMotionEndT));

    EnemyChr().Compositor().SetAcceleration(&vecAccel);

    m_bCheckVelocityFlag = false;
    m_bHitFlag = false;

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseHunChr::CFlyawayBoundForBurstStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    m_pBaseHunChr->SetBurstMode();

    return CCommonEnemyObserver::CFlyawayBoundStatus::OnEnd();
};


//
// *********************************************************************************
//


CBaseHunChr::CDeathStatusObserver::CDeathStatusObserver(CBaseHunChr* pBaseHunChr)
: m_pBaseHunChr(pBaseHunChr)
{
    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
        SetParameter(DEATHKIND_NEXUS, nullptr, 1.0f);
    else
        SetParameter(DEATHKIND_MOTION, nullptr, 1.0f);
};


/*virtual*/ void CBaseHunChr::CDeathStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::CDeathStatusObserver::OnStart();
    m_pBaseHunChr->ClearRevengeMode();
    m_pBaseHunChr->ClearBurstMode();
};


//
// *********************************************************************************
//


CBaseHunChr::CBaseHunChr(ENEMYID::VALUE enemyId)
: CBaseChr6045(enemyId)
, m_bRevengeModeActive(false)
, m_bBurstModeActie(false)
, m_revengeDamageHeap(0)
, m_revengeDamageMax(0)
, m_modelColorCtrl()
{
    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);

    /* CBaseHunChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseHunChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                 new CBaseHunChr::CAppearStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new CBaseHunChr::CThinkingStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new CBaseHunChr::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new CBaseHunChr::CAttackBStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new CBaseHunChr::CAttackCStatusObserver(this));
    AttachStatusObserver(BASEHUN::STATUS_ATTACK_D,                  new CBaseHunChr::CAttackDStatusObserver(this));
    AttachStatusObserver(BASEHUN::STATUS_ATTACK_F,                  new CBaseHunChr::CAttackFStatusObserver);
    AttachStatusObserver(BASEHUN::STATUS_ATTACK_F_CONNECT,          new CBaseHunChr::CAttackFConnectFromRunStatusStatusObserver);
    AttachThrowStatusObserver();
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                    new CBaseHunChr::CRunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new CBaseHunChr::CGuardStatusObserver(this));
    AttachStatusObserver(BASEHUN::STATUS_BURST_FLYAWAY,             new CBaseHunChr::CFlyawayForBurstStatusObserver(this));
    AttachStatusObserver(BASEHUN::STATUS_BURST_FLYAWAY_BOUND,       new CBaseHunChr::CFlyawayBoundForBurstStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseHunChr::CDeathStatusObserver(this));

#ifdef _DEBUG
    /* CBaseChr6045 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CBaseChr6045::CJumpReadyStatusObserver);
#endif /* _DEBUG */
};


/*virtual*/ CBaseHunChr::~CBaseHunChr(void)
{
    ;
};


/*virtual*/ bool CBaseHunChr::Initialize(PARAMETER* pParameter, bool bReplaceParameter) /*override*/
{
    bool bResult = CBaseChr6045::Initialize(pParameter, bReplaceParameter);
    if (bResult)
    {
        m_modelColorCtrl.Initialize(&Compositor());
        m_modelColorCtrl.SetModelColorControlEnable(true);
    };

    return bResult;
};


/*virtual*/ void CBaseHunChr::Run(void) /*override*/
{
    CBaseChr6045::Run();
    m_modelColorCtrl.Period();
};


/*virtual*/ bool CBaseHunChr::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS statusNow = GetStatus();
    if ((statusNow == BASEHUN::STATUS_BURST_FLYAWAY) ||
        (statusNow == BASEHUN::STATUS_BURST_FLYAWAY_BOUND) ||
        (statusNow == BASEHUN::STATUS_ATTACK_F))
        return false;

    return CBaseChr6045::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ void CBaseHunChr::OnMessageTouchdown(float fHeight) /*override*/
{
    CBaseChr6045::OnMessageTouchdown(fHeight);

    if (GetStatus() == BASEHUN::STATUS_BURST_FLYAWAY)
        SetStatus(BASEHUN::STATUS_BURST_FLYAWAY_BOUND);    
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE CBaseHunChr::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    CHARACTERTYPES::ATTACKRESULTTYPE attackResult = CBaseChr6045::OnDamage(rCalc);

    bool bIsNotInRevengeMode = (!m_bRevengeModeActive);
    bool bIsNotInBurstMode = (!m_bBurstModeActie);

    bool bIsAttackEffective = (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE);

    bool bIsAttackNotGuarded = (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD) &&
                               (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT);

    if (bIsNotInBurstMode   &&
        bIsNotInRevengeMode &&
        bIsAttackEffective  &&
        bIsAttackNotGuarded)
    {
		int32 dmgAmount = static_cast<int32>(rCalc.CalcDamage(attackResult));
        AddRevengeDamage(dmgAmount);
	};

	return attackResult;
};


void CBaseHunChr::InitRevengeMode(int32 damageAmountForRevenge)
{
    m_revengeDamageMax = damageAmountForRevenge;
    ClearRevengeMode();
};


void CBaseHunChr::SetRevengeMode(void)
{
    m_bRevengeModeActive = true;
    m_revengeDamageHeap = m_revengeDamageMax;
    SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


void CBaseHunChr::ClearRevengeMode(void)
{
    m_bRevengeModeActive = false;
    m_revengeDamageHeap = 0;
    ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    CalcBodyColor();
};


void CBaseHunChr::AddRevengeDamage(int32 amount)
{
    m_revengeDamageHeap += amount;
    if (m_revengeDamageHeap >= m_revengeDamageMax)
        SetRevengeMode();
    CalcBodyColor();
};


void CBaseHunChr::SetBurstMode(void)
{
    m_bBurstModeActie = true;
    SetFlag(ENEMYTYPES::FLAG_NOREACTION);
    CalcBodyColor();
    AIModerator().SetCatchTriggerAction(BASEHUN_AI::CATCHTRIGGER_BURST);
};


void CBaseHunChr::ClearBurstMode(void)
{
    m_bBurstModeActie = false;
    ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    CalcBodyColor();
};


void CBaseHunChr::CalcBodyColor(void)
{
    if (m_bBurstModeActie)
    {
        m_modelColorCtrl.SetModelColorChange(255.0f, 0.0f, 0.0f, 0.5f);
    }
    else
    {
        float fRevengeDamageRatio = static_cast<float>(m_revengeDamageHeap) / static_cast<float>(m_revengeDamageMax);
        float fColorChangeRate = 1.0f;

        if (fRevengeDamageRatio < 0.3f)
        {
            fColorChangeRate = 0.0f;
        }
        else if (fRevengeDamageRatio < 0.6f)
        {
            fColorChangeRate = 0.25f;
        }
        else if (fRevengeDamageRatio < 1.0f)
        {
            fColorChangeRate = 0.6f;
        };

        float r, g, b;
        r = g = b = 0.0f;
        m_modelColorCtrl.GetBaseModelColor(&r, &g, &b);

        r += ((255.0f - r) * fColorChangeRate);
        g += ((128.0f - g) * fColorChangeRate);
        b += (-b * fColorChangeRate);

        m_modelColorCtrl.SetModelColorChange(r, g, b, 0.5f);
    };
};


bool CBaseHunChr::IsInBurstMode(void) const
{
    return m_bBurstModeActie;
};


bool CBaseHunChr::IsInRevengeMode(void) const
{
    return m_bRevengeModeActive;
};


void CBaseHunChr::AttachThrowStatusObserver(void)
{
    m_pThrow = new CBaseHunChr::CThrowStatusObserver;

    RwV3d vecBone = { 0.0f, 0.45f, 0.1f };
    m_pThrow->SetTraceBoneData(3, &vecBone);

    RwV3d vecThrow = { 0.0f, 2.0f, -18.0f };
    m_pThrow->SetThrowVector(&vecThrow);

    AttachStatusObserver(ENEMYTYPES::STATUS_THROW, m_pThrow);
};