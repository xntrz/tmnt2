#include "090UltimateShredderAI.hpp"

#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/090UltimateShredder.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Map/MapCamera.hpp"


static float s_fAttackNearDistanceSQ = 2.25f;
static float s_fAttackMiddleDistanceSQ = 64.0f;


template<class TState, class TRecords, size_t iRecordNum>
inline static Utils6034::CProbabilityTable::CListData
SetListData(TState state, TRecords(&aRecords)[iRecordNum])
{
    Utils6034::CProbabilityTable::CListData listData;

    listData.iListIndex = C090UltimateShredderAI::CActionTable::GetStateHandle(state);
    listData.aRecords = aRecords;
    listData.iRecordsNum = iRecordNum;

    return listData;
};


template<class TState, class TSubState, class TRecords, size_t iRecordNum>
inline static Utils6034::CProbabilityTable::CListData
SetListData(TState state, TSubState subState, TRecords(&aRecords)[iRecordNum])
{
    Utils6034::CProbabilityTable::CListData listData;

    listData.iListIndex = C090UltimateShredderAI::CActionTable::GetStateHandle(state, subState);
    listData.aRecords = aRecords;
    listData.iRecordsNum = iRecordNum;

    return listData;
};


//
// *********************************************************************************
//


/*static*/ int32
C090UltimateShredderAI::CActionTable::GetListNum(void)
{
    return ((DISTANCE_STATE_MAX * HP_STATE_MAX) + SPECIAL_STATE_MAX + DEFENCE_STATE_MAX + MOVE_STATE_MAX);
};


/*static*/ int32
C090UltimateShredderAI::CActionTable::GetStateHandle(DISTANCE_STATE distanceState, HP_STATE hpState)
{
    if (distanceState >= DISTANCE_STATE_MAX)
        distanceState = DISTANCE_STATE_MIN;

    if (hpState >= HP_STATE_MAX)
        hpState = HP_STATE_MIN;

    return (hpState + (HP_STATE_MAX * distanceState));
};


/*static*/ int32
C090UltimateShredderAI::CActionTable::GetStateHandle(SPECIAL_STATE state)
{
    if (state >= SPECIAL_STATE_MAX)
        state = SPECIAL_STATE_MIN;

    return (DISTANCE_STATE_MAX * HP_STATE_MAX) + state;
};


/*static*/ int32
C090UltimateShredderAI::CActionTable::GetStateHandle(DEFENCE_STATE state)
{
    if (state >= DEFENCE_STATE_MAX)
        state = DEFENCE_STATE_MIN;

    return ((DISTANCE_STATE_MAX * HP_STATE_MAX) + SPECIAL_STATE_MAX) + state;
};


/*static*/ int32
C090UltimateShredderAI::CActionTable::GetStateHandle(MOVE_STATE state)
{
    if (state >= MOVE_STATE_MAX)
        state = MOVE_STATE_MIN;
    
    return (DISTANCE_STATE_MAX * HP_STATE_MAX) + SPECIAL_STATE_MAX + DEFENCE_STATE_MAX + state;
};


//
// *********************************************************************************
//


C090UltimateShredderAI::CActionDispatcher::CActionDispatcher(void)
: Utils6034::IActionDispatcher(CActionTable::GetListNum())
, m_stageType(STAGE_TYPE_OTHER)
{
    ;
};


/*virtual*/ C090UltimateShredderAI::CActionDispatcher::~CActionDispatcher(void)
{
    ;
};


/*virtual*/ bool
C090UltimateShredderAI::CActionDispatcher::DispatchNormalAttack(int32 iTargetNo,
                                                                float fTargetDistanceSQ) /*override*/
{
    HP_STATE hpState = GetHpState();
    DISTANCE_STATE distanceState = GetDistanceState(fTargetDistanceSQ);

    int32 iStateHandle = CActionTable::GetStateHandle(distanceState, hpState);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C090UltimateShredderAI::CActionDispatcher::DispatchAerialCounterAttack(int32 iTargetNo) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_COUNTER_AERIAL);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C090UltimateShredderAI::CActionDispatcher::DispatchCrowdCounterAttack(int32 iTargetNo) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_COUNTER_CROWD);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C090UltimateShredderAI::CActionDispatcher::DispatchDamagedAction(int32 iTargetNo,
                                                                 float fTargetDistanceSQ) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_COUNTER_DAMAGED);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C090UltimateShredderAI::CActionDispatcher::DispatchDefence(void) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(DEFENCE_STATE_GUARD);
    return DispatchAction(iStateHandle, 0);
};


/*virtual*/ int32
C090UltimateShredderAI::CActionDispatcher::CalcMoveProbability(void) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(MOVE_STATE_WALK);

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    int8 iActionType = m_probabilityTable.CalcProbability(iStateHandle, difficulty);

    return iActionType;
};


/*virtual*/ void
C090UltimateShredderAI::CActionDispatcher::OnInitialize(void) /*override*/
{
    /*
     *  probability for DISTANCE at HP HIGH
     */
    Utils6034::CProbabilityRecord aProbabilityHpHighDistNear[] = { { ACTION_TYPE_ATTACK_ELEC,   { 25, 23, 20 } },
                                                                   { ACTION_TYPE_COUNTER,       {  0,  8, 20 } }, 
                                                                   { ACTION_TYPE_ATTACK_TENGU,  { 25, 23, 20 } }, 
                                                                   { ACTION_TYPE_ATTACK_THROW,  { 25, 23, 20 } }, 
                                                                   { ACTION_TYPE_ATTACK_CHARGE, { 25, 23, 20 } }, };

    Utils6034::CProbabilityRecord aProbabilityHpHighDistMiddle[] = { { ACTION_TYPE_ATTACK_TENGU,   { 25, 25, 25 } },
                                                                     { ACTION_TYPE_ATTACK_CHARGE,  { 25, 25, 25 } },
                                                                     { ACTION_TYPE_ATTACK_ELEC,    { 25, 25, 25 } },
                                                                     { ACTION_TYPE_ATTACK_THUNDER, { 25, 25, 25 } }, };

    Utils6034::CProbabilityRecord aProbabilityHpHighDistLong[] = { { ACTION_TYPE_ATTACK_TENGU,   { 40, 40, 40 } },
                                                                   { ACTION_TYPE_ATTACK_ELEC,    { 30, 30, 30 } },
                                                                   { ACTION_TYPE_ATTACK_THUNDER, { 30, 30, 30 } }, };

    /*
     *  probability for DISTANCE at HP LOW
     */
    Utils6034::CProbabilityRecord aProbabilityHpLowDistNear[] = { { ACTION_TYPE_ATTACK_CHARGE,        { 13, 13, 13 } },
                                                                  { ACTION_TYPE_ATTACK_CHARGE_TRIPLE, { 13, 13, 13 } },
                                                                  { ACTION_TYPE_ATTACK_THROW,         { 37, 37, 37 } }, 
                                                                  { ACTION_TYPE_ATTACK_TENGU,         { 37, 37, 37 } }, };

    Utils6034::CProbabilityRecord aProbabilityHpLowDistMiddle[] = { { ACTION_TYPE_ATTACK_ELEC,          { 26, 26, 26 } },
                                                                    { ACTION_TYPE_ATTACK_CHARGE_TRIPLE, { 11, 11, 11 } },
                                                                    { ACTION_TYPE_ATTACK_CHARGE,        { 11, 11, 11 } },
                                                                    { ACTION_TYPE_ATTACK_THUNDER,       { 26, 26, 26 } },
                                                                    { ACTION_TYPE_ATTACK_TENGU,         { 26, 26, 26 } }, };

    Utils6034::CProbabilityRecord aProbabilityHpLowDistLong[] = { { ACTION_TYPE_ATTACK_TENGU,   { 50, 50, 50 } },
                                                                  { ACTION_TYPE_ATTACK_ELEC,    { 25, 25, 25 } },
                                                                  { ACTION_TYPE_ATTACK_THUNDER, { 25, 25, 25 } }, };

    /*
     *  probability for SPECIAL
     */
    Utils6034::CProbabilityRecord aProbabilityAerial[] = { { ACTION_TYPE_ATTACK_JUMP,  { 50, 50, 50 } },
                                                           { ACTION_TYPE_NOTHING,      { 50, 50, 50 } }, };

    Utils6034::CProbabilityRecord aProbabilityCrowd[] = { { ACTION_TYPE_ATTACK_JUMP,  { 50, 50, 50 } },
                                                          { ACTION_TYPE_NOTHING,      { 50, 50, 50 } }, };

    Utils6034::CProbabilityRecord aProbabilityDamaged[] = { { ACTION_TYPE_DEFENCE_AVOID,    {  0, 30, 50 } },
                                                            { ACTION_TYPE_ATTACK_JUMP,      {  0, 40, 35 } },
                                                            { ACTION_TYPE_DEFENCE_GUARD,    { 50, 30, 15 } },
                                                            { ACTION_TYPE_NOTHING,          { 50,  0,  0 } }, };

    /*
     *  probability for DEFENCE
     */
    Utils6034::CProbabilityRecord aProbabilityGuard[] = { { ACTION_TYPE_DEFENCE_GUARD, { 50, 50, 20 } },
                                                          { ACTION_TYPE_DEFENCE_AVOID, { 20, 30, 40 } },
                                                          { ACTION_TYPE_ATTACK_JUMP,   { 30, 20, 40 } },
                                                          { ACTION_TYPE_NOTHING,       {  0,  0,  0 } }, };

     /*
     *  probability for MOVE
     */
    Utils6034::CProbabilityRecord aProbabilityNove[] = { { AI_STATE_MOVE_WALK,        { 50, 30, 20 } },
                                                         { AI_STATE_MOVE_DASH,        { 20, 30, 30 } },
                                                         { AI_STATE_MOVE_DASH_CIRCLE, { 30, 40, 50 } }, };
   
    /*
     *  init probability list
     */
    Utils6034::CProbabilityTable::CListData aListData[] =
    {
        SetListData(DISTANCE_STATE_NEAR,   HP_STATE_100PAR, aProbabilityHpHighDistNear),
        SetListData(DISTANCE_STATE_MIDDLE, HP_STATE_100PAR, aProbabilityHpHighDistMiddle),
        SetListData(DISTANCE_STATE_LONG,   HP_STATE_100PAR, aProbabilityHpHighDistLong),
        SetListData(DISTANCE_STATE_NEAR,   HP_STATE_50PAR,  aProbabilityHpLowDistNear),
        SetListData(DISTANCE_STATE_MIDDLE, HP_STATE_50PAR,  aProbabilityHpLowDistMiddle),
        SetListData(DISTANCE_STATE_LONG,   HP_STATE_50PAR,  aProbabilityHpLowDistLong),
        SetListData(SPECIAL_STATE_COUNTER_AERIAL,           aProbabilityAerial),
        SetListData(SPECIAL_STATE_COUNTER_CROWD,            aProbabilityCrowd),
        SetListData(SPECIAL_STATE_COUNTER_DAMAGED,          aProbabilityDamaged),
        SetListData(DEFENCE_STATE_GUARD,                    aProbabilityGuard),
        SetListData(MOVE_STATE_WALK,                        aProbabilityNove),
    };

    Utils6034::CProbabilityTable::CListData* apListData[COUNT_OF(aListData)];
    for (int32 i = 0; i < COUNT_OF(aListData); ++i)
        apListData[i] = &aListData[i];

    m_probabilityTable.CopyListDataArray(apListData, COUNT_OF(apListData));

    /*
     *  init stage type
     */
    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M58O))
    {
        m_stageType = STAGE_TYPE_DECK;
    }
    else if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M58OB))
    {
        m_stageType = STAGE_TYPE_LAB;
    };    
};


bool
C090UltimateShredderAI::CActionDispatcher::DispatchAction(int32 iStateHandle,
                                                          int32 iTargetNo)
{
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    int8 iActionType = m_probabilityTable.CalcProbability(iStateHandle, difficulty);

    return PushState(iActionType, iTargetNo);
};


bool
C090UltimateShredderAI::CActionDispatcher::PushState(int32 iActionType,
                                                     int32 iTargetNo)
{
    bool bResult = false;

    switch (iActionType)
    {
    case ACTION_TYPE_NOTHING:
        break;

    case ACTION_TYPE_ATTACK_TENGU:
    case ACTION_TYPE_ATTACK_CHARGE:
    case ACTION_TYPE_ATTACK_THROW:
    case ACTION_TYPE_ATTACK_THUNDER:
    case ACTION_TYPE_ATTACK_JUMP:
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

    case ACTION_TYPE_ATTACK_ELEC:
        {
            Utils6034::CWaitData waitData;
            waitData.fWaitTime = GetWaitTime();

            MessageStackAIStateContext().PushMessage(AI_STATE_WAIT, waitData);
            MessageStackAIStateContext().PushMessage(AI_STATE_ELEC);
            bResult = true;
        }
        break;

    case ACTION_TYPE_ATTACK_CHARGE_TRIPLE:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_ATTACK_TRIPLE);
            bResult = true;
        }
        break;

    case ACTION_TYPE_COUNTER:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_COUNTER);
            bResult = true;
        }
        break;

    case ACTION_TYPE_DEFENCE_GUARD:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_GUARD);
            bResult = true;
        }
        break;

    case ACTION_TYPE_DEFENCE_AVOID:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_AVOID);
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
C090UltimateShredderAI::CActionDispatcher::GetAttackType(int32 iActionType) const
{
    switch (iActionType)
    {
    case ACTION_TYPE_ATTACK_TENGU:
        return ORDERTYPE_ATTACK_TENGU;

    case ACTION_TYPE_ATTACK_CHARGE:
        return ORDERTYPE_ATTACK_CHARGE;

    case ACTION_TYPE_ATTACK_THROW:
        return ORDERTYPE_ATTACK_THROW;

    case ACTION_TYPE_ATTACK_THUNDER:
        return ORDERTYPE_ATTACK_THUNDER;

    case ACTION_TYPE_ATTACK_JUMP:
        return ORDERTYPE_ATTACK_JUMP;

    default:
        ASSERT(false);
        break;
    };

    return ORDERTYPE_ATTACK_TENGU;
};


float
C090UltimateShredderAI::CActionDispatcher::GetWaitTime(void) const
{
    float afWaitTime[] = { 1.2f, 0.5f, 0.2f };
    static_assert(COUNT_OF(afWaitTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afWaitTime[difficulty];
};


C090UltimateShredderAI::HP_STATE
C090UltimateShredderAI::CActionDispatcher::GetHpState(void) const
{
    float fHpRatio = CEnemyUtils::GetEnemyHPRate(&CharacterController().EnemyChr());

    HP_STATE hpState = HP_STATE_MAX;
    if (m_stageType == STAGE_TYPE_DECK)
        hpState = HP_STATE_100PAR;
    else if (m_stageType == STAGE_TYPE_LAB)
        hpState = HP_STATE_50PAR;
    else
        hpState = (fHpRatio <= 0.5f ? HP_STATE_50PAR : HP_STATE_100PAR);

    return hpState;
};


C090UltimateShredderAI::DISTANCE_STATE
C090UltimateShredderAI::CActionDispatcher::GetDistanceState(float fTargetDistanceSQ) const
{
    DISTANCE_STATE distanceState = DISTANCE_STATE_MAX;
    if (fTargetDistanceSQ < s_fAttackNearDistanceSQ)
        distanceState = DISTANCE_STATE_NEAR;
    else if (fTargetDistanceSQ < s_fAttackMiddleDistanceSQ)
        distanceState = DISTANCE_STATE_MIDDLE;
    else
        distanceState = DISTANCE_STATE_LONG;

    return distanceState;
};


//
// *********************************************************************************
//


C090UltimateShredderAI::CThinkingState::CThinkingState(void)
: Utils6034::CBaseThinkingState(AI_STATE_THINK, AI_STATE_MOVE_MAX)
{
    Utils6034::IAIState* pAIState = nullptr;

    pAIState = new Utils6034::CWalkDispatcher(AI_STATE_MOVE_WALK);
    m_moveStateCtx.RegistAIState(pAIState);

    pAIState = new Utils6034::CDashDispatcher(AI_STATE_MOVE_DASH);
    m_moveStateCtx.RegistAIState(pAIState);

    pAIState = new Utils6034::CCircleDashDispatcher(AI_STATE_MOVE_DASH_CIRCLE);
    m_moveStateCtx.RegistAIState(pAIState);

    m_moveStateCtx.SetDefaultState(AI_STATE_MOVE_WALK);
    m_moveStateCtx.StartDefaultState();
};


/*virtual*/ C090UltimateShredderAI::CThinkingState::RUNRESULT
C090UltimateShredderAI::CThinkingState::OnRun(void) /*override*/
{
    /* TODO: broken guard (read C089FeudalJapanShredderAI::CThinkingState::OnRun) */
    TargetManager().Update();

    if (!TargetManager().IsTargetSeeMe())
    {
        RwV3d vecTargetPos = TargetManager().GetTargetPos();
        CharacterController().DispatchDashPos(&vecTargetPos);

        return RUNRESULT_END;
    };

    return Utils6034::CBaseThinkingState::OnRun();
};


/*virtual*/ bool
C090UltimateShredderAI::CThinkingState::OnAllTargetDown(void) /*override*/
{
    return false;
};


/*virtual*/ bool
C090UltimateShredderAI::CThinkingState::OnTargetAttack(int32 iTargetNo) /*override*/
{
    if (!ActionDispatcher().DispatchDefence())
        return true;

    return false;
};


/*virtual*/ bool
C090UltimateShredderAI::CThinkingState::OnEndMove(void) /*override*/
{
    if (TargetManager().IsTargetFlyaway())
        return true;

    int32 iTargetNo = TargetManager().GetTargetNo();
    float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();

    if (!ActionDispatcher().DispatchNormalAttack(iTargetNo, fTargetDistSQ))
        return true;

    return false;
};


//
// *********************************************************************************
//


C090UltimateShredderAI::CCounterState::CCounterState(void)
: CCounterState(AI_STATE_COUNTER)
{
    ;
};


C090UltimateShredderAI::CCounterState::CCounterState(int32 iStateHandle)
: Utils6034::IBaseAIState(iStateHandle)
, m_step(-1)
{
    ;
};


/*virtual*/ C090UltimateShredderAI::CCounterState::STARTRESULT
C090UltimateShredderAI::CCounterState::OnStart(void) /*override*/
{
    m_step = 0;
    return STARTRESULT_END;
};


/*virtual*/ int32
C090UltimateShredderAI::CCounterState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C090UltimateShredderAI::CCounterState::RUNRESULT
C090UltimateShredderAI::CCounterState::OnRun(void) /*override*/
{
    TargetManager().Update();    

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    AIModerator().EnemyCharacter().Compositor().GetPosition(&vecMyPos);

    RwV3d vecTargetPos = TargetManager().GetTargetPos();

    switch (m_step++)
    {
    case 0:
        {
            RwV3d vecDirecton = Math::VECTOR3_ZERO;
            if (Math::Rand() & 1)
                Math::Vec3_Sub(&vecDirecton, &vecTargetPos, &vecMyPos); // to player
            else
                Math::Vec3_Sub(&vecDirecton, &vecMyPos, &vecTargetPos); // from player

            Utils6034::MakeXByDirection(&vecDirecton, &vecDirecton, &Math::VECTOR3_AXIS_Y);
            Math::Vec3_Add(&vecTargetPos, &vecTargetPos, &vecDirecton);

            CharacterController().DispatchDashPos(&vecTargetPos);
        }
        break;

    case 1:
        {
            RwV3d vecDirecton = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecDirecton, &vecTargetPos, &vecMyPos);
            AIModerator().EnemyCharacter().Compositor().SetDirectionFromVector(&vecDirecton);

            int32 iTargetNo = TargetManager().GetTargetNo();
            float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
            ActionDispatcher().DispatchNormalAttack(iTargetNo, fTargetDistSQ);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return (m_step > 1 ? RUNRESULT_END : RUNRESULT_CONTINUE);
};


//
// *********************************************************************************
//


C090UltimateShredderAI::CAvoidState::CAvoidState(void)
: CCounterState(AI_STATE_AVOID)
{
    ;
};


/*virtual*/ C090UltimateShredderAI::CAvoidState::STARTRESULT
C090UltimateShredderAI::CAvoidState::OnStart(void) /*override*/
{
    CharacterController().FlagNoReaction().IncCounter();

    return CCounterState::OnStart();
};


/*virtual*/ int32
C090UltimateShredderAI::CAvoidState::OnEnd(void) /*override*/
{
    CharacterController().FlagNoReaction().AddTime(0.5f);
    CharacterController().FlagNoReaction().DecCounter();

    return CCounterState::OnEnd();
};


//
// *********************************************************************************
//


C090UltimateShredderAI::CTripleAttackState::CTripleAttackState(void)
: Utils6034::IBaseAIState(AI_STATE_ATTACK_TRIPLE)
, m_attackNum(-1)
{
    ;
};


/*virtual*/ C090UltimateShredderAI::CTripleAttackState::STARTRESULT
C090UltimateShredderAI::CTripleAttackState::OnStart(void) /*override*/
{
    m_attackNum = 0;
    return STARTRESULT_END;
};


/*virtual*/ int32
C090UltimateShredderAI::CTripleAttackState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C090UltimateShredderAI::CTripleAttackState::RUNRESULT
C090UltimateShredderAI::CTripleAttackState::OnRun(void) /*override*/
{
    ++m_attackNum;

    int32 iTargetNo = TargetManager().GetTargetNo();
    CharacterController().DispatchAttack(ORDERTYPE_ATTACK_CHARGE, iTargetNo);

    return (m_attackNum >= 3 ? RUNRESULT_END : RUNRESULT_CONTINUE);
};


//
// *********************************************************************************
//


C090UltimateShredderAI::CElecState::CElecState(Utils6034::CFixTimer* pElecTimer)
: Utils6034::IBaseAIState(AI_STATE_ELEC)
, m_pElecTimer(pElecTimer)
{
    ;
};


/*virtual*/ C090UltimateShredderAI::CElecState::STARTRESULT
C090UltimateShredderAI::CElecState::OnStart(void) /*override*/
{
    return STARTRESULT_END;
};


/*virtual*/ int32 C090UltimateShredderAI::CElecState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C090UltimateShredderAI::CElecState::RUNRESULT
C090UltimateShredderAI::CElecState::OnRun(void) /*override*/
{
    TargetManager().Update();

    int32 iTargetNo = TargetManager().GetTargetNo();

    if (m_pElecTimer->IsAlarm())
    {
        m_pElecTimer->Reset();
        CharacterController().DispatchAttack(ORDERTYPE_ATTACK_ELEC, iTargetNo);
    }
    else
    {
        CharacterController().DispatchAttack(ORDERTYPE_ATTACK_THUNDER, iTargetNo);
    };

    return RUNRESULT_END;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator*
C090UltimateShredderAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C090UltimateShredderAI(pEnemyChr);
};


C090UltimateShredderAI::C090UltimateShredderAI(CEnemyCharacter* pEnemyChr)
: Utils6034::IBaseAIModerator(pEnemyChr, &m_actionDispatcher, AI_STATE_MAX)
, m_actionDispatcher()
, m_timerElecAttackWait(0.0f)
{
    ;
};


/*virtual*/ C090UltimateShredderAI::~C090UltimateShredderAI(void)
{
    ;
};


/*virtual*/ void C090UltimateShredderAI::Run(void) /*override*/
{
    m_timerElecAttackWait.Update();

    if (Utils6034::IsDownState(EnemyCharacter().GetStatus()))
    {
        MessageStackAIStateContext().Cleanup();
        MessageStackAIStateContext().ChangeState(AI_STATE_COUNTER_DOWN);
    };

    if (CharacterController().IsEnableChangeAction())
    {
        MessageStackAIStateContext().RunState();
        CharacterController().Update();
    };
};


/*virtual*/ void C090UltimateShredderAI::NotifyDamage(int32 iAmount) /*override*/
{
    MessageStackAIStateContext().Cleanup();
    MessageStackAIStateContext().ChangeState(AI_STATE_COUNTER_DAMAGED);
};


/*virtual*/ void C090UltimateShredderAI::NotifyGuard(void) /*override*/
{
    ;
};


/*virtual*/ void C090UltimateShredderAI::OnInitialize(void) /*override*/
{
    /* attach ai state units */
    Utils6034::IAIState* pAIState = nullptr;

    Utils6034::CWaitState* pWaitAIState = new Utils6034::CWaitState(AI_STATE_WAIT);
    MessageStackAIStateContext().RegistAIState(pWaitAIState);
    MessageStackAIStateContext().SetReceiver(pWaitAIState);

    Utils6034::CNormalAttackState* pAttackAIState = new Utils6034::CNormalAttackState(AI_STATE_ATTACK);
    MessageStackAIStateContext().RegistAIState(pAttackAIState);
    MessageStackAIStateContext().SetReceiver(pAttackAIState);

    pAIState = new C090UltimateShredderAI::CThinkingState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C090UltimateShredderAI::CTripleAttackState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C090UltimateShredderAI::CAvoidState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C090UltimateShredderAI::CCounterState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseCounterDownState(AI_STATE_COUNTER_DOWN);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseDamagedCounterState(AI_STATE_COUNTER_DAMAGED);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseGuardState(AI_STATE_GUARD);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C090UltimateShredderAI::CElecState(&m_timerElecAttackWait);
    MessageStackAIStateContext().RegistAIState(pAIState);

    /* start ai state */
    MessageStackAIStateContext().SetDefaultState(AI_STATE_THINK);
    MessageStackAIStateContext().StartDefaultState();

    /* init & start timers */
    m_timerElecAttackWait.SetAlarmTime(GetElecWaitTime());
    m_timerElecAttackWait.Start();
};


float C090UltimateShredderAI::GetElecWaitTime(void) const
{
    float afElecWaitTime[] = { 15.0f, 8.0f, 4.0f };
    static_assert(COUNT_OF(afElecWaitTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afElecWaitTime[difficulty];
};