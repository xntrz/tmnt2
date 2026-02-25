#include "BaseEliteChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseEliteAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


/*virtual*/ void CBaseEliteChr::CAppearStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
};


/*virtual*/ CBaseEliteChr::CAppearStatusObserver::RESULT
CBaseEliteChr::CAppearStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CAppearStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteChr::CHideStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CBaseEliteChr::CHideStatusObserver::RESULT
CBaseEliteChr::CHideStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CHideStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteChr::CDeathStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);
    EnemyChr().Compositor().SetEnableCatchHit(false);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
};


/*virtual*/ CBaseEliteChr::CDeathStatusObserver::RESULT
CBaseEliteChr::CDeathStatusObserver::Observing(void) /*override*/
{
    bool bIsNexusMap = CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                       CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X);
    if (bIsNexusMap)
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CDeathStatusObserver::OnEnd(void) /*override*/
{
    CEnemyUtils::EntryNexusDeathEffect(&EnemyChr(), -1.0f);
    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteChr::CIdleStatusObserver::OnStart(void) /*override*/
{
    m_fWaitTime = EnemyChr().AIThinkOrder().OrderWait().m_fWaitTimer;

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
};


/*virtual*/ CBaseEliteChr::CIdleStatusObserver::RESULT
CBaseEliteChr::CIdleStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_WAIT)
        return RESULT_END;

    if (EnemyChr().AIThinkOrder().OrderWait().m_iWaitType == ELITEAI::ORDERTYPE_WAIT_TIMEOUT)
    {
        m_fWaitTime -= CGameProperty::GetElapsedTime();
        if (m_fWaitTime > 0.0f)            
            return RESULT_CONTINUE;
    };
    
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    if (EnemyChr().AIThinkOrder().OrderWait().m_iWaitType != ELITEAI::ORDERTYPE_WAIT_IDLE)
    {
        CEnemyUtilsElite::RotateToPlayer(&EnemyChr().Compositor(),
                                         EnemyChr().AIThinkOrder().OrderWait().m_iPlayerNo,
                                         EnemyChr().Feature().m_fRotateRate);
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CIdleStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteChr::CWalkStatusObserver::OnStart(void) /*override*/
{
    m_moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    switch (m_moveType)
    {
    case ELITEAI::ORDERTYPE_MOVE_WALK:
        {
            m_fMoveSpeed = EnemyChr().Feature().m_fWalkMoveSpeed;
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::WALK, true);
        }
        break;

    case ELITEAI::ORDERTYPE_MOVE_RUN:
        {
            m_fMoveSpeed = EnemyChr().Feature().m_fRunMoveSpeed;
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::RUN, true);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


/*virtual*/ CBaseEliteChr::CWalkStatusObserver::RESULT
CBaseEliteChr::CWalkStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_MOVE)
        return RESULT_END;

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    m_fMoveTime -= CGameProperty::GetElapsedTime();
    if (m_fMoveTime < 0.0f)
    {
        int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
        if (moveType != m_moveType)
        {
            m_moveType = moveType;

            switch (m_moveType)
            {
            case ELITEAI::ORDERTYPE_MOVE_WALK:
                {
                    m_fMoveSpeed = EnemyChr().Feature().m_fWalkMoveSpeed;
                    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::WALK, true);
                }
                break;

            case ELITEAI::ORDERTYPE_MOVE_RUN:
                {
                    m_fMoveSpeed = EnemyChr().Feature().m_fRunMoveSpeed;
                    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::RUN, true);
                    m_fMoveTime = 3.0f;
                }
                break;

            default:
                return RESULT_END;
            };
        };
    };

    CEnemyUtilsElite::MoveTo(&EnemyChr().Compositor(),
                             &EnemyChr().AIThinkOrder().OrderMove().m_vAt,
                             m_fMoveSpeed,
                             EnemyChr().Feature().m_fRotateRate);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CWalkStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteChr::CDashStatusObserver::OnStart(void) /*override*/
{
    m_fMoveTime = EnemyChr().Feature().m_fDashMoveTime;
    m_dashType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    m_step = 0;

    float fDirDashStrafe = 0.0f;
    switch (m_dashType)
    {
    case ELITEAI::ORDERTYPE_MOVE_DASH:
        {
            fDirDashStrafe = 0.0f;
            m_pszMotionNext = ENEMYTYPES::MOTIONNAMES::DASH2;
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DASH1, true);
        }
        break;

    case ELITEAI::ORDERTYPE_MOVE_DASH_LEFT:
        {
            fDirDashStrafe = MATH_DEG2RAD(45.0f);
            m_pszMotionNext = ENEMYTYPES::MOTIONNAMES::DASH_LEFT_2;
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DASH_LEFT_1, true);
        }
        break;

    case ELITEAI::ORDERTYPE_MOVE_DASH_RIGHT:
        {
            fDirDashStrafe = -MATH_DEG2RAD(45.0f);
            m_pszMotionNext = ENEMYTYPES::MOTIONNAMES::DASH_RIGHT_2;
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DASH_RIGHT_1, true);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
    
    float fDir = EnemyChr().Compositor().GetDirection();
    int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(orderTargetNo);
    if (pPlrChr)
    {
        fDir = CEnemyUtilsElite::RotateToPlayer(&EnemyChr().Compositor(), orderTargetNo);

        float fDirToDashStrafe = (EnemyChr().Compositor().GetDirection() - fDirDashStrafe);
        EnemyChr().Compositor().SetDirection(fDirToDashStrafe);
    };

    RwV3d vecVelocity = { 0.0f, 0.0f, EnemyChr().Feature().m_fDashMoveSpeed };
    CEnemyUtils::RotateVectorByDirection(&vecVelocity, fDir + fDirDashStrafe);
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    if (m_dashType == ELITEAI::ORDERTYPE_MOVE_DASH)
    {
        CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH1,
                                  new CEnemyTracer(&EnemyChr()),
                                  &Math::VECTOR3_ZERO,
                                  EnemyChr().Compositor().GetDirection());
    };

    CEffectManager::PlayTrace(EFFECTID::ID_ALL_DASH2,
                              new CEnemyTracer(&EnemyChr()),
                              &Math::VECTOR3_ZERO,
                              EnemyChr().Compositor().GetDirection());

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ CBaseEliteChr::CDashStatusObserver::RESULT
CBaseEliteChr::CDashStatusObserver::Observing(void) /*override*/
{
    ASSERT(EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_MOVE);
    
    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            if (ProcDashTimer())
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
                EnemyChr().Compositor().ChangeMotion(m_pszMotionNext, true);
                ++m_step;
            };
        }
        break;

    case 1:
        {
            if (ProcDashTimer())
            {
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
                EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
                EnemyChr().Compositor().ChangeMotion(m_pszMotionNext, true);
                ++m_step;
            };
        }
        break;

    case 2:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (CEnemyUtilsElite::IsCrashWallPossible(&EnemyChr().Compositor()))
    {
        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CDashStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


bool CBaseEliteChr::CDashStatusObserver::ProcDashTimer(void)
{
    ASSERT(m_step >= 0);
    ASSERT(m_step <  2);

    m_fMoveTime -= CGameProperty::GetElapsedTime();

    if (m_fMoveTime <= 0.0f)
        return true;

    return false;
};


//
// *********************************************************************************
//


CBaseEliteChr::CWarpBaseStatusObserver::CWarpBaseStatusObserver(WARPTYPE warpType)
: CBaseEliteChr::CWarpBaseStatusObserver(0.2f, warpType)
{
    ;
};


CBaseEliteChr::CWarpBaseStatusObserver::CWarpBaseStatusObserver(float fEndTime, WARPTYPE warpType)
: m_warpType(warpType)
, m_fEndTime(fEndTime)
, m_fTime(0.0f)
, m_p1(Math::VECTOR3_ZERO)
, m_p2(Math::VECTOR3_ZERO)
, m_p3(Math::VECTOR3_ZERO)
, m_vecTargetPos(Math::VECTOR3_ZERO)
, m_fTargetRot(0.0f)
, m_vecBezierEndPos(Math::VECTOR3_ZERO)
, m_orderTargetNo(-1)
{
    ;
};


/*virtual*/ void CBaseEliteChr::CWarpBaseStatusObserver::OnStart(void) /*override*/
{
    m_fTime = 0.0f;

    CreateBezierPoint(&m_vecTargetPos, m_fTargetRot);

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ);

    float fDashMoveSpeed = EnemyChr().Feature().m_fDashMoveSpeed;
    EnemyChr().Compositor().SetMaxVelocity(fDashMoveSpeed * 6.0f);
};


/*virtual*/ CBaseEliteChr::CWarpBaseStatusObserver::RESULT
CBaseEliteChr::CWarpBaseStatusObserver::Observing(void) /*override*/
{
    float dt = CGameProperty::GetElapsedTime();
    if (dt <= 0.0f)
        dt = 0.001f;

    m_fTime += dt;
    float t = (m_fTime / m_fEndTime);

    RwV3d vecBezierNowPos = Math::VECTOR3_ZERO;
    CEnemyUtils::BezierCalc(&vecBezierNowPos, &m_p1, &m_p2, &m_p3, t);

    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

    RwV3d vecNewVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecNewVelocity, &vecBezierNowPos, &vecMyFootPos);
    Math::Vec3_Scale(&vecNewVelocity, &vecNewVelocity, (1.0f / dt));

    vecNewVelocity.y = EnemyChr().Compositor().GetVelocity().y;
    EnemyChr().Compositor().SetVelocity(&vecNewVelocity);

    if (CEnemyUtilsElite::IsCrashWallPossible(&EnemyChr().Compositor()))
        return RESULT_END;

    RwV3d vecMoveDir = vecNewVelocity;
    Math::Vec3_Normalize(&vecMoveDir, &vecMoveDir);
    vecMoveDir.y = 0.0f;

    float fDir = GetThetaVec2Vec(&vecMoveDir, &Math::VECTOR3_AXIS_Z);
    if (vecNewVelocity.x < 0.0f)
        fDir *= -1.0f;
    EnemyChr().Compositor().SetDirection(fDir);

    if (t <= 0.5f)
    {
        m_p3 = m_vecBezierEndPos;
        m_p3.x += std::sin(m_fTargetRot + MATH_PI);
        m_p3.y = 0.0f;
        m_p3.z += std::cos(m_fTargetRot + MATH_PI);
    }
    else
    {
        float fDirToEndPos = CEnemyUtils::GetDirection(&vecMyFootPos, &m_vecBezierEndPos);
        float fRotRate = EnemyChr().Feature().m_fRotateRate;
        EnemyChr().Compositor().RotateDirection(fDirToEndPos, fRotRate * 10.0f);
    };

    if (t >= 1.0f)
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CWarpBaseStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
    if (pPlrChr)
        pPlrChr->GetFootPosition(&m_vecTargetPos);

    CEnemyUtilsElite::RotateToTarget(&EnemyChr().Compositor(), &m_vecTargetPos);

    return ENEMYTYPES::STATUS_THINKING;
};


void CBaseEliteChr::CWarpBaseStatusObserver::CreateBezierPoint(const RwV3d* pvecTargetPos, float fTargetRot)
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

    float fDistToTarget = CEnemyUtils::GetDistance(&vecMyFootPos, pvecTargetPos);
    fDistToTarget *= 0.6f;
    fDistToTarget = Clamp(fDistToTarget, 5.0f, 10.0f);

    float fInterRot = fTargetRot;
    if ((CWorldMap::GetMapHeight(&m_p2) <= CWorldMap::INVALID_MAP_HEIGHT) || (m_warpType == WARPTYPE_RIGHT))
        fInterRot += -MATH_DEG2RAD(100.0f);
    else
        fInterRot +=  MATH_DEG2RAD(100.0f);
    
    m_p1 = vecMyFootPos;
    m_p1.y = 0.0f;

    m_p2 = *pvecTargetPos;
    m_p2.x += (std::sin(fInterRot) * fDistToTarget);
    m_p2.y = 0.0f;
    m_p2.z += (std::cos(fInterRot) * fDistToTarget);

    m_p3 = *pvecTargetPos;
    m_p3.x += std::sin(fTargetRot + MATH_PI);
    m_p3.y = 0.0f;
    m_p3.z += std::cos(fTargetRot + MATH_PI);
};


void CBaseEliteChr::CWarpBaseStatusObserver::SetPlayerNo(int32 playerNo)
{
    m_orderTargetNo = playerNo;

    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
    if (pPlrChr)
    {
        pPlrChr->GetFootPosition(&m_vecTargetPos);
        m_fTargetRot = pPlrChr->GetDirection();
        m_vecBezierEndPos = m_vecTargetPos;
    }
    else
    {
        m_orderTargetNo = -1;
    };
};


void CBaseEliteChr::CWarpBaseStatusObserver::SetTargetPos(const RwV3d* pvecTargetPos)
{
    m_vecTargetPos = *pvecTargetPos;
    m_vecBezierEndPos = *pvecTargetPos;
    m_fTargetRot = EnemyChr().Compositor().GetDirection();

    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
    if (pPlrChr)
    {
        m_fTargetRot = pPlrChr->GetDirection();

        RwMatrix matRotY;
        RwMatrixSetIdentity(&matRotY);
        Math::Matrix_RotateY(&matRotY, m_fTargetRot);

        RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecPlrFootPos);

        RwV3dTransformVector(&m_vecTargetPos, pvecTargetPos, &matRotY);
        Math::Vec3_Add(&m_vecTargetPos, &m_vecTargetPos, &vecPlrFootPos);

        m_vecBezierEndPos = m_vecTargetPos;
    };
};


float CBaseEliteChr::CWarpBaseStatusObserver::GetThetaVec2Vec(const RwV3d* v1, const RwV3d* v2) const
{
    float cos = Math::Vec3_Dot(v1, v2) / (Math::Vec3_Length(v1) * Math::Vec3_Length(v2));
    return Math::ACos(cos);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteChr::CGuardStatusObserver::OnStart(void) /*override*/
{
    m_apszGuardMotion[0] = ENEMYTYPES::MOTIONNAMES::GUARD_2;
    m_apszGuardMotion[1] = ENEMYTYPES::MOTIONNAMES::GUARD_3;
    m_step = 0;
    m_fDefenceTime = 0.7f;
    m_defenceType = EnemyChr().AIThinkOrder().OrderDefence().m_iDefenceType;

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_1, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ CBaseEliteChr::CGuardStatusObserver::RESULT
CBaseEliteChr::CGuardStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            EnemyChr().Compositor().ChangeMotion(m_apszGuardMotion[m_step], true);
            ++m_step;
        }
        break;

    case 1:
        {
            m_fDefenceTime -= CGameProperty::GetElapsedTime();

            if (EnemyChr().AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_DEFENCE)
            {
                if (m_defenceType == ELITEAI::ORDERTYPE_DEFENCE_LONG)
                    m_fDefenceTime = 0.5f;
                else
                    m_fDefenceTime = 0.1f;

                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            };

            if (m_fDefenceTime < 0.0f)
            {
                EnemyChr().Compositor().ChangeMotion(m_apszGuardMotion[m_step], true);
                ++m_step;
            };
        }
        break;

    case 2:
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


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CGuardStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ ENEMYTYPES::STATUS CBaseEliteChr::CJumpReadyStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    vecVelocity.y = EnemyChr().Feature().m_fJumpInitializeSpeed;
    vecVelocity.z = EnemyChr().Feature().m_fWalkMoveSpeed;
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


/*virtual*/ CBaseEliteChr::CKnockStatusObserver::RESULT
CBaseEliteChr::CKnockStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().TestFlag(ENEMYTYPES::FLAG_INVINCIBILITY))
        return RESULT_END;

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);

    return CCommonEnemyObserver::CKnockStatus::Observing();
};


//
// *********************************************************************************
//


CBaseEliteChr::CBaseEliteChr(ENEMYID::VALUE enemyId)
: CEnemyCharacter(enemyId)
, m_knockBackCtrl()
{
    SetFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE);

    m_knockBackCtrl.Initialize(this, -1, -1.0f);
};


/*virtual*/ CBaseEliteChr::~CBaseEliteChr(void)
{
    ;
};


/*virtual*/ void CBaseEliteChr::Run(void) /*override*/
{
    m_knockBackCtrl.Period();
    CEnemyCharacter::Run();
};


/*virtual*/ void CBaseEliteChr::OnMessageReceivedDamage(int32 iAmount) /*override*/
{
    m_knockBackCtrl.ReceivedDamage(iAmount);
    CEnemyCharacter::OnMessageReceivedDamage(iAmount);
};


CBaseEliteAI& CBaseEliteChr::AIModerator(void)
{
    return static_cast<CBaseEliteAI&>(CEnemyCharacter::AIModerator());
};


const CBaseEliteAI& CBaseEliteChr::AIModerator(void) const
{
    return static_cast<const CBaseEliteAI&>(CEnemyCharacter::AIModerator());
};