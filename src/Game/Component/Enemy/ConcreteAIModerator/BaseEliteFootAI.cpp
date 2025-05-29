#include "BaseEliteFootAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseEliteFootChr.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"


/*virtual*/ bool CBaseEliteFootAI::CAIBehaviorMove::IsRunnable(void) /*override*/
{
    if (!CBaseEliteAI::CAIBehaviorMove::IsRunnable())
        return false;

    if (CBaseEliteFootAI::IsEliteCStage())
    {
        RwV3d vecNearerTargetPos = PlayerWatcher().GetTargetPlayerData().position;
        if (!AIParam().IsInsideTerritory(&vecNearerTargetPos))
            return false;
    };

    return true;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorMove::OnActive(void) /*override*/
{
    OrderMove(ELITEAI::ORDERTYPE_MOVE_WALK);

    if (!PlayerWatcher().IsPlayerSuitRange(-1, 1.5f))
    {
        OrderMove(ELITEAI::ORDERTYPE_MOVE_RUN);

        if (EnemyChr().IsAttachedStatusObserver(ENEMYTYPES::STATUS_DASH) &&
            (m_fDashTimeout < 0.0f))
        {
            if (IsSatisfyFreq(ELITEFOOTAI::FREQUENCY_DASH))
                OrderMove(ELITEAI::ORDERTYPE_MOVE_DASH);
            
            m_fDashTimeout = 1.0f;
        };

        if (!CBaseEliteFootAI::IsEliteCStage())
        {
            RwV3d vecNearerTargetPos = PlayerWatcher().GetTargetPlayerData().position;            

            if (AIParam().IsInsideTerritory(&vecNearerTargetPos) &&
                (m_fReachLimit < 0.0f) &&
                EnemyChr().IsAttachedStatusObserver(ENEMYTYPES::STATUS_ATTACK_RUN))
            {
                OrderMove(ELITEFOOTAI::ORDERTYPE_MOVE_WARP_NO);
                m_fReachLimit = 3.0f;
            };
        };
    };

    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


CBaseEliteFootAI::CAIBehaviorNormalAttack::CAIBehaviorNormalAttack(void)
: CAIBehaviorNormalAttack(ELITEAI::AIBEHAVIORNAME::ATTACK)
{
    ;
};


CBaseEliteFootAI::CAIBehaviorNormalAttack::CAIBehaviorNormalAttack(const char* pszName)
: CBaseEliteAI::CAIBehaviorAttack(pszName)
, m_bSpinDown(false)
, m_fAttackTimeout(0.0f)
{
    ;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorNormalAttack::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fAttackTimeout, "Attack");
};


/*virtual*/ bool CBaseEliteFootAI::CAIBehaviorNormalAttack::IsRunnable(void) /*override*/
{
    if (m_fAttackTimeout > 0.0f)
        return false;
    
    if (!CBaseEliteAI::CAIBehaviorAttack::IsRunnable())
        return false;

    int32 numPlayers = CAIUtils::GetPlayerNum();
    if (numPlayers == 1)
    {
        uint32 hEliteDummy = AIParam().GetEliteDummyHandle();
        CGameObject* pGameObj = CGameObjectManager::GetObject(hEliteDummy);
        if (pGameObj)
        {
            CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pGameObj);
            if (CEnemyUtils::IsAttackStateNormal(pChrCompositor->GetStatus()))
                return false;
        };
    };
    
    m_fAttackTimeout = GetFreqRatio(ELITEFOOTAI::FREQUENCY_WAIT) * 2.0f;

    return true;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorNormalAttack::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_SUIT);

    OrderAttack(ELITEAI::ORDERTYPE_ATTACK_NORMAL);

    if (IsSatisfyFreq(ELITEFOOTAI::FREQUENCY_ATTACK_SPECIAL))
    {
        if (CBaseEliteFootAI::IsAttackEnabled(&EnemyChr(), ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN))
        {
            int32 attackType = ELITEFOOTAI::ORDERTYPE_ATTACK_SPIN;

            if (IsSatisfyFreq(ELITEFOOTAI::FREQUENCY_ATTACK_SPECIAL_EX))
                attackType = ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN;

            OrderAttack(attackType);
            m_bSpinDown = (attackType == ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN);
        }
        else
        {
            int32 attackType = ELITEFOOTAI::ORDERTYPE_ATTACK_SWING;

            bool bIsAttackAvailable = CBaseEliteFootAI::IsAttackEnabled(&EnemyChr(), ELITEFOOTAI::ORDERTYPE_ATTACK_DROP);
            bool bIsSatisfyFreq = IsSatisfyFreq(ELITEFOOTAI::FREQUENCY_ATTACK_SPECIAL_EX);

            if (bIsAttackAvailable && bIsSatisfyFreq)
                attackType = ELITEFOOTAI::ORDERTYPE_ATTACK_DROP;

            OrderAttack(attackType);
        };
    };

    OrderPush(ELITEAI::PRIORITY_LOW);

    if (m_AIThinkOrder.OrderAttack().m_iAttackType == ELITEFOOTAI::ORDERTYPE_ATTACK_DROP)
    {
        float fWaitTime = GetFreqRatio(ELITEFOOTAI::FREQUENCY_WAIT) * 0.5f;
        OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, fWaitTime);
        OrderPush(ELITEAI::PRIORITY_LOW);
    };
};


//
// *********************************************************************************
//


CBaseEliteFootAI::CAIBehaviorAvoidAttack::CAIBehaviorAvoidAttack(void)
: CAIBehaviorNormalAttack(ELITEAI::AIBEHAVIORNAME::AVOID_ATTACK)
, m_bAvoidOnly(false)
{
    ;
};


/*virtual*/ bool CBaseEliteFootAI::CAIBehaviorAvoidAttack::IsRunnable(void) /*override*/
{
    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
        return false;

    if (AIParam().GetAttackCountDamage() >= ELITEFOOTAI::AVOID_ATTACK_REACTION_DAMAGE)
        return true;

    if ((AIParam().GetStatus() == ENEMYTYPES::STATUS_GETUP) &&
        (EnemyChr().AIThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE))
        return true;

    bool bIsChargeAttack = PlayerWatcher().IsChargeAttack(3.0f);
    bool bIsDonBarrier = PlayerWatcher().IsDonBarrier(4.0f);
    bool bIsLeoDash = PlayerWatcher().IsLeoDash(4.0f);

    if ((bIsChargeAttack && !bIsDonBarrier) || bIsLeoDash)
        return true;
    
    return false;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorAvoidAttack::OnActive(void) /*override*/
{
    PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_ATTACKLAST);

    if ((AIParam().GetStatus() == ENEMYTYPES::STATUS_GETUP) ||
        (AIParam().IsCounterAttack()))
    {
        if (IsSatisfyFreq(ELITEAI::FREQUENCY_ATTACK_AVOID) || AIParam().IsCounterAttack())
        {
            Avoid();
        }
        else
        {
            CAIBehaviorNormalAttack::OnActive();
        };
    }
    else if (IsSatisfyFreq(ELITEAI::FREQUENCY_ATTACK_AVOID))
    {
        Avoid();
    }
    else if (IsSatisfyFreq(ELITEAI::FREQUENCY_GUARD))
    {
        Guard();
    };

    AIParam().ClearAttackCountDamage();
};


/*virtual*/ CAIBehavior::RESULT
CBaseEliteFootAI::CAIBehaviorAvoidAttack::Update(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorAvoidAttack::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


void CBaseEliteFootAI::CAIBehaviorAvoidAttack::Guard(void)
{
    OrderDefence(ELITEAI::ORDERTYPE_DEFENCE_NEAR);
    OrderPush(ELITEAI::PRIORITY_LOW);
};


void CBaseEliteFootAI::CAIBehaviorAvoidAttack::Avoid(void)
{
    OrderMove(ELITEFOOTAI::ORDERTYPE_MOVE_WARP_NO);
    OrderPush(ELITEAI::PRIORITY_NORMAL);

    CAIBehaviorNormalAttack::OnActive();

    if (!m_bAvoidOnly)
        AfterAvoidSet();

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


void CBaseEliteFootAI::CAIBehaviorAvoidAttack::AfterAvoidSet(void)
{
    OrderMove(ELITEFOOTAI::ORDERTYPE_MOVE_WARP_POS);
    m_AIThinkOrder.OrderMove().m_vAt = { 0.0f, 0.0f, 6.0f };
    OrderPush(ELITEAI::PRIORITY_NORMAL);

    float fWaitTime = GetFreqRatio(ELITEFOOTAI::FREQUENCY_WAIT) * 0.5f;
    OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, fWaitTime);
    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorHighAttack::OnActive(void) /*override*/
{
    if (CBaseEliteFootAI::IsAttackEnabled(&EnemyChr(), ELITEAI::ORDERTYPE_ATTACK_HIGH))
        OrderAttack(ELITEAI::ORDERTYPE_ATTACK_HIGH);
    else
        OrderAttack(ELITEFOOTAI::ORDERTYPE_ATTACK_SWING);

    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


CBaseEliteFootAI::CAIBehaviorSpearAttack::CAIBehaviorSpearAttack(void)
: CAIBehavior(ELITEFOOTAI::AIBEHAVIORNAME::ATTACKSPEAR)
, m_fSpearTimeout(0.0f)
{
    ;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorSpearAttack::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fSpearTimeout, "Spear");
};


/*virtual*/ bool CBaseEliteFootAI::CAIBehaviorSpearAttack::IsRunnable(void) /*override*/
{
    if (m_fSpearTimeout > 0.0f)
        return false;

    if (!AIParam().IsActivityPermission())
        return false;

    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
        return false;

    if (PlayerWatcher().IsPlayerSuitRange(-1, 1.5f))
        return false;

    if (PlayerWatcher().IsPlayerWithinRange(-1, 6.0f))
        m_fSpearTimeout = 10.0f;
    else
        m_fSpearTimeout = 3.0f;

    return true;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorSpearAttack::OnActive(void) /*override*/
{
    RwV3d vecTargetPos = PlayerWatcher().GetTargetPlayerData().position;
    vecTargetPos.y = CWorldMap::GetMapHeight(&vecTargetPos);

    OrderAttack(ELITEFOOTAI::ORDERTYPE_ATTACK_SPEAR);
    m_AIThinkOrder.OrderAttack().m_vAt = vecTargetPos;
    OrderPush(ELITEAI::PRIORITY_NORMAL);
};


//
// *********************************************************************************
//


CBaseEliteFootAI::CAIBehaviorDefend::CAIBehaviorDefend(void)
: CAIBehavior(ELITEFOOTAI::AIBEHAVIORNAME::DEFEND)
, m_fWarpTime(0.0f)
, m_fDefendTime(0.0f)
, m_avecWaitPosOffset()
{
    for (int32 i = 0; i < COUNT_OF(m_avecWaitPosOffset); ++i)
        m_avecWaitPosOffset[i] = Math::VECTOR3_ZERO;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorDefend::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fDefendTime, "Defend");

    m_avecWaitPosOffset[0] = {  0.0f,  0.0f,  0.0f }; // real elite guard offset center
    m_avecWaitPosOffset[1] = {  0.0f,  0.0f, -2.0f }; // elite guard shadow dummy 1 offset forward
    m_avecWaitPosOffset[2] = { -2.0f,  0.0f,  0.0f }; // elite guard shadow dummy 2 offset right
    m_avecWaitPosOffset[3] = {  2.0f,  0.0f,  0.0f }; // elite guard shadow dummy 3 offset left
};


/*virtual*/ bool CBaseEliteFootAI::CAIBehaviorDefend::IsRunnable(void) /*override*/
{
    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ALL))
        return false;

    m_fWarpTime -= CGameProperty::GetElapsedTime();

    RwV3d vecMyTerritoryCenter = Math::VECTOR3_ZERO;
    GetMyTerritoryCenter(&vecMyTerritoryCenter);

    float fDistance = CEnemyUtilsElite::GetDistance(&EnemyChr().Compositor(), &vecMyTerritoryCenter);
    if (fDistance < 0.5f)
        return false;

    RwV3d vecTargetPos = PlayerWatcher().GetTargetPlayerData().position;
    if (AIParam().IsInsideTerritory(&vecTargetPos))
    {
        m_fDefendTime = 3.0f;
		return false;
    };

	return (m_fDefendTime <= 0.0f);
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorDefend::OnActive(void) /*override*/
{
    if ((m_fWarpTime <= 0.0f) &&
        EnemyChr().IsAttachedStatusObserver(ENEMYTYPES::STATUS_ATTACK_RUN) &&
        (AIParam().GetTerritoryDist() > 5.0f))
    {
        OrderMove(ELITEFOOTAI::ORDERTYPE_MOVE_WARP_POS);
        m_fWarpTime = 1.0f;
    }
    else
    {
        RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

        if (AIParam().GetTerritoryDist(&vecMyFootPos) > 2.0f)
            OrderMove(ELITEAI::ORDERTYPE_MOVE_RUN);
        else
            OrderMove(ELITEAI::ORDERTYPE_MOVE_WALK);
    };

    RwV3d vecMyTerritoryCenter = Math::VECTOR3_ZERO;
    GetMyTerritoryCenter(&vecMyTerritoryCenter);

    m_AIThinkOrder.OrderMove().m_vAt = vecMyTerritoryCenter;
    m_AIThinkOrder.OrderMove().m_iPlayerNo = -1;

    OrderPush(ELITEAI::PRIORITY_LOW);
};


void CBaseEliteFootAI::CAIBehaviorDefend::GetMyTerritoryCenter(RwV3d* pvecTerritoryCenter) const
{
    ASSERT(pvecTerritoryCenter != nullptr);

    int32 eliteDummyNo = AIParam().GetEliteDummyNo();
    ASSERT(eliteDummyNo >= 0);
    ASSERT(eliteDummyNo < COUNT_OF(m_avecWaitPosOffset));

    RwV3d vecTerritoryCenter = Math::VECTOR3_ZERO;
    AIParam().GetTerritoryCenter(&vecTerritoryCenter);
    Math::Vec3_Add(&vecTerritoryCenter, &vecTerritoryCenter, &m_avecWaitPosOffset[eliteDummyNo]);

    *pvecTerritoryCenter = vecTerritoryCenter;
};


//
// *********************************************************************************
//


CBaseEliteFootAI::CAIBehaviorBunshin::CAIBehaviorBunshin(void)
: CAIBehavior(ELITEFOOTAI::AIBEHAVIORNAME::BUNSHIN)
, m_phase(0)
{
    Reset();
};


/*virtual*/ bool CBaseEliteFootAI::CAIBehaviorBunshin::IsRunnable(void) /*override*/
{
    float fHpRatio = CEnemyUtils::GetEnemyHPRate(&EnemyChr());

    if ((m_phase == 0) && (fHpRatio <= 0.7f))
        return true;

    if ((m_phase == 1) && (fHpRatio <= 0.4f))
        return true;

    return false;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorBunshin::OnActive(void) /*override*/
{
    OrderAttack(ELITEFOOTAI::ORDERTYPE_ATTACK_BUNSHIN);
    OrderPush(ELITEAI::PRIORITY_HIGH);

    float fWaitTime = GetFreqRatio(ELITEFOOTAI::FREQUENCY_WAIT);
    OrderWait(ELITEAI::ORDERTYPE_WAIT_TIMEOUT, fWaitTime);
    OrderPush(ELITEAI::PRIORITY_LOW);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    ++m_phase;
};


/*virtual*/ void CBaseEliteFootAI::CAIBehaviorBunshin::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


void CBaseEliteFootAI::CAIBehaviorBunshin::Reset(void)
{
    m_phase = 0;
};


//
// *********************************************************************************
//


/*static*/ bool CBaseEliteFootAI::IsAttackEnabled(CEnemyCharacter* pEnemyChr, int32 attackType)
{
    CBaseEliteFootChr* pBaseEliteFootChr = static_cast<CBaseEliteFootChr*>(pEnemyChr);
    CBaseEliteFootChr::ELITETYPE eliteType = pBaseEliteFootChr->GetEliteType();

    switch (attackType)
    {
    case ELITEAI::ORDERTYPE_ATTACK_NORMAL:
        return true;

    case ELITEAI::ORDERTYPE_ATTACK_HIGH:
    case ELITEFOOTAI::ORDERTYPE_ATTACK_SPINDOWN:
    case ELITEFOOTAI::ORDERTYPE_ATTACK_SPIN:
        {
            if ((eliteType == CBaseEliteFootChr::ELITETYPE_A) ||
                (eliteType == CBaseEliteFootChr::ELITETYPE_D))
                return true;
        }
        break;

    case ELITEFOOTAI::ORDERTYPE_ATTACK_SWING:
    case ELITEFOOTAI::ORDERTYPE_ATTACK_DROP:
        {
            if (eliteType == CBaseEliteFootChr::ELITETYPE_B)
                return true;
        }
        break;

    case ELITEFOOTAI::ORDERTYPE_ATTACK_SPEAR:
        {
            if (eliteType == CBaseEliteFootChr::ELITETYPE_C)
                return true;
        }        
        break;

    case ELITEFOOTAI::ORDERTYPE_ATTACK_BUNSHIN:
        return true;

    default:
        break;
    };

    return false;
};


/*static*/ bool CBaseEliteFootAI::IsEliteCStage(void)
{
    return ((CEnemyUtils::GetNowMapID() == MAPID::ID_M54F) ||
            (CEnemyUtils::GetNowMapID() == MAPID::ID_M54FB));
};


/*static*/ CAIModerator* CBaseEliteFootAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseEliteFootAI(pEnemyChr);
};


CBaseEliteFootAI::CBaseEliteFootAI(CEnemyCharacter* pEnemyChr)
: CBaseEliteAI(pEnemyChr)
, m_fAttackCounterClearTimer(0.0f)
{
    /* init behavior units (attach order is important for priority node) */
    CAIBehavior* pBehavior = nullptr;

    pBehavior = new CBaseEliteFootAI::CAIBehaviorBunshin;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseEliteFootAI::CAIBehaviorAvoidAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseEliteAI::CAIBehaviorGuard(true);
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseEliteAI::CAIBehaviorAvoidEnclose;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseEliteFootAI::CAIBehaviorHighAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new CBaseEliteFootAI::CAIBehaviorNormalAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    if ((EnemyChr().GetID() == ENEMYID::ID_ELITE_FOOT_C) ||
        (EnemyChr().GetID() == ENEMYID::ID_ELITE_FOOT_DUMMY_C))
    {
        if (IsEliteCStage())
        {
            pBehavior = new CBaseEliteFootAI::CAIBehaviorDefend;
            Attach(pBehavior, ELITEAI::PRIORITY_HIGH);
        };

        pBehavior = new CBaseEliteFootAI::CAIBehaviorSpearAttack;
        Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);
    };

    pBehavior = new CBaseEliteFootAI::CAIBehaviorMove;
    Attach(pBehavior, ELITEAI::PRIORITY_LOW);

    /* set elite guard no */
    AIParam().SetEliteDummyNo(0);
};


/*virtual*/ CBaseEliteFootAI::~CBaseEliteFootAI(void)
{
    ;
};


/*virtual*/ void CBaseEliteFootAI::Run(void) /*override*/
{
    m_fAttackCounterClearTimer -= CGameProperty::GetElapsedTime();

    CBaseEliteAI::Run();

    if (m_fAttackCounterClearTimer < 0.0f)
    {
        AIParam().ClearAttackCountDamage();
        m_fAttackCounterClearTimer = 0.0f;
    };
};


/*virtual*/ void CBaseEliteFootAI::AttackCountDamage(int32 iAmount) /*override*/
{
    AIParam().AddAttackCountDamage(iAmount);
    m_fAttackCounterClearTimer = 1.0f;
};


/*virtual*/ void CBaseEliteFootAI::CatchAttack(uint32 hAttacker) /*override*/
{
    if (!hAttacker)
        return;
    
    CGameObject* pAttackerObj = CGameObjectManager::GetObject(hAttacker);
    GAMEOBJECTTYPE::VALUE attackerObjType = pAttackerObj->GetType();

    int32 attackerPlayerNo = CEnemyUtils::FindPlayerNo(hAttacker);

    AIParam().PlayerWatcher().SetLastAttackerType(attackerObjType);
    AIParam().PlayerWatcher().SetPlayerData(attackerPlayerNo, CPlayerWatcher::PLAYER_DATA_ATTACK);
};


/*virtual*/ void CBaseEliteFootAI::OnStatusEnd(ENEMYTYPES::STATUS statusEnd) /*override*/
{
    switch (static_cast<int32>(statusEnd))
    {
    case CBaseEliteFootChr::STATUS_BUNSHIN:
        AIParam().ThinkOrderDeleteRequest(ELITEAI::PRIORITY_HIGH);
        ThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
        break;

    case ENEMYTYPES::STATUS_ATTACK_RUN: // warp
        ThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
        break;

    default:
        break;
    };
};

//
// *********************************************************************************
//


/*static*/ CAIModerator* CBaseEliteFootDummyAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseEliteFootDummyAI(pEnemyChr);
};


CBaseEliteFootDummyAI::CBaseEliteFootDummyAI(CEnemyCharacter* pEnemyChr)
: CBaseEliteAI(pEnemyChr)
{
    /* init behavior units */
    CAIBehavior* pBehavior = nullptr;

    pBehavior = new CBaseEliteAI::CAIBehaviorGuard(false);
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);

    pBehavior = new CBaseEliteFootAI::CAIBehaviorHighAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    pBehavior = new CBaseEliteFootAI::CAIBehaviorNormalAttack;
    Attach(pBehavior, ELITEAI::PRIORITY_NORMAL);

    if ((EnemyChr().GetID() == ENEMYID::ID_ELITE_FOOT_C) ||
        (EnemyChr().GetID() == ENEMYID::ID_ELITE_FOOT_DUMMY_C))
    {
        if (CBaseEliteFootAI::IsEliteCStage())
        {
            pBehavior = new CBaseEliteFootAI::CAIBehaviorDefend;
            Attach(pBehavior, ELITEAI::PRIORITY_HIGH);
        };
    };

    Attach(new CBaseEliteFootAI::CAIBehaviorMove, ELITEAI::PRIORITY_LOW);

    /* set elite guard no */
    AIParam().SetEliteDummyNo(1);
};


/*virtual*/ CBaseEliteFootDummyAI::~CBaseEliteFootDummyAI(void)
{
    ;
};