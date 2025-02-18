#include "055KurageAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseFlyingEnemyChr.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/037TriceratonFlyingHarness.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


static float CANCEL_TIMER_ATTACK_READY = 2.5f;
static float CANCEL_TIMER_ATTACK_AWAY  = 0.5f;
static float CANCEL_TIMER_PURPOSE      = 1.5f;


/*static*/ CAIModerator* C055KurageAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C055KurageAI(pEnemyChr);
};


C055KurageAI::C055KurageAI(CEnemyCharacter* pEnemyChr)
: CBaseBatAI(pEnemyChr)
, m_bBombingRequest(false)
, m_fDirection(0.0f)
, m_fAttackWaitTimer(0.0f)
{
    ;
};


/*virtual*/ C055KurageAI::~C055KurageAI(void)
{
    ;
};


/*virtual*/ CAIThinkOrder::ORDER C055KurageAI::ActionSelector(int32 iNo) /*override*/
{
    if (IsOutsidePatrolArea())
        return CAIThinkOrder::ORDER_RUN;

    if (!CAIUtils::GetActivePlayer(iNo))
        return CAIThinkOrder::ORDER_WAIT;

    if (IsTakeRun())
        return CAIThinkOrder::ORDER_RUN;

    if (IsTakeBombing() && IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_B))
    {
        m_bBombingRequest = true;
        return CAIThinkOrder::ORDER_ATTACK;
    };

    if (IsTakeAttack())
        return CAIThinkOrder::ORDER_ATTACK;

    return CAIThinkOrder::ORDER_MOVE;
};


/*virtual*/ bool C055KurageAI::OnActionOfWait(int32 iNo) /*override*/
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

    SetState(STATE_PURPOSE);
    SetCancelTimer(CANCEL_TIMER_PURPOSE);

    return true;
};


/*virtual*/ bool C055KurageAI::OnActionOfMove(int32 iNo) /*override*/
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

    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;

    vecFootPosPlayer.x += (Math::RandFloatRange(0.5f, 1.5f) * fDistanceOfSuitable);
    vecFootPosPlayer.z += (Math::RandFloatRange(0.5f, 1.5f) * fDistanceOfSuitable);
    
    float fMoveDist = CAIUtils::CalcMovePointEscapeObstacle(&vecFootPosPlayer, &EnemyCharacter(), &vecFootPosPlayer, 1.0f, fRadiusOfAction);
    if (fMoveDist < 0.0f)
        return false;

    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecFootPosPlayer);

    m_targetInfo.Reset();

    SetState(STATE_ATTACK_AWAY);
    SetCancelTimer(CANCEL_TIMER_ATTACK_AWAY);

    return true;
};


/*virtual*/ bool C055KurageAI::OnActionOfRun(int32 iNo) /*override*/
{
    float fRatioOfMove = Characteristic().m_fRatioOfMove;
    if (Math::RandFloat() > fRatioOfMove)
        return false;

    RwV3d vecMovePos = Math::VECTOR3_ZERO;

    if (IsOutsidePatrolArea())
    {
        vecMovePos = EnemyCharacter().CharacterParameter().m_feature.m_vPatrolOrigin;
    }
    else
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

        RwV3d vecDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDir, &vecFootPosPlayer, &vecFootPosEnemy);
        Math::Vec3_Normalize(&vecDir, &vecDir);
        Math::Vec3_Negate(&vecDir, &vecDir);

        float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
        float fRand = Math::RandFloatRange(1.0f, 2.0f);

        Math::Vec3_Scale(&vecDir, &vecDir, (fDistanceOfSuitable * fRand));

        float fRandRot = (Math::RandFloat() * MATH_PI2);

        RwMatrix matRot;
        RwMatrixSetIdentityMacro(&matRot);
        Math::Matrix_RotationYawPitchRoll(&matRot, fRandRot, 0.0f, 0.0f);
        RwV3dTransformVector(&vecDir, &vecDir, &matRot);

        Math::Vec3_Add(&vecFootPosEnemy, &vecFootPosEnemy, &vecDir);

        vecMovePos = vecFootPosEnemy;
    };

    float fRadiusOfAction = Characteristic().m_fRadiusOfAction;
    float fMoveDist = CAIUtils::CalcMovePointEscapeObstacle(&vecMovePos, &EnemyCharacter(), &vecMovePos, 1.0f, fRadiusOfAction);
    if (fMoveDist < 0.0f)
        return false;

    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, &vecMovePos);

    m_targetInfo.Reset();

    SetState(STATE_ATTACK_AWAY);
    SetCancelTimer(CANCEL_TIMER_ATTACK_AWAY);

    m_fDirection = EnemyCharacter().Compositor().GetDirection();

    return true;
};


/*virtual*/ bool C055KurageAI::OnActionOfAttack(int32 iNo) /*override*/
{
    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN) ||
        m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE))
        return false;

    if (m_bBombingRequest)
    {
        m_bBombingRequest = false;

        AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackB, iNo);

        SetState(STATE_ATTACK);
        SetCancelTimer(0.0f);

        return true;
    };

    if (!IsSatifyFrequency(CEnemyParameter::FREQUENCY_ATTACK_A))
        return false;

    AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveWalk, 1.0f, iNo);

    SetState(STATE_ATTACK);
    SetCancelTimer(CANCEL_TIMER_ATTACK_READY);

    float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
    m_fAttackWaitTimer = ((1.0f - fRatioOfActivity) * 10.0f);

    return true;
};


/*virtual*/ C055KurageAI::UNDERACTION C055KurageAI::OnUnderAllActions(void) /*override*/
{
    UNDERACTION underAction = CBaseBatAI::OnUnderAllActions();

    m_fAttackWaitTimer -= CGameProperty::GetElapsedTime();
    if (m_fAttackWaitTimer < 0.0f)
        m_fAttackWaitTimer = 0.0f;

    return underAction;
};


/*virtual*/ C055KurageAI::UNDERACTION C055KurageAI::OnUnderMove(void) /*override*/
{
    UNDERACTION underAction = OnUnderWait();
    if (underAction)
        return underAction;

    if (m_fCancelTimer <= 0.0f)
    {
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
        m_targetInfo.Reset();
        SetState(STATE_ATTACK);
        return UNDERACTIONS_THINKOVER;
    };

#ifdef _DEBUG
	DebugDrawMovePoint();
#endif /* _DEBUG */

    int32 moveType = ThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);

    if (moveTarget)
    {
        if (IsOutsidePatrolArea())
        {
            ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
            return UNDERACTIONS_THINKOVER;
        };
    }
    else if (IsTakeBombing())
    {
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return UNDERACTIONS_THINKOVER;
    }
    else
    {
        RwV3d vecPos = Math::VECTOR3_ZERO;
        EnemyCharacter().Compositor().GetFootPosition(&vecPos);
        vecPos.y = 0.0f;

        RwV3d vecAt = ThinkOrder().OrderMove().m_vAt;
        vecAt.y = 0.0f;

        float fDist = CEnemyUtils::GetDistance(&vecPos, &vecAt);
        float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
        if (fDist <= fDistanceOfSuitable)
        {
            ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            SetState(STATE_IDLE);
            return UNDERACTIONS_THINKOVER;
        };
    };

    float fDirection = EnemyCharacter().Compositor().GetDirection();
    if (IsHitWall(m_fDirection))
    {
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
        m_targetInfo.Reset();
        SetState(STATE_ATTACK);
        return UNDERACTIONS_THINKOVER;
    };

    return UNDERACTIONS_CONTINUOUS;
};


/*virtual*/ bool C055KurageAI::IsTakeAttack(void) /*override*/
{
    return (m_fAttackWaitTimer <= 0.0f);
};


/*virtual*/ void C055KurageAI::OnSetActionOfRun(void)
{
    int32 targetNo = m_targetInfo.Get();

    if (!OnActionOfRun(targetNo))
        OnActionOfWait(targetNo);
    
    m_fAttackWaitTimer = 0.0f;
};


/*virtual*/ void C055KurageAI::OnSetDirection(float fDir)
{
    m_fDirection = fDir;
};


/*virtual*/ float C055KurageAI::OnGetDirection(void) const
{
    return m_fDirection;
};


/*virtual*/ bool C055KurageAI::IsTakeBombing(void)
{
    float fDistanceOfSuitable = Characteristic().m_fDistanceOfSuitable;
    float fDist = m_targetInfo.GetDistance();
    if (fDist > fDistanceOfSuitable)
        return false;

    int32 targetNo = m_targetInfo.Get();
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(targetNo);
    if (!pPlayerChr)
        return false;

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);

    RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecFootPosEnemy);

    float fToPlayerHeight = (vecFootPosEnemy.y - vecFootPosPlayer.y);
    float fRadius = pPlayerChr->GetCollisionParameter().m_fRadius;

    float fHeight = static_cast<CBaseFlyingEnemyChr&>(EnemyCharacter()).GetAltitude() - CWorldMap::GetMapHeight(&vecFootPosPlayer);

    if ((fToPlayerHeight - fRadius) > fHeight)
        return false;

    if ((fToPlayerHeight + fRadius) < fHeight)
        return false;

    return true;
};