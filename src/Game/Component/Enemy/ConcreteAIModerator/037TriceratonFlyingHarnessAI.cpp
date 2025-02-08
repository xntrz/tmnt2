#include "037TriceratonFlyingHarnessAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseFlyingEnemyChr.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/037TriceratonFlyingHarness.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


/*static*/ float C037TriceratonFlyingHarnessAI::CANCEL_TIMER_ATTACK_READY = 1.5f;
/*static*/ float C037TriceratonFlyingHarnessAI::CANCEL_TIMER_ATTACK_AWAY = 1.5f;
/*static*/ float C037TriceratonFlyingHarnessAI::CANCEL_TIMER_PURPOSE = 1.0f;


/*static*/ CAIModerator* C037TriceratonFlyingHarnessAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C037TriceratonFlyingHarnessAI(pEnemyChr);
};


C037TriceratonFlyingHarnessAI::C037TriceratonFlyingHarnessAI(CEnemyCharacter* pEnemyChr)
    : CBaseFlyingEnemyAI(pEnemyChr)
    , m_prevHP(0)
    , m_fDirection(0.0f)
{
    int32 playerNum = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfTargetDamaged);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfDamageAccumulation);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfTurtleCharacteristic);
    };
};


/*virtual*/ C037TriceratonFlyingHarnessAI::~C037TriceratonFlyingHarnessAI(void)
{
    ;
};


/*virtual*/ CAIThinkOrder::ORDER C037TriceratonFlyingHarnessAI::ActionSelector(int32 iNo) /*override*/
{
    if (IsOutsidePatrolArea())
        return CAIThinkOrder::ORDER_RUN;

    if (!CAIUtils::GetActivePlayer(iNo))
        return CAIThinkOrder::ORDER_WAIT;

    if (IsTakeAttack())
        return CAIThinkOrder::ORDER_ATTACK;

    if (IsTakeRun())
        return CAIThinkOrder::ORDER_RUN;

    return CAIThinkOrder::ORDER_MOVE;
};


/*virtual*/ bool C037TriceratonFlyingHarnessAI::OnActionOfWait(int32 iNo) /*override*/
{
    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;

    RwV3d vecDir = { Math::RandFloat() - 0.5f, 0.0f, 0.0f };

    float fRand = (Math::RandFloat() * 0.3f);
    vecDir.x *= fRand;
    vecDir.y *= fRand;
    vecDir.z = (fRand * fDistanceOfSuitable);

    RwMatrix matRotY;
    RwMatrixSetIdentityMacro(&matRotY);
    Math::Matrix_RotateY(&matRotY, (m_fDirection + MATH_PI));
    RwV3dTransformVector(&vecDir, &vecDir, &matRotY);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetPosition(&vecPos);
    vecPos.y = 0.0f;

    Math::Vec3_Add(&vecPos, &vecPos, &vecDir);

    float fRadiusOfAction = Characteristic().m_fRadiusOfAction;
    float fMoveDist = CAIUtils::CalcMovePointEscapeObstacle(&vecPos, &EnemyCharacter(), &vecPos, 1.0f, fRadiusOfAction);

    if (fMoveDist >= 0.0f)
        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecPos);
    else
        AIOT::SetNothingOrder(ThinkOrder());

    SetCancelTimer(CANCEL_TIMER_PURPOSE);
    SetState(STATE_PURPOSE);

    return true;
};


/*virtual*/ bool C037TriceratonFlyingHarnessAI::OnActionOfMove(int32 iNo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerChr)
        return false;

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);
    vecFootPosPlayer.y = 0.0f;

    RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecFootPosEnemy);
    vecFootPosEnemy.y = 0.0f;

    float fDist = CEnemyUtils::GetDistance(&vecFootPosEnemy, &vecFootPosPlayer);
    if (fDist < 0.0f)
        return false;

    float fRadiusOfAction = Characteristic().m_fRadiusOfAction;
    if (fDist >= fRadiusOfAction)
        return false;

    float fRatioOfMove = Characteristic().m_fRatioOfMove;
    float fMoveChance = Math::RandFloat();
    if (fMoveChance > fRatioOfMove)
        return false;

    float fMoveDist = CAIUtils::CalcMovePointEscapeObstacle(&vecFootPosPlayer, &EnemyCharacter(), &vecFootPosPlayer, 1.0f, fRadiusOfAction);
    if (fMoveDist < 0.0f)
        return false;

    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, iNo);

    SetState(STATE_ATTACK_READY);
    SetCancelTimer(CANCEL_TIMER_ATTACK_READY);

    return true;
};


/*virtual*/ bool C037TriceratonFlyingHarnessAI::OnActionOfRun(int32 iNo) /*override*/
{
    float fRatioOfMove = Characteristic().m_fRatioOfMove;
    float fMoveChance = Math::RandFloat();
    if (fMoveChance > fRatioOfMove)
        return false;

    if (IsOutsidePatrolArea())
    {
        RwV3d vecPatrolOrigin = EnemyCharacter().CharacterParameter().m_feature.m_vPatrolOrigin;

        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecPatrolOrigin);

        SetState(STATE_ATTACK_AWAY);
        SetCancelTimer(CANCEL_TIMER_ATTACK_AWAY);

        return true;
    };

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);
    if (pPlayerChr)
    {
        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);
        vecFootPosPlayer.y = 0.0f;

        RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
        EnemyCharacter().Compositor().GetFootPosition(&vecFootPosEnemy);
        vecFootPosEnemy.y = 0.0f;

        RwV3d vecDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDir, &vecFootPosPlayer, &vecFootPosEnemy);
        Math::Vec3_Normalize(&vecDir, &vecDir);
        Math::Vec3_Negate(&vecDir, &vecDir);

        float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
        float fRand = Math::RandFloatRange(0.5f, 1.0f);

        Math::Vec3_Scale(&vecDir, &vecDir, (fDistanceOfSuitable * fRand));

        float fRot = (Math::RandFloat() * MATH_PI2);

        RwMatrix matDir;
        RwMatrixSetIdentityMacro(&matDir);
        Math::Matrix_RotationYawPitchRoll(&matDir, fRot, 0.0f, 0.0f);
        RwV3dTransformVector(&vecDir, &vecDir, &matDir);

        Math::Vec3_Add(&vecFootPosEnemy, &vecFootPosEnemy, &vecDir);

        RwV3d vecMovePos = Math::VECTOR3_ZERO;
        float fRadiusOfAction = Characteristic().m_fRadiusOfAction;
        float fMoveDist = CAIUtils::CalcMovePointEscapeObstacle(&vecFootPosEnemy, &EnemyCharacter(), &vecFootPosEnemy, 1.0f, fRadiusOfAction);
        if (fMoveDist >= 0.0f)
        {
            AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecFootPosEnemy);

            SetState(STATE_ATTACK_AWAY);
            SetCancelTimer(CANCEL_TIMER_ATTACK_AWAY);

            return true;
        };
    };

    return false;
};


/*virtual*/ bool C037TriceratonFlyingHarnessAI::OnActionOfAttack(int32 iNo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);

    if (!pPlayerChr)
        return false;

    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN) ||
        m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE))
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

        float fDistToPlayer = CEnemyUtils::GetDistance(&vecBodyPosEnemy, &vecBodyPosPlayer);
        float fDistToMap = CEnemyUtils::GetDistance(&vecBodyPosEnemy, &pCollisionResult->m_vClosestPt);

        if (fDistToMap < fDistToPlayer)
            return false;
    };

    int32 attackType = AIOT::Null;

    if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE_RATE))
        attackType = AIOT::FireA;
    else if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_REPEATABLE))
        attackType = AIOT::FireRepeatA;

    if (attackType != AIOT::Null)
    {
        RwV3d vecAt = Math::VECTOR3_ZERO;
        AIOT::SetFireOrder(ThinkOrder(), attackType, iNo, &vecAt);

        SetState(STATE_ATTACK);
        SetCancelTimer(0.0f);

        return true;
    };

    return false;
};


/*virtual*/ C037TriceratonFlyingHarnessAI::UNDERACTION
C037TriceratonFlyingHarnessAI::OnUnderAllActions(void) /*override*/
{
    int32 nowHP = EnemyCharacter().CharacterParameter().m_feature.m_iHP;
    int32 prevHP = m_prevHP;
    m_prevHP = nowHP;

    if (nowHP >= prevHP)
    {
        UpdateCancelTimer();
        return UNDERACTIONS_CONTINUOUS;
    };

    m_targetInfo.Reset();

    float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
    float fActivityChance = Math::RandFloat();
    if (fActivityChance >= fRatioOfActivity)
    {
        UpdateCancelTimer();
        return UNDERACTIONS_CONTINUOUS;
    };

    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
    m_targetInfo.Reset();
    SetState(STATE_ATTACK);

    return UNDERACTIONS_THINKOVER;
};


/*virtual*/ C037TriceratonFlyingHarnessAI::UNDERACTION
C037TriceratonFlyingHarnessAI::OnUnderWait(void) /*override*/
{
    CAIThinkOrder::ORDER order = ThinkOrder().GetOrder();

    if ((order != CAIThinkOrder::ORDER_MOVE) && IsOutsidePatrolArea())
        return UNDERACTIONS_THINKOVER;

    int32 iNo = SearchTarget();

    if ((iNo >= 0) &&
        (GetState() == STATE_PURPOSE) &&
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


/*virtual*/ bool C037TriceratonFlyingHarnessAI::IsMoveEndForTargetNumber(void) /*override*/
{
    return (IsTakeAttack());
};


/*virtual*/ bool C037TriceratonFlyingHarnessAI::IsMoveEndForTargetPosition(void) /*override*/
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecPos);
    vecPos.y = 0.0f;

    RwV3d vecAt = ThinkOrder().OrderMove().m_vAt;
    vecAt.y = 0.0f;

    float fRadius = EnemyCharacter().Compositor().GetCollisionParameter().m_fRadius;
    float fDist = CEnemyUtils::GetDistance(&vecPos, &vecAt);

    return (fDist <= fRadius);
};


/*virtual*/ bool C037TriceratonFlyingHarnessAI::IsTakeAttack(void)
{
    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
    float fDist = m_targetInfo.GetDistance();

    SUITABLEDISTANCE suitableDist = GetDistanceOfSuitable(fDist, fDistanceOfSuitable);

    if (!((suitableDist == SUITABLEDISTANCE_INNER) ||
        (suitableDist == SUITABLEDISTANCE_EQUAL)))
        return false;

    int32 targetNo = m_targetInfo.Get();
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(targetNo);

    if (!pPlayerChr)
        return false;

    float fRadius = pPlayerChr->GetCollisionParameter().m_fRadius;

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);

    RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecFootPosEnemy);

    float fAltitude = ((vecFootPosEnemy.y - vecFootPosPlayer.y) - fRadius);
    float fAttackAltitude = static_cast<C037TriceratonFlyingHarness&>(EnemyCharacter()).GetAttackAltitude();

    if (fAttackAltitude < fAltitude)
        return false;

    fAltitude = ((vecFootPosEnemy.y - vecFootPosPlayer.y) + fRadius);

    if (fAttackAltitude > fAltitude)
        return false;

    if (!static_cast<CBaseFlyingEnemyChr&>(EnemyCharacter()).IsStayTrajectory(pPlayerChr, fDist))
        return false;

    return true;
};


/*virtual*/ bool C037TriceratonFlyingHarnessAI::IsTakeRun(void)
{
    if (GetState() == STATE_ATTACK)
        return true;

    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
    float fDist = m_targetInfo.GetDistance();

    SUITABLEDISTANCE suitableDist = GetDistanceOfSuitable(fDist, fDistanceOfSuitable);

    if (suitableDist == SUITABLEDISTANCE_INNER)
        return true;

    return false;
};


/*virtual*/ CPlayerCharacter* C037TriceratonFlyingHarnessAI::OnGetTarget(void)
{
    int32 targetNo = m_targetInfo.Get();
    return CAIUtils::GetActivePlayer(targetNo);
};


/*virtual*/ void C037TriceratonFlyingHarnessAI::OnSetDirection(float fDir)
{
    m_fDirection = fDir;
};


/*virtual*/ float C037TriceratonFlyingHarnessAI::OnGetDirection(void)
{
    return m_fDirection;
};