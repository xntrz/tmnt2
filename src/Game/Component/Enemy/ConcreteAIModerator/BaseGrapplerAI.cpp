#include "BaseGrapplerAI.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseGrapplerEnemyChr.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"


/*static*/ CAIModerator* CBaseGrapplerAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseGrapplerAI(pEnemyChr);
};


CBaseGrapplerAI::CBaseGrapplerAI(CEnemyCharacter* pEnemyChr)
: CBaseGeneralEnemyAIModerator(pEnemyChr)
{
    int32 playerNum = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfGeometry);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfCollision);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfTargetDamaged);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfDamageAccumulation);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfTurtleCharacteristic);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfTurtleStatus);
    };
};


/*virtual*/ CBaseGrapplerAI::~CBaseGrapplerAI(void)
{
    ;
};


/*virtual*/ CAIThinkOrder::ORDER CBaseGrapplerAI::ActionSelector(int32 iNo) /*override*/
{
    if (IsOutsidePatrolArea())
    {
        float fRatioOfMove = Characteristic().m_fRatioOfMove;
        float fRand = Math::RandFloat();

        if (fRand < fRatioOfMove)
            return CAIThinkOrder::ORDER_RUN;
    };

    if (!CAIUtils::GetActivePlayer(iNo))
        return CAIThinkOrder::ORDER_WAIT;

    if (IsTakeGuard())
        return CAIThinkOrder::ORDER_DEFENCE;

    if (IsTakeAttack() || IsTakeRunAttack())
       return CAIThinkOrder::ORDER_ATTACK;

    if ((ThinkOrder().GetOrder()  == CAIThinkOrder::ORDER_MOVE) &&
        (ThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE))
        return CAIThinkOrder::ORDER_WAIT;

    return CAIThinkOrder::ORDER_MOVE;
};


/*virtual*/ bool CBaseGrapplerAI::OnActionOfWait(int32 iNo) /*override*/
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
    float fDistOfSuitable = Characteristic().m_fDistanceOfSuitable;

    RwV3d vecAt = { fDistOfSuitable * fRandX, 0.0f, fDistOfSuitable * fRandZ };

    float fRatioOfMove = Characteristic().m_fRatioOfMove;
    Math::Vec3_Scale(&vecAt, &vecAt, fRatioOfMove + 0.5f);

    EnemyCharacter().Compositor().RotateVectorByDirection(&vecAt, &vecAt);

    RwV3d vPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetPosition(&vPos);

    Math::Vec3_Add(&vecAt, &vecAt, &vPos);
    vecAt.y = 0.0f;

    float fRadiusOfActivity = Characteristic().m_fRadiusOfAction;
    RwV3d vecPos = vecAt;
    float fDist = CAIUtils::CalcMovePointEscapeObstacle(&vecPos, &EnemyCharacter(), &vecPos, 1.0f, fRadiusOfActivity);

    float fRand = Math::RandFloat();
    if ((fRand >= fRatioOfMove) || (fDist < 0.0f))
    {
        float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
        fRand = Math::RandFloat();
        if (fRand >= fRatioOfActivity)
            return true;

        if (iNo > -1)
        {
            float fDirMe = EnemyCharacter().Compositor().GetDirection();
            float fDirTarget = m_targetInfo.GetAngle();

            float fDirDiff = (fDirTarget - fDirMe);
            fDirDiff = std::fabs(fDirDiff);

            float fFrontViewRatio = Characteristic().m_fRatioOfFrontView;
            float fFrontView = (fFrontViewRatio * MATH_DEG2RAD(36.0f));

            if (fFrontView < fDirDiff)
                AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveTurn, 1.0f, iNo);
        }
        else
        {
            AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveTurn, 1.0f, &vecPos);
        };

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
        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, iNo);

    return true;
};


/*virtual*/ bool CBaseGrapplerAI::OnActionOfMove(int32 iNo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerChr)
        return false;

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

    float fRand = Math::RandFloat();
    float fRatioOfMove = Characteristic().m_fRatioOfMove;
    if (fRand <= fRatioOfMove)
    {
        if ((fRadiusOfAction * 0.5f) > fDist)
        {
            float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
            fRand = Math::RandFloat();

            if (fRand >= fRatioOfActivity)
            {
                AIOT::TYPE sidewalk = IsTakeSideWalk();
                if (sidewalk != AIOT::Null)
                    AIOT::SetMoveOrder(ThinkOrder(), sidewalk, 1.0f, iNo);
                else
                    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, iNo);

                return true;
            };
        };

        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveRun, 1.0f, iNo);
        return true;
    };

    float fDirMe = EnemyCharacter().Compositor().GetDirection();
    float fDirTarget = m_targetInfo.GetAngle();

    float fDirDiff = (fDirTarget - fDirMe);
    fDirDiff = std::fabs(fDirDiff);

    float fRatioOfFrontView = Characteristic().m_fRatioOfFrontView;
    float fFrontView = (fRatioOfFrontView * MATH_DEG2RAD(36.0f));

    if (fFrontView >= fDirDiff)
        return false;

    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveTurn, 1.0f, iNo);

    return true;
};


/*virtual*/ bool CBaseGrapplerAI::OnActionOfRun(int32 iNo) /*override*/
{
    RwV3d vPatrolOrigin = EnemyCharacter().CharacterParameter().m_feature.m_vPatrolOrigin;    
    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveRun, 1.0f, &vPatrolOrigin);

    m_targetInfo.Reset();

    return true;
};


/*virtual*/ bool CBaseGrapplerAI::OnActionOfAttack(int32 iNo) /*override*/
{
    if (!CAIUtils::GetActivePlayer(iNo))
        return false;

    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE))
        return false;

    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN))
        return false;

    bool bResult = true;

    if (IsTakeRunAttack() && IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_RUN))
    {
        AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackRun, iNo);
    }
    else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_C))
    {
        AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackC, iNo);
    }
    else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_B))
    {
        AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackB, iNo);
    }
    else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_AAA))
    {
        AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackAAA, iNo);
    }
    else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_AA))
    {
        AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackAA, iNo);
    }
    else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_A))
    {
        AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackA, iNo);
    }
    else
    {
        bResult = false;
    };

    return bResult;
};


/*virtual*/ bool CBaseGrapplerAI::OnActionOfDefence(int32 iNo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerChr)
        return false;

    bool bResult = false;

    PLAYERTYPES::STATUS status = pPlayerChr->GetStatus();
    switch (status)
    {
    case PLAYERTYPES::STATUS_DASH:
    case PLAYERTYPES::STATUS_ATTACK_RUN:
        bResult = IsSatifyFrequency(CEnemyParameter::FREQUENCY_GUARD_RUN_ATTACK);
        break;

    case PLAYERTYPES::STATUS_ATTACK_A:
    case PLAYERTYPES::STATUS_ATTACK_AA:
    case PLAYERTYPES::STATUS_ATTACK_AAB:
    case PLAYERTYPES::STATUS_ATTACK_AAC:
    case PLAYERTYPES::STATUS_ATTACK_AABB:
    case PLAYERTYPES::STATUS_ATTACK_AABC:
    case PLAYERTYPES::STATUS_ATTACK_AABBB:
    case PLAYERTYPES::STATUS_ATTACK_AABBC:
        bResult = IsSatifyFrequency(CEnemyParameter::FREQUENCY_GUARD_A);
        break;

    case PLAYERTYPES::STATUS_ATTACK_B_CHARGE:
    case PLAYERTYPES::STATUS_ATTACK_B:
        bResult = IsSatifyFrequency(CEnemyParameter::FREQUENCY_GUARD_B);
        break;

    case PLAYERTYPES::STATUS_ATTACK_JUMP:
        bResult = IsSatifyFrequency(CEnemyParameter::FREQUENCY_GUARD_JUMP_ATTACK);
        break;

    case PLAYERTYPES::STATUS_ATTACK_KNIFE:
    case PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP:
    case PLAYERTYPES::STATUS_THROWN_COMBINATION:
    case PLAYERTYPES::STATUS_THROWN_COMBINATION_END:
    case PLAYERTYPES::STATUS_THROWN_MISS:
        bResult = IsSatifyFrequency(CEnemyParameter::FREQUENCY_GUARD_SHOT);
        break;

    case PLAYERTYPES::STATUS_LIFT_CHALLENGE:
    case PLAYERTYPES::STATUS_THROW:
    case PLAYERTYPES::STATUS_THROW_BACK:
    case PLAYERTYPES::STATUS_THROW_COMBINATION:
        bResult = IsSatifyFrequency(CEnemyParameter::FREQUENCY_GUARD_THROW);
        break;

    default:
        ASSERT(false);
        break;
    };

    if (!bResult)
        return false;

    AIOT::SetDefenceOrder(ThinkOrder());

    return true;
};


/*virtual*/ CBaseGrapplerAI::UNDERACTION CBaseGrapplerAI::OnUnderAllActions(void) /*override*/
{
    return UNDERACTIONS_CONTINUOUS;
};


/*virtual*/ CBaseGrapplerAI::UNDERACTION CBaseGrapplerAI::OnUnderWait(void) /*override*/
{
    /**
     *  TODO rework to custom code or leave it as in retail game next three lines.
     *
     *  Next 3 lines of code makes enemy "jitter" on the edge of patrol radius.
     *
     *  For example enemy ran to the player and get out of patrol area while exec order and next lines makes
     *  enemy thinkover then we get to the CBaseGrapplerAI::OnActionOfRun so basically all fine until
     *  next think tick - CBaseGeneralEnemyAI::OnUnderMove calls "OnUnderWait" before process so we go here again
     *  and enemy STILL is out side patrol area and order is set to MOVE from the "OnActionOfRun"!
     *  This makes enemy do sequence of "Move once-Think-Move once-Think-..." each think cycle, also this cause
     *  ridiculous situations in retail game when player stays infront of enemy and enemy just trying to return
     *  patrol area by starting rotating for 1 pixel then think again and so on.
     *  (try it in enemy AI testbed by enabling showing all AI areas and AI order info in debug menu)
     *
     *  This could be done faster via order RUN to corner point inside patrol area. So probably to fix this jitter
     *  behaviour is to order RUN to random position inside patrol area instead MOVE in the "OnActionOfRun" then
     *  override "OnUnderRun" for CBaseGrapplerAI and check when enemy reached this point by "IsMoveEndForTargetPosition"
     *  then return THINKOVER from "OnUnderRun".
     *
     *  (Also required to change CBaseGrapplerEnemyChr::CMoveStatusObserver::Observing, and IsMoveEndForTargetPosition
     *   to check OrderRun() insted OrderMove())
     */   
    CAIThinkOrder::ORDER order = ThinkOrder().GetOrder();
    if ((order == CAIThinkOrder::ORDER_MOVE) && IsOutsidePatrolArea())
        return UNDERACTIONS_THINKOVER;

    if (IsTakeGuard())
    {
        int32 iNo = m_targetInfo.Get();
        if (OnActionOfDefence(iNo))
            return UNDERACTIONS_FORCE_CHANGED;
    };

	int32 iNo = SearchTarget();

    if ((iNo >= 0) &&
        !m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE) &&
        !m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN))
    {
        float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
        float fRand = Math::RandFloat();
        if (fRand < fRatioOfActivity)
        {
            order = ThinkOrder().GetOrder();
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


/*virtual*/ bool CBaseGrapplerAI::IsMoveEndForTargetNumber(void) /*override*/
{
    if (IsTakeAttack() || IsTakeRunAttack() || IsTakeGuard())
        return true;

    return false;
};


/*virtual*/ bool CBaseGrapplerAI::IsMoveEndForTargetPosition(void) /*override*/
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


/*virtual*/ bool CBaseGrapplerAI::IsTakeAttack(void)
{
    float fDistOfSuitable = Characteristic().m_fDistanceOfSuitable;
    
    RwV3d vAt = { 0.0f, 0.0f, fDistOfSuitable };
    EnemyCharacter().Compositor().RotateVectorByDirection(&vAt, &vAt);

    RwV3d vPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vPos);

    Math::Vec3_Add(&vAt, &vAt, &vPos);

    CBaseGrapplerEnemyChr::MOVABLETYPE movableType = static_cast<CBaseGrapplerEnemyChr&>(EnemyCharacter()).GetMovableType(&vAt);
    switch (movableType)
    {
    case CBaseGrapplerEnemyChr::MOVABLETYPE_IMPOSSIBLE:
    case CBaseGrapplerEnemyChr::MOVABLETYPE_JUMP:
    case CBaseGrapplerEnemyChr::MOVABLETYPE_FALLDOWN:
    case CBaseGrapplerEnemyChr::MOVABLETYPE_HOLE:
        return false;

    case CBaseGrapplerEnemyChr::MOVABLETYPE_POSSIBLE:
    case CBaseGrapplerEnemyChr::MOVABLETYPE_BODILYCRUSH:
        {
            float fDist = m_targetInfo.GetDistance();
            if (fDist > fDistOfSuitable)
                break;
            
            float fDirTarget = m_targetInfo.GetAngle();
            float fDirMe = EnemyCharacter().Compositor().GetDirection();

            float fDirDiff = (fDirTarget - fDirMe);
            fDirDiff = std::fabs(fDirDiff);

            float fRatioOfFrontView = Characteristic().m_fRatioOfFrontView;
            float fFrontView = (fRatioOfFrontView * MATH_DEG2RAD(36.0f));

            if (fFrontView >= fDirDiff)
                return true;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return false;
};


/*virtual*/ bool CBaseGrapplerAI::IsTakeRunAttack(void)
{
    float fDistOfSuitable = Characteristic().m_fDistanceOfSuitable * 2.0f;

	RwV3d vAt = { 0.0f, 0.0f, fDistOfSuitable };
    EnemyCharacter().Compositor().RotateVectorByDirection(&vAt, &vAt);

    RwV3d vPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vPos);

    Math::Vec3_Add(&vAt, &vAt, &vPos);

    CBaseGrapplerEnemyChr::MOVABLETYPE movableType = static_cast<CBaseGrapplerEnemyChr&>(EnemyCharacter()).GetMovableType(&vAt);
    switch (movableType)
    {
    case CBaseGrapplerEnemyChr::MOVABLETYPE_IMPOSSIBLE:
    case CBaseGrapplerEnemyChr::MOVABLETYPE_JUMP:
    case CBaseGrapplerEnemyChr::MOVABLETYPE_FALLDOWN:
    case CBaseGrapplerEnemyChr::MOVABLETYPE_HOLE:
        return false;
        
    case CBaseGrapplerEnemyChr::MOVABLETYPE_POSSIBLE:
    case CBaseGrapplerEnemyChr::MOVABLETYPE_BODILYCRUSH:
        {
            float fDist = m_targetInfo.GetDistance();
            if (fDist > fDistOfSuitable)
                break;

            if (ThinkOrder().GetOrder() != CAIThinkOrder::ORDER_MOVE)
                break;

            int32 moveType = ThinkOrder().OrderMove().m_iMoveType;
            int32 moveId = AIOT::Get(moveType);
            if (moveId != AIOT::MoveRun)
                break;

            float fDirTarget = m_targetInfo.GetAngle();
            float fDirMe = EnemyCharacter().Compositor().GetDirection();

            float fDirDiff = (fDirTarget - fDirMe);
            fDirDiff = std::fabs(fDirDiff);

            float fRatioOfFrontView = Characteristic().m_fRatioOfFrontView;
            float fFrontView = (fRatioOfFrontView * MATH_DEG2RAD(36.0f));

            if (fFrontView >= fDirDiff)
                return true;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return false;
};


/*virtual*/ bool CBaseGrapplerAI::IsTakeGuard(void)
{
    int32 iNo = m_targetInfo.Get();

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);
    if (pPlayerChr == nullptr)
        return false;

    if (!m_targetInfo.TestState(CAIUtils::PLAYER_STATE_ATTACK))
        return false;

    float fDistOfSuitable = Characteristic().m_fDistanceOfSuitable;
    float fTargetDist = m_targetInfo.GetDistance();
    if (fTargetDist > fDistOfSuitable)
        return false;

    float fDirTarget = m_targetInfo.GetAngle();
    float fDirMe = EnemyCharacter().Compositor().GetDirection();

    float fDirDiff = fDirTarget - fDirMe;
    fDirDiff = std::fabs(fDirDiff);

    float fRatioOfFrontView = Characteristic().m_fRatioOfFrontView;
    float fFrontView = (fRatioOfFrontView * MATH_DEG2RAD(90.0f));

    if (fFrontView < fDirDiff)
        return false;

    float fDirPlayer = pPlayerChr->GetDirection();

    fDirDiff = fDirPlayer - fDirMe;
    fDirDiff = std::fabs(fDirDiff);

    fDirDiff -= MATH_PI;

    if ((fDirDiff <=  MATH_DEG2RAD(72.0f)) &&
        (fDirDiff >= -MATH_DEG2RAD(72.0f)))
        return true;

    return false;
};