#include "BaseNinjaRatsAI.hpp"

#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseNinjaRatsChr.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"

#ifdef _DEBUG
#include "Game/Component/Enemy/EnemyDebug.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#endif /* _DEBUG */


#define START_THINK_TIMER               (1.5f)
#define MAP_HEIGHT                      (9.75f) // sewers map height


static const char* s_apszStratName[] =
{
    "WAIT",
    "BARRAGE",
    "PATTERN",
    "FOLLOW",
    "MAN_TO_MAN",
    "HAPPY",
};

static_assert(COUNT_OF(s_apszStratName) == CBaseNinjaRatsAI::STRATEGYNUM, "table incorrect");


/*static*/ CBaseNinjaRatsAI::STATE CBaseNinjaRatsAI::m_state;
/*static*/ int32 CBaseNinjaRatsAI::m_ratNum = 0;


/*static*/ CAIModerator* CBaseNinjaRatsAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new CBaseNinjaRatsAI(pEnemyChr);
};


CBaseNinjaRatsAI::CBaseNinjaRatsAI(CEnemyCharacter* pEnemyChr)
: CAIModerator(pEnemyChr)
, m_ratNo(-1)
, m_bJumpAttack(false)
, m_fRotRad(0.0f)
, m_fRotDir(0.0f)
, m_targetPlayerNo(GAMETYPES::PLAYERS_MAX)
{
    ASSERT(m_ratNum >= 0);
    if (m_ratNum++ == 0)
    {
        m_state.strategyExec = STRATEGY_WAIT;
        m_state.strategyThink = STRATEGY_WAIT;
        m_state.fWaitTime = START_THINK_TIMER;
        m_state.runCnt = 0;
    };

    switch (pEnemyChr->GetID())
    {
    case ENEMYID::ID_NINJA_RATS_A: m_ratNo = 3; break;
    case ENEMYID::ID_NINJA_RATS_B: m_ratNo = 0; break;
    case ENEMYID::ID_NINJA_RATS_C: m_ratNo = 1; break;
    case ENEMYID::ID_NINJA_RATS_D: m_ratNo = 2; break;
    default: ASSERT(false); break;
    };

    m_fRotRad = static_cast<float>(m_ratNo) * (MATH_PI / static_cast<float>(RAT_MAX));
    m_fRotDir = (m_ratNo % 2) ? 1.0f : -1.0f;
};


/*virtual*/ CBaseNinjaRatsAI::~CBaseNinjaRatsAI(void)
{
    ASSERT(m_ratNum > 0);
    --m_ratNum;
};


/*virtual*/ void CBaseNinjaRatsAI::Run(void) /*override*/
{
    if (++m_state.runCnt >= m_ratNum)
    {
        m_state.runCnt = 0;
        ThinkStrategy();

        switch (m_state.strategyExec)
        {
        case STRATEGY_WAIT:         ExecuteWaitStrategy();     break;
        case STRATEGY_BARRAGE:      ExecuteBarrageStrategy();  break;
        case STRATEGY_PATTERN:      ExecutePatternStrategy();  break;
        case STRATEGY_FOLLOW:       ExecuteFollowStrategy();   break;
        case STRATEGY_MAN_TO_MAN:   ExecuteManToManStrategy(); break;
        case STRATEGY_HAPPY:        ExecuteHappyStrategy();    break;
        default: ASSERT(false); break;
        };
    };

    switch (m_state.strategyExec)
    {
    case STRATEGY_WAIT:         ExecuteWaitTactics();     break;
    case STRATEGY_BARRAGE:      ExecuteBarrageTactics();  break;
    case STRATEGY_PATTERN:      ExecutePatternTactics();  break;
    case STRATEGY_FOLLOW:       ExecuteFollowTactics();   break;
    case STRATEGY_MAN_TO_MAN:   ExecuteManToManTactics(); break;
    case STRATEGY_HAPPY:        ExecuteHappyTactics();    break;
    default: ASSERT(false); break;
    };

    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
};


/*virtual*/ void CBaseNinjaRatsAI::Draw(void) /*override*/
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
        INFOFLAG_STRAT      = (1 << 1),
        INFOFLAG_WAIT_T     = (1 << 2),
    };

    uint32 infoFlag = 0;

    char szInfoOrder[128];
    szInfoOrder[0] = '\0';

    char szStrat[64];
    szStrat[0] = '\0';

    char szWaitT[64];
    szWaitT[0] = '\0';

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
    std::sprintf(szInfoOrder, "ORDER: %s", s_apszOrderName[eOrder]);
    infoFlag |= INFOFLAG_ORDER;

    /* get strategy */
    std::sprintf(szStrat, "STRAT: %s", s_apszStratName[m_state.strategyExec]);
    infoFlag |= INFOFLAG_STRAT;

    /* get wait time */
    std::sprintf(szWaitT, "Wait T: %.02f", m_state.fWaitTime);
    if (m_state.fWaitTime > 0.0f)
        infoFlag |= INFOFLAG_WAIT_T;

    /* draw by info flag */
    RwRGBA color = { 0xFF, 0xFF, 0x7F, 0xFF };

    CDebugShape::m_fLabelHeight = 14.0f;
    CDebugShape::m_fLabelOffsetY = 0.0f;

    if (infoFlag & INFOFLAG_ORDER)
    {
        CDebugShape::ShowLabel(&vecMyPos, szInfoOrder, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_STRAT)
    {
        CDebugShape::ShowLabel(&vecMyPos, szStrat, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    CDebugShape::m_fLabelHeight *= 0.9f;

    if (infoFlag & INFOFLAG_WAIT_T)
    {
        CDebugShape::ShowLabel(&vecMyPos, szWaitT, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    CDebugShape::m_fLabelOffsetY = 0.0f;
#endif /* _DEBUG */
};


void CBaseNinjaRatsAI::ThinkStrategy(void)
{
    int32 stunnedPlayerNo = GetFirstStunnedPlr();
    if (stunnedPlayerNo >= 0)
    {
        m_state.targetPlayerNo = stunnedPlayerNo;
        ChangeStrategy(STRATEGY_BARRAGE);
    };

    if (m_state.fWaitTime > 0.0f)
    {
        m_state.fWaitTime -= CGameProperty::GetElapsedTime();
        if (m_state.fWaitTime < 0.0f)
        {
            switch (m_state.strategyExec)
            {
            case STRATEGY_HAPPY:
            case STRATEGY_MAN_TO_MAN:
            case STRATEGY_WAIT:
                m_state.strategyThink = STRATEGY_FOLLOW;
                break;

            case STRATEGY_BARRAGE:
                m_state.strategyThink = STRATEGY_HAPPY;
                break;

            default:
                break;
            };
        };
    };

    if (m_state.strategyExec != m_state.strategyThink)
    {
        if (m_state.strategyExec == STRATEGY_WAIT)
        {
            if (CGameData::PlayParam().GetStage() == STAGEID::ID_ST18FB)
                CMessageManager::Request(SEGROUPID::VALUE(64));
        };

        m_state.strategyExec = m_state.strategyThink;
        switch (m_state.strategyExec)
        {
        case STRATEGY_BARRAGE:
            m_state.fWaitTime = 3.5f;
            InitBarrgeStrategy();
            break;

        case STRATEGY_PATTERN:
            break;

        case STRATEGY_FOLLOW:
            InitFollowStrategy();
            break;

        case STRATEGY_MAN_TO_MAN:
            m_state.fWaitTime = 20.0f;
            InitManToManStrategy();
            break;

        case STRATEGY_HAPPY:
            m_state.fWaitTime = 8.0f;
            InitHappyStrategy();
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CBaseNinjaRatsAI::ChangeStrategy(STRATEGY strategy)
{
#ifdef _DEBUG
    if (m_state.strategyThink != strategy)
        OUTPUT("CHANGING STRAT TO: %s\n", s_apszStratName[strategy]);
#endif /* _DEBUG */    
    
    m_state.strategyThink = strategy;
};


void CBaseNinjaRatsAI::CheckNormalAttack(void)
{
    if (m_targetPlayerNo == GAMETYPES::PLAYERS_MAX)
    {
        int32 nearestPlrNo = -1;
        float fNearestPlrDist = 0.0f;
        GetNearestFrontPlrDist(&nearestPlrNo, &fNearestPlrDist);
        
        if (nearestPlrNo < 0)
            return;

        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(nearestPlrNo);
        
        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        float fDir = CEnemyUtils::GetDirection(&EnemyCharacter().Compositor(), &vecFootPosPlr);
        float fMyDir = EnemyCharacter().Compositor().GetDirection();
        float fDirDiff = (fDir - fMyDir);
        fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
        fDirDiff = std::fabs(fDirDiff);

        bool bAttack = false;
		int32 attackType = -1;
        if (fDirDiff >= MATH_DEG2RAD(45.0f))
        {
            if (fNearestPlrDist < 2.0f)
            {
                attackType = ORDERTYPE::ATTACK_SINGLE;
                bAttack = true;
            };
        }
        else if (fNearestPlrDist < 3.0f)
        {
            attackType = ORDERTYPE::ATTACK_STUN;
            bAttack = true;
        };

        if (bAttack)
        {
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
			ThinkOrder().OrderAttack().m_iAttackType = attackType;
            ThinkOrder().OrderAttack().m_iPlayerNo = nearestPlrNo;
            m_targetPlayerNo = nearestPlrNo;
        }
        else
        {
            m_state.avecTargetPos[m_ratNo] = vecFootPosPlr;
        };
    }
    else
    {
        RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
        EnemyCharacter().Compositor().GetFootPosition(&vecFootPosMe);

        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_targetPlayerNo);
        ASSERT(pPlrChr != nullptr);

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        RwV3d vecDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDist, &vecFootPosMe, &vecFootPosPlr);

        float fDist = Math::Vec3_Length(&vecDist);
        if (fDist >= 3.0f)
            m_targetPlayerNo = GAMETYPES::PLAYERS_MAX;
    };
};


void CBaseNinjaRatsAI::ExecuteWaitStrategy(void)
{
    ;
};


void CBaseNinjaRatsAI::ExecuteWaitTactics(void)
{
    ThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
    ThinkOrder().OrderWait().m_iWaitType = ORDERTYPE::WAIT_IDLE;
};


void CBaseNinjaRatsAI::InitBarrgeStrategy(void)
{
    CMessageManager::Request(SEGROUPID::VALUE(66));

    for (int32 i = 0; i < RAT_MAX; ++i)
    {
        m_state.abSdPlayAttack[i] = false;
        m_state.abSdPlayHappy[i] = false;
    };
};


void CBaseNinjaRatsAI::ExecuteBarrageStrategy(void)
{
    ;
};


void CBaseNinjaRatsAI::ExecuteBarrageTactics(void)
{
    ENEMYTYPES::STATUS statusNow = EnemyCharacter().GetStatus();
    switch (statusNow)
    {
    case ENEMYTYPES::STATUS_QUIT:
    case ENEMYTYPES::STATUS_DEATH:
    case ENEMYTYPES::STATUS_THINKING:
        break;

    case ENEMYTYPES::STATUS_RUN:
        {
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
            ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_RUN_NO;
            ThinkOrder().OrderMove().m_iPlayerNo = m_state.targetPlayerNo;

            CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_state.targetPlayerNo);
            ASSERT(pPlrChr != nullptr);

            RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
            pPlrChr->GetFootPosition(&vecFootPosPlr);

            float fDist = CEnemyUtils::GetDistance(&EnemyCharacter().Compositor(), &vecFootPosPlr);
            if (fDist >= 4.0f)
                break;    

            float fDir = CEnemyUtils::GetDirection(&EnemyCharacter().Compositor(), &vecFootPosPlr);
            float fMyDir = EnemyCharacter().Compositor().GetDirection();
            float fDirDiff = (fDir - fMyDir);
            fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
            fDirDiff = std::fabs(fDirDiff);

            if ((fDirDiff < MATH_DEG2RAD(90.0f)) && (fDist < 3.0f))
            {
                switch (pPlrChr->GetStatus())
                {
                case PLAYERTYPES::STATUS_FLYAWAY_FRONT:
                case PLAYERTYPES::STATUS_FLYAWAY_BACK:
                case PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT:
                case PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK:
                case PLAYERTYPES::STATUS_STUN:
                case PLAYERTYPES::STATUS_DOWN_FRONT:
                case PLAYERTYPES::STATUS_DOWN_BACK:
                    {
                        ThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
                        ThinkOrder().OrderWait().m_iWaitType = ORDERTYPE::WAIT_TURN_NO;
                        ThinkOrder().OrderWait().m_iPlayerNo = m_state.targetPlayerNo;
                        ThinkOrder().OrderWait().m_fWaitTimer = 0.0f;

                        if (!m_state.abSdPlayHappy[m_ratNo])
                        {
                            m_state.abSdPlayHappy[m_ratNo] = true;
                            PlayHappySE();
                        };
                    }
                    break;

                default:
                    {
                        ThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
                        ThinkOrder().OrderAttack().m_iAttackType = ORDERTYPE::ATTACK_CHAIN;
                        ThinkOrder().OrderAttack().m_iPlayerNo = m_state.targetPlayerNo;

                        m_targetPlayerNo = m_state.targetPlayerNo;

                        if (!m_state.abSdPlayAttack[m_ratNo])
                        {
                            m_state.abSdPlayAttack[m_ratNo] = true;
                            PlayAttackSE();
                        };
                    }
                    break;
                };
            };
        }
        break;

    case CBaseNinjaRatsChr::STATUS_ATTACK_CHAIN:
        {
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
            ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_DASH;
            ThinkOrder().OrderMove().m_iPlayerNo = m_state.targetPlayerNo;
            ThinkOrder().OrderMove().m_fMoveTimer = 0.0f;
        }
        break;

    default:
        {
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
            ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_RUN_NO;
            ThinkOrder().OrderMove().m_iPlayerNo = m_state.targetPlayerNo;
            ThinkOrder().OrderMove().m_fMoveTimer = 0.0f;
        }
        break;
    };
};


void CBaseNinjaRatsAI::ExecutePatternStrategy(void)
{
    float dt = CGameProperty::GetElapsedTime();

    m_state.fDegForRotRad += ((MATH_PI2 / 10.0f) * dt);

    if (m_state.fDegForRotRad >= MATH_PI2)
        ChangeStrategy(STRATEGY_MAN_TO_MAN);
    
    m_state.fDegForRotRad = Math::Fmod(m_state.fDegForRotRad, MATH_PI2);

    float c = (6.5f - (std::cos(m_state.fDegForRotRad) * 4.5f));
    m_state.fRotRad += (((m_state.fRotDir * 9.0f) / c) * dt);
    m_state.fRotRad = Math::Fmod(m_state.fRotRad, MATH_PI2);

    float x = std::sin(m_state.fRotRad) * c;
    float y = MAP_HEIGHT;
    float z = std::cos(m_state.fRotRad) * c;

    m_state.avecTargetPos[0] = {  x, y,  z };
    m_state.avecTargetPos[1] = { -z, y,  x };
    m_state.avecTargetPos[2] = { -x, y, -z };
    m_state.avecTargetPos[3] = {  z, y, -x };
};


void CBaseNinjaRatsAI::ExecutePatternTactics(void)
{
    ExecuteFollowTactics();
};


void CBaseNinjaRatsAI::InitFollowStrategy(void)
{
    m_state.fRotDir = (m_state.fRotDir >= 0.0f) ? -1.0f : 1.0f;
    m_state.fDegForRotRad = 0.0f;
};


void CBaseNinjaRatsAI::ExecuteFollowStrategy(void)
{
    float dt = CGameProperty::GetElapsedTime();

    m_state.fDegForRotRad += ((MATH_PI2 / 6.0f) * dt);

    if (m_state.fDegForRotRad >= (MATH_PI2 * 2.0f))
        ChangeStrategy(STRATEGY_PATTERN);
    
    m_state.fDegForRotRad = Math::Fmod(m_state.fDegForRotRad, MATH_PI2 * 2.0f);

    float c = (6.5f - (std::cos(m_state.fDegForRotRad) * 4.5f));
    m_state.fRotRad += (((m_state.fRotDir * 9.0f) / c) * dt);
    m_state.fRotRad = Math::Fmod(m_state.fRotRad, MATH_PI2);

    float s = m_state.fDegForRotRad;
    float d = m_state.fRotRad;

    for (int32 i = 0; i < RAT_MAX; ++i)
    {
        m_state.avecTargetPos[i].x = std::sin(d) * c;
        m_state.avecTargetPos[i].y = MAP_HEIGHT;
        m_state.avecTargetPos[i].z = std::cos(d) * c;

        s -= MATH_DEG2RAD(9.0f);
        c = (6.5f - (std::cos(s) * 4.5f));

        d -= (((m_state.fRotDir * 9.0f) / c) * 0.15f);
    };
};


void CBaseNinjaRatsAI::ExecuteFollowTactics(void)
{
    ENEMYTYPES::STATUS statusNow = EnemyCharacter().GetStatus();
    switch (statusNow)
    {
    case ENEMYTYPES::STATUS_QUIT:
    case ENEMYTYPES::STATUS_DEATH:
    case ENEMYTYPES::STATUS_THINKING:
        break;

    case ENEMYTYPES::STATUS_AERIAL:
        {
            if (m_bJumpAttack)
            {
                RwV3d vecVel = Math::VECTOR3_ZERO;
                EnemyCharacter().Compositor().GetVelocity(&vecVel);

                if (vecVel.y < 5.0f)
                {
                    m_bJumpAttack = false;
                    ThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
                    ThinkOrder().OrderAttack().m_iAttackType = ORDERTYPE::ATTACK_JUMP;
                };
            };
        }
        break;

    case ENEMYTYPES::STATUS_RUN:
        {
            int32 farthestPlrNo = -1;
            float fFarthestPlrDist = 0.0f;
            GetFarthestFrontPlrDist(&farthestPlrNo, &fFarthestPlrDist);

            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
            if (farthestPlrNo >= 0)
            {
                if (fFarthestPlrDist >= 7.29f)
                    ThinkOrder().OrderMove().m_iPlayerNo = farthestPlrNo;
                else
                    ThinkOrder().OrderMove().m_iPlayerNo = -1;
                ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_JUMP;
            };
            ThinkOrder().OrderMove().m_vAt = m_state.avecTargetPos[m_ratNo];

            CheckNormalAttack();
        }
        break;

    case ENEMYTYPES::STATUS_GETUP:
        {
			m_bJumpAttack = true;
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
            ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_JUMP;
            ThinkOrder().OrderMove().m_iPlayerNo = -1;
            ThinkOrder().OrderMove().m_vAt = m_state.avecTargetPos[m_ratNo];
        }
        break;

    default:
        {
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
            ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_RUN_POS;
            ThinkOrder().OrderMove().m_fMoveTimer = 0.0f;
        }
        break;
    };

    if (ThinkOrder().GetOrder() == CAIThinkOrder::ORDER_MOVE)
        ThinkOrder().OrderMove().m_vAt = m_state.avecTargetPos[m_ratNo];
};


void CBaseNinjaRatsAI::InitManToManStrategy(void)
{
    ;
};


void CBaseNinjaRatsAI::ExecuteManToManStrategy(void)
{
    RwV3d avecFootPosPlr[GAMETYPES::PLAYERS_MAX] = { Math::VECTOR3_ZERO };

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        pPlrChr->GetFootPosition(&avecFootPosPlr[i]);
    };

    switch (numPlayers)
    {
    case 1:
        {
            RwV3d vecFootPosPlr = avecFootPosPlr[0];
            vecFootPosPlr.y = 0.0f;

            float fDir = 0.0f;
            if (!Math::Vec3_Equal(&vecFootPosPlr, &Math::VECTOR3_ZERO))
                fDir = std::atan2(vecFootPosPlr.x, vecFootPosPlr.z);

            float fDistSQ = Math::Vec3_Dot(&vecFootPosPlr, &vecFootPosPlr);
            float fDist = std::sqrt(fDistSQ);

            for (int32 i = 1; i < 4; ++i)
            {
                fDir += (MATH_PI * 0.5f);
                avecFootPosPlr[i].x = std::sin(fDir) * (12.0f - fDist);
                avecFootPosPlr[i].y = MAP_HEIGHT;
                avecFootPosPlr[i].z = std::cos(fDir) * (12.0f - fDist);
            };
        }
        break;

    case 2:
        {
            for (int32 i = 0; i < 2; ++i)
            {
                RwV3d vecFootPosPlr = avecFootPosPlr[i];
                vecFootPosPlr.y = 0.0f;

                float fDir = 0.0f;
                if (!Math::Vec3_Equal(&vecFootPosPlr, &Math::VECTOR3_ZERO))
                    fDir = std::atan2(vecFootPosPlr.x, vecFootPosPlr.z);
                
                float fDistSQ = Math::Vec3_Dot(&vecFootPosPlr, &vecFootPosPlr);
                float fDist = std::sqrt(fDistSQ);

                for (int32 j = 2; j < 4; ++j)
                {
                    avecFootPosPlr[j].x = std::sin(fDir + MATH_PI) * (12.0f - fDist);
                    avecFootPosPlr[j].y = MAP_HEIGHT;
                    avecFootPosPlr[j].z = std::cos(fDir + MATH_PI) * (12.0f - fDist);
                };
            };
        }
        break;

    case 3:
        {
            float afDir[3] = { 0.0f };
            for (int32 i = 0; i < COUNT_OF(afDir); ++i)
            {
                RwV3d vecFootPosPlr = avecFootPosPlr[i];
                vecFootPosPlr.y = 0.0f;

                afDir[i] = 0.0f;
                if (!Math::Vec3_Equal(&vecFootPosPlr, &Math::VECTOR3_ZERO))
                    afDir[i] = std::atan2(vecFootPosPlr.x, vecFootPosPlr.z);
            };

            float fMaxWidth = 0.0f;
            int32 targetIndex = 0;

            for (int32 i = 0; i < COUNT_OF(afDir); ++i)
            {
                float fDir = afDir[i];
                float fDirNext = afDir[(i + 1) % COUNT_OF(afDir)];
                float fWidth = (fDir - fDirNext);

                fWidth = std::fabs(fWidth);

                if (fWidth > MATH_PI)
                    fWidth = (MATH_PI2 - fWidth);

                if (fMaxWidth < fWidth)
                {
                    fMaxWidth = fWidth;
                    targetIndex = (i + 2) % COUNT_OF(afDir);
                };
            };

            RwV3d vecFootPosPlr = avecFootPosPlr[targetIndex];
            vecFootPosPlr.y = 0.0f;

            float fDir = 0.0f;
            if (!Math::Vec3_Equal(&vecFootPosPlr, &Math::VECTOR3_ZERO))
                fDir = std::atan2(vecFootPosPlr.x, vecFootPosPlr.z);

            float fDistSQ = Math::Vec3_Dot(&vecFootPosPlr, &vecFootPosPlr);
            float fDist = std::sqrt(fDistSQ);

            avecFootPosPlr[3].x = std::sin(afDir[targetIndex] + MATH_PI) * (12.0f - fDist);
            avecFootPosPlr[3].y = MAP_HEIGHT;
            avecFootPosPlr[3].z = std::cos(afDir[targetIndex] + MATH_PI) * (12.0f - fDist);
        }
        break;

    default:
        break;
    };

    int32 aRatIndex[RAT_MAX] = { -1 };
    bool abUsedIndex[RAT_MAX] = { false };
    int32 freeRatStartIndex = 0;
    for (int32 i = 0; i < numPlayers; ++i)
    {
        aRatIndex[i] = GetTargetRatNo(i);
        abUsedIndex[i] = true;
        ++freeRatStartIndex;
    };

    for (int32 i = freeRatStartIndex; i < COUNT_OF(aRatIndex); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(abUsedIndex); ++j)
        {
            if (!abUsedIndex[j])
            {
                aRatIndex[i] = j;
                abUsedIndex[j] = true;
                break;
            };
        };
    };

    m_state.avecTargetPos[aRatIndex[0]] = avecFootPosPlr[0];
    m_state.avecTargetPos[aRatIndex[1]] = avecFootPosPlr[1];
    m_state.avecTargetPos[aRatIndex[2]] = avecFootPosPlr[2];
    m_state.avecTargetPos[aRatIndex[3]] = avecFootPosPlr[3];
};


void CBaseNinjaRatsAI::ExecuteManToManTactics(void)
{
    ENEMYTYPES::STATUS statusNow = EnemyCharacter().GetStatus();
    switch (statusNow)
    {
    case ENEMYTYPES::STATUS_QUIT:
    case ENEMYTYPES::STATUS_DEATH:
    case ENEMYTYPES::STATUS_THINKING:
        break;

    case ENEMYTYPES::STATUS_AERIAL:
        {
            if (m_bJumpAttack)
            {
                RwV3d vecVel = Math::VECTOR3_ZERO;
                EnemyCharacter().Compositor().GetVelocity(&vecVel);

                if (vecVel.y < 5.0f)
                {
                    m_bJumpAttack = false;
                    ThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
                    ThinkOrder().OrderAttack().m_iAttackType = ORDERTYPE::ATTACK_JUMP;
                };
            };
        }
        break;

    case ENEMYTYPES::STATUS_RUN:
        {
            float fRotRad = m_fRotRad;
            RwV3d vecTargetPos = Math::VECTOR3_ZERO;
			float y = 0.0f;

            for (int32 i = 0; i < 2; ++i)
            {
                fRotRad = m_fRotRad;
                fRotRad += CGameProperty::GetElapsedTime() * 3.0f * m_fRotDir;
                fRotRad = Math::Fmod(fRotRad, MATH_PI2);

                vecTargetPos = m_state.avecTargetPos[m_ratNo];
                vecTargetPos.x += (std::sin(fRotRad) * 3.0f);
                vecTargetPos.z += (std::cos(fRotRad) * 3.0f);
                
				y = vecTargetPos.y;
				vecTargetPos.y = 0.0f;

                float fDistSQ = Math::Vec3_Dot(&vecTargetPos, &vecTargetPos);
                float fDist = std::sqrt(fDistSQ);
                if (fDist <= 12.0f)
                    break;
                
                m_fRotDir *= -1.0f;
            };

            m_fRotRad = fRotRad;

			vecTargetPos.y = y;
            m_state.avecTargetPos[m_ratNo] = vecTargetPos;

            CheckNormalAttack();
        }
        break;

    case ENEMYTYPES::STATUS_GETUP:
        {
			m_bJumpAttack = true;
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
            ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_JUMP;
            ThinkOrder().OrderMove().m_iPlayerNo = -1;
            ThinkOrder().OrderMove().m_vAt = m_state.avecTargetPos[m_ratNo];
        }
        break;

    default:
        {
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
            ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_RUN_POS;
            ThinkOrder().OrderMove().m_fMoveTimer = 0.0f;
        }
        break;
    };

    if (ThinkOrder().GetOrder() == CAIThinkOrder::ORDER_MOVE)
        ThinkOrder().OrderMove().m_vAt = m_state.avecTargetPos[m_ratNo];
};


void CBaseNinjaRatsAI::InitHappyStrategy(void)
{
    ;
};


void CBaseNinjaRatsAI::ExecuteHappyStrategy(void)
{
    m_state.fRotRad += (((m_state.fRotDir * 3.0f) / 2.0f) * CGameProperty::GetElapsedTime());
    m_state.fRotRad = Math::Fmod(m_state.fRotRad, MATH_PI2);

    float x = std::sin(m_state.fRotRad) * 2.0f;
    float y = MAP_HEIGHT;
    float z = std::cos(m_state.fRotRad) * 2.0f;

    m_state.avecTargetPos[0] = {  x, y,  z };
    m_state.avecTargetPos[1] = { -z, y,  x };
    m_state.avecTargetPos[2] = { -x, y, -z };
    m_state.avecTargetPos[3] = {  z, y, -x };
};


void CBaseNinjaRatsAI::ExecuteHappyTactics(void)
{
    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecFootPosMe);
    vecFootPosMe.y = 0.0f;

    float fDistSQ = Math::Vec3_Dot(&vecFootPosMe, &vecFootPosMe);
    if (fDistSQ >= 9.0f)
    {
        ThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
        ThinkOrder().OrderMove().m_iMoveType = ORDERTYPE::MOVE_RUN_POS;
        ThinkOrder().OrderMove().m_fMoveTimer = 0.0f;
        ThinkOrder().OrderMove().m_vAt = m_state.avecTargetPos[m_ratNo];
    }
    else
    {
        ThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
        ThinkOrder().OrderWait().m_iWaitType = ORDERTYPE::WAIT_TURN_POS;
        ThinkOrder().OrderWait().m_fWaitTimer = 0.0f;
        ThinkOrder().OrderWait().m_vAt = m_state.avecTargetPos[(m_ratNo + 3) % RAT_MAX];
    };    

    ENEMYTYPES::STATUS statusNow = EnemyCharacter().Compositor().GetStatus();
    if ((statusNow >= ENEMYTYPES::STATUS_KNOCK_FRONT) &&
        (statusNow <= ENEMYTYPES::STATUS_FLYAWAY_BACK))
        ChangeStrategy(STRATEGY_FOLLOW);
};


int32 CBaseNinjaRatsAI::GetTargetRatNo(int32 playerNo) const
{
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(playerNo);
    ASSERT(pPlrChr != nullptr);

    static int32 s_aPlayerIdRatNo[] =
    {
        1, // leo
        0, // rap
        2, // mic
        3, // don
        0, // sla
        2, // cas
        1, // kar
        3, // spl
    };

    static_assert(COUNT_OF(s_aPlayerIdRatNo) == PLAYERID::ID_MAX, "update me");

    PLAYERID::VALUE playerId = pPlrChr->GetID();
    ASSERT(playerId >= 0);
    ASSERT(playerId < COUNT_OF(s_aPlayerIdRatNo));

    return s_aPlayerIdRatNo[playerId];
};


void CBaseNinjaRatsAI::GetNearestFrontPlrDist(int32* nearestPlayerNo,
                                              float* pfNearestPlayerDist) const
{
    *nearestPlayerNo = -1;
    *pfNearestPlayerDist = 4.0f;

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        if (!pPlrChr)
            continue;

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        float fDir = CEnemyUtils::GetDirection(&EnemyCharacter().Compositor(), &vecFootPosPlr);
        float fMyDir = EnemyCharacter().Compositor().GetDirection();
        float fDirDiff = (fDir - fMyDir);
        fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
        fDirDiff = std::fabs(fDirDiff);

        uint8 viewAngleFreq = EnemyCharacter().FrequencyParameter(CBaseNinjaRatsChr::FREQUENCY_VIEW_ANGLE);
        float fViewAngle = MATH_DEG2RAD(static_cast<float>(viewAngleFreq));

        if (fViewAngle > fDirDiff)
        {
            float fDist = CEnemyUtils::GetDistance(&EnemyCharacter().Compositor(), &vecFootPosPlr);
            if (fDist < *pfNearestPlayerDist)
            {
                *nearestPlayerNo = i;
                *pfNearestPlayerDist = fDist;
            };            
        };
    };
};


void CBaseNinjaRatsAI::GetFarthestFrontPlrDist(int32* farthestPlayerNo,
                                               float* pfFarthestPlayerDist) const
{
    *farthestPlayerNo = -1;
    *pfFarthestPlayerDist = -1.0f;

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        if (!pPlrChr)
            continue;

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        float fDir = CEnemyUtils::GetDirection(&EnemyCharacter().Compositor(), &vecFootPosPlr);
        float fMyDir = EnemyCharacter().Compositor().GetDirection();
        float fDirDiff = (fDir - fMyDir);
        fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
        fDirDiff = std::fabs(fDirDiff);

        if (fDirDiff < MATH_DEG2RAD(36.0f))
        {
            float fDist = CEnemyUtils::GetDistance(&EnemyCharacter().Compositor(), &vecFootPosPlr);
            if ((*pfFarthestPlayerDist < fDist) && (fDist < 8.1f))
            {
                *farthestPlayerNo = i;
                *pfFarthestPlayerDist = fDist;
            };            
        };
    };
};


int32 CBaseNinjaRatsAI::GetFirstStunnedPlr(void) const
{
    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
        if (!pPlrChr)
            continue;

        if (pPlrChr->GetStatus() == PLAYERTYPES::STATUS_STUN)
            return i;        
    };

    return -1;
};


void CBaseNinjaRatsAI::PlayHappySE(void)
{
    int32 nSE = 0;
    switch (m_ratNo)
    {
    case 0: nSE = SDCODE_SE(8427); break;
    case 1: nSE = SDCODE_SE(8430); break;
    case 2: nSE = SDCODE_SE(8433); break;
    case 3: nSE = SDCODE_SE(8436); break;
    default: break;
    };

    CGameSound::PlayObjectSE(&EnemyCharacter().Compositor(), nSE);
};


void CBaseNinjaRatsAI::PlayAttackSE(void)
{
    int32 nSE = 0;
    switch (m_ratNo)
    {
    case 0: nSE = SDCODE_SE(8428); break;
    case 1: nSE = SDCODE_SE(8431); break;
    case 2: nSE = SDCODE_SE(8434); break;
    case 3: nSE = SDCODE_SE(8437); break;
    default: break;
    };

    CGameSound::PlayObjectSE(&EnemyCharacter().Compositor(), nSE);
};