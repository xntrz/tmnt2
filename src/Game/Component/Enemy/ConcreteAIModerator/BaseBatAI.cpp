#include "BaseBatAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseFlyingEnemyChr.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


static float CANCEL_TIMER_ATTACK_READY   = 1.5f;
static float CANCEL_TIMER_ATTACK_AWAY    = 1.5f;
static float CANCEL_TIMER_PURPOSE        = 1.0f;


/*static*/ CAIModerator* CBaseBatAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseBatAI(pEnemyChr);
};


CBaseBatAI::CBaseBatAI(CEnemyCharacter* pEnemyChr)
: CBaseFlyingEnemyAI(pEnemyChr)
, m_prevHP(0)
{
    int32 playerNum = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfRandom);
        m_targetCoordinator.RegistForHateCalculator(i, new CTarget::ICalcHateOfGeometry);
    };
};


/*virtual*/ CBaseBatAI::~CBaseBatAI(void)
{
    ;
};


/*virtual*/ CAIThinkOrder::ORDER CBaseBatAI::ActionSelector(int32 iNo) /*override*/
{
    if (IsOutsidePatrolArea())
        return CAIThinkOrder::ORDER_RUN;

    if (!CAIUtils::GetActivePlayer(iNo))
        return CAIThinkOrder::ORDER_WAIT;

    if (IsTakeRun())
        return CAIThinkOrder::ORDER_RUN;

    if (IsTakeAttack())
        return CAIThinkOrder::ORDER_ATTACK;

    return CAIThinkOrder::ORDER_MOVE;
};


/*virtual*/ bool CBaseBatAI::OnActionOfWait(int32 iNo) /*override*/
{
    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable * -1.0f;

    RwV3d vecStep = Math::VECTOR3_ZERO;
    vecStep.x = (Math::RandFloat() - 0.5f);
    vecStep.y = 0.0f;
    vecStep.z = fDistanceOfSuitable;

    float fRand = (Math::RandFloat() * 0.5f);
    Math::Vec3_Scale(&vecStep, &vecStep, fRand);
    EnemyCharacter().Compositor().RotateVectorByDirection(&vecStep, &vecStep);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetPosition(&vecPos);
    Math::Vec3_Add(&vecPos, &vecPos, &vecStep);
    vecPos.y = 0.0f;

    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecPos);

    SetCancelTimer(CANCEL_TIMER_PURPOSE);
    SetState(STATE_PURPOSE);

    return true;
};


/*virtual*/ bool CBaseBatAI::OnActionOfMove(int32 iNo) /*override*/
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
    if (Math::RandFloat() > fRatioOfMove)
        return false;

    float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
    if (Math::RandFloat() >= fRatioOfActivity)
    {
        float fRandX = Math::RandFloatRange(0.5f, 1.5f);
        float fRandZ = Math::RandFloatRange(0.5f, 1.5f);

        float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;

        vecFootPosPlayer.x += (fRandX * fDistanceOfSuitable);
        vecFootPosPlayer.z += (fRandZ * fDistanceOfSuitable);

        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecFootPosPlayer);

        m_targetInfo.Reset();
        
        SetCancelTimer(CANCEL_TIMER_ATTACK_AWAY);
        SetState(STATE_ATTACK_AWAY);
    }
    else
    {
        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, iNo);

        SetCancelTimer(CANCEL_TIMER_ATTACK_READY);
        SetState(STATE_ATTACK_READY);
    };

    return true;
};


/*virtual*/ bool CBaseBatAI::OnActionOfRun(int32 iNo) /*override*/
{
    float fMoveChance = Math::RandFloat();
    float fRatioOfMove = Characteristic().m_fRatioOfMove;
    if (fMoveChance > fRatioOfMove)
        return false;

    RwV3d vecMovePos = Math::VECTOR3_ZERO;

    if (!IsOutsidePatrolArea())
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

        float fRadiusOfAction = Characteristic().m_fRadiusOfAction;
        float fRand = Math::RandFloatRange(0.25f, 0.75f);

        RwV3d vec = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vec, &vecFootPosPlayer, &vecFootPosEnemy);
        Math::Vec3_Normalize(&vec, &vec);
        Math::Vec3_Negate(&vec, &vec);
        Math::Vec3_Scale(&vec, &vec, fRand * fRadiusOfAction);

        Math::Vec3_Add(&vecMovePos, &vecFootPosEnemy, &vec);
    }
    else
    {
        vecMovePos = EnemyCharacter().CharacterParameter().m_feature.m_vPatrolOrigin;
    };

    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecMovePos);

    m_targetInfo.Reset();

    SetCancelTimer(CANCEL_TIMER_ATTACK_AWAY);
    SetState(STATE_ATTACK_AWAY);

    return true;
};


/*virtual*/ bool CBaseBatAI::OnActionOfAttack(int32 iNo) /*override*/
{
    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN) ||
        m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE))
        return false;

    if (!IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_A))
        return false;

    AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackA, iNo);

    SetCancelTimer(0.0f);
    SetState(STATE_ATTACK);

    return true;
};


/*virtual*/ CBaseBatAI::UNDERACTION CBaseBatAI::OnUnderAllActions(void) /*override*/
{
    int32 nowHP = EnemyCharacter().CharacterParameter().m_feature.m_iHP;
    int32 prevHP = m_prevHP;
    m_prevHP = nowHP;

    if (nowHP >= prevHP)
    {
        UpdateCancelTimer();
        return UNDERACTIONS_CONTINUOUS;
    };

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


/*virtual*/ CBaseBatAI::UNDERACTION CBaseBatAI::OnUnderWait(void) /*override*/
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


/*virtual*/ bool CBaseBatAI::IsMoveEndForTargetNumber(void) /*override*/
{
    return (IsTakeAttack());
};


/*virtual*/ bool CBaseBatAI::IsMoveEndForTargetPosition(void) /*override*/
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


/*virtual*/ bool CBaseBatAI::IsTakeAttack(void)
{
    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
    float fDist = m_targetInfo.GetDistance();
    if (fDist > fDistanceOfSuitable)
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
    float fAttackAltitude = static_cast<CBaseFlyingEnemyChr&>(EnemyCharacter()).GetAttackAltitude();

    if (fAttackAltitude < fAltitude)
        return false;
    
    fAltitude = ((vecFootPosEnemy.y - vecFootPosPlayer.y) + fRadius);

	if (fAttackAltitude > fAltitude)
		return false;

	if (!static_cast<CBaseFlyingEnemyChr&>(EnemyCharacter()).IsStayTrajectory(pPlayerChr, fDist))
		return false;

	return true;
};


/*virtual*/ bool CBaseBatAI::IsTakeRun(void)
{
    return (GetState() == STATE_ATTACK);
};