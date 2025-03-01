#include "BaseStoneBiterChr.hpp"

#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Enemy/EnemyWatcher.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseStoneBiterAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Sound/GameSound.hpp"


CBaseStoneBiterChr::CHideStatusObserver::CHideStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_APPEAR)
{
    ;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseStoneBiterChr::CAppearStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::TOJO, bForce);
};


/*virtual*/ CBaseStoneBiterChr::CAppearStatusObserver::RESULT
CBaseStoneBiterChr::CAppearStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseStoneBiterChr::CAppearStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseStoneBiterChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ CBaseStoneBiterChr::CThinkingStatusObserver::RESULT
CBaseStoneBiterChr::CThinkingStatusObserver::Observing(void) /*override*/
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
            else
            {
                m_nextStatus = ENEMYTYPES::STATUS_WALK_RIGHT;
            };
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case BASESTONEBITER_AI::ORDERTYPE_ATTACK_A:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case BASESTONEBITER_AI::ORDERTYPE_ATTACK_B:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case BASESTONEBITER_AI::ORDERTYPE_ATTACK_C:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
                break;

            case BASESTONEBITER_AI::ORDERTYPE_ATTACK_RUN:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_RUN;
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
        ASSERT(false);
        break;
    };

    return result;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseStoneBiterChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseStoneBiterChr::CSideMoveStatusObserver::OnStart(void) /*override*/
{
    m_moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    m_fMoveTime = EnemyChr().AIThinkOrder().OrderMove().m_fMoveTimer;

    float fWalkMoveSpeed = EnemyChr().Feature().m_fWalkMoveSpeed;
    m_fMoveSpeed = (fWalkMoveSpeed * 0.6f);

    const char* pszMotion = nullptr;

    switch (m_moveType)
    {
    case BASESTONEBITER_AI::ORDERTYPE_MOVE_SIDE_RIGHT:
        pszMotion = ENEMYTYPES::MOTIONNAMES::WALKR;
        m_fMoveAngle = -MATH_DEG2RAD(90.0f);
        break;

    case BASESTONEBITER_AI::ORDERTYPE_MOVE_SIDE_LEFT:
        pszMotion = ENEMYTYPES::MOTIONNAMES::WALKL;
        m_fMoveAngle = MATH_DEG2RAD(90.0f);
        break;

    default:
        ASSERT(false);
        break;
    };

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(pszMotion, bForce);
};


/*virtual*/ CBaseStoneBiterChr::CSideMoveStatusObserver::RESULT
CBaseStoneBiterChr::CSideMoveStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_MOVE)
        return RESULT_END;
    
    if (EnemyChr().AIThinkOrder().OrderMove().m_iMoveType != m_moveType)
        return RESULT_END;

    m_fMoveTime -= CGameProperty::GetElapsedTime();
    if (m_fMoveTime < 0.0f)
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    int32 iTargetNo = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iTargetNo);
    if (pPlayerChr)
    {
        RwV3d vecFootPos = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPos);

        float fRotRate = EnemyChr().Feature().m_fRotateRate;

        CEnemyUtils6045::RotateToTarget(&EnemyChr().Compositor(), &vecFootPos, fRotRate);
    };

    RwV3d vecNewVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vecNewVelocity, &vecNewVelocity);

    RwMatrix matRotY;
    RwMatrixSetIdentityMacro(&matRotY);
    Math::Matrix_RotateY(&matRotY, m_fMoveAngle);

    RwV3dTransformVector(&vecNewVelocity, &vecNewVelocity, &matRotY);

    RwV3d vecOldVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecOldVelocity);
    vecNewVelocity.y = vecOldVelocity.y;

    EnemyChr().Compositor().SetVelocity(&vecNewVelocity);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseStoneBiterChr::CSideMoveStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseStoneBiterChr::CAttackAStatusObserver::CAttackAStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_A)
{
    ;
};


//
// *********************************************************************************
//


CBaseStoneBiterChr::CAttackBStatusObserver::CAttackBStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
{
    ;
};


//
// *********************************************************************************
//


CBaseStoneBiterChr::CAttackCStatusObserver::CAttackCStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_C)
{
    ;
};


//
// *********************************************************************************
//


CBaseStoneBiterChr::CAttackRunStatusObserver::CAttackRunStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_RUN)
{
    ;
};


/*virtual*/ void CBaseStoneBiterChr::CAttackRunStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart();

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4334));
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseStoneBiterChr::CDeathStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, bForce);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ CBaseStoneBiterChr::CDeathStatusObserver::RESULT
CBaseStoneBiterChr::CDeathStatusObserver::Observing(void) /*override*/
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_MONSTER, &vecPos, 1.0f);

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseStoneBiterChr::CDeathStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


DEFINE_EMPTY_NEEDED_EFFECTS_FOR(CBaseStoneBiterChr);


CBaseStoneBiterChr::CBaseStoneBiterChr(ENEMYID::VALUE enemyId)
: CEnemyCharacter(enemyId)
{
    /* TODO: strange guard impl that is not working, it should use CKnockForGuardPriorityStatusObserver + CGuardStatusObserver
             from BaseChr6045 for proper guard */

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                    new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new CCommonEnemyObserver::CGuardStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,             new CCommonEnemyObserver::CCounteractStatusObserver);    
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                   new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_SLEEP,                  new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CCommonEnemyObserver::CGetupStatus);

    /* CBaseChr6045 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CBaseChr6045::CJumpReadyStatusObserver);

    /* CBaseMouserChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseStoneBiterChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new CBaseStoneBiterChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK_RIGHT,             new CBaseStoneBiterChr::CSideMoveStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new CBaseStoneBiterChr::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new CBaseStoneBiterChr::CAttackBStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new CBaseStoneBiterChr::CAttackCStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_RUN,             new CBaseStoneBiterChr::CAttackRunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseStoneBiterChr::CDeathStatusObserver);
};


/*virtual*/ CBaseStoneBiterChr::~CBaseStoneBiterChr(void)
{
    ;
};


/*virtual*/ void CBaseStoneBiterChr::Run(void) /*override*/
{
    static float s_fBohooTime = 4.0f;

    if ((GetStatus() == ENEMYTYPES::STATUS_IDLE) ||
        (GetStatus() == ENEMYTYPES::STATUS_WALK) ||
        (GetStatus() == ENEMYTYPES::STATUS_WALK_RIGHT))
    {
        s_fBohooTime += CGameProperty::GetElapsedTime();
        if (s_fBohooTime > 8.0f)
        {
            float fRand = static_cast<float>(Math::Rand()) / static_cast<float>(TYPEDEF::SINT32_MAX);
            s_fBohooTime = (fRand * 3.0f);

            CGameSound::PlayObjectSE(&Compositor(), SDCODE_SE(4326));
        };
    };

    CEnemyCharacter::Run();
};


/*virtual*/ void CBaseStoneBiterChr::OnStart(void) /*override*/
{
    CEnemyCharacter::OnStart();

    CEnemyContainer::Instance().Attach(this);
};