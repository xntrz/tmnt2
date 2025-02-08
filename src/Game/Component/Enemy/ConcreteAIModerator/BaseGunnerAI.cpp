#include "BaseGunnerAI.hpp"

#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseGrapplerEnemyChr.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Map/WorldMap.hpp"


/*static*/ CAIModerator* CBaseGunnerAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseGunnerAI(pEnemyChr);
};


CBaseGunnerAI::CBaseGunnerAI(CEnemyCharacter* pEnemyChr)
: CBaseGeneralEnemyAIModerator(pEnemyChr)
, m_fFireTimeout(0.0f)
{
    int32 playerNum = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfGeometry);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfTargetDamaged);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfTurtleCharacteristic);
    };
};


/*virtual*/ CBaseGunnerAI::~CBaseGunnerAI(void)
{
    ;
};


/*virtual*/ CAIThinkOrder::ORDER CBaseGunnerAI::ActionSelector(int32 iNo) /*override*/
{
    if (!CAIUtils::GetActivePlayer(iNo))
        return CAIThinkOrder::ORDER_WAIT;

    if (IsTakeRun())
        return CAIThinkOrder::ORDER_RUN;

    if (IsTakeFire())
    {
		if (m_fFireTimeout > 0.0f)
            return CAIThinkOrder::ORDER_WAIT;

		return CAIThinkOrder::ORDER_ATTACK;
    };

    if ((ThinkOrder().GetOrder()  == CAIThinkOrder::ORDER_MOVE) &&
        (ThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE))
        return CAIThinkOrder::ORDER_WAIT;

    return CAIThinkOrder::ORDER_MOVE;
};


/*virtual*/ bool CBaseGunnerAI::OnActionOfWait(int32 iNo) /*override*/
{
    float fThinkFreq = (Characteristic().m_fThinkingFrequency * 2.0f);
    AIOT::SetWaitOrder(ThinkOrder(), fThinkFreq);

    if ((ThinkOrder().GetOrder()  == CAIThinkOrder::ORDER_MOVE) &&
        (ThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE))
    {
        return true;
    };

    float fRandZ = Math::RandFloatRange(-1.5f, 1.5f);
    float fRandX = Math::RandFloatRange(-1.5f, 1.5f);
    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;

    RwV3d vecAt = { fDistanceOfSuitable * fRandX, 0.0f, fDistanceOfSuitable * fRandZ };

    float fRatioOfMove = Characteristic().m_fRatioOfMove;
    Math::Vec3_Scale(&vecAt, &vecAt, fRatioOfMove + 0.5f);

    EnemyCharacter().Compositor().RotateVectorByDirection(&vecAt, &vecAt);

    RwV3d vPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetPosition(&vPos);

    Math::Vec3_Add(&vecAt, &vecAt, &vPos);
    vecAt.y = 0.0f;

    float fRadiusOfAction = Characteristic().m_fRadiusOfAction;
	RwV3d vecPos = vecAt;
    float fDist = CAIUtils::CalcMovePointEscapeObstacle(&vecPos, &EnemyCharacter(), &vecPos, 1.0f, fRadiusOfAction);

    float fRand = Math::RandFloat();
    if ((fRand >= fRatioOfMove) || (fDist < 0.0f))
    {
        float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
        fRand = Math::RandFloat();
        if (fRand >= fRatioOfActivity)
            return true;

        if (iNo > -1)
            AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveTurn, 1.0f, iNo);
        else
            AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveTurn, 1.0f, &vecPos);

        return true;
    };

    if (iNo <= -1)
    {
        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecPos);
        return true;
    };

    AIOT::TYPE sidewalk = IsTakeSideWalk();
    if (sidewalk != AIOT::Null)
        AIOT::SetMoveOrder(ThinkOrder(), sidewalk, 1.0f, iNo);
    else
        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveTurn, 1.0f, iNo);

    return true;
};


/*virtual*/ bool CBaseGunnerAI::OnActionOfMove(int32 iNo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerChr)
        return false;

    float fRatioOfMove = Characteristic().m_fRatioOfMove;
    float fRand = Math::RandFloat();
    if (fRand > fRatioOfMove)
    {
        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveTurn, 1.0f, iNo);
        return true;
    };

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);
    vecFootPosPlayer.y = 0.0f;

    float fRadiusOfAction = Characteristic().m_fRadiusOfAction;

    RwV3d vecPos = vecFootPosPlayer;
    float fDist = CAIUtils::CalcMovePointEscapeObstacle(&vecPos, &EnemyCharacter(), &vecPos, 1.0f, fRadiusOfAction);

    if (fDist < 0.0f)
        return false;

    if (fDist >= fRadiusOfAction)
        return false;

    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
    float fTargetDist = m_targetInfo.GetDistance();
    if (fTargetDist > fDistanceOfSuitable)
    {
        float fRatioOfAcitivity = Characteristic().m_fRatioOfActivity;

        if (((fRadiusOfAction * 0.5f) <= fDist) || (Math::RandFloat() < fRatioOfAcitivity))
        {
            AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveRun, 1.0f, iNo);
        }
        else
        {
            AIOT::TYPE sidewalk = IsTakeSideWalk();
            if (sidewalk != AIOT::Null)
                AIOT::SetMoveOrder(ThinkOrder(), sidewalk, 1.0f, iNo);
            else
                AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, iNo);
        };

        return true;
    };

    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveTurn, 1.0f, iNo);

    return true;
};


/*virtual*/ bool CBaseGunnerAI::OnActionOfRun(int32 iNo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);
    if (pPlayerChr == nullptr)
        return false;

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);
    vecFootPosPlayer.y = 0.0f;

    RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecFootPosEnemy);
    vecFootPosEnemy.y = 0.0f;

    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;

    RwV3d vecAt = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecAt, &vecFootPosPlayer, &vecFootPosEnemy);
    Math::Vec3_Normalize(&vecAt, &vecAt);
    Math::Vec3_Negate(&vecAt, &vecAt);
    Math::Vec3_Scale(&vecAt, &vecAt, fDistanceOfSuitable);
    Math::Vec3_Add(&vecAt, &vecAt, &vecFootPosEnemy);

    float fRadiusOfAction = Characteristic().m_fRadiusOfAction;

    RwV3d vecPos = vecAt;
    float fDist = CAIUtils::CalcMovePointEscapeObstacle(&vecPos, &EnemyCharacter(), &vecPos, 1.0f, fRadiusOfAction);

    if (fDist < 0.0f)
        return false;

    if (fDist > fRadiusOfAction)
        return false;
    
    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveRun, 1.0f, &vecPos);
    m_targetInfo.Reset();

    return true;
};


/*virtual*/ bool CBaseGunnerAI::OnActionOfAttack(int32 iNo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);

    if (pPlayerChr == nullptr)
        return false;

    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE))
        return false;

    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN))
        return false;

    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_UNUSUAL))
        return false;

    RwV3d vecBodyPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetBodyPosition(&vecBodyPosPlayer);

    RwV3d vecBodyPosEnemy = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetBodyPosition(&vecBodyPosEnemy);

    bool bCollision = CWorldMap::CheckCollisionLine(&vecBodyPosEnemy, &vecBodyPosPlayer);
    if (bCollision)
    {
        const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
        ASSERT(pCollisionResult != nullptr);

        MAPTYPES::ATTRIBUTE attribute = pCollisionResult->m_attribute;
        if (!(attribute & MAPTYPES::ATTRIBUTE_ONEWAY))
        {
            RwV3d vecClosestPt = pCollisionResult->m_vClosestPt;

            float fToDistPlayer = CEnemyUtils::GetDistance(&vecBodyPosEnemy, &vecBodyPosPlayer);
            float fToDistMap    = CEnemyUtils::GetDistance(&vecBodyPosEnemy, &vecClosestPt);

            if (fToDistMap < fToDistPlayer)
                return false;
        };
    };

    RwV3d vec = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vec, &vecBodyPosEnemy, &vecBodyPosPlayer);

    RwMatrix mat;
    RwMatrixSetIdentityMacro(&mat);
    Math::Matrix_RotateY(&mat, -std::atan2(vec.x, vec.z));

    RwV3dTransformVector(&vec, &vec, &mat);

    float fAngle = std::atan2(vec.y, vec.z);
    int32 attackId = AIOT::Null;

    if ((fAngle > -MATH_DEG2RAD(15.0f)) &&
        (fAngle <  MATH_DEG2RAD(15.0f)))
    {
        // middle
        if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE_RATE))
        {
            attackId = AIOT::FireB;
        }
        else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_REPEATABLE))
        {
            attackId = AIOT::FireRepeatB;
        };
    }
    else if ((fAngle > MATH_DEG2RAD(0.0f)) &&
             (fAngle < MATH_DEG2RAD(45.0f)))
    {
        // down
        if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE_RATE))
        {
            attackId = AIOT::FireC;
        }
        else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_REPEATABLE))
        {
            attackId = AIOT::FireRepeatC;
        };
    }
    else if ((fAngle > -MATH_DEG2RAD(45.0f)) &&
             (fAngle <  MATH_DEG2RAD(0.0f)))
    {
        // up
        if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE_RATE))
        {
            attackId = AIOT::FireA;
        }
        else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_REPEATABLE))
        {
            attackId = AIOT::FireRepeatA;
        };
    };

	if (attackId == AIOT::Null)
		return false;

    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    ASSERT(pMapCamera != nullptr);

    RwCamera* pCamera = pMapCamera->GetRwCamera();
    ASSERT(pCamera != nullptr);

    RwSphere sphere = { 0 };
    sphere.center = vecBodyPosEnemy;
    sphere.radius = EnemyCharacter().Compositor().GetCollisionParameter().m_fRadius;

    float fRatioOfActivity = Characteristic().m_fRatioOfActivity;

    if (RwCameraFrustumTestSphere(pCamera, &sphere) || (Math::RandFloat() <= fRatioOfActivity))
    {
        RwV3d vecAt = { 0.0f, 0.0f, -fAngle };
        AIOT::SetFireOrder(ThinkOrder(), attackId, iNo, &vecAt);

        float fFireInterval = GetFrequency(CEnemyParameter::FREQUENCY_COMMON_ATTACKINTERVAL);
        float fRand = Math::RandFloat();
        m_fFireTimeout = (fFireInterval + fRand);

        return true;
    };

    return false;
};


/*virtual*/ CBaseGunnerAI::UNDERACTION CBaseGunnerAI::OnUnderAllActions(void) /*override*/
{
    CAIThinkOrder::ORDER order = ThinkOrder().GetOrder();

    if ((order != CAIThinkOrder::ORDER_RUN) && IsOutsidePatrolArea())
        return UNDERACTIONS_THINKOVER;

    m_fFireTimeout -= CGameProperty::GetElapsedTime();

    return UNDERACTIONS_CONTINUOUS;
};


/*virtual*/ CBaseGunnerAI::UNDERACTION CBaseGunnerAI::OnUnderWait(void) /*override*/
{
    int32 targetNo = SearchTarget();

    if ((targetNo >= 0) &&
        !m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE) &&
        !m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN))
    {
        float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
        float fRand = Math::RandFloat();
        if (fRand < fRatioOfActivity)
        {
            CAIThinkOrder::ORDER order = ThinkOrder().GetOrder();
            switch (order)
            {
            case CAIThinkOrder::ORDER_WAIT:
                {
                    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                    return UNDERACTIONS_THINKOVER;
                }
                break;

            case CAIThinkOrder::ORDER_MOVE:
                {
                    int32 moveType = ThinkOrder().OrderMove().m_iMoveType;
                    int32 moveTarget = AIOT::GetTarget(moveType);

                    if (!moveTarget)
                    {
                        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                        return UNDERACTIONS_THINKOVER;
                    };
                }
                break;

            default:
                break;
            };
        };
    };

    return UNDERACTIONS_CONTINUOUS;
};


/*virtual*/ bool CBaseGunnerAI::IsMoveEndForTargetNumber(void) /*override*/
{
    float fTargetDist = m_targetInfo.GetDistance();
    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
    if (fTargetDist > fDistanceOfSuitable)
        return false;

    float fRange = GetFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE);
    if (fTargetDist > fRange)
        return false;

    float fDirMe = EnemyCharacter().Compositor().GetDirection();
    float fDirTarget = m_targetInfo.GetAngle();

    float fDirDiff = (fDirTarget - fDirMe);
    fDirDiff = std::fabs(fDirDiff);

    float fRatioOfFrontView = Characteristic().m_fRatioOfFrontView;
    float fFrontView = (fRatioOfFrontView * MATH_DEG2RAD(18.0f));
    if (fDirDiff > fFrontView)
        return false;
    
    return true;
};


/*virtual*/ bool CBaseGunnerAI::IsMoveEndForTargetPosition(void) /*override*/
{
    RwV3d vPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vPos);
    vPos.y = 0.0f;

    RwV3d vAt = ThinkOrder().OrderMove().m_vAt;
    vAt.y = 0.0f;

    float fDist = CEnemyUtils::GetDistance(&vPos, &vAt);

    float fCollRadius = EnemyCharacter().Compositor().GetCollisionParameter().m_fRadius;

    return ((fCollRadius * 2.0f) >= fDist);
};


/*virtual*/ bool CBaseGunnerAI::IsTakeFire(void)
{
    int32 targetNo = m_targetInfo.Get();
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(targetNo);
    if (pPlayerChr == nullptr)
        return false;

    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
    float fTargetDist = m_targetInfo.GetDistance();
    if (fTargetDist > fDistanceOfSuitable)
        return false;

    float fRange = GetFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE);
    if (fTargetDist > fRange)
        return false;

    float fDirMe = EnemyCharacter().Compositor().GetDirection();
    float fDirTarget = m_targetInfo.GetAngle();

    float fDirDiff = (fDirTarget - fDirMe);
    fDirDiff = std::fabs(fDirDiff);

    float fRatioOfFrontView = Characteristic().m_fRatioOfFrontView;
    float fFrontView = (fRatioOfFrontView * MATH_DEG2RAD(18.0f));
    if (fDirDiff > fFrontView)
        return false;

    if (!static_cast<CBaseGeneralEnemyChr&>(EnemyCharacter()).IsStayTrajectory(pPlayerChr, fTargetDist))
        return false;

    return true;
};


/*virtual*/ bool CBaseGunnerAI::IsTakeRun(void)
{
    CAIThinkOrder::ORDER order = ThinkOrder().GetOrder();

    if (order == CAIThinkOrder::ORDER_RUN)
        return false;

    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
	float fRatioOfMove = Characteristic().m_fRatioOfMove;
    float fTargetDist = m_targetInfo.GetDistance();

    if ( (((fDistanceOfSuitable * 0.25f) >= fTargetDist) && (Math::RandFloat() < fRatioOfMove)) ||
         (IsOutsidePatrolArea() && (Math::RandFloat() < fRatioOfMove)) )
        return true;

    return false;
};