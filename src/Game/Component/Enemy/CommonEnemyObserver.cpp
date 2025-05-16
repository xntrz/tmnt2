#include "CommonEnemyObserver.hpp"
#include "EnemyUtils.hpp"
#include "AIUtils.hpp"
#include "EnemyTracer.hpp"

#include "ConcreteAIModerator/BaseAI6045.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/CharacterEffectModule.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"


CMotionManagedStatusObserver::CMotionManagedStatusObserver(void)
: m_aszMotion()
, m_abMotionChain()
, m_motionPrev(-1)
, m_motionCurr(0)
, m_bPlayFlag(false)
{
    static_assert(COUNT_OF(m_aszMotion) == COUNT_OF(m_abMotionChain), "should be same count");

    int32 motionMax = COUNT_OF(m_aszMotion);
    for (int32 i = 0; i < motionMax; ++i)
    {
        m_aszMotion[i][0] = '\0';
        m_abMotionChain[i] = false;
    };
};


/*virtual*/ CMotionManagedStatusObserver::~CMotionManagedStatusObserver(void)
{
    ;
};


/*virtual*/ void CMotionManagedStatusObserver::Append(void) /*override*/
{
    m_motionPrev = -1;
    m_motionCurr = 0;
    m_bPlayFlag = false;

    static_assert(COUNT_OF(m_aszMotion) == COUNT_OF(m_abMotionChain), "should be same count");

    int32 motionMax = COUNT_OF(m_aszMotion);
    for (int32 i = 0; i < motionMax; ++i)
    {
        m_aszMotion[i][0] = '\0';
        m_abMotionChain[i] = false;
    };
};


/*virtual*/ void CMotionManagedStatusObserver::Remove(void) /*override*/
{
    MotionStop();

    m_motionCurr = 0;
};


/*virtual*/ void CMotionManagedStatusObserver::Update(void) /*override*/
{
    if (!m_bPlayFlag)
        return;

    if (m_motionPrev == -1)
    {
        m_motionPrev = m_motionCurr;
        return;
    };

    float endT = 0.0f;

    if (m_abMotionChain[m_motionCurr])
        endT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
    else
        endT = EnemyChr().Compositor().GetMotionEndTime();

    float nowT = EnemyChr().Compositor().GetMotionTime();
    if (nowT >= endT)
    {
        m_bPlayFlag = false;

        do
        {
            ++m_motionCurr;

            if (m_motionCurr >= COUNT_OF(m_aszMotion))
            {
                m_motionCurr = 0;
                OnMotionEnd();
                return;
            };            
        } while (m_aszMotion[m_motionCurr][0] == '\0');

        if (OnMotionChange(m_motionPrev, m_motionCurr))
            MotionPlay(m_motionCurr, true);
    };
};


/*virtual*/ void CMotionManagedStatusObserver::MotionRegist(int32 id, const char* pszMotion, bool bChainFlag /*= false*/)
{
    ASSERT(id >= 0);
    ASSERT(id < COUNT_OF(m_aszMotion));
    ASSERT(pszMotion != nullptr);    
    ASSERT(std::strlen(pszMotion) < sizeof(m_aszMotion[0]));

    std::strcpy(m_aszMotion[id], pszMotion);
    m_abMotionChain[id] = bChainFlag;
};


/*virtual*/ void CMotionManagedStatusObserver::MotionUnregist(int32 id)
{
    ASSERT(id >= 0);
    ASSERT(id < COUNT_OF(m_aszMotion));

    m_aszMotion[id][0] = '\0';
    m_abMotionChain[id] = false;
};


/*virtual*/ const char* CMotionManagedStatusObserver::MotionGet(int32 id) const
{
    ASSERT(id >= 0);
    ASSERT(id < COUNT_OF(m_aszMotion));

    return ((m_aszMotion[id][0] != '\0') ? m_aszMotion[id] : nullptr);
};


/*virtual*/ bool CMotionManagedStatusObserver::MotionPlay(int32 id, bool bReset /*= false*/)
{
    ASSERT(id >= 0);
    ASSERT(id < COUNT_OF(m_aszMotion));

    m_motionCurr = id;

    while (m_aszMotion[m_motionCurr][0] == '\0')
    {
        ++m_motionCurr;

        if (m_motionCurr >= COUNT_OF(m_aszMotion))
        {
            m_motionCurr = 0;
            return false;
        };
    };

    EnemyChr().Compositor().ChangeMotion(m_aszMotion[m_motionCurr], bReset);
    EnemyChr().Compositor().PlayMotion();

    m_motionPrev = -1;
    m_bPlayFlag = true;

    return true;
};


/*virtual*/ void CMotionManagedStatusObserver::MotionStop(void)
{
    if (m_bPlayFlag)
    {
        m_bPlayFlag = false;

        EnemyChr().Compositor().StopMotion();
    };
};


/*virtual*/ int32 CMotionManagedStatusObserver::MotionCurrent(void) const
{
    return m_motionCurr;
};


/*virtual*/ bool CMotionManagedStatusObserver::IsMotionEnd(void) const
{
    return (!m_bPlayFlag);
};


/*virtual*/ bool CMotionManagedStatusObserver::OnMotionChange(int32 idOld, int32 idNew)
{
	return true;
};


/*virtual*/ void CMotionManagedStatusObserver::OnMotionEnd(void)
{
    ;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CIdleStatus::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, bForce);

    m_fTime = 0.0f;
};


/*virtual*/ CCommonEnemyObserver::CIdleStatus::RESULT CCommonEnemyObserver::CIdleStatus::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_WAIT)
        return RESULT_END;

    float fWaitTime = EnemyChr().AIThinkOrder().OrderWait().m_fWaitTimer;
    if (m_fTime < fWaitTime)
        m_fTime += CGameProperty::GetElapsedTime();
    else
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    int32 waitType = EnemyChr().AIThinkOrder().OrderWait().m_iWaitType;
    switch (waitType)
    {
    case BASEAI6045::ORDERTYPE_WAIT_TURN_NO:
        {
            float fRotSpeed = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
            int32 iRotTarget = EnemyChr().AIThinkOrder().OrderWait().m_iPlayerNo;

            CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(), iRotTarget, fRotSpeed, MATH_DEG2RAD(3.0f));
        }
        break;

    case BASEAI6045::ORDERTYPE_WAIT_TURN_POS:
        {
            float fRotSpeed = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
            RwV3d vRotPosition = EnemyChr().AIThinkOrder().OrderWait().m_vAt;

            CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &vRotPosition, fRotSpeed, 0.0f);
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CIdleStatus::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CJumpReadyStatus::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::JUMP_READY, true);
};


/*virtual*/ CCommonEnemyObserver::CJumpReadyStatus::RESULT CCommonEnemyObserver::CJumpReadyStatus::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CJumpReadyStatus::OnEnd(void) /*override*/
{
    float fJumpInitSpeed = EnemyChr().CharacterParameter().m_feature.m_fJumpInitializeSpeed;

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vVelocity);

    vVelocity.y += fJumpInitSpeed;
    EnemyChr().Compositor().SetVelocity(&vVelocity);

    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CAerialStatus::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::JUMP);

    CHARACTERTYPES::FLAG cflag = CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION
                               | CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ;
    EnemyChr().Compositor().SetCharacterFlag(cflag);

    ENEMYTYPES::FLAG eflag = ENEMYTYPES::FLAG_AERIAL_STATUS;
    EnemyChr().SetFlag(eflag);

    float fMaxVel = EnemyChr().CharacterParameter().m_feature.m_fAerialMoveSpeed;
    EnemyChr().Compositor().SetMaxVelocity(fMaxVel);
};


/*virtual*/ CCommonEnemyObserver::CAerialStatus::RESULT CCommonEnemyObserver::CAerialStatus::Observing(void) /*override*/
{
    float fJumpInitSpeed = EnemyChr().CharacterParameter().m_feature.m_fJumpInitializeSpeed;
    CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), fJumpInitSpeed);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CAerialStatus::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    CHARACTERTYPES::FLAG cflag = CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION
                               | CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ;
    EnemyChr().Compositor().ClearCharacterFlag(cflag);

    return ENEMYTYPES::STATUS_TOUCHDOWN;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CTouchdownStatus::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::TOUCHDOWN);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
};


/*virtual*/ CCommonEnemyObserver::CTouchdownStatus::RESULT CCommonEnemyObserver::CTouchdownStatus::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CTouchdownStatus::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CMoveStatus::OnStart(void) /*override*/
{
    if (Status() == ENEMYTYPES::STATUS_RUN)
    {
        bool bForce = true;
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::RUN, bForce);

        m_fMoveSpeed = EnemyChr().CharacterParameter().m_feature.m_fRunMoveSpeed;
        
        m_moveTypeJudgeBegin    = BASEAI6045::ORDERTYPE_MOVE_RUN_POS;
        m_moveTypeJudgeEnd      = BASEAI6045::ORDERTYPE_MOVE_RUN_NO;
        m_moveTypeJudgeMovePos  = BASEAI6045::ORDERTYPE_MOVE_RUN_POS;
    }
    else
    {
        ASSERT(Status() == ENEMYTYPES::STATUS_WALK);

        bool bForce = true;
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::WALK, bForce);
        
        m_fMoveSpeed = EnemyChr().CharacterParameter().m_feature.m_fWalkMoveSpeed;
        
        m_moveTypeJudgeBegin    = BASEAI6045::ORDERTYPE_MOVE_WALK_POS;
        m_moveTypeJudgeEnd      = BASEAI6045::ORDERTYPE_MOVE_WALK_NO;
        m_moveTypeJudgeMovePos  = BASEAI6045::ORDERTYPE_MOVE_WALK_POS;
    };

    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
    m_fMoveTime = 0.0f;
};


/*virtual*/ CCommonEnemyObserver::CMoveStatus::RESULT CCommonEnemyObserver::CMoveStatus::Observing(void) /*override*/
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

    /* calc distance to order pos */
    RwV3d vMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vMyPos);
    
    RwV3d vDir = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vDir, &vMarkPos, &vMyPos);
    vDir.y = 0.0f;

    float fDist = Math::Vec3_Length(&vDir);
    if (fDist >= 0.125f)
    {
        float fRotSpeed = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
        CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &vMarkPos, fRotSpeed, MATH_DEG2RAD(3.0f));

        RwV3d vVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
        EnemyChr().Compositor().RotateVectorByDirection(&vVelocity, &vVelocity);
        EnemyChr().Compositor().SetVelocity(&vVelocity);
    }
    else
    {
        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CMoveStatus::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CDashStatus::OnStart(void) /*override*/
{
    // TODO
};


/*virtual*/ CCommonEnemyObserver::CDashStatus::RESULT CCommonEnemyObserver::CDashStatus::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CDashStatus::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CGuardStatus::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    m_step = STEP_START;
    m_fRemainGuardTime = 3.0f;
};


/*virtual*/ CCommonEnemyObserver::CGuardStatus::RESULT CCommonEnemyObserver::CGuardStatus::Observing(void) /*override*/
{
    switch (m_step)
    {
    case STEP_START:
        {
            bool bForce = true;
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_1, bForce);

			++m_step;
        }
        break;

    case STEP_START_RUN:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                ++m_step;
        }
        break;

    case STEP_HOLD:
        {
            bool bForce = true;
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_2, bForce);

            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

            ++m_step;
        }
        break;

    case STEP_HOLD_RUN:
        {
            if ((EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_DEFENCE) ||
                (m_fRemainGuardTime <= 0.0f))
                ++m_step;

            m_fRemainGuardTime -= CGameProperty::GetElapsedTime();
        }
        break;

    case STEP_END:
        {
            bool bForce = true;
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_3, bForce);

            ++m_step;
        }
        break;

    case STEP_END_RUN:
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


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CGuardStatus::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CKnockStatus::OnStart(void) /*override*/
{
    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_KNOCK:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::KNOCK, true);
        break;

    case ENEMYTYPES::STATUS_KNOCK_FRONT:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::KNOCK_FRONT, true);
        break;

    case ENEMYTYPES::STATUS_KNOCK_BACK:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::KNOCK_BACK, true);
        break;

    default:
        ASSERT(false);
        break;
    };

    RwV3d vAttackDir = EnemyChr().Compositor().AttackParameter().m_vDirection;
    EnemyChr().Compositor().SetDirectionFromVector(&vAttackDir);

    RwV3d vAttackVel = EnemyChr().Compositor().AttackParameter().m_vVelocity;
    EnemyChr().Compositor().SetVelocity(&vAttackVel);

    float endT = EnemyChr().Compositor().GetMotionEndTime();

    RwV3d vAccel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vAccel, &vAttackVel, -(1.0f / endT));

    EnemyChr().Compositor().SetAcceleration(&vAccel);
};


/*virtual*/ CCommonEnemyObserver::CKnockStatus::RESULT CCommonEnemyObserver::CKnockStatus::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CKnockStatus::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);

	return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CCommonEnemyObserver::CFlyawayStatus::CFlyawayStatus(bool bHandleCrashWallFlag)
: m_nextStatus(ENEMYTYPES::STATUS_THINKING)
, m_fJumpInitSpeed(0.0f)
, m_bCrashWallFlag(bHandleCrashWallFlag)

{
    ;
};


/*virtual*/ void CCommonEnemyObserver::CFlyawayStatus::OnStart(void) /*override*/
{
    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_FLYAWAY_FRONT:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_FRONT, true);
        break;

    case ENEMYTYPES::STATUS_FLYAWAY_BACK:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_BACK, true);
        break;

    case ENEMYTYPES::STATUS_THROWN_FRONT:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::LIFT_SUCCESS, true);
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    RwV3d vAttackDir = EnemyChr().Compositor().AttackParameter().m_vDirection;
    EnemyChr().Compositor().SetDirectionFromVector(&vAttackDir);

    RwV3d vAttackVel = EnemyChr().Compositor().AttackParameter().m_vVelocity;
    EnemyChr().Compositor().SetVelocity(&vAttackVel);

    float fSpeed = std::fabs(vAttackVel.y);

    m_fJumpInitSpeed = fSpeed * 0.9f;
    m_nextStatus     = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CCommonEnemyObserver::CFlyawayStatus::RESULT CCommonEnemyObserver::CFlyawayStatus::Observing(void) /*override*/
{
    CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), m_fJumpInitSpeed);

    if (m_bCrashWallFlag && EnemyChr().Compositor().IsWallCrashPossible())
    {
        m_nextStatus = (Status() == ENEMYTYPES::STATUS_FLYAWAY_FRONT ? ENEMYTYPES::STATUS_CRASHWALL_FRONT : ENEMYTYPES::STATUS_CRASHWALL_BACK);
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CFlyawayStatus::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CFlyawayBoundStatus::OnStart(void) /*override*/
{
    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_BOUND_FRONT, true);
        break;

    case ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_BOUND_BACK, true);
        break;

    default:
        ASSERT(false);
        break;
    };

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vVelocity);

    float endT = EnemyChr().Compositor().GetMotionEndTime();
    RwV3d vAccel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vAccel, &vVelocity, -(1.0f / endT));

    EnemyChr().Compositor().SetAcceleration(&vAccel);

    m_bCheckVelocityFlag = false;
    m_bHitFlag = false;
    m_bDirCorrection = true;
};


/*virtual*/ CCommonEnemyObserver::CFlyawayBoundStatus::RESULT CCommonEnemyObserver::CFlyawayBoundStatus::Observing(void) /*override*/
{
    if (!m_bCheckVelocityFlag)
    {
        RwV3d vVel = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetVelocity(&vVel);

        RwV3d vAccel = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetAcceleration(&vAccel);

        float vel = Math::Vec3_Length(&vVel);
        float accel = Math::Vec3_Length(&vAccel);

        if ((vel > 0.0f) && (accel > 0.0f))
        {
            float angle = Math::ATan2(vVel.x, vVel.z) - Math::ATan2(vAccel.x, vAccel.z);

            angle = CEnemyUtils::RadianCorrect(angle);
            angle = std::fabs(angle);

            if (angle < MATH_DEG2RAD(90.0f))
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

                m_bCheckVelocityFlag = true;
            };
        };
    };

    const CCharacterCompositor::COLLISIONGROUNDINFO* pGrounInfo = EnemyChr().Compositor().GetCollisionGround();
    ASSERT(pGrounInfo != nullptr);

    if (!m_bHitFlag && pGrounInfo->m_bHit)
    {
        CEnemyUtils::EntryFlyawayBoundEffectAndSE(&EnemyChr());
        EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
        m_bHitFlag = true;
    };

    if (!EnemyChr().Compositor().IsMotionEnd())
        return RESULT_CONTINUE;

    if (m_bDirCorrection && (Status() == ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK))
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        fDir += MATH_PI;
        EnemyChr().Compositor().SetDirection(fDir);
    };

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CFlyawayBoundStatus::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    return ENEMYTYPES::STATUS_GETUP;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CCrashWallStatus::OnStart(void) /*override*/
{
    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_CRASHWALL_FRONT:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::CRASHWALL_FRONT, true);
        break;

    case ENEMYTYPES::STATUS_CRASHWALL_BACK:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::CRASHWALL_BACK, true);
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    ENEMYTYPES::FLAG eflag = ENEMYTYPES::FLAG_INVINCIBILITY
                           | ENEMYTYPES::FLAG_AERIAL_STATUS;
    EnemyChr().SetFlag(eflag);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    RwV3d vPos = EnemyChr().Compositor().GetCollisionWall()->m_vPosition;
    float fDir = EnemyChr().Compositor().GetDirection();
    const char* pszName = EFFECTID::GetNameFromID(EFFECTID::ID_ALL_PITAN);
    CEffectManager::Play(pszName, &vPos, fDir);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4121));

    m_fStationaryTime = 0.0f;
};


/*virtual*/ CCommonEnemyObserver::CCrashWallStatus::RESULT CCommonEnemyObserver::CCrashWallStatus::Observing(void) /*override*/
{
    if (m_fStationaryTime > 1.0f)
        return RESULT_END;

    m_fStationaryTime += CGameProperty::GetElapsedTime();

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CCrashWallStatus::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);

    ENEMYTYPES::FLAG eflag = ENEMYTYPES::FLAG_INVINCIBILITY
                           | ENEMYTYPES::FLAG_AERIAL_STATUS;
    EnemyChr().ClearFlag(eflag);

    return (Status() == ENEMYTYPES::STATUS_CRASHWALL_FRONT ? ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT : ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK);
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CCrashWallFallStatus::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    ENEMYTYPES::FLAG eflag = ENEMYTYPES::FLAG_INVINCIBILITY
                           | ENEMYTYPES::FLAG_AERIAL_STATUS;
    EnemyChr().SetFlag(eflag);

    RwV3d vVelocity = { 0.0f, -2.0f, 0.0f };
    EnemyChr().Compositor().SetVelocity(&vVelocity);
};


/*virtual*/ CCommonEnemyObserver::CCrashWallFallStatus::RESULT CCommonEnemyObserver::CCrashWallFallStatus::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CCrashWallFallStatus::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    if (Status() == ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK)
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        fDir += MATH_PI;
        EnemyChr().Compositor().SetDirection(fDir);
    };

    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);

    ENEMYTYPES::FLAG eflag = ENEMYTYPES::FLAG_INVINCIBILITY
                           | ENEMYTYPES::FLAG_AERIAL_STATUS;
    EnemyChr().ClearFlag(eflag);

    return ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CCrashWallTouchdownStatus::OnStart(void) /*override*/
{
    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::CRASHWALL_TOUCHDOWN_FRONT, true);
        break;

    case ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::CRASHWALL_TOUCHDOWN_BACK, true);
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CCommonEnemyObserver::CCrashWallTouchdownStatus::RESULT CCommonEnemyObserver::CCrashWallTouchdownStatus::Observing(void) /*override*/
{
    if (!EnemyChr().Compositor().IsMotionEnd())
        return RESULT_CONTINUE;

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CCrashWallTouchdownStatus::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    return ENEMYTYPES::STATUS_GETUP;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CGetupStatus::OnStart(void) /*override*/
{
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);

    m_bStepFlag = false;
};


/*virtual*/ CCommonEnemyObserver::CGetupStatus::RESULT CCommonEnemyObserver::CGetupStatus::Observing(void) /*override*/
{
    if (!EnemyChr().Compositor().IsMotionEnd())
        return RESULT_CONTINUE;

    if (!m_bStepFlag)
    {
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_GETUP, true);
        m_bStepFlag = true;
        return RESULT_CONTINUE;
    };

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CGetupStatus::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CCharacterEffectStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    
    m_bBodyHitFlag = EnemyChr().Compositor().IsEnableBodyHitSelfToOther();
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    m_fTargetTime = EnemyChr().Compositor().AttackParameter().m_fTroubleTime;
    m_fTime = 0.0f;

    IModule* pModule = nullptr;
    CCharacterCompositor* pChr = &EnemyChr().Compositor();
    ENEMYTYPES::STATUS currentStatus = Status();

    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_DINDLE:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DINDLE, true);
        pModule = CharacterEffectModule::CreateDindleEffectModule(pChr);
        break;

    case ENEMYTYPES::STATUS_STUN:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::STUN, true);
        pModule = CharacterEffectModule::CreateStunEffectModule(pChr);
        break;

    case ENEMYTYPES::STATUS_SLEEP:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::STUN, true);
        pModule = CharacterEffectModule::CreateSleepEffectModule(pChr);
        break;

    case ENEMYTYPES::STATUS_FREEZE:
        EnemyChr().Compositor().StopMotion();
        pModule = CharacterEffectModule::CreateFreezeEffectModule(pChr);
        break;

    case ENEMYTYPES::STATUS_BIND:
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::STUN, true);
        pModule = CharacterEffectModule::CreateBindEffectModule(pChr);
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().Compositor().IncludeModule(pModule);
};


/*virtual*/ CCommonEnemyObserver::CCharacterEffectStatusObserver::RESULT CCommonEnemyObserver::CCharacterEffectStatusObserver::Observing(void) /*override*/
{
    m_fTime += CGameProperty::GetElapsedTime();
    if (m_fTime >= m_fTargetTime)
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CCharacterEffectStatusObserver::OnEnd(void) /*override*/
{
    if (m_bBodyHitFlag)
        EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CCounteractStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::INASARE, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CCommonEnemyObserver::CCounteractStatusObserver::RESULT CCommonEnemyObserver::CCounteractStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CCounteractStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CCaughtStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::LIFT_CHALLENGE, true);
    EnemyChr().Compositor().SetEnableBodyHit(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ CCommonEnemyObserver::CCaughtStatusObserver::RESULT CCommonEnemyObserver::CCaughtStatusObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CCaughtStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableBodyHit(true);

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CLiftedStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::LIFT, true);
    EnemyChr().Compositor().SetEnableBodyHit(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
};


/*virtual*/ CCommonEnemyObserver::CLiftedStatusObserver::RESULT CCommonEnemyObserver::CLiftedStatusObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CLiftedStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableBodyHit(true);

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CLiftedWalkStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::LIFT_WALK, true);
    EnemyChr().Compositor().SetEnableBodyHit(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
};


/*virtual*/ CCommonEnemyObserver::CLiftedWalkStatusObserver::RESULT CCommonEnemyObserver::CLiftedWalkStatusObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CLiftedWalkStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableBodyHit(true);

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CCommonEnemyObserver::CThrownFrontStatusObserver::CThrownFrontStatusObserver(bool bHandleCrashWallFlag, float fForce)
: CFlyawayStatus(bHandleCrashWallFlag)
, m_fThrownForce(fForce)
{
    ;
};


/*virtual*/ void CCommonEnemyObserver::CThrownFrontStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::LIFT_SUCCESS, true);

    CHARACTERTYPES::FLAG cflag = CHARACTERTYPES::FLAG_CANCEL_GRAVITY
                               | CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ;
    EnemyChr().Compositor().ClearCharacterFlag(cflag);

    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vVelocity);

    float fForce = (m_fThrownForce * 20.0f);

    vVelocity.x *= fForce;
    vVelocity.y *= (fForce * 0.5f);
    vVelocity.z *= fForce;

    EnemyChr().Compositor().SetVelocity(&vVelocity);

    float fSpeed = std::fabs(vVelocity.y);

    m_fJumpInitSpeed = fSpeed * 0.9f;
    m_nextStatus     = ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CThrownBackStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::THROW_BACK, true);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    EnemyChr().Compositor().SetEnableBodyHit(false);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
};


/*virtual*/ CCommonEnemyObserver::CThrownBackStatusObserver::RESULT CCommonEnemyObserver::CThrownBackStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CThrownBackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    EnemyChr().Compositor().SetEnableBodyHit(true);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    return ENEMYTYPES::STATUS_FLYAWAY_BACK;
};


//
// *********************************************************************************
//


CCommonEnemyObserver::COneShotMotion::COneShotMotion(const char* pszMotion)
: m_szMotion()
{
    ASSERT(pszMotion != nullptr);
    ASSERT(std::strlen(pszMotion) < sizeof(m_szMotion));

    std::strcpy(m_szMotion, pszMotion);
};


/*virtual*/ void CCommonEnemyObserver::COneShotMotion::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(m_szMotion, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
};


/*virtual*/ CCommonEnemyObserver::COneShotMotion::RESULT CCommonEnemyObserver::COneShotMotion::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::COneShotMotion::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CCommonEnemyObserver::CWalkingAppearStatus::CWalkingAppearStatus(float fAppearDinstance)
: m_fAppearDistance(fAppearDinstance)
, m_vInitialPos(Math::VECTOR3_ZERO)
, m_fTime(0.0f)
{
    ;
};


/*virtual*/ void CCommonEnemyObserver::CWalkingAppearStatus::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetEnableCatchHit(false);

    EnemyChr().Compositor().GetPosition(&m_vInitialPos);
    m_vInitialPos.y = 0.0f;

    m_fTime = 0.0f;

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::WALK, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    float fWalkMoveSpeed = EnemyChr().CharacterParameter().m_feature.m_fWalkMoveSpeed;
    RwV3d vVelocity = { 0.0f, 0.0f, fWalkMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vVelocity, &vVelocity);
    EnemyChr().Compositor().SetVelocity(&vVelocity);
};


/*virtual*/ CCommonEnemyObserver::CWalkingAppearStatus::RESULT CCommonEnemyObserver::CWalkingAppearStatus::Observing(void) /*override*/
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vPosition);
    vPosition.y = 0.0f;

    float fDistance = CEnemyUtils::GetDistance(&vPosition, &m_vInitialPos);
    if (fDistance > m_fAppearDistance)
        return RESULT_END;

    float fSpeed = EnemyChr().CharacterParameter().m_feature.m_fWalkMoveSpeed;
    float fTime = (m_fAppearDistance / fSpeed);
    if (m_fTime >= fTime)
        return RESULT_END;

    m_fTime += CGameProperty::GetElapsedTime();

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CWalkingAppearStatus::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().Compositor().SetEnableCatchHit(true);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vPosition);

    float fPatrolRadius = EnemyChr().CharacterParameter().m_feature.m_fPatrolRadius;
    EnemyChr().SetPatrolArea(&vPosition, fPatrolRadius);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CNoneAppearStatus::OnStart(void) /*override*/
{
    ;
};


/*virtual*/ CCommonEnemyObserver::CNoneAppearStatus::RESULT CCommonEnemyObserver::CNoneAppearStatus::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CNoneAppearStatus::OnEnd(void) /*override*/
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vPosition);

    float fPatrolRadius = EnemyChr().CharacterParameter().m_feature.m_fPatrolRadius;

    EnemyChr().SetPatrolArea(&vPosition, fPatrolRadius);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CFallAppearStatus::OnStart(void) /*override*/
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vPosition);

    vPosition.y += 8.0f;

    EnemyChr().Compositor().SetPosition(&vPosition);
};


/*virtual*/ CCommonEnemyObserver::CFallAppearStatus::RESULT CCommonEnemyObserver::CFallAppearStatus::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CFallAppearStatus::OnEnd(void) /*override*/
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vPosition);

    float fPatrolRadius = EnemyChr().CharacterParameter().m_feature.m_fPatrolRadius;

    EnemyChr().SetPatrolArea(&vPosition, fPatrolRadius);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CCommonEnemyObserver::CMotionAppearStatus::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetEnableCatchHit(false);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::TOJO, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vPosition);

    vPosition.y = CWorldMap::GetMapHeight(&vPosition);

    EnemyChr().Compositor().SetPosition(&vPosition);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ CCommonEnemyObserver::CMotionAppearStatus::RESULT CCommonEnemyObserver::CMotionAppearStatus::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CMotionAppearStatus::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableCatchHit(true);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vPosition);

    float fPatrolRadius = EnemyChr().CharacterParameter().m_feature.m_fPatrolRadius;

    EnemyChr().SetPatrolArea(&vPosition, fPatrolRadius);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CCommonEnemyObserver::CBaseThrow::CBaseThrow(void)
: m_throwBoneID(CHARACTERTYPES::BONEID_POSITION)
, m_vBoneOffset(Math::VECTOR3_ZERO)
, m_vThrowDirection(Math::VECTOR3_ZERO)
, m_fLiftPlayerDirection(0.0f)
, m_step(0)
, m_bThrowHit(false)
, m_bSendCatch(false)
, m_bThrowSuccess(false)
, m_throwDamage(0)
, m_throwTarget(-1)
, m_liftInfo()
{
    static_assert(std::is_pod<decltype(m_liftInfo)>::value, "memset not pod type");

    std::memset(&m_liftInfo, 0, sizeof(m_liftInfo));
};


/*virtual*/ void CCommonEnemyObserver::CBaseThrow::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::THROW_STANDBY, true);

    m_step = 0;
    m_bThrowHit = false;
    m_bSendCatch = false;
    m_bThrowSuccess = false;
};


/*virtual*/ CCommonEnemyObserver::CBaseThrow::RESULT CCommonEnemyObserver::CBaseThrow::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:        
        {
            float fMotionConnectT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
            float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
            if (fMotionNowT < fMotionConnectT)
            {
                if (m_bThrowHit)
                {
                    if (m_bSendCatch)
                    {
                        SendLiftMessage();
                        break;
                    };

                    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_throwTarget);
                    if (!pPlrChr)
                        break;

                    CGameObjectManager::SendMessage(pPlrChr, CHARACTERTYPES::MESSAGEID_CATCH);
                    pPlrChr->RequestDamage(m_throwDamage);

                    m_bSendCatch = true;

                    OnThrowHit();
                }
                else
                {
                    OnThrowStandby();
                };
            }
            else
            {
                if (m_bSendCatch)
                {
                    SendLiftMessage();
                    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::THROW_SUCCESS, true);
                    m_step = 2;
                }
                else
                {
                    m_step = 1;
                };
            };
        }
        break;

    case 1:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                return RESULT_END;
            };            
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
            {
                RwV3d vecThrow = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().RotateVectorByDirection(&vecThrow, &m_vThrowDirection);

                CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_throwTarget);
                if (pPlrChr)
                    CGameObjectManager::SendMessage(pPlrChr, CHARACTERTYPES::MESSAGEID_THROW, &vecThrow);

                ++m_step;
                m_bThrowSuccess = true;
                m_bSendCatch = false;
                m_bThrowHit = false;
            }
            else
            {
                SendLiftMessage();
            };
        }
        break;

    case 3:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                return RESULT_END;
            };
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;    
};


/*virtual*/ ENEMYTYPES::STATUS CCommonEnemyObserver::CBaseThrow::OnEnd(void) /*override*/
{
    if (!IsThrowSuccess() && IsSendCatch())
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_throwTarget);
        if (pPlayerChr)
        {
            RwV3d vVelocity = Math::VECTOR3_ZERO;
            CGameObjectManager::SendMessage(pPlayerChr, CHARACTERTYPES::MESSAGEID_MISSTHROW, &vVelocity);
        };
    };

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ void CCommonEnemyObserver::CBaseThrow::OnThrowStandby(void)
{
    ;
};


/*virtual*/ void CCommonEnemyObserver::CBaseThrow::OnLift(void)
{
    ;
};


/*virtual*/ void CCommonEnemyObserver::CBaseThrow::OnThrowHit(void)
{
    ;
};


void CCommonEnemyObserver::CBaseThrow::SetAttackParam(const CHitCatchData* pCatch)
{
    if (!IsSendCatch())
    {
        CHitCatchData::RESULT catchResult = pCatch->GetResult();

        if ((catchResult == CHitCatchData::RESULT_THROWFRONT) ||
            (catchResult == CHitCatchData::RESULT_THROWBACK))
        {
            CGameObject* pGameObject = pCatch->GetObject();
            ASSERT(pGameObject->GetType() == GAMEOBJECTTYPE::CHARACTER);

            m_throwTarget = CEnemyUtils::FindPlayerNo(pGameObject->GetHandle());
            ASSERT(m_throwTarget >= 0);
            ASSERT(m_throwTarget < CAIUtils::GetPlayerNum());

            m_bThrowHit = true;
            m_throwDamage = pCatch->GetAttack()->GetPower();
        };
    };
};


void CCommonEnemyObserver::CBaseThrow::SetThrowVector(const RwV3d* pvVec)
{
    m_vThrowDirection = *pvVec;
};


void CCommonEnemyObserver::CBaseThrow::SetTraceBoneData(int32 boneID, const RwV3d* pvOffset)
{
    m_throwBoneID = boneID;
    m_vBoneOffset = *pvOffset;
};


void CCommonEnemyObserver::CBaseThrow::SendLiftMessage(void)
{
    GetLiftPosition(&m_liftInfo.m_vPosition);

    float fDir = EnemyChr().Compositor().GetDirection();
    m_liftInfo.m_fDirection = fDir + m_fLiftPlayerDirection;

    OnLift();

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_throwTarget);
    if (pPlayerChr)
        CGameObjectManager::SendMessage(pPlayerChr, CHARACTERTYPES::MESSAGEID_LIFT, &m_liftInfo);
};


void CCommonEnemyObserver::CBaseThrow::GetLiftPosition(RwV3d* pvPos)
{
    if (m_bSendCatch)
    {
        float fMotionNowT = EnemyChr().Compositor().GetMotionTime();
        float fMotionPreT = fMotionNowT;
        float fMotionNewT = CGameProperty::GetElapsedTime() + fMotionNowT;
        EnemyChr().Compositor().SetMotionTime(fMotionNewT);
        EnemyChr().Compositor().UpdateMatrices();

        EnemyChr().Compositor().GetBonePosition(pvPos, m_throwBoneID, &m_vBoneOffset);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vPosition);

        Math::Vec3_Add(pvPos, pvPos, &vPosition);

        EnemyChr().Compositor().SetMotionTime(fMotionPreT);
        EnemyChr().Compositor().UpdateMatrices();
    }
    else
    {
        *pvPos = Math::VECTOR3_ZERO;
    };
};


bool CCommonEnemyObserver::CBaseThrow::IsThrowSuccess(void) const
{
    return m_bThrowSuccess;
};


bool CCommonEnemyObserver::CBaseThrow::IsSendCatch(void) const
{
    return m_bSendCatch;
};


bool CCommonEnemyObserver::CBaseThrow::IsThrowHit(void) const
{
    return m_bThrowHit;
};