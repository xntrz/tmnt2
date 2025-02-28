#include "BaseChr6045.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Sound/GameSound.hpp"


CBaseChr6045::CJumpActionStatusObserver::CJumpActionStatusObserver(void)
: m_step(STEP_JUMP_READY)
, m_fJumpInitSpeed(0.0f)
, m_pszMotionJumpReady(nullptr)
, m_pszMotionJump(nullptr)
, m_pszMotionTouchdown(nullptr)
{
    ;
};


/*virtual*/ void CBaseChr6045::CJumpActionStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(m_pszMotionJumpReady, bForce);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    m_step = STEP_JUMP_READY;
};


/*virtual*/ CBaseChr6045::CJumpActionStatusObserver::RESULT
CBaseChr6045::CJumpActionStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case STEP_JUMP_READY:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                bool bForce = true;
                EnemyChr().Compositor().ChangeMotion(m_pszMotionJump, bForce);

                SetJumpSetting();

                m_step = STEP_JUMP;
            };
        }
        break;

    case STEP_JUMP:
        {
            CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), m_fJumpInitSpeed);

            RwV3d vecVelocity = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetVelocity(&vecVelocity);

            const CCharacterCompositor::COLLISIONGROUNDINFO* pGroundInfo = EnemyChr().Compositor().GetCollisionGround();
            ASSERT(pGroundInfo != nullptr);

            if ((vecVelocity.y <= 0.0f) && pGroundInfo->m_bHit)
            {
                bool bForce = true;
                EnemyChr().Compositor().ChangeMotion(m_pszMotionTouchdown, bForce);

                SetTouchdownSetting();

                m_step = STEP_TOUCHDOWN;
            };
        }
        break;

    case STEP_TOUCHDOWN:
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


/*virtual*/ ENEMYTYPES::STATUS CBaseChr6045::CJumpActionStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ void CBaseChr6045::CJumpActionStatusObserver::SetJumpSetting(void)
{
    RwV3d vecVelocity = { 0.0f, m_fJumpInitSpeed, 0.0f };
    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


/*virtual*/ void CBaseChr6045::CJumpActionStatusObserver::SetTouchdownSetting(void)
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
};


void CBaseChr6045::CJumpActionStatusObserver::SetParameter(const char* pszMotionJumpReady,
                                                           const char* pszMotionJump,
                                                           const char* pszMotionTouchdown,
                                                           float fJumpInitSpeed)
{
    m_pszMotionJumpReady = pszMotionJumpReady;
    m_pszMotionJump      = pszMotionJump;
    m_pszMotionTouchdown = pszMotionTouchdown;
    m_fJumpInitSpeed     = fJumpInitSpeed;

    ASSERT(m_pszMotionJumpReady != nullptr);
    ASSERT(m_pszMotionJump != nullptr);
    ASSERT(m_pszMotionTouchdown != nullptr);
};


//
// *********************************************************************************
//


/*virtual*/ CBaseChr6045::CKnockForGuardPriorityStatusObserver::RESULT
CBaseChr6045::CKnockForGuardPriorityStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_DEFENCE)
        return RESULT_END;

    return CCommonEnemyObserver::CKnockStatus::Observing();
};


/*virtual*/ ENEMYTYPES::STATUS CBaseChr6045::CKnockForGuardPriorityStatusObserver::OnEnd(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_DEFENCE)
        return CCommonEnemyObserver::CKnockStatus::OnEnd();

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ ENEMYTYPES::STATUS CBaseChr6045::CJumpReadyStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);
    vecVelocity.y = 0.0f;

    float fMoveSpeed = Math::Vec3_Length(&vecVelocity);
    if (Math::FEqual(fMoveSpeed, 0.0f))
    {
        float fAerialMoveSpeed = EnemyChr().Feature().m_fAerialMoveSpeed;

        RwV3d vecVelocityXZ = { 0.0f, 0.0f, fAerialMoveSpeed };
        EnemyChr().Compositor().RotateVectorByDirection(&vecVelocityXZ, &vecVelocityXZ);

        Math::Vec3_Add(&vecVelocity, &vecVelocity, &vecVelocityXZ);
    };

    float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
    vecVelocity.y = fJumpInitSpeed;

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


CBaseChr6045::CGuardStatusObserver::CGuardStatusObserver(void)
: m_step(STEP_GUARD_BEGIN)
, m_fGuardTime(0.0f)
, m_fGuardTotalTime(0.0f)
, m_fTimer(0.0f)
, m_fTotalTimer(0.0f)
, m_bCounterActionFlag(false)
{
    ;
};


/*virtual*/ void CBaseChr6045::CGuardStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_1, bForce);

    m_fTimer = 0.0f;
    m_fTotalTimer = 0.0f;
    m_bCounterActionFlag = false;
    m_step = STEP_GUARD_BEGIN;
};


/*virtual*/ CBaseChr6045::CGuardStatusObserver::RESULT
CBaseChr6045::CGuardStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case STEP_GUARD_BEGIN:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                bool bForce = true;
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_2, bForce);

                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

                m_step = STEP_GUARD;
            };
        }
        break;

    case STEP_GUARD:
        {
            if (EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_DEFENCE)
            {
                m_fTimer = 0.0f;
            }
            else if (m_bCounterActionFlag)
            {
                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
                return RESULT_END;
            };

            if (m_fTimer < m_fGuardTime)
            {
                m_fTimer += CGameProperty::GetElapsedTime();
            }
            else
            {
                bool bForce = true;
                EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_3, bForce);

                m_step = STEP_GUARD_END;
            };
        }
        break;

    case STEP_GUARD_END:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    if (m_fTotalTimer >= m_fGuardTotalTime)
        return PassageGuardTotalTime();

    m_fTotalTimer += CGameProperty::GetElapsedTime();
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseChr6045::CGuardStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ CBaseChr6045::CGuardStatusObserver::RESULT
CBaseChr6045::CGuardStatusObserver::PassageGuardTotalTime(void)
{
    return RESULT_CONTINUE;
};


void CBaseChr6045::CGuardStatusObserver::SetParameter(float fGuardTime, float fGuardTotalTime)
{
    m_fGuardTime = fGuardTime;
    m_fGuardTotalTime = fGuardTotalTime;
};


//
// *********************************************************************************
//


CBaseChr6045::COneShotMotionForAttackPlayerAim::COneShotMotionForAttackPlayerAim(const char* pszMotion)
: m_pszMotion(pszMotion)
, m_bMotionEnd(false)
, m_orderTargetNo(-1)
{
    ASSERT(m_pszMotion != nullptr);
};


/*virtual*/ void CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart(void) /*override*/
{
    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(m_pszMotion, bForce);

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    m_orderTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());

    m_bMotionEnd = false;
};


/*virtual*/ CBaseChr6045::COneShotMotionForAttackPlayerAim::RESULT
CBaseChr6045::COneShotMotionForAttackPlayerAim::Observing(void) /*override*/
{
    float fRotRate = EnemyChr().Feature().m_fRotateRate;
    CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(), m_orderTargetNo, fRotRate, MATH_DEG2RAD(3.0f));

    if (!EnemyChr().Compositor().IsMotionEnd())
        return RESULT_CONTINUE;

    m_bMotionEnd = true;
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseChr6045::COneShotMotionForAttackPlayerAim::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseChr6045::COneShotMotionForAttack::COneShotMotionForAttack(const char* pszMotion)
: m_pszMotion(pszMotion)
, m_bMotionEnd(false)
{
    ASSERT(m_pszMotion != nullptr);
};


/*virtual*/ void CBaseChr6045::COneShotMotionForAttack::OnStart(void) /*override*/
{
    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(m_pszMotion, bForce);

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());

    m_bMotionEnd = false;
};


/*virtual*/ CBaseChr6045::COneShotMotionForAttack::RESULT
CBaseChr6045::COneShotMotionForAttack::Observing(void) /*override*/
{
    if (!EnemyChr().Compositor().IsMotionEnd())
        return RESULT_CONTINUE;

    m_bMotionEnd = true;
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseChr6045::COneShotMotionForAttack::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseChr6045::CDeathStatusObserver::CDeathStatusObserver(void)
: CDeathStatusObserver(nullptr, DEATHKIND_NONE, 0.0f)
{
    ;
};


CBaseChr6045::CDeathStatusObserver::CDeathStatusObserver(const char* pszMotion,
                                                         DEATHKIND deathKind,
                                                         float fEffectScale)
: m_pszMotion(pszMotion)
, m_deathKind(deathKind)
, m_fEffectScale(fEffectScale)
{
    ;
};


/*virtual*/ void CBaseChr6045::CDeathStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    if (m_pszMotion)
    {
        bool bForce = true;
        EnemyChr().Compositor().ChangeMotion(m_pszMotion, bForce);
    };

    if (m_deathKind == DEATHKIND_MOTION)
    {
        bool bForce = true;
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, bForce);
        EnemyChr().Compositor().StopMotion();
        EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
    };
};


/*virtual*/ CBaseChr6045::CDeathStatusObserver::RESULT
CBaseChr6045::CDeathStatusObserver::Observing(void) /*override*/
{
    if (m_deathKind == DEATHKIND_MOTION)
        return RESULT_CONTINUE;

    if (m_pszMotion)
    {
        if (!EnemyChr().Compositor().IsMotionEnd())
            return RESULT_CONTINUE;
    };

    EntryDeathEffect();

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseChr6045::CDeathStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_QUIT;
};


void CBaseChr6045::CDeathStatusObserver::EntryDeathEffect(void)
{
    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

    switch (m_deathKind)
    {
    case DEATHKIND_NONE:
        break;

    case DEATHKIND_NORMAL:
        EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_NORMAL, &vecBodyPos, m_fEffectScale);
        break;

    case DEATHKIND_MACHINE:
        EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_MACHINE, &vecBodyPos, m_fEffectScale);
        break;
        
    case DEATHKIND_MONSTER:
        EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_MONSTER, &vecBodyPos, m_fEffectScale);
        break;

    case DEATHKIND_NEXUS:
        CEnemyUtils::EntryNexusDeathEffect(&EnemyChr(), -1.0f);
        break;

    default:
        ASSERT(false);
        break;
    };    
};


void CBaseChr6045::CDeathStatusObserver::SetParameter(DEATHKIND deathKind, const char* pszMotion, float fEffectScale)
{
    m_deathKind = deathKind;
    m_pszMotion = pszMotion;
    m_fEffectScale = fEffectScale;
};


//
// *********************************************************************************
//


CBaseChr6045::CNothingStatusObserver::CNothingStatusObserver(const char* pszMotion, ENEMYTYPES::STATUS nextStatus)
: m_pszMotion(pszMotion)
, m_nextStatus(nextStatus)
{
    ;
};


/*virtual*/ void CBaseChr6045::CNothingStatusObserver::OnStart(void) /*override*/
{
    if (m_pszMotion)
    {
        bool bForce = true;
        EnemyChr().Compositor().ChangeMotion(m_pszMotion, bForce);
    };
};


/*virtual*/ CBaseChr6045::CNothingStatusObserver::RESULT
CBaseChr6045::CNothingStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseChr6045::CNothingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


CBaseChr6045::CBaseChr6045(ENEMYID::VALUE enemyId)
: CEnemyCharacter(enemyId)
, m_pThrowObserver(nullptr)
, m_knockBackCtrl()
, m_bKnockBackCtrlActive(false)
, m_pAIModerator(nullptr)
{
    m_knockBackCtrl.Initialize(this, -1, -1.0f);
    m_knockBackCtrl.SetKnockBackControlEnable(false);
};


/*virtual*/ CBaseChr6045::~CBaseChr6045(void)
{
    ;
};


/*virtual*/ bool CBaseChr6045::Initialize(PARAMETER* pParameter, bool bReplaceParameter) /*override*/
{
    bool bResult = CEnemyCharacter::Initialize(pParameter, bReplaceParameter);
    if (pParameter->m_pfnAIInstance)
    {
        CAIModerator& AIModerator = CEnemyCharacter::AIModerator();
        m_pAIModerator = &((CBaseAI6045&)AIModerator);
    };
	
	return bResult;
};


/*virtual*/ void CBaseChr6045::Run(void) /*override*/
{
    CEnemyCharacter::Run();
    m_knockBackCtrl.Period();
};


/*virtual*/ void CBaseChr6045::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);

    if (m_pThrowObserver)
        m_pThrowObserver->SetAttackParam(pCatch);
};


/*virtual*/ void CBaseChr6045::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    CEnemyCharacter::OnMessageReceivedDamage(iAmount);
    m_knockBackCtrl.ReceivedDamage(iAmount);
};


CBaseAI6045& CBaseChr6045::AIModerator(void)
{
    ASSERT(m_pAIModerator != nullptr);
    return *m_pAIModerator;
};


void CBaseChr6045::SetKnockBackCtrlEnable(bool bEnable)
{
    m_knockBackCtrl.SetKnockBackControlEnable(bEnable);
};


bool CBaseChr6045::IsKnockBackCtrlActive(void) const
{
    return m_bKnockBackCtrlActive;
};