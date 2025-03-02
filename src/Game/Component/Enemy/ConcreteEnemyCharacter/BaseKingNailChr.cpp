#include "BaseKingNailChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseKingNailAI.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Sound/GameSound.hpp"


CBaseKingNailChr::CHideStatusObserver::CHideStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_APPEAR)
{
    ;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseKingNailChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CBaseKingNailChr::CThinkingStatusObserver::RESULT
CBaseKingNailChr::CThinkingStatusObserver::Observing(void) /*override*/
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
            };
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case BASEAI6045::ORDERTYPE_ATTACK_COUNTER:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASEKINGNAIL::STATUS_ATTACK_COUNTER);
                break;

            case BASEKINGNAIL_AI::ORDERTYPE_ATTACK_A:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case BASEKINGNAIL_AI::ORDERTYPE_ATTACK_B:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case BASEKINGNAIL_AI::ORDERTYPE_ATTACK_C:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
                break;

            case BASEKINGNAIL_AI::ORDERTYPE_ATTACK_D:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASEKINGNAIL::STATUS_ATTACK_D);
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


/*virtual*/ ENEMYTYPES::STATUS CBaseKingNailChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


CBaseKingNailChr::CAttackAStatusObserver::CAttackAStatusObserver(void)
: CBaseChr6045::CAttackSalivaShotStatusObserver(ENEMYTYPES::MOTIONNAMES::ATTACK_A)
{
    m_eMagicID = MAGICID::ID_TBSHOT;
    m_iSEID = SDCODE_SE(4315);
    m_iBoneID = 4;
    m_vecOffset = { 1.0f, 0.0f, -0.2f };
    m_fAimArea = MATH_DEG2RAD(36.0f);
    m_fScale = 1.25f;
    m_iAimRateFreqTableNo = CEnemyParameter::FREQUENCY_FIRE_CONVERGENCE;
    m_bPlayerAimFlag = true;
};


//
// *********************************************************************************
//


CBaseKingNailChr::CAttackBStatusObserver::CAttackBStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
, m_bRotateEnableFlag(false)
{
    ;
};


/*virtual*/ void CBaseKingNailChr::CAttackBStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart();
    m_bRotateEnableFlag = true;
};


/*virtual*/ CBaseKingNailChr::CAttackBStatusObserver::RESULT
CBaseKingNailChr::CAttackBStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        m_bRotateEnableFlag = !m_bRotateEnableFlag;

    if (m_bRotateEnableFlag)
        return CBaseChr6045::COneShotMotionForAttackPlayerAim::Observing();

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


CBaseKingNailChr::CAttackCStatusObserver::CAttackCStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_C)
{

};


//
// *********************************************************************************
//


CBaseKingNailChr::CAttackDStatusObserver::CAttackDStatusObserver(CBaseKingNailChr* pBaseKingNailChr)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(BASEKINGNAIL::MOTIONNAMES::ATTACK_D1)
, m_pBaseKingNailChr(pBaseKingNailChr)
, m_iStep(0)
, m_fTimer(0.0f)
, m_fSpinTime(0.0f)
{
    ;
};


/*virtual*/ void CBaseKingNailChr::CAttackDStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart();

    m_fSpinTime = 2.0f;
    m_iStep = 0;
};


/*virtual*/ CBaseKingNailChr::CAttackDStatusObserver::RESULT
CBaseKingNailChr::CAttackDStatusObserver::Observing(void) /*override*/
{
    switch (m_iStep)
    {
    case 0:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().ChangeMotion(BASEKINGNAIL::MOTIONNAMES::ATTACK_D2, true);
                ++m_iStep;
            };
        }
        break;

    case 1:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().ChangeMotion(BASEKINGNAIL::MOTIONNAMES::ATTACK_D3, true);
                ++m_iStep;
                m_fTimer = 0.0f;
            };
        }
        break;

    case 2:
        {
            if (m_fTimer < m_fSpinTime)
            {
                float fRotRate = EnemyChr().Feature().m_fRotateRate;
                CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(), m_orderTargetNo, fRotRate, MATH_DEG2RAD(3.0f));

                float fWalkMoveSpeed = EnemyChr().Feature().m_fWalkMoveSpeed;
                CEnemyUtils6045::ReplaceVelocityToNowDirection(&EnemyChr().Compositor(), fWalkMoveSpeed);

                m_fTimer += CGameProperty::GetElapsedTime();
            }
            else
            {
                EnemyChr().Compositor().ChangeMotion(BASEKINGNAIL::MOTIONNAMES::ATTACK_D4, true);
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);                
                EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

                ++m_iStep;
                m_bMotionEnd = true;
            };
        }
        break;

    case 3:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKingNailChr::CAttackDStatusObserver::OnEnd(void) /*override*/
{
    if (m_bMotionEnd)
        m_pBaseKingNailChr->AIModerator().DecisionUnitCommonParameter().SetAttackWaitTime(0.5f);

    return CBaseChr6045::COneShotMotionForAttackPlayerAim::OnEnd();
};


//
// *********************************************************************************
//


CBaseKingNailChr::CAttackCounterStatusObserver::CAttackCounterStatusObserver(CBaseKingNailChr* pBaseKingNailChr)
: CAttackDStatusObserver(pBaseKingNailChr)
{
    ;
};


/*virtual*/ void CBaseKingNailChr::CAttackCounterStatusObserver::OnStart(void) /*override*/
{
    CAttackDStatusObserver::OnStart();

    m_fSpinTime = 0.5f;

    if (m_pBaseKingNailChr->IsKnockBackCtrlActive())
        m_pBaseKingNailChr->SetKnockBackCtrlEnable(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKingNailChr::CAttackCounterStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);

    if (m_pBaseKingNailChr->IsKnockBackCtrlActive())
        m_pBaseKingNailChr->SetKnockBackCtrlEnable(true);

    return CAttackDStatusObserver::OnEnd();
};


//
// *********************************************************************************
//


CBaseKingNailChr::CGuardStatusObserver::CGuardStatusObserver(void)
{
    float fGuardTime = 0.5f;
    float fGuardTotalTime = 1.5f;
    SetParameter(fGuardTime, fGuardTotalTime);
};


/*virtual*/ CBaseKingNailChr::CGuardStatusObserver::RESULT
CBaseKingNailChr::CGuardStatusObserver::PassageGuardTotalTime(void) /*override*/
{
    if (EnemyChr().IsAIModeratorExist())
    {
        uint8 freq = EnemyChr().AIModerator().AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_COMMON_2);
        uint8 rand = static_cast<uint8>(Math::RandRange(0, 100));

        if (freq > rand)
        {
            static_cast<CBaseAI6045&>(EnemyChr().AIModerator()).SetCatchTriggerAction(BASEKINGNAIL_AI::CATCHTRIGGER_COUNTER);
            m_bCounterActionFlag = true;
            return RESULT_END;
        };
    };

    m_fTotalTimer = 0.0f;
    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


/*virtual*/ ENEMYTYPES::STATUS CBaseKingNailChr::CJumpReadyStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);
    vecVelocity.y = 0.0f;

    float fMoveSpeed = Math::Vec3_Length(&vecVelocity);
    if (fMoveSpeed < 0.1f)
    {
        float fWalkMoveSpeed = EnemyChr().Feature().m_fWalkMoveSpeed;

        vecVelocity = { 0.0f, 0.0f, fWalkMoveSpeed };
        EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    };

    float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
    vecVelocity.y = fJumpInitSpeed;

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


CBaseKingNailChr::CDeathStatusObserver::CDeathStatusObserver(CBaseKingNailChr* pBaseKingNailChr)
{
    ASSERT(pBaseKingNailChr != nullptr);

    if (pBaseKingNailChr->GetID() == ENEMYID::ID_KING_NAIL)
    {
        if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
            CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
            SetParameter(DEATHKIND_NEXUS, nullptr, 1.0f);
        else if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M05N))
            SetParameter(DEATHKIND_MOTION, nullptr, 1.0f);
        else
            SetParameter(DEATHKIND_MONSTER, nullptr, 2.0f);
    }
    else
    {
        SetParameter(DEATHKIND_MONSTER, nullptr, 2.0f);
    };
};


//
// *********************************************************************************
//


CBaseKingNailChr::CBaseKingNailChr(ENEMYID::VALUE enemyId)
: CBaseChr6045(enemyId)
{
    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                    new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,             new CCommonEnemyObserver::CCounteractStatusObserver);    
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                   new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_SLEEP,                  new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    
    /* CBaseKingNailChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseKingNailChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new CBaseKingNailChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new CBaseKingNailChr::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new CBaseKingNailChr::CAttackBStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new CBaseKingNailChr::CAttackCStatusObserver);
    AttachStatusObserver(BASEKINGNAIL::STATUS_ATTACK_D,             new CBaseKingNailChr::CAttackDStatusObserver(this));
    AttachStatusObserver(BASEKINGNAIL::STATUS_ATTACK_COUNTER,       new CBaseKingNailChr::CAttackCounterStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseKingNailChr::CDeathStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CBaseKingNailChr::CJumpReadyStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new CBaseKingNailChr::CGuardStatusObserver);
};


/*virtual*/ CBaseKingNailChr::~CBaseKingNailChr(void)
{
    ;
};


/*virtual*/ bool CBaseKingNailChr::Initialize(PARAMETER* pParameter, bool bReplaceParameter) /*override*/
{
    bool bResult = CBaseChr6045::Initialize(pParameter, bReplaceParameter);

    DetachStatusObserver(ENEMYTYPES::STATUS_APPEAR);

    uint8 appearType = FrequencyParameter(CEnemyParameter::FREQUENCY_COMMON_1);
    switch (appearType)
    {
    case ENEMYTYPES::APPEARTYPE_WALK_SLOW:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(4.5f));
        break;

    case ENEMYTYPES::APPEARTYPE_WALK_FAST:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CWalkingAppearStatus(3.0f));
        break;

    case ENEMYTYPES::APPEARTYPE_FALL:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CFallAppearStatus);
        break;

    case ENEMYTYPES::APPEARTYPE_NONE:
    case ENEMYTYPES::APPEARTYPE_FLY_UP:
    case ENEMYTYPES::APPEARTYPE_FLY_DOWN:
    case ENEMYTYPES::APPEARTYPE_MOTION:
        AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR, new CCommonEnemyObserver::CNoneAppearStatus);
        break;

    default:
        break;
    };

    return bResult;
};