#include "BaseRazorfistChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseRazorfistAI.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Sound/GameSound.hpp"


CBaseRazorfistChr::CHideStatusObserver::CHideStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_APPEAR)
{
    ;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseRazorfistChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CBaseRazorfistChr::CThinkingStatusObserver::RESULT
CBaseRazorfistChr::CThinkingStatusObserver::Observing(void) /*override*/
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
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASERAZORFIST::STATUS_ATTACK_COUNTER);
                break;

            case BASERAZORFIST_AI::ORDERTYPE_ATTACK_A:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case BASERAZORFIST_AI::ORDERTYPE_ATTACK_B:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case BASERAZORFIST_AI::ORDERTYPE_ATTACK_C:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
                break;

            case BASERAZORFIST_AI::ORDERTYPE_ATTACK_D:
                m_nextStatus = static_cast<ENEMYTYPES::STATUS>(BASERAZORFIST::STATUS_ATTACK_D);
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


/*virtual*/ ENEMYTYPES::STATUS CBaseRazorfistChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


CBaseRazorfistChr::CAttackAStatusObserver::CAttackAStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttack(ENEMYTYPES::MOTIONNAMES::ATTACK_A)
{
    ;
};


//
// *********************************************************************************
//


CBaseRazorfistChr::CAttackBStatusObserver::CAttackBStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseRazorfistChr::CAttackCStatusObserver::CAttackCStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_C)
{

};


//
// *********************************************************************************
//


CBaseRazorfistChr::CAttackDStatusObserver::CAttackDStatusObserver(void)
: CBaseChr6045::CAttackSalivaShotStatusObserver(BASERAZORFIST::MOTIONNAMES::ATTACK_D)
{
    m_eMagicID = MAGICID::ID_TBSHOT;
    m_iSEID = SDCODE_SE(4315);
    m_iBoneID = 4;
    m_vecOffset = { 0.3f, 0.0f, 0.1f };
    m_fAimArea = MATH_DEG2RAD(18.0f);
    m_fScale = 0.9f;
    m_iAimRateFreqTableNo = CEnemyParameter::FREQUENCY_FIRE_CONVERGENCE;
    m_bPlayerAimFlag = true;
};


//
// *********************************************************************************
//


CBaseRazorfistChr::CAttackCounterStatusObserver::CAttackCounterStatusObserver(CBaseRazorfistChr* pBaseRazorfistChr)
: m_pBaseRazorfistChr(pBaseRazorfistChr)
{
    ;
};


/*virtual*/ void CBaseRazorfistChr::CAttackCounterStatusObserver::OnStart(void) /*override*/
{
    CAttackAStatusObserver::OnStart();

    if (m_pBaseRazorfistChr->IsKnockBackCtrlActive())
        m_pBaseRazorfistChr->SetKnockBackCtrlEnable(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseRazorfistChr::CAttackCounterStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);

    if (m_pBaseRazorfistChr->IsKnockBackCtrlActive())
        m_pBaseRazorfistChr->SetKnockBackCtrlEnable(true);

    return CAttackAStatusObserver::OnEnd();
};


//
// *********************************************************************************
//


CBaseRazorfistChr::CGuardStatusObserver::CGuardStatusObserver(void)
{
    float fGuardTime = 0.5f;
    float fGuardTotalTime = 1.5f;
    SetParameter(fGuardTime, fGuardTotalTime);
};


/*virtual*/ CBaseRazorfistChr::CGuardStatusObserver::RESULT
CBaseRazorfistChr::CGuardStatusObserver::PassageGuardTotalTime(void) /*override*/
{
    if (EnemyChr().IsAIModeratorExist())
    {
        uint8 freq = EnemyChr().AIModerator().AIFreqParam().GetFrequency(CEnemyParameter::FREQUENCY_COMMON_2);
        uint8 rand = static_cast<uint8>(Math::RandRange(0, 100));

        if (freq > rand)
        {
            static_cast<CBaseAI6045&>(EnemyChr().AIModerator()).SetCatchTriggerAction(BASERAZORFIST_AI::CATCHTRIGGER_COUNTER);
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


/*virtual*/ ENEMYTYPES::STATUS CBaseRazorfistChr::CJumpReadyStatusObserver::OnEnd(void) /*override*/
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


CBaseRazorfistChr::CDeathStatusObserver::CDeathStatusObserver(CBaseRazorfistChr* pBaseKingNailChr)
{
    ASSERT(pBaseKingNailChr != nullptr);

    if (pBaseKingNailChr->GetID() == ENEMYID::ID_RAZORFIST)
    {
        if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
            CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
            SetParameter(DEATHKIND_NEXUS, nullptr, 1.0f);
        else if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M04NB))
            SetParameter(DEATHKIND_MOTION, nullptr, 1.0f);
        else
            SetParameter(DEATHKIND_MONSTER, nullptr, 1.25f);
    }
    else
    {
        SetParameter(DEATHKIND_MONSTER, nullptr, 1.25f);
    };
};


//
// *********************************************************************************
//


CBaseRazorfistChr::CBaseRazorfistChr(ENEMYID::VALUE enemyId)
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
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    
    /* CBaseRazorfistChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseRazorfistChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new CBaseRazorfistChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new CBaseRazorfistChr::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new CBaseRazorfistChr::CAttackBStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new CBaseRazorfistChr::CAttackCStatusObserver);
    AttachStatusObserver(BASERAZORFIST::STATUS_ATTACK_D,            new CBaseRazorfistChr::CAttackDStatusObserver);
    AttachStatusObserver(BASERAZORFIST::STATUS_ATTACK_COUNTER,      new CBaseRazorfistChr::CAttackCounterStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseRazorfistChr::CDeathStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CBaseRazorfistChr::CJumpReadyStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new CBaseRazorfistChr::CGuardStatusObserver);
};


/*virtual*/ CBaseRazorfistChr::~CBaseRazorfistChr(void)
{
    ;
};


/*virtual*/ bool CBaseRazorfistChr::Initialize(PARAMETER* pParameter, bool bReplaceParameter) /*override*/
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