#include "BaseFootNinjaAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseFootNinjaChr.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


static const float BACKAWAY_TIMER = 5.0f;


/*static*/ CAIModerator* CBaseFootNinjaAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseFootNinjaAI(pEnemyChr);
};


CBaseFootNinjaAI::CBaseFootNinjaAI(CEnemyCharacter* pEnemyChr)
: CBaseGrapplerAI(pEnemyChr)
, m_bBackAwayRequest(false)
, m_fBackAwayTimer(0.0f)
, m_bJumpAttackRequest(false)
{
    ;
};


/*virtual*/ CBaseFootNinjaAI::~CBaseFootNinjaAI(void)
{
    ;
};


/*virtual*/ CAIThinkOrder::ORDER CBaseFootNinjaAI::ActionSelector(int32 iNo) /*override*/
{
    CAIThinkOrder::ORDER order = CAIThinkOrder::ORDER_NOTHING;

    if (IsTakeBackAway())
    {
        m_bBackAwayRequest = true;
        order = CAIThinkOrder::ORDER_ATTACK;
    }
    else if (IsTakeJumpAttack())
    {
        m_bJumpAttackRequest = true;
        order = CAIThinkOrder::ORDER_ATTACK;
    }
    else
    {
        order = CBaseGrapplerAI::ActionSelector(iNo);
    };

    return order;
};


/*virtual*/ bool CBaseFootNinjaAI::OnActionOfAttack(int32 iNo) /*override*/
{
    bool bResult = false;

    if (m_bBackAwayRequest)
    {
        m_bBackAwayRequest = false;
        m_fBackAwayTimer = BACKAWAY_TIMER;

        AIOT::SetAttackOrder(ThinkOrder(), AIOT::Special1, iNo);

        bResult = true;
    }
    else if (m_bJumpAttackRequest)
    {
        m_bJumpAttackRequest = false;

        AIOT::SetMoveOrder(ThinkOrder(), AIOT::MoveJump, 1.0f, iNo);

        bResult = true;
    }
    else
    {
        bResult = CBaseGrapplerAI::OnActionOfAttack(iNo);
    };

    return bResult;
};


/*virtual*/ CBaseFootNinjaAI::UNDERACTION CBaseFootNinjaAI::OnUnderAllActions(void) /*override*/
{
    m_fBackAwayTimer -= CGameProperty::GetElapsedTime();

    return CBaseGrapplerAI::OnUnderAllActions();
};


/*virtual*/ CBaseFootNinjaAI::UNDERACTION CBaseFootNinjaAI::OnUnderWait(void) /*override*/
{
    if (IsTakeBackAway())
        m_bBackAwayRequest = true;
    
    if (IsTakeJumpAttack())
        m_bJumpAttackRequest = true;

    if (m_bBackAwayRequest || m_bJumpAttackRequest)
    {
        int32 targetNo = m_targetInfo.Get();
        if (OnActionOfAttack(targetNo))
            return UNDERACTIONS_FORCE_CHANGED;
    };

    return CBaseGrapplerAI::OnUnderWait();
};


/*virtual*/ bool CBaseFootNinjaAI::IsTakeBackAway(void)
{
    if (m_fBackAwayTimer > 0.0f)
        return false;

    if (!IsTakeGuard())
        return false;

    float fDistOfSuitable = Characteristic().m_fDistanceOfSuitable;

    RwV3d vecStep = { 0.0f, 0.0f, fDistOfSuitable };
    EnemyCharacter().Compositor().RotateVectorByDirection(&vecStep, &vecStep);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecPos);

    Math::Vec3_Negate(&vecStep, &vecStep);
    Math::Vec3_Add(&vecPos, &vecPos, &vecStep);

    CBaseFootNinjaChr::MOVABLETYPE movableType = static_cast<CBaseFootNinjaChr&>(EnemyCharacter()).GetMovableType(&vecPos);
    if ((movableType == CBaseFootNinjaChr::MOVABLETYPE_FALLDOWN) ||
        (movableType == CBaseFootNinjaChr::MOVABLETYPE_HOLE))
        return false;

    return true;
};


/*virtual*/ bool CBaseFootNinjaAI::IsTakeJumpAttack(void)
{
    if (!EnemyCharacter().IsAttachedStatusObserver(ENEMYTYPES::STATUS_JUMP_ATTACK))
        return false;

    CAIThinkOrder::ORDER order = ThinkOrder().GetOrder();
    if (order != CAIThinkOrder::ORDER_MOVE)
        return false;

    int32 moveType = ThinkOrder().OrderMove().m_iMoveType;
    int32 moveId = AIOT::Get(moveType);
    if (moveId != AIOT::MoveRun)
        return false;

    float fDistOfSuitable = Characteristic().m_fDistanceOfSuitable;
    float fTargetDist = m_targetInfo.GetDistance();
    if ((fTargetDist >= (fDistOfSuitable - 1.0f)) &&
        (fTargetDist <= (fDistOfSuitable + 1.5f)))
    {
        int32 targetNo = m_targetInfo.Get();
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(targetNo);
        if (pPlayerChr)
        {
            if (static_cast<CBaseFootNinjaChr&>(EnemyCharacter()).IsStayTrajectory(pPlayerChr, fTargetDist))
            {
                if (IsSatifyFrequency(CEnemyParameter::FREQUENCY_COMMON_2))
                    return true;
            };
        };
    };

    return false;
};