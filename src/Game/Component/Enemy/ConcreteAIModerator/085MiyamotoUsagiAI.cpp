#include "085MiyamotoUsagiAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/085MiyamotoUsagi.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorMove::OnActive(void) /*override*/
{
    OrderMove(ELITEAI::ORDERTYPE_MOVE_RUN);

    RwV3d vecNearestPlrPos = PlayerWatcher().GetTargetPlayerData().position;
    if (AIParam().IsInsideTerritory(&vecNearestPlrPos))
    {
        if (!PlayerWatcher().IsPlayerSuitRange(-1, 2.0f) &&
            EnemyChr().IsAttachedStatusObserver(ENEMYTYPES::STATUS_DASH))
        {
            OrderMove(ELITEAI::ORDERTYPE_MOVE_DASH);
        };
    };

    if (!m_bEnablePlayer)
    {
        OrderMove(ELITEAI::ORDERTYPE_MOVE_DASH_LEFT);
        OrderPush(ELITEAI::PRIORITY_LOW);
    };

    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


C085MiyamotoUsagiAI::CAIBehaviorAvoid::CAIBehaviorAvoid(void)
: CAIBehavior(USAGIAI::AIBEHAVIORNAME::AVOID)
{
    ;
};


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorAvoid::IsRunnable(void) /*override*/
{
    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
        return false;

    if (PlayerWatcher().IsAttackWithinRange(5.0f, MATH_DEG2RAD(90.0f)))
    {
        if (PlayerWatcher().IsPlayerInTheFront(-1, MATH_DEG2RAD(70.0f)))
        {
            if (!PlayerWatcher().IsLeoDash(4.0f))
                return true;
        };
    };

    if (PlayerWatcher().IsDonBarrier(3.0f))
        return true;

    return false;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorAvoid::OnActive(void) /*override*/
{
    if (PlayerWatcher().IsPlayerWithinRange(-1, 3.0f))
    {
        OrderMove(ELITEAI::ORDERTYPE_MOVE_BACK);
        OrderPush(ELITEAI::PRIORITY_LOW);

        OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMAL);
        if (PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(30.0f)))
        {
            if (PlayerWatcher().IsPlayerStop(-1) && IsSatisfyFreq(USAGIAI::FREQUENCY_ATTACK_NORMALB))
                OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMALB);
        };
        OrderPush(ELITEAI::PRIORITY_LOW);
    }
    else
    {
        OrderMove(ELITEAI::ORDERTYPE_MOVE_BACK);
        OrderPush(ELITEAI::PRIORITY_LOW);

        if (IsSatisfyFreq(USAGIAI::FREQUENCY_MOVE_CONTINUE))
        {
            OrderMove(ELITEAI::ORDERTYPE_MOVE_BACK);
            OrderPush(ELITEAI::PRIORITY_LOW);
        };

        OrderAttack(USAGIAI::ORDERTYPE_ATTACK_DASH);
        OrderPush(ELITEAI::PRIORITY_LOW);
    };
};


//
// *********************************************************************************
//


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorAttack::IsRunnable(void) /*override*/
{
    if (!CBaseEliteAI::CAIBehaviorAttack::IsRunnable())
        return false;

    if (PlayerWatcher().IsPlayerStop(-1))
        return true;

    if (PlayerWatcher().IsPlayerLookAtMe(-1, MATH_DEG2RAD(90.0f)))
        return true;

    if (PlayerWatcher().IsPlayerWithinRange(-1, 1.6f))
        return true;

    return false;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorAttack::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_SUIT);

    OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMAL);
    OrderPush(ELITEAI::PRIORITY_LOW);

    AIParam().SetAttackResult(ELITEAI::ATTACK_RESULT_STAB);
};


/*virtual*/ C085MiyamotoUsagiAI::CAIBehaviorAttack::RESULT
C085MiyamotoUsagiAI::CAIBehaviorAttack::Update(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE)
        return RESULT_END;

    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
        return RESULT_END;

    if ((AIParam().GetAttackResult() == ELITEAI::ATTACK_RESULT_NONE) ||
        (!PlayerWatcher().IsPlayerStop(-1)))
    {
        if (IsSatisfyFreq(USAGIAI::FREQUENCY_ATTACK_NORMALA))
        {
            OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMALA);
            OrderPush(ELITEAI::PRIORITY_NORMAL);
        };

        OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, 0.2f);
        OrderPush(ELITEAI::PRIORITY_LOW);

        return RESULT_FORCEEND;
    };

    if (PlayerWatcher().IsPlayerGuardState())
    {
        if (IsSatisfyFreq(USAGIAI::FREQUENCY_ATTACK_KICK) &&
            PlayerWatcher().IsPlayerWithinRange(-1, 1.5f))
        {
            OrderAttack(USAGIAI::ORDERTYPE_ATTACK_KICK);
            OrderPush(ELITEAI::PRIORITY_NORMAL);
        }
        else if (!PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(70.0f)))
        {
            OrderMove(ELITEAI::ORDERTYPE_MOVE_BACK);
            OrderPush(ELITEAI::PRIORITY_NORMAL);
        };
    }
    else if ((m_AIThinkOrder.OrderAttack().m_iAttackType == ELITEAI::ORDERTYPE_ATTACK_NORMAL) &&
             (IsSatisfyFreq(USAGIAI::FREQUENCY_ATTACK_NORMALA)))
    {
        OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMALA);
        OrderPush(ELITEAI::PRIORITY_NORMAL);
    }
    else
    {
        OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMALB);
        OrderPush(ELITEAI::PRIORITY_NORMAL);
    };

    OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, 0.2f);
    OrderPush(ELITEAI::PRIORITY_LOW);

    AIParam().SetAttackResult(ELITEAI::ATTACK_RESULT_STAB);

    return RESULT_FORCEEND;
};


//
// *********************************************************************************
//


C085MiyamotoUsagiAI::CAIBehaviorKickAttack::CAIBehaviorKickAttack(void)
: CAIBehavior(USAGIAI::AIBEHAVIORNAME::ATTACK_KICK)
, m_fKickTimeout(0.0f)
{
    ;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorKickAttack::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fKickTimeout, "KickAttack");
};


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorKickAttack::IsRunnable(void) /*override*/
{
    if (m_fKickTimeout > 0.0f)
        return false;

    if (!AIParam().IsActionPermission())
        return false;

    if (!PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(70.0f)))
        return false;

    if (!PlayerWatcher().IsPlayerGuardState())
        return false;

    m_fKickTimeout = 2.0f;

    if (!IsSatisfyFreq(USAGIAI::FREQUENCY_ATTACK_KICK))
        return false;

    return true;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorKickAttack::OnActive(void) /*override*/
{
    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_KICK);
    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


C085MiyamotoUsagiAI::CAIBehaviorRunAttack::CAIBehaviorRunAttack(void)
: CAIBehavior(USAGIAI::AIBEHAVIORNAME::ATTACK_RUN)
, m_fRunAttackTimeout(10.0f)
{
    ;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorRunAttack::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fRunAttackTimeout, "RunAttack");
};


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorRunAttack::IsRunnable(void) /*override*/
{
    if (m_fRunAttackTimeout > 0.0f)
        return false;

    if (!AIParam().IsActivityPermission())
        return false;

    if (PlayerWatcher().IsChargeAttack(3.0f))
    {
        if (!PlayerWatcher().IsDonBarrier(4.0f))
        {
            PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_TEMP);
            return true;
        };
    };

    if (PlayerWatcher().GetPlayerNumThere(3.0f))
        return false;

    if (!PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(180.0f), 24.0f))
        return false;

    m_fRunAttackTimeout = 8.0f;

    return true;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorRunAttack::OnActive(void) /*override*/
{
    if (IsSatisfyFreq(USAGIAI::FREQUENCY_ATTACK_RUN_SIDE))
    {
        OrderMove(ELITEAI::ORDERTYPE_MOVE_DASH_RIGHT);
        OrderPush(ELITEAI::PRIORITY_LOW);

        if (IsSatisfyFreq(USAGIAI::FREQUENCY_MOVE_CONTINUE))
        {
            OrderMove(ELITEAI::ORDERTYPE_MOVE_DASH_LEFT);
            OrderPush(ELITEAI::PRIORITY_LOW);
        };
    }
    else
    {
        OrderMove(ELITEAI::ORDERTYPE_MOVE_DASH_LEFT);
        OrderPush(ELITEAI::PRIORITY_LOW);

        if (IsSatisfyFreq(USAGIAI::FREQUENCY_MOVE_CONTINUE))
        {
            OrderMove(ELITEAI::ORDERTYPE_MOVE_DASH_RIGHT);
            OrderPush(ELITEAI::PRIORITY_LOW);
        };
    };

    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_DASH);
    OrderPush(ELITEAI::PRIORITY_NORMAL);

    OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, 0.2f);
    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


C085MiyamotoUsagiAI::CAIBehaviorAvoidAttack::CAIBehaviorAvoidAttack(void)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::AVOID_ATTACK)
{
    ;
};


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorAvoidAttack::IsRunnable(void) /*override*/
{
    if (PlayerWatcher().IsLeoDash(4.0f))
        return true;

    if (!AIParam().IsActivityPermission())
        return false;

    if (EnemyChr().AIThinkOrder().GetAnswer() != CAIThinkOrder::RESULT_REFUSE)
        return false;

    if (PlayerWatcher().GetPlayerNumThere(1.0f) <= 0)
        return false;

    return true;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorAvoidAttack::OnActive(void) /*override*/
{
    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_AVOID);
    OrderPush(ELITEAI::PRIORITY_LOW);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorAvoidAttack::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


//
// *********************************************************************************
//


C085MiyamotoUsagiAI::CAIBehaviorCounterAttack::CAIBehaviorCounterAttack(void)
: CAIBehavior(USAGIAI::AIBEHAVIORNAME::ATTACK_COUNTER)
{
    ;
};


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorCounterAttack::IsRunnable(void) /*override*/
{
    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
    {
        AIParam().ClearAttackCountDamage();
        return false;
    };

    if (AIParam().GetAttackCountDamage() <= USAGIAI::COUNTER_ATTACK_REACTION_DAMAGE)
        return false;

    if (PlayerWatcher().GetLastAttackerType() != GAMEOBJECTTYPE::CHARACTER)
        return false;

    AIParam().ClearAttackCountDamage();

    if (IsSatisfyFreq(ELITEAI::FREQUENCY_ATTACK_AVOID))
        return false;

    return true;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorCounterAttack::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_ATTACKLAST);

    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_COUNTER);
    OrderPush(ELITEAI::PRIORITY_NORMAL);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorCounterAttack::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


//
// *********************************************************************************
//


C085MiyamotoUsagiAI::CAIBehaviorMutekiGuard::CAIBehaviorMutekiGuard(void)
: CAIBehavior(USAGIAI::AIBEHAVIORNAME::ATTACK_MUTEKI)
, m_fMutekiDuration(0.0f)
{
    ;
};


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorMutekiGuard::IsRunnable(void) /*override*/
{
    if (!PlayerWatcher().IsShootShuriken(16.0f))
    {
        if (!PlayerWatcher().IsChargeAttack(3.0f))
            return false;
    };

    if (PlayerWatcher().IsDonBarrier(4.0f))
        return false;

    return true;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorMutekiGuard::OnActive(void) /*override*/
{
    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_MUTEKI);
    OrderPush(ELITEAI::PRIORITY_LOW);

    m_fMutekiDuration = 4.0f;

    AIParam().ClearAttackCount();
};


/*virtual*/ C085MiyamotoUsagiAI::CAIBehaviorMutekiGuard::RESULT
C085MiyamotoUsagiAI::CAIBehaviorMutekiGuard::Update(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE)
    {
        AIParam().ClearCounterAttack();
        return RESULT_END;
    };

    m_fMutekiDuration -= CGameProperty::GetElapsedTime();

    if ((m_fMutekiDuration <= 0.0f) ||
        (AIParam().GetAttackCount() >= 3) ||
        (PlayerWatcher().GetLastAttackerType() == GAMEOBJECTTYPE::EFFECT))
    {
        PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_ATTACKLAST);

        OrderAttack(USAGIAI::ORDERTYPE_ATTACK_DASH);
        OrderPush(ELITEAI::PRIORITY_NORMAL);

        OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, 0.5f);
        OrderPush(ELITEAI::PRIORITY_LOW);

        AIParam().ClearAttackCount();

        return RESULT_FORCEEND;
    };

    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_MUTEKI);
    OrderPush(ELITEAI::PRIORITY_LOW);

    return RESULT_CONTINUE;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorMutekiGuard::OnEnd(void) /*override*/
{
    AIParam().ClearAttackCount();
};


//
// *********************************************************************************
//


C085MiyamotoUsagiAI::CAIBehaviorAllPlayerAttack::CAIBehaviorAllPlayerAttack(void)
: CAIBehavior(USAGIAI::AIBEHAVIORNAME::ATTACK_ALLPLAYER)
{
    ;
};


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorAllPlayerAttack::IsRunnable(void) /*override*/
{
    if (CAIUtils::GetPlayerNum() <= 1)
        return false;

    if (AIParam().GetStatus() != ENEMYTYPES::STATUS_GETUP)
        return false;

    float fHpRatio = CEnemyUtils::GetEnemyHPRate(&EnemyChr());
    if (fHpRatio > 0.5f)
        return false;

    if (!IsSatisfyFreq(USAGIAI::FREQUENCY_ATTACK_ALL_PLR))
        return false;

    return true;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorAllPlayerAttack::OnActive(void) /*override*/
{
    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_DASH_PLAYER_1);
    m_AIThinkOrder.OrderAttack().m_iPlayerNo = 0;
    OrderPush(ELITEAI::PRIORITY_NORMAL);

    int32 numPlayers = (CAIUtils::GetPlayerNum() - 2);
    for (int32 i = 0; i < numPlayers; ++i)
    {
        OrderAttack(USAGIAI::ORDERTYPE_ATTACK_DASH_PLAYER_2_3);
        m_AIThinkOrder.OrderAttack().m_iPlayerNo = (i + 1);
        OrderPush(ELITEAI::PRIORITY_NORMAL);
    };

    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_DASH_PLAYER_4);
    m_AIThinkOrder.OrderAttack().m_iPlayerNo = (CAIUtils::GetPlayerNum() - 1);
    OrderPush(ELITEAI::PRIORITY_NORMAL);
};


//
// *********************************************************************************
//


C085MiyamotoUsagiAI::CAIBehaviorStabAttack::CAIBehaviorStabAttack(void)
: CAIBehavior(USAGIAI::AIBEHAVIORNAME::ATTACK_STAB)
{
    ;
};


/*virtual*/ bool C085MiyamotoUsagiAI::CAIBehaviorStabAttack::IsRunnable(void) /*override*/
{
    ELITEAI::ATTACK_RESULT attackResult = AIParam().GetAttackResult();
    AIParam().SetAttackResult(ELITEAI::ATTACK_RESULT_NONE);

    if (attackResult != ELITEAI::ATTACK_RESULT_STAB)
        return false;

    if (EnemyChr().AIThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE)
        return false;

    if (!IsSatisfyFreq(USAGIAI::FREQUENCY_ATTACK_STAB))
        return false;

    return true;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorStabAttack::OnActive(void) /*override*/
{
    m_fStabAttackDuration = 3.0f;
    m_numRollBack = 0;

    AIParam().ClearAttackCount();

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ C085MiyamotoUsagiAI::CAIBehaviorStabAttack::RESULT
C085MiyamotoUsagiAI::CAIBehaviorStabAttack::Update(void) /*override*/
{
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);

    m_fStabAttackDuration -= CGameProperty::GetElapsedTime();
    if ((m_fStabAttackDuration < 0.0f) || (AIParam().GetAttackCountDamage() >= USAGIAI::COUNTER_ATTACK_REACTION_DAMAGE))
    {
        OrderMove(ELITEAI::ORDERTYPE_MOVE_BACK);
        OrderPush(ELITEAI::PRIORITY_LOW);

        AIParam().ClearAttackCount();

        return RESULT_END;
    };

    if (AIParam().GetAttackCount() <= 0)
    {
        OrderWait(ELITEAI::ORDERTYPE_WAIT_TURN);
        OrderPush(ELITEAI::PRIORITY_LOW);
	
        return RESULT_CONTINUE;
    };

    if (++m_numRollBack < 3)
    {
        OrderMove(ELITEAI::ORDERTYPE_MOVE_BACK);
        OrderPush(ELITEAI::PRIORITY_LOW);

        m_fStabAttackDuration = 3.0f;

        AIParam().ClearAttackCount();

        return RESULT_CONTINUE;
    };

    OrderAttack(USAGIAI::ORDERTYPE_ATTACK_AVOID);
    OrderPush(ELITEAI::PRIORITY_LOW);

    return RESULT_END;
};


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorStabAttack::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


//
// *********************************************************************************
//


/*virtual*/ void C085MiyamotoUsagiAI::CAIBehaviorHighRotateAttack::OnActive(void) /*override*/
{
    OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMAL);
    OrderPush(ELITEAI::PRIORITY_NORMAL);
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* C085MiyamotoUsagiAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C085MiyamotoUsagiAI(pEnemyChr);
};


C085MiyamotoUsagiAI::C085MiyamotoUsagiAI(CEnemyCharacter* pEnemyChr)
: CBaseEliteAI(pEnemyChr)
{
    /* init behavior units (attach order is important for priority node) */
    CAIBehavior* pBehavior = nullptr;

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorStabAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorAllPlayerAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorMutekiGuard;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorCounterAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorAvoidAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseEliteAI::CAIBehaviorGuard(true);
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorHighRotateAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorKickAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorRunAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorAvoid;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new C085MiyamotoUsagiAI::CAIBehaviorMove;
    Attach(pBehavior, ELITEAI::PRIORITY_LOW);
};


/*virtual*/ C085MiyamotoUsagiAI::~C085MiyamotoUsagiAI(void)
{
    ;
};


/*virtual*/ void C085MiyamotoUsagiAI::AttackCountInc(void) /*override*/
{
    AIParam().AddAttackCount();
};


/*virtual*/ void C085MiyamotoUsagiAI::AttackCountDamage(int32 iAmount) /*override*/
{
    AIParam().AddAttackCountDamage(iAmount);
};


/*virtual*/ void C085MiyamotoUsagiAI::CatchAttack(uint32 hObj, GAMEOBJECTTYPE::VALUE objType) /*override*/
{
    if (hObj)
    {
        AIParam().PlayerWatcher().SetLastAttackerType(objType);
        AIParam().PlayerWatcher().SetPlayerData(CEnemyUtils::FindPlayerNo(hObj), CPlayerWatcher::PLAYER_DATA_ATTACK);
    };
};


/*virtual*/ void C085MiyamotoUsagiAI::OnStatusEnd(ENEMYTYPES::STATUS statusEnd) /*override*/
{
    if (static_cast<int32>(statusEnd) != C085MiyamotoUsagi::STATUS_MUTEKI)
    {
        if ((AIParam().GetAttackCountDamage() >= USAGIAI::COUNTER_ATTACK_REACTION_DAMAGE) ||
            AIParam().PlayerWatcher().IsShootShuriken(16.0f))
        {
            AIParam().ThinkOrderDelete(ELITEAI::PRIORITY_ALL);
        };
    };
};