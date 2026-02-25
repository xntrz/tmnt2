#include "BaseGeneralEnemyChr.hpp"

#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseGeneralEnemyAI.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/Intersection.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*virtual*/ void CBaseGeneralEnemyChr::CHideStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CBaseGeneralEnemyChr::CHideStatusObserver::RESULT
CBaseGeneralEnemyChr::CHideStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGeneralEnemyChr::CHideStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_APPEAR;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseGeneralEnemyChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ CBaseGeneralEnemyChr::CThinkingStatusObserver::RESULT
CBaseGeneralEnemyChr::CThinkingStatusObserver::Observing(void) /*override*/
{
    RESULT result = RESULT_END;

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_NOTHING:
    case CAIThinkOrder::ORDER_WAIT:
        {
            m_nextStatus = ENEMYTYPES::STATUS_IDLE;
        }
        break;

    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
            int32 moveId = AIOT::Get(moveType);

            switch (moveId)
            {
            case AIOT::MoveTurn:
                m_nextStatus = ENEMYTYPES::STATUS_TURN;
                break;

            case AIOT::MoveWalkLeft:
                m_nextStatus = ENEMYTYPES::STATUS_WALK_LEFT;
                break;

            case AIOT::MoveWalkRight:
                m_nextStatus = ENEMYTYPES::STATUS_WALK_RIGHT;
                break;

            case AIOT::MoveWalk:
                m_nextStatus = ENEMYTYPES::STATUS_WALK;
                break;

            case AIOT::MoveRun:
                m_nextStatus = ENEMYTYPES::STATUS_RUN;
                break;

            case AIOT::MoveJump:
                m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
                break;

            default:
                m_nextStatus = ENEMYTYPES::STATUS_IDLE;
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_RUN:
        {
            m_nextStatus = ENEMYTYPES::STATUS_RUN;
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            int32 attackId = AIOT::Get(attackType);

            switch (attackId)
            {
            case AIOT::AttackA:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case AIOT::AttackAA:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_AA;
                break;

            case AIOT::AttackAAA:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_AAA;
                break;

            case AIOT::AttackB:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case AIOT::AttackC:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
                break;

            case AIOT::AttackRun:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_RUN;
                break;

            case AIOT::FireA:
                m_nextStatus = ENEMYTYPES::STATUS_FIRE_A;
                break;

            case AIOT::FireB:
                m_nextStatus = ENEMYTYPES::STATUS_FIRE_B;
                break;

            case AIOT::FireC:
                m_nextStatus = ENEMYTYPES::STATUS_FIRE_C;
                break;

            case AIOT::FireRepeatA:
                m_nextStatus = ENEMYTYPES::STATUS_FIRE_AA;
                break;

            case AIOT::FireRepeatB:
                m_nextStatus = ENEMYTYPES::STATUS_FIRE_BB;
                break;

            case AIOT::FireRepeatC:
                m_nextStatus = ENEMYTYPES::STATUS_FIRE_CC;
                break;

            case AIOT::Special1:
                m_nextStatus = ENEMYTYPES::STATUS_SPECIAL;
                break;

            default:
                m_nextStatus = ENEMYTYPES::STATUS_IDLE;
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
        result = RESULT_CONTINUE;
        break;
    };

    if (!EnemyChr().IsAttachedStatusObserver(m_nextStatus))
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        m_nextStatus = ENEMYTYPES::STATUS_IDLE;
    };

    return result;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGeneralEnemyChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseGeneralEnemyChr::CIdleStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE);

    m_fTime = 0.0f;
};


/*virtual*/ CBaseGeneralEnemyChr::CIdleStatusObserver::RESULT
CBaseGeneralEnemyChr::CIdleStatusObserver::Observing(void) /*override*/
{
    m_fTime += CGameProperty::GetElapsedTime();

    if (m_fTime >= 1.0f)
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
        return RESULT_END;
    };

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    if (order == CAIThinkOrder::ORDER_NOTHING)
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return RESULT_END;
    }
    else if (order != CAIThinkOrder::ORDER_WAIT)
    {
        return RESULT_END;
    };    

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGeneralEnemyChr::CIdleStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseGeneralEnemyChr::CTurnStatusObserver::CTurnStatusObserver(float fBaseTurnAngleDeg,
                                                               const char* pszMotionTurnLeft,
                                                               const char* pszMotionTurnRight)
: m_szTurnL()
, m_szTurnR()
, m_fDirection(0.0f)
, m_fBaseTurnAngle(fBaseTurnAngleDeg)
, m_fRadPerFrame(0.0f)
, m_iTurnCount(0)
, m_fBeforeMotionTime(0.0f)
{
    /* init turn left motion */
    ASSERT(pszMotionTurnLeft != nullptr);
    ASSERT(std::strlen(pszMotionTurnLeft) < sizeof(m_szTurnL));

    std::strcpy(m_szTurnL, pszMotionTurnLeft);

    /* init turn right motion */
    ASSERT(pszMotionTurnRight != nullptr);
    ASSERT(std::strlen(pszMotionTurnRight) < sizeof(m_szTurnR));

    std::strcpy(m_szTurnR, pszMotionTurnRight);
};


/*virtual*/ void CBaseGeneralEnemyChr::CTurnStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    RwV3d vMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vMyPos);

    RwV3d vMarkPos = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vMarkPos);

    if (Math::FEqual(vMyPos.x, vMarkPos.x) &&
        Math::FEqual(vMyPos.z, vMarkPos.z))
    {
        m_fDirection = EnemyChr().Compositor().GetDirection();
    }
    else
    {
        vMarkPos.y = 0.0f;
        m_fDirection = CEnemyUtils::GetDirection(&vMyPos, &vMarkPos);
    };

    float fDirDiff = (m_fDirection - EnemyChr().Compositor().GetDirection());
    fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);

    float fDirDiffAbs = std::fabs(fDirDiff);

    m_iTurnCount = static_cast<int32>((m_fBaseTurnAngle - 1.0f + MATH_RAD2DEG(fDirDiffAbs)) / m_fBaseTurnAngle);

    if (m_iTurnCount <= 0)
    {
        EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE);
    }
    else
    {
        if (fDirDiff <= 0.0f)
        {
            EnemyChr().Compositor().ChangeMotion(m_szTurnR);
            m_fRadPerFrame = -1.0f;
        }
        else
        {
            EnemyChr().Compositor().ChangeMotion(m_szTurnL);
            m_fRadPerFrame = 1.0f;
        };

        float fAngleRatio = (MATH_DEG2RAD(m_fBaseTurnAngle) * MATH_INV_PI);
        float fRotRate = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;

        EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
        EnemyChr().Compositor().SetMotionSpeed(fAngleRatio * fRotRate);

        float endT = EnemyChr().Compositor().GetMotionEndTime();
        float speedT = (endT / (fAngleRatio * fRotRate));

        float fTurnAngle = MATH_RAD2DEG(fDirDiffAbs) / static_cast<float>(m_iTurnCount);
        m_fRadPerFrame *= MATH_DEG2RAD(fTurnAngle) / (speedT / CGameProperty::GetElapsedTime());
    };
};


/*virtual*/ CBaseGeneralEnemyChr::CTurnStatusObserver::RESULT
CBaseGeneralEnemyChr::CTurnStatusObserver::Observing(void) /*override*/
{
    if (!IsRotation())
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return RESULT_END;
    };

    float fDir = EnemyChr().Compositor().GetDirection();
    fDir += m_fRadPerFrame;
    EnemyChr().Compositor().SetDirection(fDir);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGeneralEnemyChr::CTurnStatusObserver::OnEnd(void) /*override*/
{
    //EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
	EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
	EnemyChr().Compositor().SetMotionSpeed(1.0f);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ bool CBaseGeneralEnemyChr::CTurnStatusObserver::IsRotation(void)
{
    float fDirection = EnemyChr().Compositor().GetDirection();

    float fDiff = (m_fDirection - fDirection);
    fDiff = std::fabs(fDiff);

    float fRadPerFrame = m_fRadPerFrame;
    fRadPerFrame = std::fabs(fRadPerFrame);

    if (fDiff <= fRadPerFrame)
        return false;

    float nowT = EnemyChr().Compositor().GetMotionTime();
    if (nowT < m_fBeforeMotionTime)
        --m_iTurnCount;

    m_fBeforeMotionTime = nowT;

    return (m_iTurnCount > 0);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseGeneralEnemyChr::CDummyTurnStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::WALK);

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    RwV3d vecAt = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vecAt);
	vecAt.y = 0.0f;

    m_fDirection = CEnemyUtils::GetDirection(&vecFootPos, &vecAt);

    if (Math::FEqual(vecFootPos.x, vecAt.x) &&
        Math::FEqual(vecFootPos.z, vecAt.z))
        m_fDirection = EnemyChr().Compositor().GetDirection();
};


/*virtual*/ CBaseGeneralEnemyChr::CDummyTurnStatusObserver::RESULT
CBaseGeneralEnemyChr::CDummyTurnStatusObserver::Observing(void) /*override*/
{
    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    if (order == CAIThinkOrder::ORDER_MOVE)
    {
        int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
        int32 moveId = AIOT::Get(moveType);

        if (moveId != AIOT::MoveTurn)
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
            return RESULT_END;
        };
    }
    else if (order != CAIThinkOrder::ORDER_RUN)
    {
        return RESULT_END;
    };

    float fRotateRate = EnemyChr().Feature().m_fRotateRate;
    EnemyChr().Compositor().RotateDirection(m_fDirection, fRotateRate);
	
    float fDirEnd = (fRotateRate * MATH_PI) * CGameProperty::GetElapsedTime();
	
    float fDirMe = EnemyChr().Compositor().GetDirection();
    float fDirTarget = m_fDirection;
	
    float fDirDiff = (fDirMe - fDirTarget);
    fDirDiff = std::fabs(fDirDiff);
	
    if (fDirDiff > fDirEnd)
        return RESULT_CONTINUE;
    
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);    
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGeneralEnemyChr::CDummyTurnStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseGeneralEnemyChr::CSideWalkStatusObserver::CSideWalkStatusObserver(float fMotionSpeed, const char* pszMotionSideWalk)
: m_order(CAIThinkOrder::ORDER_NOTHING)
, m_fMoveSpeed(0.0f)
, m_fMoveTime(0.0f)
, m_fMotionSpeed(fMotionSpeed)
, m_szSideWalkMotion()
{
    ASSERT(pszMotionSideWalk != nullptr);
    ASSERT(std::strlen(pszMotionSideWalk) < sizeof(m_szSideWalkMotion));

    std::strcpy(m_szSideWalkMotion, pszMotionSideWalk);
};


/*virtual*/ void CBaseGeneralEnemyChr::CSideWalkStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().Compositor().SetMotionSpeed(m_fMotionSpeed);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

    ENEMYTYPES::STATUS currentStatus = Status();
    ASSERT((currentStatus == ENEMYTYPES::STATUS_WALK_LEFT) ||
           (currentStatus == ENEMYTYPES::STATUS_WALK_RIGHT));
      
    EnemyChr().Compositor().ChangeMotion(m_szSideWalkMotion);

    m_order = EnemyChr().AIThinkOrder().GetOrder();
    m_fMoveSpeed = EnemyChr().CharacterParameter().m_feature.m_fWalkMoveSpeed;

    float fRand = Math::RandFloatRange(0.5f, 1.0f);
    float fMoveTimer = EnemyChr().AIThinkOrder().OrderMove().m_fMoveTimer;
    m_fMoveTime = fRand * fMoveTimer;
};


/*virtual*/ CBaseGeneralEnemyChr::CSideWalkStatusObserver::RESULT
CBaseGeneralEnemyChr::CSideWalkStatusObserver::Observing(void) /*override*/
{
    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    if (order != m_order)
        return RESULT_END;

    m_fMoveTime -= CGameProperty::GetElapsedTime();
    if (m_fMoveTime < 0.0f)
        return RESULT_END;

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecPos);
    vecPos.y = 0.0f;

    RwV3d vecAt = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vecAt);
    vecAt.y = 0.0f;

    float fDir = CEnemyUtils::GetDirection(&vecPos, &vecAt);
    float fRotRate = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
    EnemyChr().Compositor().RotateDirection(fDir, fRotRate);

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    
    ENEMYTYPES::STATUS currentStatus = Status();
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_WALK_LEFT:
        vecVelocity = { m_fMoveSpeed, 0.0f, m_fMoveSpeed * 0.2f };
        break;

    case ENEMYTYPES::STATUS_WALK_RIGHT:
        vecVelocity = { -m_fMoveSpeed, 0.0f, m_fMoveSpeed * 0.2f };
        break;

    default:
        ASSERT(false);
        break;
    };

    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGeneralEnemyChr::CSideWalkStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetMotionSpeed(1.0f);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseGeneralEnemyChr::CGuardStatusObserver::CGuardStatusObserver(float fGuardTime)
: m_orgOrder(CAIThinkOrder::ORDER_NOTHING)
, m_fGuardTime(fGuardTime)
, m_fRemainGuardTime(0.0f)
{
    ;
};


/*virtual*/ void CBaseGeneralEnemyChr::CGuardStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    MotionRegist(MOTIONID_GUARD_BEGIN, ENEMYTYPES::MOTIONNAMES::GUARD_1);
    MotionRegist(MOTIONID_GUARD_HOLD,  ENEMYTYPES::MOTIONNAMES::GUARD_2);
    MotionRegist(MOTIONID_GUARD_END,   ENEMYTYPES::MOTIONNAMES::GUARD_3);
    MotionPlay(MOTIONID_GUARD_BEGIN, true);

    m_orgOrder = EnemyChr().AIThinkOrder().GetOrder();
    m_fRemainGuardTime = m_fGuardTime;
};


/*virtual*/ CBaseGeneralEnemyChr::CGuardStatusObserver::RESULT
CBaseGeneralEnemyChr::CGuardStatusObserver::Observing(void) /*override*/
{
    m_fRemainGuardTime -= CGameProperty::GetElapsedTime();

    if (MotionCurrent() == MOTIONID_GUARD_HOLD)
    {
        if ((m_fRemainGuardTime < 0.0f) || (EnemyChr().AIThinkOrder().GetOrder() != m_orgOrder))
            MotionPlay(MOTIONID_GUARD_END, true);
    };

    if (IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGeneralEnemyChr::CGuardStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseGeneralEnemyChr::CDeathMechStatusObserver::CDeathMechStatusObserver(float fSparkPlayTime)
: m_fSparkPlayTime(fSparkPlayTime)
, m_fTimer(0.0f)
{
    ;
};


/*virtual*/ void CBaseGeneralEnemyChr::CDeathMechStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);

    m_fTimer = m_fSparkPlayTime;

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4337));
};


/*virtual*/ CBaseGeneralEnemyChr::CDeathMechStatusObserver::RESULT
CBaseGeneralEnemyChr::CDeathMechStatusObserver::Observing(void) /*override*/
{
    m_fTimer -= CGameProperty::GetElapsedTime();
    if (m_fTimer <= 0.0f)
        return RESULT_END;

    float fElapsedRate = (m_fTimer / m_fSparkPlayTime);
    float fRand = Math::RandFloat();

    if (fElapsedRate < fRand)
    {
        RwV3d vecPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetPosition(&vecPos);

        float fRadius = EnemyChr().Compositor().GetCollisionParameter().m_fRadius * 1.5f;

        float fLow  = -fRadius;
        float fHigh =  fRadius;

        vecPos.x += Math::RandFloatRange(fLow, fHigh);
        vecPos.z += Math::RandFloatRange(fLow, fHigh);        
        vecPos.y += Math::RandFloatRange(0.0f, fHigh * 0.5f);

        /* effect may be NULL here (for example at 4 round of nexus tournament (KITTY)) */
        uint32 hEffect = CEffectManager::Play(EFFECTID::ID_SPARKS, &vecPos);
        if (hEffect)
        {
            RwMatrix matRot;
            RwMatrixSetIdentity(&matRot);
            Math::Matrix_RotationYawPitchRoll(&matRot, 0.0f, -MATH_DEG2RAD(90.0f), 0.0f);

            CEffectManager::SetDirection(hEffect, &matRot);
            CEffectManager::SetScale(hEffect, 0.4f);
        };
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGeneralEnemyChr::CDeathMechStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vPosition);

    EFFECT_GENERIC::CallDeathEffect(EFFECT_GENERIC::DEATHTYPE_MACHINE, &vPosition);
	
	return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


/*static*/ void CBaseGeneralEnemyChr::GetMoveAt(CAIThinkOrder& AIThinkOrder, RwV3d* at)
{
    CAIThinkOrder::ORDER order = AIThinkOrder.GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = AIThinkOrder.OrderMove().m_iMoveType;
            int32 targetFlag = AIOT::GetTarget(moveType);

            if (targetFlag)
            {
                ASSERT(targetFlag == AIOT::TargetNo);

                int32 targetNo = AIThinkOrder.OrderMove().m_iPlayerNo;

                CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(targetNo);
                if (pPlayerChr)
                    pPlayerChr->GetFootPosition(at);
            }
            else
            {
                *at = AIThinkOrder.OrderMove().m_vAt;
            };
        }
        break;

    case CAIThinkOrder::ORDER_RUN:
        {
            *at = AIThinkOrder.OrderRun().m_vAt;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


CBaseGeneralEnemyChr::CBaseGeneralEnemyChr(ENEMYID::VALUE idEnemy)
: CEnemyCharacter(idEnemy)
, m_iLastAttackerNo(-1)
, m_aiDamageAccumulation()
, m_vPrePosition(Math::VECTOR3_ZERO)
{
    std::memset(m_aiDamageAccumulation, 0, sizeof(m_aiDamageAccumulation));
};


/*virtual*/ CBaseGeneralEnemyChr::~CBaseGeneralEnemyChr(void)
{
    ;
};


/*virtual*/ void CBaseGeneralEnemyChr::Run(void) /*override*/
{
    RwV3d vPrePosition = Math::VECTOR3_ZERO;
    Compositor().GetFootPosition(&vPrePosition);

    CEnemyCharacter::Run();

    m_vPrePosition = vPrePosition;
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
CBaseGeneralEnemyChr::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    CGameObject* pAttacker = rCalc.GetAttacker();
    if (pAttacker && (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER))
    {
        m_iLastAttackerNo = -1;

        int32 playerNum = CGameProperty::GetPlayerNum();
        for (int32 i = 0; i < playerNum; ++i)
        {
            if (CAIUtils::GetActivePlayer(i) == pAttacker)
            {
                m_iLastAttackerNo = i;
                break;
            };
        };
    };

    return CEnemyCharacter::OnDamage(rCalc);
};


/*virtual*/ void CBaseGeneralEnemyChr::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    if (m_iLastAttackerNo >= 0)
        m_aiDamageAccumulation[m_iLastAttackerNo] += iAmount;

    CEnemyCharacter::OnMessageReceivedDamage(iAmount);
};


/*virtual*/ int32 CBaseGeneralEnemyChr::GetDamageAccumulation(int32 no) const
{
    ASSERT(no >= 0);
    ASSERT(no < COUNT_OF(m_aiDamageAccumulation));

    return m_aiDamageAccumulation[no];
};


/*virtual*/ CBaseGeneralEnemyChr::MOVABLETYPE
CBaseGeneralEnemyChr::GetMovableType(RwV3d* pVecAt) const
{
    RwV3d vecAt = *pVecAt;
    vecAt.y += 0.2f;

    RwV3d vecPos = Math::VECTOR3_ZERO;
    Compositor().GetFootPosition(&vecPos);
    vecPos.y += 0.2f;

    MOVABLETYPE movtype = MOVABLETYPE_POSSIBLE;

    if (Math::Vec3_FEqual(&vecPos, &vecAt))
    {
        movtype = MOVABLETYPE_POSSIBLE;
    }
    else
    {
        float fRadius = Compositor().GetCollisionParameter().m_fRadius;
        float fJumpInitSpeed = Feature().m_fJumpInitializeSpeed;

        CAIUtils::CHECKMOVELINE_RESULT result = CAIUtils::CheckMoveLine(&vecPos,
                                                                        &vecAt,
                                                                        (fRadius + fRadius),
                                                                        10,
                                                                        fJumpInitSpeed);
        switch (result)
        {
        case CAIUtils::CHECKMOVELINE_RESULT_NEEDJUMP:
            {
                movtype = MOVABLETYPE_JUMP;
            }
            break;

        case CAIUtils::CHECKMOVELINE_RESULT_FAILURE:
            {
                if (CAIUtils::CheckMoveLine_FindHole())
                    movtype = MOVABLETYPE_HOLE;
                else
                    movtype = MOVABLETYPE_IMPOSSIBLE;
            }
            break;

        case CAIUtils::CHECKMOVELINE_RESULT_PERMISSION:
        default:
            {
                float h1 = CWorldMap::GetMapHeight(&vecPos, fRadius);
                float h2 = CWorldMap::GetMapHeight(&vecAt, fRadius);

                float fHeight = (h1 - h2);

                float fGravity = CGameProperty::GetGravity();
                float fJumpHeight = CEnemyUtils::GetJumpableHeight(fJumpInitSpeed, fGravity);

                if (fJumpHeight < fHeight)
                    movtype = MOVABLETYPE_FALLDOWN;
                else
                    movtype = MOVABLETYPE_POSSIBLE;
            }
            break;
        };
    };

    return movtype;
};


/*virtual*/ bool CBaseGeneralEnemyChr::IsStayTrajectory(CPlayerCharacter* pPlayerChr, float fRange)
{
    if (Math::FEqual(fRange, 0.0f))
        return false;

    /* init sphere */
    RwSphere sphere = { 0 };
    sphere.radius = pPlayerChr->GetCollisionParameter().m_fRadius;
    pPlayerChr->GetBodyPosition(&sphere.center);

    /* init line */
    RwLine line = { 0 };
    Compositor().GetBodyPosition(&line.start);

    line.end = { 0.0f, 0.0f, fRange };
    Compositor().RotateVectorByDirection(&line.end, &line.end);

    line.end.x += line.start.x;
    line.end.y = sphere.center.y;
    line.end.z += line.start.z;

    return Intersection::LineAndSphere(&line, &sphere);
};