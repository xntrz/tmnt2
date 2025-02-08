#include "BaseGrapplerEnemyChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseGrapplerAI.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/System/Sound/GameSound.hpp"


CBaseGrapplerEnemyChr::CMoveStatusObserver::CMoveStatusObserver(void)
: m_order(CAIThinkOrder::ORDER_NOTHING)
, m_nextStatus(ENEMYTYPES::STATUS_THINKING)
, m_vMovePos(Math::VECTOR3_ZERO)
, m_fAttackRange(0.0f)
, m_fMoveSpeed(0.0f)
, m_fRotateRate(0.0f)
, m_fDistanceOfSuitable(0.0f)
, m_pszMotion(nullptr)
{
    ;
};


/*virtual*/ void CBaseGrapplerEnemyChr::CMoveStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    if (m_pszMotion)
        EnemyChr().Compositor().ChangeMotion(m_pszMotion);

    m_order = EnemyChr().AIThinkOrder().GetOrder();

    CBaseGeneralEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &m_vMovePos);
    m_vMovePos.y = 0.0f;

    m_fAttackRange = 0.0f;

    Observing();
};


/*virtual*/ CBaseGrapplerEnemyChr::CMoveStatusObserver::RESULT
CBaseGrapplerEnemyChr::CMoveStatusObserver::Observing(void) /*override*/
{
    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    if (order != m_order)
    {
        m_nextStatus = ENEMYTYPES::STATUS_THINKING;
        return RESULT_END;
    };

    /* set direction */
    RwV3d vPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vPos);
    vPos.y = 0.0f;

    RwV3d vAt = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vAt);
    vAt.y = 0.0f;

    float fDir = CEnemyUtils::GetDirection(&vPos, &vAt);
    EnemyChr().Compositor().RotateDirection(fDir, m_fRotateRate);

    /* set velocity */
    RwV3d vVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vVelocity, &vVelocity);
    EnemyChr().Compositor().SetVelocity(&vVelocity);

    /* check order move type */
    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_WALK:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
            if (AIOT::Get(moveType) != AIOT::MoveWalk)
            {
                m_nextStatus = ENEMYTYPES::STATUS_THINKING;
                return RESULT_END;
            };
        }
        break;

    case ENEMYTYPES::STATUS_RUN:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
            if (AIOT::Get(moveType) != AIOT::MoveRun)
            {
                m_nextStatus = ENEMYTYPES::STATUS_THINKING;
                return RESULT_END;
            };
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGrapplerEnemyChr::CMoveStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseGrapplerEnemyChr::CWalkStatusObserver::OnStart(void) /*override*/
{
    m_pszMotion             = ENEMYTYPES::MOTIONNAMES::WALK;
    m_fRotateRate           = EnemyChr().Feature().m_fRotateRate;
    m_fMoveSpeed            = EnemyChr().Feature().m_fWalkMoveSpeed;
    m_fDistanceOfSuitable   = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;

    CMoveStatusObserver::OnStart();
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseGrapplerEnemyChr::CRunStatusObserver::OnStart(void) /*override*/
{
    m_pszMotion             = ENEMYTYPES::MOTIONNAMES::RUN;
    m_fRotateRate           = EnemyChr().Feature().m_fRotateRate * 1.5f;
    m_fMoveSpeed            = EnemyChr().Feature().m_fRunMoveSpeed;
    m_fDistanceOfSuitable   = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;

    CMoveStatusObserver::OnStart();
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseGrapplerEnemyChr::CDeathStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);
};


/*virtual*/ CBaseGrapplerEnemyChr::CDeathStatusObserver::RESULT
CBaseGrapplerEnemyChr::CDeathStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGrapplerEnemyChr::CDeathStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vPosition);

    EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_NORMAL, &vPosition);

    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseGrapplerEnemyChr::CAttackStatusObserver::OnStart(void) /*override*/
{
    const char* pszMotionName = nullptr;

    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_ATTACK_A:
        pszMotionName = ENEMYTYPES::MOTIONNAMES::ATTACK_A;
        break;

    case ENEMYTYPES::STATUS_ATTACK_AA:
        pszMotionName = ENEMYTYPES::MOTIONNAMES::ATTACK_AA;
        break;

    case ENEMYTYPES::STATUS_ATTACK_AAA:
        pszMotionName = ENEMYTYPES::MOTIONNAMES::ATTACK_AAA;
        break;

    case ENEMYTYPES::STATUS_ATTACK_B:
        pszMotionName = ENEMYTYPES::MOTIONNAMES::ATTACK_B;
        break;

    case ENEMYTYPES::STATUS_ATTACK_C:
        pszMotionName = ENEMYTYPES::MOTIONNAMES::ATTACK_C;
        break;

    case ENEMYTYPES::STATUS_ATTACK_RUN:
        {
            pszMotionName = ENEMYTYPES::MOTIONNAMES::ATTACK_RUN;

            RwV3d vPos = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetPosition(&vPos);

            float fDir = EnemyChr().Compositor().GetDirection();

            uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DASH2, &vPos, fDir);
            ASSERT(hEffect);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().Compositor().ChangeMotion(pszMotionName, true);

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ CBaseGrapplerEnemyChr::CAttackStatusObserver::RESULT
CBaseGrapplerEnemyChr::CAttackStatusObserver::Observing(void) /*override*/
{
    if (!EnemyChr().Compositor().IsMotionEnd())
        return RESULT_CONTINUE;

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGrapplerEnemyChr::CAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_ATTACK_A:
        MotionRegist(MOTIONID_ATTACK_A,     ENEMYTYPES::MOTIONNAMES::ATTACK_A, false);
        break;

    case ENEMYTYPES::STATUS_ATTACK_AA:
        MotionRegist(MOTIONID_ATTACK_A,     ENEMYTYPES::MOTIONNAMES::ATTACK_A, true);
        MotionRegist(MOTIONID_ATTACK_AA,    ENEMYTYPES::MOTIONNAMES::ATTACK_AA, false);
        break;

    case ENEMYTYPES::STATUS_ATTACK_AAA:
        MotionRegist(MOTIONID_ATTACK_A,     ENEMYTYPES::MOTIONNAMES::ATTACK_A, true);
        MotionRegist(MOTIONID_ATTACK_AA,    ENEMYTYPES::MOTIONNAMES::ATTACK_AA, true);
        MotionRegist(MOTIONID_ATTACK_AAA,   ENEMYTYPES::MOTIONNAMES::ATTACK_AAA, false);
        break;

    default:
        ASSERT(false);
        break;
    };

    MotionPlay(MOTIONID_ATTACK_A, true);
    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver::RESULT
CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver::Observing(void) /*override*/
{
    if (IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ bool CBaseGrapplerEnemyChr::CAttackASeriesStatusObserver::OnMotionChange(int32 idOld, int32 idNew) /*override*/
{
    MotionPlay(idNew, true);
    CGameSound::PlayAttackSE(&EnemyChr().Compositor());

    return false;
};


//
// *********************************************************************************
//


CBaseGrapplerEnemyChr::CBaseGrapplerEnemyChr(ENEMYID::VALUE idEnemy)
: CBaseGeneralEnemyChr(idEnemy)
{
    /* CBaseGeneralEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new CBaseGeneralEnemyChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                      new CBaseGeneralEnemyChr::CGuardStatusObserver(3.0f));
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new CBaseGeneralEnemyChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CBaseGeneralEnemyChr::CIdleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_TURN,                       new CBaseGeneralEnemyChr::CDummyTurnStatusObserver);

    /* CBaseGrapplerEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                       new CBaseGrapplerEnemyChr::CWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                        new CBaseGrapplerEnemyChr::CRunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                      new CBaseGrapplerEnemyChr::CDeathStatusObserver);

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                     new CCommonEnemyObserver::CNoneAppearStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                     new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                  new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CCommonEnemyObserver::CKnockFrontStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CCommonEnemyObserver::CKnockBackStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,              new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,               new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,        new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,         new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FRONT,            new CCommonEnemyObserver::CCrashWallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_BACK,             new CCommonEnemyObserver::CCrashWallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT,       new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK,        new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT,  new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK,   new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                     new CCommonEnemyObserver::CDindleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                       new CCommonEnemyObserver::CStunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_SLEEP,                      new CCommonEnemyObserver::CSleepStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CFreezeStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_BIND,                       new CCommonEnemyObserver::CBindStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,                 new CCommonEnemyObserver::CCounteractStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_CAUGHT,                     new CCommonEnemyObserver::CCaughtStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_LIFTED,                     new CCommonEnemyObserver::CLiftedStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_LIFTED_WALK,                new CCommonEnemyObserver::CLiftedWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THROWN_FRONT,               new CCommonEnemyObserver::CThrownFrontStatusObserver(true, 1.0f));
    AttachStatusObserver(ENEMYTYPES::STATUS_THROWN_BACK,                new CCommonEnemyObserver::CThrownBackStatusObserver);
};


/*virtual*/ CBaseGrapplerEnemyChr::~CBaseGrapplerEnemyChr(void)
{
    ;
};