#include "091DrakoAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/091Drako.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"

#ifdef _DEBUG
#include "Game/Component/Enemy/EnemyDebug.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#endif /* _DEBUG */


#define DISTANCE_NEAR                   (4.0f)
#define DISTANCE_NEAR_BACK              (DISTANCE_NEAR - 0.5f)
#define DISTANCE_NEAR_FRONT             (DISTANCE_NEAR + 1.0f)
#define DISTANCE_NEAR_SIDE              (DISTANCE_NEAR + 1.0f)
#define DISTANCE_MIDDLE                 (DISTANCE_NEAR * 2.0f)

#define START_THINK_TIMER               (1.5f)
#define TIMEOUT_QUAKE                   (5.0f)
#define TIMEOUT_BEAM                    (5.0f)
#define GUARD_AA_TRIGGER_TIME           (0.5f)


/*static*/ CAIModerator* C091DrakoAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C091DrakoAI(pEnemyChr);
};


C091DrakoAI::C091DrakoAI(CEnemyCharacter* pEnemyChr)
: CAIModerator(pEnemyChr)
, m_rand()
, m_eAIPattern(AIPATTERN_DEFAULT)
, m_fThinkTimer(START_THINK_TIMER)
, m_numPlayers(0)
, m_aPlayerData()
, m_aPlayerInfo()
, m_eOrder(CAIThinkOrder::ORDER_WAIT)
, m_iOrderType(-1)
, m_iOrderTargetNo(-1)
, m_vecOrderPos(Math::VECTOR3_ZERO)
, m_fGuardTimer(0.0f)
, m_eMyPrevStatus(ENEMYTYPES::STATUS_MAX)
, m_fTimeoutBeam(0.0f)
, m_fTimeoutQuake(0.0f)
, m_fHpRatioPrev(0.0f)
{
    std::memset(m_aPlayerData, 0, sizeof(m_aPlayerData));
    std::memset(m_aPlayerInfo, 0, sizeof(m_aPlayerInfo));
};


/*virtual*/ C091DrakoAI::~C091DrakoAI(void)
{
    ;
};


/*virtual*/ void C091DrakoAI::Run(void) /*override*/
{
    CheckPlayerStatus();
    CheckAIPattern();
    CheckDisableOrder();

#ifdef _DEBUG
    if (CheckDebugOrder())
        return;
#endif /* _DEBUG */    

    GuardProc();

    if (ThinkProc())
        ReserveOrderProc();
};


/*virtual*/ void C091DrakoAI::Draw(void) /*override*/
{
    CAIModerator::Draw();
#ifdef _DEBUG
#endif /* _DEBUG */  
};


void C091DrakoAI::GuardProc(void)
{
    ENEMYTYPES::STATUS eMyStatusNow = EnemyCharacter().GetStatus();
    if (eMyStatusNow == ENEMYTYPES::STATUS_GUARD)
    {
        m_fGuardTimer += CGameProperty::GetElapsedTime();
    }
    else if (m_fGuardTimer > 0.0f)
    {
        m_fGuardTimer -= (CGameProperty::GetElapsedTime() / 10.0f);
    };
};


bool C091DrakoAI::ThinkProc(void)
{
    m_fThinkTimer -= CGameProperty::GetElapsedTime();
    if (ThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_ACCEPT)
    {
        m_fThinkTimer = 0.0f;
    }
    else if (m_fThinkTimer > 0.0f)
    {
        return false;
    };

    uint8 thinkFreq = AIFreqParam().GetFrequency(C091Drako::FREQUENCY_THINKING);
    m_fThinkTimer = static_cast<float>(thinkFreq) / 60.0f;
    m_fThinkTimer *= (GetRandNormal() + 1.0f);
    m_fThinkTimer = std::fabs(m_fThinkTimer);

    return true;
};


void C091DrakoAI::ReserveOrderProc(void)
{
    ENEMYTYPES::STATUS eMyStatusNow = EnemyCharacter().GetStatus();
    switch (eMyStatusNow)
    {
    case ENEMYTYPES::STATUS_IDLE:
    case ENEMYTYPES::STATUS_WALK:
    case ENEMYTYPES::STATUS_RUN:
        ReserveOrderDefault();
        break;

    case ENEMYTYPES::STATUS_GUARD:
        ReserveOrderOnGuard();
        break;

    case ENEMYTYPES::STATUS_GETUP:
    case ENEMYTYPES::STATUS_KNOCK_FRONT:
    case ENEMYTYPES::STATUS_KNOCK_BACK:
    case ENEMYTYPES::STATUS_FLYAWAY_FRONT:
    case ENEMYTYPES::STATUS_FLYAWAY_BACK:
    case ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT:
    case ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK:
        ReserveOrderOnDamaged();
        break;

    default:
        ReserveIdle();
        break;
    };

    if (m_fGuardTimer > GUARD_AA_TRIGGER_TIME)
    {
        m_fGuardTimer = 0.0f;
        ReserveAttackAntiAir();
    };

    SetReservedOrder();

    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
};


void C091DrakoAI::CheckPlayerStatus(void)
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecMyFootPos);

    float fMyDir = EnemyCharacter().Compositor().GetDirection();

    m_numPlayers = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < m_numPlayers; ++i)
    {        
        IGamePlayer* pGameplayer = CGameProperty::Player(i);
        CPlayerCharacter* pPlrChr = pGameplayer->GetCurrentCharacter();

        /* init player data */
        RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecPlrFootPos);

        m_aPlayerData[i].vecFootPos = vecPlrFootPos;
        m_aPlayerData[i].fDirection = pPlrChr->GetDirection();
        m_aPlayerData[i].eStatus = pPlrChr->GetStatus();

        /* init player info */
        RwV3d vecDistance = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDistance, &vecPlrFootPos, &vecMyFootPos);
        vecDistance.y = 0.0f;

        m_aPlayerInfo[i].fDistance = std::sqrt(vecDistance.x * vecDistance.x +
                                               vecDistance.z * vecDistance.z);

        if (!Math::Vec3_IsEqual(&vecDistance, &Math::VECTOR3_ZERO))
            m_aPlayerInfo[i].fDirection = std::atan2(vecDistance.x, vecDistance.z);

        float fDirDiff = (m_aPlayerInfo[i].fDirection - fMyDir);
        fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);

        float fDirDiffAbs = std::fabs(fDirDiff);
        ASSERT(fDirDiffAbs <= MATH_PI);

        m_aPlayerInfo[i].fDirection = fDirDiff;

        if (fDirDiffAbs <= MATH_DEG2RAD(45.0f))
            m_aPlayerInfo[i].eSide = PLAYER_INFO::SIDE_FRONT;
        else if (fDirDiffAbs >= MATH_DEG2RAD(135.0f))
            m_aPlayerInfo[i].eSide = PLAYER_INFO::SIDE_BACK;
        else
            m_aPlayerInfo[i].eSide = ((m_aPlayerInfo[i].fDirection >= 0.0) ? PLAYER_INFO::SIDE_LEFT :
                                                                             PLAYER_INFO::SIDE_RIGHT);

        m_aPlayerInfo[i].bIsAttack = IsAttackStatus(m_aPlayerData[i].eStatus);
        m_aPlayerInfo[i].bIsAttackCore = IsAttackStatus(m_aPlayerData[i].eStatus);

        float fMotionNowT = pPlrChr->GetMotionTime();
        float fMotionEndT = pPlrChr->GetMotionEndTime();
        if ((fMotionEndT - 0.3f) < fMotionNowT)
            m_aPlayerInfo[i].bIsAttackCore = false;

        m_aPlayerInfo[i].bIsJump = IsJumpStatus(m_aPlayerData[i].eStatus);
        m_aPlayerInfo[i].bIsDown = IsDownStatus(m_aPlayerData[i].eStatus);
        m_aPlayerInfo[i].bIsGuard = IsGuardStatus(m_aPlayerData[i].eStatus);
    };
};


void C091DrakoAI::CheckAIPattern(void)
{
    float fHpRatioNow = CEnemyUtils::GetEnemyHPRate(&EnemyCharacter());
    if (fHpRatioNow >= 0.66f)
    {
        /* 100% - 66% hp */
        m_eAIPattern = AIPATTERN_HP_HIGH;
    }
    else if (fHpRatioNow >= 0.33f)
    {
        /* 66% - 33% hp */
        m_eAIPattern = AIPATTERN_HP_MEDIUM;
    }
    else
    {
        /* < 33% hp */
        m_eAIPattern = AIPATTERN_HP_LOW;
    };

    m_fHpRatioPrev = fHpRatioNow;
};


void C091DrakoAI::CheckDisableOrder(void)
{
    if (m_fTimeoutBeam > 0.0f)
        m_fTimeoutBeam -= CGameProperty::GetElapsedTime();

    if (m_fTimeoutQuake > 0.0f)
        m_fTimeoutQuake -= CGameProperty::GetElapsedTime();

    ENEMYTYPES::STATUS eMyNowStatus = EnemyCharacter().GetStatus();
    switch (static_cast<int32>(eMyNowStatus))
    {
    case C091Drako::STATUS_ATTACK_BEAM:
        {
            if (m_eMyPrevStatus != eMyNowStatus)
                m_fTimeoutBeam = TIMEOUT_BEAM;
        }
        break;

    case C091Drako::STATUS_ATTACK_QUAKE:
        {
            if (m_eMyPrevStatus != eMyNowStatus)
                m_fTimeoutQuake = TIMEOUT_QUAKE;
        }
        break;

    default:
        break;
    };

    m_eMyPrevStatus = eMyNowStatus;
};


void C091DrakoAI::SetReservedOrder(void)
{
    ThinkOrder().SetOrder(m_eOrder);

    switch (m_eOrder)
    {
    case CAIThinkOrder::ORDER_WAIT:
        {
            ThinkOrder().OrderWait().m_iWaitType  = m_iOrderType;
            ThinkOrder().OrderWait().m_fWaitTimer = m_fThinkTimer;
            ThinkOrder().OrderWait().m_iPlayerNo  = m_iOrderTargetNo;
            ThinkOrder().OrderWait().m_vAt        = m_vecOrderPos;
        }
        break;

    case CAIThinkOrder::ORDER_MOVE:
        {
            ThinkOrder().OrderMove().m_iMoveType  = m_iOrderType;
            ThinkOrder().OrderMove().m_fMoveTimer = m_fThinkTimer;
            ThinkOrder().OrderMove().m_iPlayerNo  = m_iOrderTargetNo;
            ThinkOrder().OrderMove().m_vAt        = m_vecOrderPos;
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            ThinkOrder().OrderAttack().m_iAttackType = m_iOrderType;
            ThinkOrder().OrderAttack().m_iPlayerNo   = m_iOrderTargetNo;
            ThinkOrder().OrderAttack().m_vAt         = m_vecOrderPos;
        }
        break;

    case CAIThinkOrder::ORDER_DEFENCE:
        {
            ThinkOrder().OrderDefence().m_iDefenceType = m_iOrderType;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    ReserveIdle();
};


void C091DrakoAI::ReserveOrderOnDamaged(void)
{
    if (IsNearAttackPlayer())
        ReserveGuard();
    else
        ReserveOrderOnGuard();
};


void C091DrakoAI::ReserveOrderOnGuard(void)
{
    if (IsNearAttackCorePlayer())
    {
        ReserveGuard();
    }
    else if (GetNearBackPlayerNum())
    {
        ReserveAttackTail();
    }
    else if (GetNearSidePlayerNum())
    {
        ReserveAttackEnergyBall();
    }
    else if (GetNearFrontPlayerNum())
    {
        if (IsNearFrontGuardPlayerOnly())
            ReserveAttackThrow();
        else
            ReserveAttackSomersault();
    }
    else if (GetMiddleFrontPlayerNum())
    {
        ReserveAttackRush();
    }
    else if (GetMiddlePlayerNum())
    {
        ReserveAttackBeam();
    }
    else if (IsNearFrontDownPlayerOnly())
    {
        ReserveIdle();
    }
    else
    {
        ReserveWalkNearestPlayer();
    };
};


void C091DrakoAI::ReserveOrderDefault(void)
{
    if (IsNearAttackCorePlayer())
    {
        ReserveGuard();
    }
    else
    {
        if (ReserveOrderDefault_OnHpAll())
            return;

        switch (m_eAIPattern)
        {
        case AIPATTERN_HP_HIGH:   ReserveOrderDefault_OnHpHigh();   break;
        case AIPATTERN_HP_MEDIUM: ReserveOrderDefault_OnHpMedium(); break;
        case AIPATTERN_HP_LOW:    ReserveOrderDefault_OnHpLow();    break;
        default: break;
        };
    };
};


bool C091DrakoAI::ReserveOrderDefault_OnHpAll(void)
{
    if (GetNearBackPlayerNum())
    {
        ReserveAttackTail();
        return true;
    };

    if (GetNearSidePlayerNum())
    {
        ReserveAttackEnergyBall();
        return true;
    };

    return false;
};


void C091DrakoAI::ReserveOrderDefault_OnHpHigh(void)
{
    if (!GetNearFrontPlayerNum())
    {
        if (GetMiddlePlayerNum())
        {
            if (CanBeam())
            {
                ReserveAttackBeam();
                return;
            };
        }
        else if (IsNearFrontDownPlayerOnly())
        {
            ReserveIdle();
            return;
        };

        ReserveWalkNearestPlayer();
        return;
    };

    if (!IsNearFrontGuardPlayerOnly())
        ReserveAttackEnergyBall();
    else
        ReserveAttackThrow();
};


void C091DrakoAI::ReserveOrderDefault_OnHpMedium(void)
{
    if (GetNearFrontPlayerNum())
    {
        if (IsNearFrontGuardPlayerOnly())
            ReserveAttackThrow();
        else
            ReserveAttackRush();

        return;
    };

    if (GetMiddleFrontPlayerNum())
    {
        ReserveAttackRush();
        return;
    };

    if (IsNearFrontDownPlayerOnly())
        ReserveIdle();
    else
        ReserveRunNearestPlayer();
};


void C091DrakoAI::ReserveOrderDefault_OnHpLow(void)
{
    if (!GetNearFrontPlayerNum())
    {
        if (GetMiddlePlayerNum())
        {
            if (CanQuake())
            {
                ReserveAttackQuake();
                return;
            };
        }
        else if (IsNearFrontDownPlayerOnly())
        {
            ReserveIdle();
            return;
        };

        ReserveWalkNearestPlayer();
        return;
    };

    if (!IsNearFrontGuardPlayerOnly())
        ReserveAttackEnergyBall();
    else
        ReserveAttackThrow();
};


void C091DrakoAI::ReserveIdle(void)
{
    m_eOrder = CAIThinkOrder::ORDER_WAIT;
    m_iOrderType = ORDERTYPE::WAIT_IDLE;
};


void C091DrakoAI::ReserveWalkNearestPlayer(void)
{
    m_eOrder = CAIThinkOrder::ORDER_MOVE;
    m_iOrderType = ORDERTYPE::MOVE_WALK_NO;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C091DrakoAI::ReserveRunNearestPlayer(void)
{
    m_eOrder = CAIThinkOrder::ORDER_MOVE;
    m_iOrderType = ORDERTYPE::MOVE_RUN_NO;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C091DrakoAI::ReserveGuard(void)
{
    m_eOrder = CAIThinkOrder::ORDER_DEFENCE;
};


void C091DrakoAI::ReserveAttackEnergyBall(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_ENERGY_BALL;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C091DrakoAI::ReserveAttackTail(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_TAIL;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C091DrakoAI::ReserveAttackSomersault(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_SOMERSAULT;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C091DrakoAI::ReserveAttackRush(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_RUSH;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C091DrakoAI::ReserveAttackAntiAir(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_ANTI_AIR;
};


void C091DrakoAI::ReserveAttackBeam(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_BEAM;
};


void C091DrakoAI::ReserveAttackQuake(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_QUAKE;
};


void C091DrakoAI::ReserveAttackThrow(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_THROW;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


bool C091DrakoAI::CanBeam(void) const
{
    return (m_fTimeoutBeam <= 0.0f);
};


bool C091DrakoAI::CanQuake(void) const
{
    return (m_fTimeoutQuake <= 0.0f);
};


float C091DrakoAI::GetRandNormal(void)
{
    return m_rand.Generate();
};


bool C091DrakoAI::IsAttackStatus(PLAYERTYPES::STATUS status) const
{
    return (status == PLAYERTYPES::STATUS_ATTACK_A)
        || (status == PLAYERTYPES::STATUS_ATTACK_AA)
        || (status == PLAYERTYPES::STATUS_ATTACK_AAB)
        || (status == PLAYERTYPES::STATUS_ATTACK_AAC)
        || (status == PLAYERTYPES::STATUS_ATTACK_AABB)
        || (status == PLAYERTYPES::STATUS_ATTACK_AABC)
        || (status == PLAYERTYPES::STATUS_ATTACK_AABBB)
        || (status == PLAYERTYPES::STATUS_ATTACK_AABBC)
        || (status == PLAYERTYPES::STATUS_ATTACK_B)
        || (status == PLAYERTYPES::STATUS_ATTACK_RUN)
        || (status == PLAYERTYPES::STATUS_ATTACK_JUMP);
};


bool C091DrakoAI::IsJumpStatus(PLAYERTYPES::STATUS status) const
{
    return (status == PLAYERTYPES::STATUS_JUMP_READY)
        || (status == PLAYERTYPES::STATUS_JUMP)
        || (status == PLAYERTYPES::STATUS_JUMP_2ND)
        || (status == PLAYERTYPES::STATUS_JUMP_WALL)
        || (status == PLAYERTYPES::STATUS_JUMP_WALL_FAILURE)
        || (status == PLAYERTYPES::STATUS_AERIAL)
        || (status == PLAYERTYPES::STATUS_AERIAL_MOVE);
};


bool C091DrakoAI::IsDownStatus(PLAYERTYPES::STATUS status) const
{
    return (status == PLAYERTYPES::STATUS_TUMBLER_FRONT)
        || (status == PLAYERTYPES::STATUS_TUMBLER_BACK)
        || (status == PLAYERTYPES::STATUS_DOWN_FRONT)
        || (status == PLAYERTYPES::STATUS_DOWN_BACK);
};


bool C091DrakoAI::IsGuardStatus(PLAYERTYPES::STATUS status) const
{
    return (status == PLAYERTYPES::STATUS_GUARD_READY)
        || (status == PLAYERTYPES::STATUS_GUARD_IMPACT)
        || (status == PLAYERTYPES::STATUS_GUARD_FINISH)
        || (status == PLAYERTYPES::STATUS_GUARD_BACK)
        || (status == PLAYERTYPES::STATUS_GUARD);
};


int32 C091DrakoAI::GetNearestPlayerNo(void) const
{
    /*
     *  NOTE: it will pick always nearest player for order even if player is down
     */

    float fNearestDist = TYPEDEF::FLOAT_MAX;
    int32 nearestPlrNo = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        if (m_aPlayerInfo[i].fDistance < fNearestDist)
        {
            fNearestDist = m_aPlayerInfo[i].fDistance;
            nearestPlrNo = i;
        };
    };

    return nearestPlrNo;
};


int32 C091DrakoAI::GetNearBackPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsInBack = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_BACK);
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR_BACK);
        bool bIsDown = m_aPlayerInfo[i].bIsDown;

        if (bIsNearDist && bIsInBack && !bIsDown)
            ++result;
    };

    return result;
};


int32 C091DrakoAI::GetNearSidePlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsInSide = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_LEFT) ||
                         (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_RIGHT);
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR_SIDE);
        bool bIsDown = m_aPlayerInfo[i].bIsDown;

        if (bIsNearDist && bIsInSide && !bIsDown)
            ++result;
    };

    return result;
};


int32 C091DrakoAI::GetNearFrontPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR_FRONT);
        bool bIsDown = m_aPlayerInfo[i].bIsDown;

        if (bIsNearDist && bIsInFront && !bIsDown)
            ++result;
    };

    return result;
};


int32 C091DrakoAI::GetMiddleFrontPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);
        bool bIsMiddleDist = (m_aPlayerInfo[i].fDistance >  DISTANCE_NEAR_FRONT) &&
                             (m_aPlayerInfo[i].fDistance <= DISTANCE_MIDDLE);
        bool bIsDown = m_aPlayerInfo[i].bIsDown;

        if (bIsMiddleDist && bIsInFront && !bIsDown)
            ++result;
    };

    return result;
};


int32 C091DrakoAI::GetMiddlePlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        float fNearDist = m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_BACK ? (DISTANCE_NEAR_BACK) : (DISTANCE_NEAR_FRONT);
        bool bIsMiddleDist = (m_aPlayerInfo[i].fDistance > fNearDist) &&
                             (m_aPlayerInfo[i].fDistance <= DISTANCE_MIDDLE);
        bool bIsDown = m_aPlayerInfo[i].bIsDown;

        if (bIsMiddleDist && !bIsDown)
            ++result;
    };

    return result;
};


bool C091DrakoAI::IsNearAttackPlayer(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsAttack = (m_aPlayerInfo[i].bIsAttack);

        if (bIsNearDist && bIsAttack)
            return true;
    };

    return false;
};


bool C091DrakoAI::IsNearAttackCorePlayer(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsAttackCore = (m_aPlayerInfo[i].bIsAttackCore);

        if (bIsNearDist && bIsAttackCore)
            return true;
    };

    return false;
};


bool C091DrakoAI::IsNearFrontDownPlayerOnly(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR_FRONT);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);
        bool bIsDown = (m_aPlayerInfo[i].bIsDown);

        if (bIsNearDist && bIsInFront)
        {
			if (!bIsDown)
				return true;
        };
    };

	return false;
};


bool C091DrakoAI::IsNearFrontGuardPlayerOnly(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR_FRONT);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);
        bool bIsGuard = (m_aPlayerInfo[i].bIsGuard);

        if (!bIsNearDist && bIsInFront && !bIsGuard)
            return false;
    };

    return true;
};


bool C091DrakoAI::CheckDebugOrder(void)
{
#ifdef _DEBUG
    return false;
#else /* _DEBUG */
	return false;
#endif /* _DEBUG */
};