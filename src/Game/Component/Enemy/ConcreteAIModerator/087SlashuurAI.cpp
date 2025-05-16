#include "087SlashuurAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/087Slashuur.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Map/WorldMap.hpp"

#ifdef _DEBUG
#include "Game/Component/Enemy/EnemyDebug.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#endif /* _DEBUG */


#define DISTANCE_NEAR                   (4.325f)
#define DISTANCE_MIDDLE                 (DISTANCE_NEAR * 2.0f)

#define START_THINK_TIMER               (1.5f)
#define TIMEOUT_SHOT                    (4.0f)
#define TIMEOUT_ESCAPE                  (2.0f)
#define TIMEOUT_TELEPORT                (15.0f)
#define TIMEOUT_TELEPORT_TRIGGER_COUNT  (3)
#define GUARD_ESCAPE_TRIGGER_TIME       (0.5f)


/*static*/ CAIModerator* C087SlashuurAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C087SlashuurAI(pEnemyChr);
};


C087SlashuurAI::C087SlashuurAI(CEnemyCharacter* pEnemyChr)
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
, m_iTeleportReps(0)
, m_fTimeoutTeleport(0.0f)
, m_fTimeoutShot(0.0f)
, m_fTimeoutEscape(0.0f)
, m_fTimeoutRunAttack(0.0f)
, m_fHpRatioPrev(0.0f)
, m_bDrainAbility(false)
{
    std::memset(m_aPlayerData, 0, sizeof(m_aPlayerData));
    std::memset(m_aPlayerInfo, 0, sizeof(m_aPlayerInfo));
};


/*virtual*/ C087SlashuurAI::~C087SlashuurAI(void)
{
    ;
};


/*virtual*/ void C087SlashuurAI::Run(void) /*override*/
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


/*virtual*/ void C087SlashuurAI::Draw(void) /*override*/
{
    CAIModerator::Draw();
#ifdef _DEBUG
    if (!CEnemyDebug::SHOW_AI_INFO)
        return;

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecMyPos);

    float fHeight = EnemyCharacter().Compositor().GetCollisionParameter().m_fHeight;

    vecMyPos.y += (fHeight * 1.5f);

    enum
    {
        INFOFLAG_ORDER      = (1 << 0),
        INFOFLAG_PAT        = (1 << 1),
        INFOFLAG_TGT_NO     = (1 << 2),
        INFOFLAG_THINK_T    = (1 << 3),
        INFOFLAG_TP_COUNT   = (1 << 4),
        INFOFLAG_TP_TMOUT   = (1 << 5),
    };

    uint32 infoFlag = 0;

    char szInfoOrder[128];
    szInfoOrder[0] = '\0';

    char szPattern[64];
    szPattern[0] = '\0';

    char szTgtNo[64];
    szTgtNo[0] = '\0';

    char szThinkT[64];
    szThinkT[0] = '\0';

    char szTpTmout[64];
    szTpTmout[0] = '\0';

    char szTpCount[64];
    szTpCount[0] = '\0';

    /* get order info */
    static const char* s_apszOrderName[] =
    {
        "NOTHING",
        "WAIT",
        "MOVE",
        "RUN",
        "ATTACK",
        "DEFENCE",
    };

    static_assert(COUNT_OF(s_apszOrderName) == CAIThinkOrder::ORDER_MAX, "update table");

    CAIThinkOrder::ORDER eOrder = EnemyCharacter().AIThinkOrder().GetOrder();
    if (eOrder == CAIThinkOrder::ORDER_MOVE)
    {
        std::sprintf(szInfoOrder, "ORDER: %s (%" PRIi32 " -- %" PRIi32 ")",
                     s_apszOrderName[eOrder],
                     EnemyCharacter().AIThinkOrder().OrderMove().m_iMoveType,
                     EnemyCharacter().AIThinkOrder().OrderMove().m_iPlayerNo);
    }
    else if (eOrder == CAIThinkOrder::ORDER_ATTACK)
    {
        std::sprintf(szInfoOrder, "ORDER: %s (%" PRIi32 " -- %" PRIi32 ")",
                     s_apszOrderName[eOrder],
                     EnemyCharacter().AIThinkOrder().OrderAttack().m_iAttackType,
                     EnemyCharacter().AIThinkOrder().OrderAttack().m_iPlayerNo);
    }
    else
    {
        std::sprintf(szInfoOrder, "ORDER: %s", s_apszOrderName[eOrder]);
    };
    infoFlag |= INFOFLAG_ORDER;

    /* get pattern */
    std::sprintf(szPattern, "PATTERN: %" PRIi32, m_eAIPattern);
    infoFlag |= INFOFLAG_PAT;

    /* get think time */
    std::sprintf(szThinkT, "Think T: %.02f", m_fThinkTimer);
    if (m_fThinkTimer > 0.0f)
        infoFlag |= INFOFLAG_THINK_T;

    /* get teleport timeout */
    std::sprintf(szTpTmout, "TP timeout: %.02f", m_fTimeoutTeleport);
    if (m_fTimeoutTeleport > 0.0f)
        infoFlag |= INFOFLAG_TP_TMOUT;

    /* get teleport count */
    std::sprintf(szTpCount, "TP count: %" PRIi32, m_iTeleportReps);
    if (m_iTeleportReps > 0)
        infoFlag |= INFOFLAG_TP_COUNT;

    /* draw by info flag */
    RwRGBA color = { 0xFF, 0xFF, 0x7F, 0xFF };

    CDebugShape::m_fLabelHeight = 14.0f;
    CDebugShape::m_fLabelOffsetY = 0.0f;

    if (infoFlag & INFOFLAG_ORDER)
    {
        CDebugShape::ShowLabel(&vecMyPos, szInfoOrder, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_PAT)
    {
        CDebugShape::ShowLabel(&vecMyPos, szPattern, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    CDebugShape::m_fLabelHeight *= 0.9f;

    if (infoFlag & INFOFLAG_THINK_T)
    {
        CDebugShape::ShowLabel(&vecMyPos, szThinkT, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_TP_COUNT)
    {
        CDebugShape::ShowLabel(&vecMyPos, szTpCount, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_TP_TMOUT)
    {
        CDebugShape::ShowLabel(&vecMyPos, szTpTmout, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    CDebugShape::m_fLabelOffsetY = 0.0f;
#endif /* _DEBUG */
};


void C087SlashuurAI::GuardProc(void)
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


bool C087SlashuurAI::ThinkProc(void)
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

    uint8 thinkFreq = AIFreqParam().GetFrequency(C087Slashuur::FREQUENCY_THINKING);
    m_fThinkTimer = static_cast<float>(thinkFreq) / 60.0f;
    m_fThinkTimer *= (GetRandNormal() + 1.0f);
    m_fThinkTimer = std::fabs(m_fThinkTimer);

    return true;
};


void C087SlashuurAI::ReserveOrderProc(void)
{
    ENEMYTYPES::STATUS eMyStatusNow = EnemyCharacter().GetStatus();
    switch (static_cast<int32>(eMyStatusNow))
    {
    case ENEMYTYPES::STATUS_IDLE:
    case ENEMYTYPES::STATUS_WALK:
    case ENEMYTYPES::STATUS_RUN:
        ReserveOrderDefault();
        break;

    case ENEMYTYPES::STATUS_DASH:
        ReserveRunNearestPlayer();
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

    case ENEMYTYPES::STATUS_GUARD:
    case C087Slashuur::STATUS_TELEPORT_START:
    case C087Slashuur::STATUS_TELEPORT_ATTACK:
        ReserveOrderOnGuard();
        break;

    case C087Slashuur::STATUS_ATTACK_A:
        ReserveOrderOnAttackA();
        break;

    case C087Slashuur::STATUS_ATTACK_AA:
        ReserveOrderOnAttackAA();
        break;

    default:
        ReserveIdle();
        break;
    };

    if (m_fGuardTimer > GUARD_ESCAPE_TRIGGER_TIME)
    {
        m_fGuardTimer = 0.0f;
        ReserveEscapeAttack();
    };

    SetReservedOrder();

    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
};


void C087SlashuurAI::CheckPlayerStatus(void)
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
        m_aPlayerInfo[i].bIsTrouble = IsTroubleStatus(m_aPlayerData[i].eStatus);
    };
};


void C087SlashuurAI::CheckAIPattern(void)
{
    STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();
    if (stageId == STAGEID::ID_ST20FB)
        return;

    float fHpRatioNow = CEnemyUtils::GetEnemyHPRate(&EnemyCharacter());
    if (fHpRatioNow >= 0.7f)
    {
        /* 100% - 70% hp */
        m_eAIPattern = AIPATTERN_TELEPORT_ATTACK;
    }
    else if (fHpRatioNow >= 0.2f)
    {
        /* 70% - 20% hp */
        m_eAIPattern = AIPATTERN_TELEPORT_SCYTHE;
    }
    else
    {
        /* < 20% hp */
        m_eAIPattern = AIPATTERN_TELEPORT_DRAIN;
        
        if ((m_fHpRatioPrev >= 0.2f) && (fHpRatioNow < 0.2f))    
            m_bDrainAbility = true; /* only once per enter < 20% hp */
    };

    m_fHpRatioPrev = fHpRatioNow;
};


void C087SlashuurAI::CheckDisableOrder(void)
{
    if (m_fTimeoutTeleport > 0.0f)
        m_fTimeoutTeleport -= CGameProperty::GetElapsedTime();

    if (m_iTeleportReps >= TIMEOUT_TELEPORT_TRIGGER_COUNT)
    {
        m_iTeleportReps = 0;
        m_fTimeoutTeleport = TIMEOUT_TELEPORT;
    };

    if (m_fTimeoutShot > 0.0f)
        m_fTimeoutShot -= CGameProperty::GetElapsedTime();

    if (m_fTimeoutEscape > 0.0f)
        m_fTimeoutEscape -= CGameProperty::GetElapsedTime();

    ENEMYTYPES::STATUS eMyNowStatus = EnemyCharacter().GetStatus();
    switch (static_cast<int32>(eMyNowStatus))
    {
    case ENEMYTYPES::STATUS_DASH:
    case ENEMYTYPES::STATUS_JUMP_READY:
        {
            if (m_eMyPrevStatus != eMyNowStatus)
                m_fTimeoutEscape = TIMEOUT_ESCAPE;
        }
        break;

    case C087Slashuur::STATUS_TELEPORT_ATTACK:
    case C087Slashuur::STATUS_TELEPORT_SCYTHE:
    case C087Slashuur::STATUS_TELEPORT_DRAIN:
        {
            if (m_eMyPrevStatus != eMyNowStatus)
            {
                ++m_iTeleportReps;
                m_bDrainAbility = false;
            };
        }
        break;

    case C087Slashuur::STATUS_SHOT:
        {
            if (m_eMyPrevStatus != eMyNowStatus)
                m_fTimeoutShot = TIMEOUT_SHOT;
        }
        break;

    default:
        break;
    };

    m_eMyPrevStatus = eMyNowStatus;

    if (m_eMyPrevStatus == ENEMYTYPES::STATUS_RUN)
        m_fTimeoutRunAttack += CGameProperty::GetElapsedTime();
    else
        m_fTimeoutRunAttack = 0.0f;
};


void C087SlashuurAI::SetReservedOrder(void)
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


void C087SlashuurAI::ReserveOrderOnDamaged(void)
{
    if (CanTeleportation())
    {
        switch (m_eAIPattern)
        {
        case AIPATTERN_TELEPORT_ATTACK:
            ReserveTeleportationAttack();
            break;

        case AIPATTERN_TELEPORT_SCYTHE:
            ReserveTeleportationScythe();
            break;

        case AIPATTERN_TELEPORT_DRAIN:
            if (CanDrain())
                ReserveTeleportationDrain();
            else
                ReserveTeleportationAttack();            
            break;

        default:
            break;
        };

        return;
    };

    if (IsNearAttackPlayer())
    {
        ReserveGuard();
        return;
    };

    ReserveOrderDefault();
};


void C087SlashuurAI::ReserveOrderOnGuard(void)
{
    if (IsNearAttackCorePlayer())
        ReserveGuard();
    else
        ReserveOrderDefault();
};


void C087SlashuurAI::ReserveOrderOnAttackA(void)
{
    if (GetNearFrontPlayerNum())
        ReserveAttack(ORDERTYPE::ATTACK_AA);    
    else
        ReserveIdle();
};


void C087SlashuurAI::ReserveOrderOnAttackAA(void)
{
    if (GetNearNotFrontPlayerNum())
        ReserveAttack(ORDERTYPE::ATTACK_AAC);
    else if (GetNearFrontPlayerNum())
        ReserveAttack(ORDERTYPE::ATTACK_AAB);
    else
        ReserveIdle();
};


void C087SlashuurAI::ReserveOrderDefault(void)
{
    if (GetNearBackPlayerNum())
    {
        if (!IsTroublePlayerOnly())
            ReserveEscapeDash();
        else
            ReserveRunNearestPlayer();
        
        return;
    };

    if (IsNearAttackCorePlayer())
    {
        ReserveGuard();
        return;
    };

    if (GetNearSidePlayerNum())
    {
        IsNearSideTroublePlayerOnly(); // TODO ???
        ReserveNormalAttackB();
        return;
    };

    if (GetNearFrontPlayerNum())
    {
        if (IsNearFrontTroublePlayerOnly())
            ReserveNormalAttackB();
        else if (CanRunAttack())
            ReserveRunAttack();
        else
            ReserveNormalAttack();

        return;
    };

    if (GetMiddleFrontPlayerNum())
    {
        if (!IsMiddleFrontAttackPlayer())
            ReserveRunNearestPlayer();
        else
            ReserveGuard();

        return;
    };

    if (GetMiddleNotFrontPlayerNum())
    {
        ReserveRunNearestPlayer();
        return;
    };

    if (GetFarFrontPlayerNum() && CanShot())
    {
        ReserveShot();
        return;
    };

    if (IsNearFrontDownPlayerOnly() || IsMiddleFrontDownPlayerOnly())
        ReserveIdle();
    else
        ReserveWalkNearestPlayer();
};


void C087SlashuurAI::ReserveIdle(void)
{
    m_eOrder = CAIThinkOrder::ORDER_WAIT;
    m_iOrderType = ORDERTYPE::WAIT_IDLE;
};


void C087SlashuurAI::ReserveWalkNearestPlayer(void)
{
    m_eOrder = CAIThinkOrder::ORDER_MOVE;
    m_iOrderType = ORDERTYPE::MOVE_WALK_NO;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C087SlashuurAI::ReserveRunNearestPlayer(void)
{
    m_eOrder = CAIThinkOrder::ORDER_MOVE;
    m_iOrderType = ORDERTYPE::MOVE_RUN_NO;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C087SlashuurAI::ReserveEscapeDash(void)
{
    if (CanEscape())
    {
        m_eOrder = CAIThinkOrder::ORDER_MOVE;

        float fDir = GetEscapeDashDir();
        if (CanEscapeDashDir(fDir))
        {
            m_iOrderType = ORDERTYPE::MOVE_DASH;
            m_vecOrderPos.y = fDir;
        }
        else
        {
            m_iOrderType = ORDERTYPE::MOVE_JUMP;
            m_vecOrderPos = m_aPlayerData[GetNearestPlayerNo()].vecFootPos;
        };
    }
    else
    {
        ReserveRunNearestPlayer();
    };
};


void C087SlashuurAI::ReserveGuard(void)
{
    m_eOrder = CAIThinkOrder::ORDER_DEFENCE;
};


void C087SlashuurAI::ReserveAttack(int32 iAttackOrderType)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = iAttackOrderType;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C087SlashuurAI::ReserveNormalAttack(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderTargetNo = GetNearestPlayerNo();

    int32 numPlayerNear = GetNearPlayerNum();
    if (numPlayerNear < 2)
        m_iOrderType = ORDERTYPE::ATTACK_A;
    else
        m_iOrderType = ORDERTYPE::ATTACK_B;
};


void C087SlashuurAI::ReserveNormalAttackB(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_B;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C087SlashuurAI::ReserveRunAttack(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_RUN;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C087SlashuurAI::ReserveTeleportationAttack(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_TELEPORT;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


void C087SlashuurAI::ReserveTeleportationScythe(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_TELEPORT_SCYTHE;
};


void C087SlashuurAI::ReserveTeleportationDrain(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_TELEPORT_DRAIN;
};


void C087SlashuurAI::ReserveEscapeAttack(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_ESCAPE;
};


void C087SlashuurAI::ReserveShot(void)
{
    m_eOrder = CAIThinkOrder::ORDER_ATTACK;
    m_iOrderType = ORDERTYPE::ATTACK_SHOT;
    m_iOrderTargetNo = GetNearestPlayerNo();
};


bool C087SlashuurAI::CanDrain(void) const
{
    return m_bDrainAbility;
};


bool C087SlashuurAI::CanRunAttack(void) const
{
    return (m_fTimeoutRunAttack >= 0.5f);
};


bool C087SlashuurAI::CanEscape(void) const
{
    return (m_fTimeoutEscape <= 0.0f);
};


bool C087SlashuurAI::CanShot(void) const
{
    return (m_fTimeoutShot <= 0.0f);
};


bool C087SlashuurAI::CanTeleportation(void) const
{
    return (m_fTimeoutTeleport <= 0.0f);
};


float C087SlashuurAI::GetRandNormal(void)
{
    return m_rand.Generate();
};


bool C087SlashuurAI::IsAttackStatus(PLAYERTYPES::STATUS status) const
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
        || (status == PLAYERTYPES::STATUS_ATTACK_JUMP);
};


bool C087SlashuurAI::IsJumpStatus(PLAYERTYPES::STATUS status) const
{
    return (status == PLAYERTYPES::STATUS_JUMP_READY)
        || (status == PLAYERTYPES::STATUS_JUMP)
        || (status == PLAYERTYPES::STATUS_JUMP_2ND)
        || (status == PLAYERTYPES::STATUS_JUMP_WALL)
        || (status == PLAYERTYPES::STATUS_JUMP_WALL_FAILURE)
        || (status == PLAYERTYPES::STATUS_AERIAL)
        || (status == PLAYERTYPES::STATUS_AERIAL_MOVE);
};


bool C087SlashuurAI::IsDownStatus(PLAYERTYPES::STATUS status) const
{
    return (status == PLAYERTYPES::STATUS_TUMBLER_FRONT)
        || (status == PLAYERTYPES::STATUS_TUMBLER_BACK)
        || (status == PLAYERTYPES::STATUS_DOWN_FRONT)
        || (status == PLAYERTYPES::STATUS_DOWN_BACK);
};


bool C087SlashuurAI::IsGuardStatus(PLAYERTYPES::STATUS status) const
{
    return (status == PLAYERTYPES::STATUS_GUARD_READY)
        || (status == PLAYERTYPES::STATUS_GUARD_IMPACT)
        || (status == PLAYERTYPES::STATUS_GUARD_FINISH)
        || (status == PLAYERTYPES::STATUS_GUARD_BACK) 
        || (status == PLAYERTYPES::STATUS_GUARD);
};


bool C087SlashuurAI::IsTroubleStatus(PLAYERTYPES::STATUS status) const
{
    return (status == PLAYERTYPES::STATUS_DINDLE);
};


int32 C087SlashuurAI::GetNearestPlayerNo(void) const
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


int32 C087SlashuurAI::GetNearPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);

        if (bIsNearDist && !bIsDown)
            ++result;
    };

    return result;
};


int32 C087SlashuurAI::GetNearBackPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsInBack = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_BACK);

        if (bIsNearDist && bIsInBack && !bIsDown)
            ++result;
    };

    return result;
};


int32 C087SlashuurAI::GetNearSidePlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsOnTheSide = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_LEFT) ||
                            (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_RIGHT);

        if (bIsNearDist && bIsOnTheSide && !bIsDown)
            ++result;
    };

    return result;
};


int32 C087SlashuurAI::GetNearFrontPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsNearDist && bIsInFront && !bIsDown)
            ++result;
    };

    return result;
};


int32 C087SlashuurAI::GetNearNotFrontPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsNearDist && !bIsInFront && !bIsDown)
            ++result;
    };

    return result;
};


int32 C087SlashuurAI::GetMiddleFrontPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsMiddleDist = (m_aPlayerInfo[i].fDistance >  DISTANCE_NEAR) &&
                             (m_aPlayerInfo[i].fDistance <= DISTANCE_MIDDLE);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsMiddleDist && bIsInFront && !bIsDown)
            ++result;
    };

    return result;
};


int32 C087SlashuurAI::GetMiddleNotFrontPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsMiddleDist = (m_aPlayerInfo[i].fDistance >  DISTANCE_NEAR) &&
                             (m_aPlayerInfo[i].fDistance <= DISTANCE_MIDDLE);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsMiddleDist && !bIsInFront && !bIsDown)
            ++result;
    };

    return result;
};


int32 C087SlashuurAI::GetFarFrontPlayerNum(void) const
{
    int32 result = 0;

    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsFarDist = (m_aPlayerInfo[i].fDistance > DISTANCE_MIDDLE);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsFarDist && bIsInFront && !bIsDown)
            ++result;
    };

    return result;
};


bool C087SlashuurAI::IsNearAttackPlayer(void) const
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


bool C087SlashuurAI::IsNearAttackCorePlayer(void) const
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


bool C087SlashuurAI::IsMiddleFrontAttackPlayer(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsMiddleDist = (m_aPlayerInfo[i].fDistance > DISTANCE_NEAR) &&
                             (m_aPlayerInfo[i].fDistance <= DISTANCE_MIDDLE);
        bool bIsAttack = (m_aPlayerInfo[i].bIsAttack);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsMiddleDist && bIsInFront && bIsAttack)
            return true;
    };

    return false;
};


bool C087SlashuurAI::IsNearFrontDownPlayerOnly(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsDown = (m_aPlayerInfo[i].bIsDown);
        bool bIsInFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsNearDist && bIsInFront)
        {
            if (!bIsDown)
                return false;
        };
    };

    return true;
};


bool C087SlashuurAI::IsMiddleFrontDownPlayerOnly(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsDown = m_aPlayerInfo[i].bIsDown;
        bool bIsMiddleDist = (m_aPlayerInfo[i].fDistance > DISTANCE_NEAR) &&
                             (m_aPlayerInfo[i].fDistance <= DISTANCE_MIDDLE);
        bool bIsFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsMiddleDist && bIsFront)
        {
            if (!bIsDown)
                return false;
        };
    };

    return true;
};


bool C087SlashuurAI::IsNearFrontTroublePlayerOnly(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsTrouble = m_aPlayerInfo[i].bIsTrouble;
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsFront = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_FRONT);

        if (bIsNearDist && bIsFront)
        {
            if (!bIsTrouble)
                return false;
        };
    };

    return true;
};


bool C087SlashuurAI::IsNearSideTroublePlayerOnly(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        bool bIsTrouble = m_aPlayerInfo[i].bIsTrouble;
        bool bIsNearDist = (m_aPlayerInfo[i].fDistance <= DISTANCE_NEAR);
        bool bIsOnTheSide = (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_LEFT) ||
                            (m_aPlayerInfo[i].eSide == PLAYER_INFO::SIDE_RIGHT);

        if (bIsNearDist && bIsOnTheSide)
        {
            if (!bIsTrouble)
                return false;
        };
    };

    return true;
};


bool C087SlashuurAI::IsTroublePlayerOnly(void) const
{
    for (int32 i = 0; i < m_numPlayers; ++i)
    {
        if (!m_aPlayerInfo[i].bIsTrouble)
            return false;
    };

    return true;
};


bool C087SlashuurAI::CanEscapeDashDir(float fDir) const
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecMyFootPos);

    RwV3d vecPtStart = vecMyFootPos;
    vecPtStart.y += 1.0f;

    RwV3d vecPtEnd = { vecPtStart.x + (std::sin(fDir) * 3.0f),
                       vecPtStart.y,
                       vecPtStart.z + (std::cos(fDir) * 3.0f) };

    if (CWorldMap::CheckCollisionLine(&vecPtStart, &vecPtEnd))
        return false;

    return true;
};


float C087SlashuurAI::GetEscapeDashDir(void) const
{
    int32 numPlayers = CAIUtils::GetPlayerNum();    
    float afPlayerAngle[GAMETYPES::PLAYERS_MAX + 2] = { 0.0f };
    for (int32 i = 0; i < numPlayers; ++i)
        afPlayerAngle[i + 1] = m_aPlayerInfo[i].fDirection;
    
    float* itBegin  = std::begin(afPlayerAngle) + 1;
    float* itEnd    = std::begin(afPlayerAngle) + 1 + numPlayers;
    std::sort(itBegin, itEnd, [](float a, float b) { return (a < b); });

    afPlayerAngle[0] = -MATH_PI;
    afPlayerAngle[numPlayers + 1] = MATH_PI;

    float fAngleTerm = MATH_DEG2RAD(45.0f);
    float fMinDiff = 0.0f;
    float fEscapeAngle = 0.0f;

    for (int32 i = 0; i < numPlayers + 1; ++i)
    {
        if (afPlayerAngle[i] >= -fAngleTerm)
        {
            if (afPlayerAngle[i + 1] > fAngleTerm)
            {
                if (afPlayerAngle[i] < fAngleTerm)
                    break;

                if ((afPlayerAngle[i + 1] - fAngleTerm) > (fAngleTerm - afPlayerAngle[i]))
                    afPlayerAngle[i + 1] = (MATH_PI * 0.5f) - afPlayerAngle[i];
            };
        }
        else if (afPlayerAngle[i + 1] >= -fAngleTerm)
        {
            if ((-fAngleTerm - afPlayerAngle[i]) > (afPlayerAngle[i + 1] - -fAngleTerm))
                afPlayerAngle[i] = -afPlayerAngle[i + 1] - (MATH_PI * 0.5f);
        };
        
        float fAngle = afPlayerAngle[i];
        float fAngleNext = afPlayerAngle[i + 1];
        float fAngleDiff = (fAngleNext - fAngle);
        if (fMinDiff < fAngleDiff)
        {
            fMinDiff = fAngleDiff;
            fEscapeAngle = (fAngle + fAngleNext) / 2.0f;
        };
    };

    float fDirection = EnemyCharacter().Compositor().GetDirection();
    return (fDirection + fEscapeAngle);
};


bool C087SlashuurAI::CheckDebugOrder(void)
{
#ifdef _DEBUG
    /*
    if (IGamepad::CheckFunction(IGamepad::GetDigitalTrigger(0), IGamepad::FUNCTION_GUARD))
    {
        ReserveEscapeAttack();
        SetReservedOrder();
    }
    else
    {
        ReserveIdle();
        SetReservedOrder();
    };

    return true;
    */
    return false;
#else /* _DEBUG */
	return false;
#endif /* _DEBUG */
};