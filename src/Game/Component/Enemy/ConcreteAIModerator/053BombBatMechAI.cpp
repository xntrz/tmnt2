#include "053BombBatMechAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseFlyingEnemyChr.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


/*static*/ CAIModerator* C053BombBatMechAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C053BombBatMechAI(pEnemyChr);
};


C053BombBatMechAI::C053BombBatMechAI(CEnemyCharacter* pEnemyChr)
: CBaseBatAI(pEnemyChr)
, m_bBodyBlowRequest(false)
{
    ;
};


/*virtual*/ C053BombBatMechAI::~C053BombBatMechAI(void)
{
    ;
};


/*virtual*/ CAIThinkOrder::ORDER C053BombBatMechAI::ActionSelector(int32 iNo) /*override*/
{
    CAIThinkOrder::ORDER order = CBaseBatAI::ActionSelector(iNo);

    if ((order == CAIThinkOrder::ORDER_MOVE) &&
        IsTakeBodyBlow() &&
        IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE_RATE))
    {
        order = CAIThinkOrder::ORDER_ATTACK;
        m_bBodyBlowRequest = true;
    };

    return order;
};


/*virtual*/ bool C053BombBatMechAI::OnActionOfAttack(int32 iNo) /*override*/
{
    if (m_targetInfo.TestState(CAIUtils::PLAYER_STATE_DOWN) ||
        m_targetInfo.TestState(CAIUtils::PLAYER_STATE_IMPOSSIBLE))
        return false;

    if (m_bBodyBlowRequest)
    {
        m_bBodyBlowRequest = false;

        AIOT::SetAttackOrder(ThinkOrder(), AIOT::FireB, iNo);

        SetState(STATE_ATTACK);
        SetCancelTimer(0.0f);
        
        return true;
    };

    return CBaseBatAI::OnActionOfAttack(iNo);
};


/*virtual*/ C053BombBatMechAI::UNDERACTION C053BombBatMechAI::OnUnderMove(void) /*override*/
{
    UNDERACTION underAction = CBaseBatAI::OnUnderMove();
    if (underAction)
        return underAction;

    int32 moveType = ThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);

    if (moveTarget && IsTakeBodyBlow())
    {
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return UNDERACTIONS_THINKOVER;
    };

    return underAction;
};


/*virtual*/ bool C053BombBatMechAI::IsTakeBodyBlow(void)
{
    float fRange = GetFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE);
    float fDist = m_targetInfo.GetDistance();

    if (fRange < fDist)
        return false;

    if ((fRange * 0.5f) >= fDist)
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

    return true;
};