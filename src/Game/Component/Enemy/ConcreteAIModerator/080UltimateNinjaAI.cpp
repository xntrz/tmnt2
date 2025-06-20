#include "080UltimateNinjaAI.hpp"

#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Sound/MessageManager.hpp"

#ifdef _DEBUG
#include "Game/System/Misc/DebugShape.hpp"
#endif /* _DEBUG */


/* park escape points */
static const RwV3d s_avecEscapePointsM43N[] = { { -27.0f, 0.0f,    0.0f },
                                                { -36.0f, 0.0f,    0.0f },
                                                { -36.0f, 0.0f, -100.0f }, };

/* bridge escape points */
static const RwV3d s_avecEscapePointsM44N[] = { { 0.0f, 0.0f, 0.0f }, };

static const float s_fAttackNearDistanceSQ = 2.25f;
static const float s_fAttackMiddleDistanceSQ = 64.0f;


template<class TState, class TRecords, size_t iRecordNum>
inline static Utils6034::CProbabilityTable::CListData
SetListData(TState state, TRecords(&aRecords)[iRecordNum])
{
    Utils6034::CProbabilityTable::CListData listData;

    listData.iListIndex = C080UltimateNinjaAI::CActionTable::GetStateHandle(state);
    listData.aRecords = aRecords;
    listData.iRecordsNum = iRecordNum;

    return listData;
};


//
// *********************************************************************************
//


/*static*/ int32
C080UltimateNinjaAI::CActionTable::GetListNum(void)
{
    return (DISTANCE_STATE_MAX + SPECIAL_STATE_MAX + DEFENCE_STATE_MAX + MOVE_STATE_MAX);
};


/*static*/ int32
C080UltimateNinjaAI::CActionTable::GetStateHandle(DISTANCE_STATE state)
{
    if (state >= DISTANCE_STATE_MAX)
        state = DISTANCE_STATE_MIN;

    return (state);
};


/*static*/ int32
C080UltimateNinjaAI::CActionTable::GetStateHandle(SPECIAL_STATE state)
{
    if (state >= SPECIAL_STATE_MAX)
        state = SPECIAL_STATE_MIN;

    return (DISTANCE_STATE_MAX + state);
};


/*static*/ int32
C080UltimateNinjaAI::CActionTable::GetStateHandle(DEFENCE_STATE state)
{
    if (state >= DEFENCE_STATE_MAX)
        state = DEFENCE_STATE_MIN;

    return (DISTANCE_STATE_MAX + SPECIAL_STATE_MAX + state);
};


/*static*/ int32
C080UltimateNinjaAI::CActionTable::GetStateHandle(MOVE_STATE state)
{
    if (state >= MOVE_STATE_MAX)
        state = MOVE_STATE_MIN;

    return (DISTANCE_STATE_MAX + SPECIAL_STATE_MAX + DEFENCE_STATE_MAX + state);
};


//
// *********************************************************************************
//


/*static*/ C080UltimateNinjaAI::MAP_ID
C080UltimateNinjaAI::CMapInfo::GetMapID(const CEnemyCharacter* pEnemyChr)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    pEnemyChr->Compositor().GetPosition(&vecPos);

    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M43N))
        return MAP_ID_PARK;

    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M44N))
        return ((vecPos.z <= -100.0f) ? MAP_ID_BRIDGE_2ND : MAP_ID_BRIDGE_1ST);    

    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M44NB))
        return MAP_ID_BRIDGE_PILLAR;

    return MAP_ID_OTHER;
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CActionDispatcher::CActionDispatcher(void)
: Utils6034::IActionDispatcher(CActionTable::GetListNum())
, m_iDragonPathID(-1)
{
    ;
};


/*virtual*/ bool
C080UltimateNinjaAI::CActionDispatcher::DispatchNormalAttack(int32 iTargetNo,
                                                             float fTargetDistanceSQ) /*override*/
{
    DISTANCE_STATE distanceState = DISTANCE_STATE_MAX;
    if (fTargetDistanceSQ < s_fAttackNearDistanceSQ)
        distanceState = DISTANCE_STATE_NEAR;
    else if (fTargetDistanceSQ < s_fAttackMiddleDistanceSQ)
        distanceState = DISTANCE_STATE_MIDDLE;

    if (distanceState != DISTANCE_STATE_MAX)
    {
        int32 iStateHandle = CActionTable::GetStateHandle(distanceState);
        return DispatchAction(iStateHandle, iTargetNo);
    };

    return false;
};


/*virtual*/ bool
C080UltimateNinjaAI::CActionDispatcher::DispatchCrowdCounterAttack(int32 iTargetNo) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_COUNTER_CROWD);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C080UltimateNinjaAI::CActionDispatcher::DispatchDamagedAction(int32 iTargetNo,
                                                              float fTargetDistanceSQ) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_COUNTER_DAMAGED);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C080UltimateNinjaAI::CActionDispatcher::DispatchDefence(void) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(DEFENCE_STATE_GUARD);
    return DispatchAction(iStateHandle, 0);
};


/*virtual*/ int32
C080UltimateNinjaAI::CActionDispatcher::CalcMoveProbability(void) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(MOVE_STATE_RUN);

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    int8 iActionType = m_probabilityTable.CalcProbability(iStateHandle, difficulty);

    return iActionType;
};


/*virtual*/ void
C080UltimateNinjaAI::CActionDispatcher::OnInitialize(void) /*override*/
{
    /*
     *  probability for DISTANCE
     */
    Utils6034::CProbabilityRecord aProbabilityDistNear[] = { { ACTION_TYPE_ATTACK_AA,     { 40, 35, 15 } },
                                                             { ACTION_TYPE_ATTACK_BBBB,   {  0, 25, 35 } },
                                                             { ACTION_TYPE_ATTACK_ENMAKU, { 10, 10, 20 } },
                                                             { ACTION_TYPE_ATTACK_JUMP,   { 50, 30, 30 } }, };

    Utils6034::CProbabilityRecord aProbabilityDistMiddle[] = { { ACTION_TYPE_ATTACK_BBBB,   { 10, 10, 10 } },
                                                               { ACTION_TYPE_ATTACK_FUNDOU, { 40, 40, 40 } },
                                                               { ACTION_TYPE_ATTACK_JUMP,   { 50, 50, 50 } }, };

    /*
     *  probability for SPECIAL
     */
    Utils6034::CProbabilityRecord aProbabilityAdditionAttack[] = { { ACTION_TYPE_ATTACK_AA,     {  0, 20,  0 } },
                                                                   { ACTION_TYPE_ATTACK_FUNDOU, {  2, 40, 50 } },
                                                                   { ACTION_TYPE_ATTACK_JUMP,   {  0, 40, 50 } },
                                                                   { ACTION_TYPE_NOTHING,       { 98,  0,  0 } }, };

    Utils6034::CProbabilityRecord aProbabilityCounterDamaged[] = { { ACTION_TYPE_WARP,          { 100, 60, 30 } },
                                                                   { ACTION_TYPE_ATTACK_ENMAKU, {  50, 40, 70 } },
                                                                   { ACTION_TYPE_NOTHING,       {   0,  0,  0 } }, };

    /*
     *  probability for DEFENCE
     */
    Utils6034::CProbabilityRecord aProbabilityGuard[] = { { ACTION_TYPE_DEFENCE_GUARD, { 40, 20, 20 } },
                                                          { ACTION_TYPE_WARP,          { 15, 25, 30 } },
                                                          { ACTION_TYPE_ATTACK_ENMAKU, { 15, 25, 40 } },
                                                          { ACTION_TYPE_NOTHING,       { 30, 30, 10 } }, };

    /*
     *  probability for MOVE
     */
    Utils6034::CProbabilityRecord aProbabilityRun[] = { { AI_STATE_MOVE_RUN,        { 10, 20, 30 } },
                                                        { AI_STATE_MOVE_RUN_CIRCLE, { 90, 80, 70 } }, };
    

    /*
     *  init probability list
     */
    Utils6034::CProbabilityTable::CListData aListData[] =
    {
        SetListData(DISTANCE_STATE_NEAR,            aProbabilityDistNear),
        SetListData(DISTANCE_STATE_MIDDLE,          aProbabilityDistMiddle),
        SetListData(SPECIAL_STATE_ADDITION_ATTACK,  aProbabilityAdditionAttack),
        SetListData(SPECIAL_STATE_COUNTER_DAMAGED,  aProbabilityCounterDamaged),
        SetListData(DEFENCE_STATE_GUARD,            aProbabilityGuard),
        SetListData(MOVE_STATE_RUN,                 aProbabilityRun),
    };

    Utils6034::CProbabilityTable::CListData* apListData[COUNT_OF(aListData)];
    for (int32 i = 0; i < COUNT_OF(aListData); ++i)
        apListData[i] = &aListData[i];

    m_probabilityTable.CopyListDataArray(apListData, COUNT_OF(apListData));
};


bool
C080UltimateNinjaAI::CActionDispatcher::DispatchStunAdditionAttack(int32 iTargetNo)
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_ADDITION_ATTACK);
    return DispatchAction(iStateHandle, iTargetNo);
};


bool
C080UltimateNinjaAI::CActionDispatcher::DispatchDragonAttack(int32 iPathID)
{
    m_iDragonPathID = iPathID;

    MessageStackAIStateContext().Cleanup();
    MessageStackAIStateContext().ChangeState(AI_STATE_ATTACK_DRAGON);

    return true;
};


bool
C080UltimateNinjaAI::CActionDispatcher::DispatchEscape(void)
{
    MessageStackAIStateContext().Cleanup();
    MessageStackAIStateContext().ChangeState(AI_STATE_ESCAPE);

    return true;
};


bool
C080UltimateNinjaAI::CActionDispatcher::DispatchAction(int32 iStateHandle,
                                                       int32 iTargetNo)
{
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    int8 iActionType = m_probabilityTable.CalcProbability(iStateHandle, difficulty);

    return PushState(iActionType, iTargetNo);
};


bool
C080UltimateNinjaAI::CActionDispatcher::PushState(int32 iActionType,
                                                  int32 iTargetNo)
{
    bool bResult = false;

    switch (iActionType)
    {
    case ACTION_TYPE_NOTHING:
        break;

    case ACTION_TYPE_ATTACK_AA:
    case ACTION_TYPE_ATTACK_BBBB:
    case ACTION_TYPE_ATTACK_FUNDOU:
        {
            Utils6034::CWaitData waitData;
            waitData.fWaitTime = GetWaitTime();

            Utils6034::CNormalAttackData attackData;
            attackData.iAttackType = GetAttackType(iActionType);
            attackData.iTargetNo = iTargetNo;

            MessageStackAIStateContext().PushMessage(AI_STATE_WAIT, waitData);
            MessageStackAIStateContext().PushMessage(AI_STATE_ATTACK, attackData);
            bResult = true;
        }
        break;

    case ACTION_TYPE_ATTACK_ENMAKU:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_ATTACK_SMOKE);
            bResult = true;
        }
        break;

    case ACTION_TYPE_ATTACK_JUMP:
        {
            Utils6034::CWaitData waitData;
            waitData.fWaitTime = GetWaitTime();

            MessageStackAIStateContext().PushMessage(AI_STATE_WAIT, waitData);
            MessageStackAIStateContext().PushMessage(AI_STATE_ATTACK_SHURIKEN);
            bResult = true;
        }
        break;

    case ACTION_TYPE_WARP:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_WARP);
            bResult = true;
        }
        break;

    case ACTION_TYPE_DEFENCE_GUARD:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_GUARD);
            bResult = true;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


int32
C080UltimateNinjaAI::CActionDispatcher::GetAttackType(int32 iActionType) const
{
    switch (iActionType)
    {
    case ACTION_TYPE_ATTACK_AA:
        return ORDERTYPE_ATTACK_AA;

    case ACTION_TYPE_ATTACK_BBBB:
        return ORDERTYPE_ATTACK_BBBB;

    case ACTION_TYPE_ATTACK_ENMAKU:
        return ORDERTYPE_ATTACK_ENMAKU;

    case ACTION_TYPE_ATTACK_FUNDOU:
        return ORDERTYPE_ATTACK_FUNDOU;

    case ACTION_TYPE_ATTACK_DRAGON:
        return ORDERTYPE_ATTACK_DRAGON;

    case ACTION_TYPE_ATTACK_JUMP:
        return ORDERTYPE_ATTACK_JUMP;

    default:
        ASSERT(false);
        break;
    };

    return ORDERTYPE_ATTACK_AA;
};


float
C080UltimateNinjaAI::CActionDispatcher::GetWaitTime(void) const
{
    float afWaitTime[] = { 1.0f, 0.3f, 0.0f };
    static_assert(COUNT_OF(afWaitTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afWaitTime[difficulty];
};


int32
C080UltimateNinjaAI::CActionDispatcher::GetDragonPathID(void) const
{
    return m_iDragonPathID;
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CHpWatcher::CHpWatcher(CActionDispatcher* pActionDispatcher, MAP_ID eMapID)
: m_pActionDispatcher(pActionDispatcher)
, m_eMapID(eMapID)
, m_iDragonPhase(0)
, m_bEscapeDispatched(false)
{
    ;
};


void C080UltimateNinjaAI::CHpWatcher::Update(void)
{
    switch (m_eMapID)
    {
    case MAP_ID_PARK:
    case MAP_ID_BRIDGE_1ST:
    case MAP_ID_BRIDGE_2ND:
        WatchEscape();
        break;

    case MAP_ID_OTHER:
    case MAP_ID_BRIDGE_PILLAR:
        WatchDragonAttack();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void C080UltimateNinjaAI::CHpWatcher::WatchDragonAttack(void)
{
    switch (m_iDragonPhase)
    {
    case 0:
        {
            if (IsHpLower(0.8f))
                m_pActionDispatcher->DispatchDragonAttack(m_iDragonPhase);
        }
        break;

    case 1:
        {
            if (IsHpLower(0.55f))
                m_pActionDispatcher->DispatchDragonAttack(m_iDragonPhase);
        }
        break;

    case 2:
        {
            if (IsHpLower(0.25f))
                m_pActionDispatcher->DispatchDragonAttack(m_iDragonPhase);
        }
        break;

    default:
        break;
    };
};


void C080UltimateNinjaAI::CHpWatcher::WatchEscape(void)
{
    if (!m_bEscapeDispatched && IsHpLower(0.875f))
        m_pActionDispatcher->DispatchEscape();
};


void C080UltimateNinjaAI::CHpWatcher::OnDragonAttackEnd(void)
{
    ++m_iDragonPhase;
};


void C080UltimateNinjaAI::CHpWatcher::OnEscapeEnd(void)
{
    m_bEscapeDispatched = true;
};


bool C080UltimateNinjaAI::CHpWatcher::IsHpLower(float fHpRatio) const
{
    float fHpRatioNow = CEnemyUtils::GetEnemyHPRate(&m_pActionDispatcher->CharacterController().EnemyChr());
    return (fHpRatioNow <= fHpRatio);
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CEscapePoint::CEscapePoint(void)
: CEscapePoint(MAP_ID_OTHER)
{
    ;
};


C080UltimateNinjaAI::CEscapePoint::CEscapePoint(MAP_ID eMapID)
: m_avecEscapePoints(nullptr)
, m_iPointCurrent(0)
, m_iPointMax(0)
{
    Initialize(eMapID);
};


void C080UltimateNinjaAI::CEscapePoint::Initialize(MAP_ID eMapID)
{
    m_iPointCurrent = 0;

    if (eMapID == MAP_ID_PARK)
    {
        m_avecEscapePoints = s_avecEscapePointsM43N;
        m_iPointMax = COUNT_OF(s_avecEscapePointsM43N);
    }
    else
    {
        m_avecEscapePoints = s_avecEscapePointsM44N;
        m_iPointMax = COUNT_OF(s_avecEscapePointsM44N);
    };
};


void C080UltimateNinjaAI::CEscapePoint::SetNextIndex(void)
{
    if (!IsEndIndex())
        ++m_iPointCurrent;
};


bool C080UltimateNinjaAI::CEscapePoint::IsEndIndex(void) const
{
    ASSERT(m_iPointMax > 0);

    return (m_iPointCurrent == (m_iPointMax - 1));
};


RwV3d C080UltimateNinjaAI::CEscapePoint::GetTargetPos(void) const
{
    ASSERT(m_avecEscapePoints != nullptr);

    if ((m_iPointCurrent < 0) || (m_iPointCurrent >= m_iPointMax))
        return m_avecEscapePoints[0];

    return m_avecEscapePoints[m_iPointCurrent];
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CThinkingState::CThinkingState(CActionDispatcher* pActionDispatcher)
: Utils6034::CBaseThinkingState(AI_STATE_THINK, AI_STATE_MOVE_MAX)
, m_pActionDispatcher(pActionDispatcher)
{
    Utils6034::IAIState* pAIState = nullptr;

    pAIState = new Utils6034::CRunDispatcher(AI_STATE_MOVE_RUN);
    m_moveStateCtx.RegistAIState(pAIState);

    pAIState = new Utils6034::CCircleRunDispatcher(AI_STATE_MOVE_RUN_CIRCLE);
    m_moveStateCtx.RegistAIState(pAIState);

    m_moveStateCtx.SetDefaultState(AI_STATE_MOVE_RUN);
    m_moveStateCtx.StartDefaultState();
};


/*virtual*/ C080UltimateNinjaAI::CThinkingState::RUNRESULT
C080UltimateNinjaAI::CThinkingState::OnRun(void) /*override*/
{
    /* TODO: broken guard (read C089FeudalJapanShredderAI::CThinkingState::OnRun) */
    TargetManager().Update();

    if (TargetManager().IsTargetStun())
    {
        int32 iTargetNo = TargetManager().GetTargetNo();
        m_pActionDispatcher->DispatchStunAdditionAttack(iTargetNo);

        return RUNRESULT_END;
    };

    if (!TargetManager().IsTargetSeeMe())
    {
        RwV3d vecTargetPos = TargetManager().GetTargetPos();
        CharacterController().DispatchRunPos(&vecTargetPos);

        return RUNRESULT_END;
    };

    return Utils6034::CBaseThinkingState::OnRun();
};


/*virtual*/ bool
C080UltimateNinjaAI::CThinkingState::OnAllTargetDown(void) /*override*/
{
    return false;
};


/*virtual*/ bool
C080UltimateNinjaAI::CThinkingState::OnTargetAttack(int32 iTargetNo) /*override*/
{
    if (!ActionDispatcher().DispatchDefence())
        return true;

    return false;
};


/*virtual*/ bool
C080UltimateNinjaAI::CThinkingState::OnEndMove(void) /*override*/
{
    int32 iTargetNo = TargetManager().GetTargetNo();
    float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();

    if (!ActionDispatcher().DispatchNormalAttack(iTargetNo, fTargetDistSQ))
        return true;

    return false;
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CSmokeAttackState::CSmokeAttackState(void)
: Utils6034::IBaseAIState(AI_STATE_ATTACK_SMOKE)
, m_timer(1.0f)
{
    ;
};


/*virtual*/ C080UltimateNinjaAI::CSmokeAttackState::STARTRESULT
C080UltimateNinjaAI::CSmokeAttackState::OnStart(void) /*override*/
{
    return STARTRESULT_END;
};


/*virtual*/ int32
C080UltimateNinjaAI::CSmokeAttackState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C080UltimateNinjaAI::CSmokeAttackState::RUNRESULT
C080UltimateNinjaAI::CSmokeAttackState::OnRun(void) /*override*/
{
    int32 iTargetNo = TargetManager().GetTargetNo();

    m_timer.Update();
    if (m_timer.IsAlarm())
    {
        TargetManager().Update();
        CharacterController().DispatchAttack(ORDERTYPE_ATTACK_ENMAKU, iTargetNo);

        m_timer.Start();
    }
    else
    {
        CharacterController().DispatchAttack(ORDERTYPE_ATTACK_FUNDOU, iTargetNo);
    };

    return RUNRESULT_END;
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CShurikenAttackState::CShurikenAttackState(void)
: Utils6034::IBaseAIState(AI_STATE_ATTACK_SHURIKEN)
, m_timer(GetNextShurikenTime())
{
    ;
};


/*virtual*/ C080UltimateNinjaAI::CShurikenAttackState::STARTRESULT
C080UltimateNinjaAI::CShurikenAttackState::OnStart(void) /*override*/
{
    return STARTRESULT_END;
};


/*virtual*/ int32
C080UltimateNinjaAI::CShurikenAttackState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C080UltimateNinjaAI::CShurikenAttackState::RUNRESULT
C080UltimateNinjaAI::CShurikenAttackState::OnRun(void) /*override*/
{
    int32 iTargetNo = TargetManager().GetTargetNo();

    m_timer.Update();
    if (m_timer.IsAlarm())
    {
        TargetManager().Update();
        CharacterController().DispatchAttack(ORDERTYPE_ATTACK_JUMP, iTargetNo);

        m_timer.Start();
    }
    else
    {
        CharacterController().DispatchAttack(ORDERTYPE_ATTACK_FUNDOU, iTargetNo);
    };

    return RUNRESULT_END;
};


float
C080UltimateNinjaAI::CShurikenAttackState::GetNextShurikenTime(void) const
{
    float afNextTime[] = { 20.0f, 10.0f, 5.0f };
    static_assert(COUNT_OF(afNextTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afNextTime[difficulty];
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CEscapeState::CEscapeState(CHpWatcher* pHpWatcher)
: Utils6034::IBaseAIState(AI_STATE_ESCAPE)
, m_pHpWatcher(pHpWatcher)
, m_timer(5.0f)
, m_step(-1)
, m_eMapID(MAP_ID_OTHER)
, m_escapeStep(-1)
, m_escapePoint()
{
    ;
};


/*virtual*/ C080UltimateNinjaAI::CEscapeState::STARTRESULT
C080UltimateNinjaAI::CEscapeState::OnStart(void) /*override*/
{
    m_step = 0;
    m_escapeStep = 0;
    m_eMapID = CMapInfo::GetMapID(&AIModerator().EnemyCharacter());
    m_escapePoint.Initialize(m_eMapID);

    CharacterController().FlagInvincibility().IncCounter();

    if (m_eMapID == MAP_ID_PARK)
    {
        CMessageManager::Request(SEGROUPID::VALUE(105));
        m_timer.Start();
    };

    return STARTRESULT_END;
};


/*virtual*/ int32
C080UltimateNinjaAI::CEscapeState::OnEnd(void) /*override*/
{
    AIModerator().EnemyCharacter().Compositor().SetEnableBodyHit(true);
    CharacterController().FlagInvincibility().DecCounter();

    m_pHpWatcher->OnEscapeEnd();

    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C080UltimateNinjaAI::CEscapeState::RUNRESULT
C080UltimateNinjaAI::CEscapeState::OnRun(void) /*override*/
{
    AIModerator().EnemyCharacter().Compositor().SetEnableBodyHit(false);

    switch (m_step)
    {
    case 0:
        {
            bool bEscapeEnd = (m_eMapID == MAP_ID_PARK ? Update43N() : Update44N());
            if (bEscapeEnd)
                ++m_step;
        }
        break;

    case 1:
        CharacterController().Death();
        break;

    default:
        break;        
    };

    return RUNRESULT_CONTINUE;
};


bool C080UltimateNinjaAI::CEscapeState::Update43N(void)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    AIModerator().EnemyCharacter().Compositor().GetPosition(&vecPos);

    RwV3d vecEscapePos = m_escapePoint.GetTargetPos();

    switch (m_escapeStep)
    {
    case 0:
        {
            if (vecPos.z >= -6.0f)
            {
                if ((vecEscapePos.x + 1.2f) > vecPos.x)
                {
                    m_escapePoint.SetNextIndex();
                    ++m_escapeStep;
                }
                else
                {
                    CharacterController().DispatchRunPos(&vecEscapePos);
                };
            }
            else
            {
                RwV3d vecWarpPos = vecPos;
                vecWarpPos.z = 0.0f;

                CharacterController().SetMoveOrder(ORDERTYPE_MOVE_WARP_END, &vecWarpPos);
            };
        }
        break;

    case 1:
        {
            CharacterController().SetMoveOrder(ORDERTYPE_MOVE_WARP_END, &vecEscapePos);
            ++m_escapeStep;
        }
        break;

    case 2:
        {
            RwV3d vecDirection = { 1.0f, 0.0f, 0.0f };
            AIModerator().EnemyCharacter().Compositor().SetDirectionFromVector(&vecDirection);

            ++m_escapeStep;

            m_timer.Update();
            if (m_timer.IsAlarm())
            {
                m_timer.Pause();
                ++m_escapeStep;

                CMessageManager::Request(SEGROUPID::VALUE(107));
            };
        }
        break;

    case 3:
        {
            m_timer.Update();
            if (m_timer.IsAlarm())
            {
                m_timer.Pause();
                CMessageManager::Request(SEGROUPID::VALUE(107));
                ++m_escapeStep;
            };
        }
        break;

    case 4:
        {
            TargetManager().Update();
            float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
            if (fTargetDistSQ < 64.0f)
            {
                CharacterController().SetMoveOrder(ORDERTYPE_MOVE_WARP_END, &vecEscapePos);
                ++m_escapeStep;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return (m_escapeStep > 4 ? true : false);
};


bool C080UltimateNinjaAI::CEscapeState::Update44N(void)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    AIModerator().EnemyCharacter().Compositor().GetPosition(&vecPos);
    vecPos.x = 10.0f;

    switch (m_escapeStep++)
    {
    case 0:
        CharacterController().SetMoveOrder(ORDERTYPE_MOVE_WARP_END, &vecPos);
        break;

    case 1:
        CharacterController().SetMoveOrder(ORDERTYPE_MOVE_WARP_END, &vecPos);
        break;

    default:
        ASSERT(false);
        break;
    };

    return (m_escapeStep > 1 ? true : false);
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CDragonAttackState::CDragonAttackState(CActionDispatcher* pActionDispatcher,
                                                            CHpWatcher* pHpWatcher)
: Utils6034::IBaseAIState(AI_STATE_ATTACK_DRAGON)
, m_pActionDispatcher(pActionDispatcher)
, m_pHpWatcher(pHpWatcher)
, m_step(-1)
{
    ;
};


/*virtual*/ C080UltimateNinjaAI::CDragonAttackState::STARTRESULT
C080UltimateNinjaAI::CDragonAttackState::OnStart(void) /*override*/
{
    CharacterController().FlagInvincibility().IncCounter();
    return STARTRESULT_END;
};


/*virtual*/ int32
C080UltimateNinjaAI::CDragonAttackState::OnEnd(void) /*override*/
{
    CharacterController().FlagInvincibility().DecCounter();
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C080UltimateNinjaAI::CDragonAttackState::RUNRESULT
C080UltimateNinjaAI::CDragonAttackState::OnRun(void) /*override*/
{
    OrderDragonAttack();    
    return RUNRESULT_END;
};


void C080UltimateNinjaAI::CDragonAttackState::OrderDragonAttack(void)
{
    int32 iPathID = m_pActionDispatcher->GetDragonPathID();
    CharacterController().SetAttackOrder(ORDERTYPE_ATTACK_DRAGON, iPathID);
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CDownState::CDownState(void)
: Utils6034::CBaseDownState(AI_STATE_DOWN, 0.7f)
, m_fWarpTriggerDistSQ(1.0f)
{
    ;
};


/*virtual*/ C080UltimateNinjaAI::CDownState::RUNRESULT
C080UltimateNinjaAI::CDownState::OnRun(void) /*override*/
{
    if (Utils6034::IsDownState(AIModerator().EnemyCharacter().GetStatus()))
        return RUNRESULT_CONTINUE;

    TargetManager().Update();

    float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
    if (TargetManager().IsTargetAttack() || (fTargetDistSQ < m_fWarpTriggerDistSQ))
    {
        RwV3d vecTargetPos = TargetManager().GetTargetPos();

        RwV3d vecWarpPos = Math::VECTOR3_ZERO;
        CalcWarpPos(&vecWarpPos, &vecTargetPos, &AIModerator().EnemyCharacter().Compositor(), 8.0f);

        CharacterController().SetMoveOrder(ORDERTYPE_MOVE_WARP, &vecWarpPos);
    };

    return RUNRESULT_END;
};


//
// *********************************************************************************
//


C080UltimateNinjaAI::CWarpState::CWarpState(void)
: Utils6034::IBaseAIState(AI_STATE_WARP)
{
    ;
};


/*virtual*/ C080UltimateNinjaAI::CWarpState::STARTRESULT
C080UltimateNinjaAI::CWarpState::OnStart(void) /*override*/
{
    RwV3d vecTargetPos = TargetManager().GetTargetPos();

    RwV3d vecWarpPos = Math::VECTOR3_ZERO;
    CalcWarpPos(&vecWarpPos, &vecTargetPos, &AIModerator().EnemyCharacter().Compositor(), 8.0f);

    CharacterController().SetMoveOrder(ORDERTYPE_MOVE_WARP, &vecWarpPos);

    return STARTRESULT_END;
};


/*virtual*/ int32
C080UltimateNinjaAI::CWarpState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C080UltimateNinjaAI::CWarpState::RUNRESULT
C080UltimateNinjaAI::CWarpState::OnRun(void) /*override*/
{
    return RUNRESULT_END;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* C080UltimateNinjaAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C080UltimateNinjaAI(pEnemyChr);
};


/*static*/ void C080UltimateNinjaAI::CalcWarpPos(RwV3d* pvecWarpPos,
                                                 const RwV3d* pvecTargetPos,
                                                 const CCharacterCompositor* pChrCompositor,
                                                 float fDistMax)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetPosition(&vecPos);

    RwV3d vecDist = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDist, pvecTargetPos, &vecPos);
    vecDist.y = 0.0f;

    Math::Vec3_Normalize(&vecDist, &vecDist);
    Math::Vec3_Scale(&vecDist, &vecDist, fDistMax);
    Math::Vec3_Add(&vecDist, &vecDist, pvecTargetPos);

    vecPos.y += 1.0f;
    vecDist.y = vecPos.y;

    Utils6034::GetMovePosByWorld(pvecWarpPos, &vecPos, &vecDist);

    pvecWarpPos->y = CWorldMap::GetMapHeight(pvecWarpPos);
};


C080UltimateNinjaAI::C080UltimateNinjaAI(CEnemyCharacter* pEnemyChr)
: Utils6034::IBaseAIModerator(pEnemyChr, &m_actionDispatcher, AI_STATE_MAX)
, m_actionDispatcher()
, m_hpWatcher(&m_actionDispatcher, CMapInfo::GetMapID(pEnemyChr))
{
    ;
};


/*virtual*/ C080UltimateNinjaAI::~C080UltimateNinjaAI(void)
{
    ;
};


/*virtual*/ void C080UltimateNinjaAI::Run(void) /*override*/
{
    m_hpWatcher.Update();

    if (Utils6034::IsDownState(EnemyCharacter().GetStatus()) &&
        MessageStackAIStateContext().GetCurrentStateHandle() != AI_STATE_ATTACK_DRAGON &&
        MessageStackAIStateContext().GetCurrentStateHandle() != AI_STATE_ESCAPE)
    {
        MessageStackAIStateContext().ChangeState(AI_STATE_DOWN);
    };

    if (CharacterController().IsEnableChangeAction())
    {
        MessageStackAIStateContext().RunState();
        CharacterController().Update();
    };
};


/*virtual*/ void C080UltimateNinjaAI::NotifyDamage(int32 iAmount) /*override*/
{
    if (MessageStackAIStateContext().GetCurrentStateHandle() == AI_STATE_ATTACK_DRAGON)
        return;

    MessageStackAIStateContext().Cleanup();
    MessageStackAIStateContext().ChangeState(AI_STATE_COUNTER_DAMAGED);
};


/*virtual*/ void C080UltimateNinjaAI::NotifyGuard(void) /*override*/
{
    m_hpWatcher.OnDragonAttackEnd();
};


/*virtual*/ void C080UltimateNinjaAI::OnInitialize(void) /*override*/
{
    /* attach ai state units */
    Utils6034::IAIState* pAIState = nullptr;

    Utils6034::CWaitState* pWaitAIState = new Utils6034::CWaitState(AI_STATE_WAIT);
    MessageStackAIStateContext().RegistAIState(pWaitAIState);
    MessageStackAIStateContext().SetReceiver(pWaitAIState);

    Utils6034::CNormalAttackState* pAttackAIState = new Utils6034::CNormalAttackState(AI_STATE_ATTACK);
    MessageStackAIStateContext().RegistAIState(pAttackAIState);
    MessageStackAIStateContext().SetReceiver(pAttackAIState);

    pAIState = new Utils6034::CBaseGuardState(AI_STATE_GUARD);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseDamagedCounterState(AI_STATE_COUNTER_DAMAGED);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C080UltimateNinjaAI::CDownState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C080UltimateNinjaAI::CThinkingState(&m_actionDispatcher);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C080UltimateNinjaAI::CDragonAttackState(&m_actionDispatcher, &m_hpWatcher);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C080UltimateNinjaAI::CWarpState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C080UltimateNinjaAI::CEscapeState(&m_hpWatcher);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C080UltimateNinjaAI::CSmokeAttackState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C080UltimateNinjaAI::CShurikenAttackState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    /* start ai state */
    MessageStackAIStateContext().SetDefaultState(AI_STATE_THINK);
    MessageStackAIStateContext().StartDefaultState();

    /* start target mgr */
    TargetManager().Start();
};