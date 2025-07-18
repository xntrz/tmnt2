#include "074TraximusAI.hpp"

#include "Game/Component/Enemy/ConcreteEnemyCharacter/074Traximus.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


static float s_fAttackNearDistanceSQ = 12.25f;
static float s_fAttackMiddleDistanceSQ = 49.0f;


template<class TState, class TRecords, size_t iRecordNum>
inline static Utils6034::CProbabilityTable::CListData
SetListData(TState state, TRecords(&aRecords)[iRecordNum])
{
    Utils6034::CProbabilityTable::CListData listData;

    listData.iListIndex = C074TraximusAI::CActionTable::GetStateHandle(state);
    listData.aRecords = aRecords;
    listData.iRecordsNum = iRecordNum;

    return listData;
};


//
// *********************************************************************************
//


/*static*/ int32 C074TraximusAI::CActionTable::GetListNum(void)
{
    return (DISTANCE_STATE_MAX + SPECIAL_STATE_MAX + DEFENCE_STATE_MAX);
};


/*static*/ int32 C074TraximusAI::CActionTable::GetStateHandle(DISTANCE_STATE state)
{
    if (state >= DISTANCE_STATE_MAX)
        state = DISTANCE_STATE_MIN;

    return state;
};


/*static*/ int32 C074TraximusAI::CActionTable::GetStateHandle(SPECIAL_STATE state)
{
    if (state >= SPECIAL_STATE_MAX)
        state = SPECIAL_STATE_MIN;

    return (DISTANCE_STATE_MAX + state);
};


/*static*/ int32 C074TraximusAI::CActionTable::GetStateHandle(DEFENCE_STATE state)
{
    if (state >= DEFENCE_STATE_MAX)
        state = DEFENCE_STATE_MIN;

    return (DISTANCE_STATE_MAX + SPECIAL_STATE_MAX + state);
};


//
// *********************************************************************************
//


C074TraximusAI::CActionDispatcher::CActionDispatcher(void)
: Utils6034::IActionDispatcher(CActionTable::GetListNum())
{
    ;
};


/*virtual*/ C074TraximusAI::CActionDispatcher::~CActionDispatcher(void)
{
    ;
};


/*virtual*/ bool
C074TraximusAI::CActionDispatcher::DispatchNormalAttack(int32 iTargetNo,
                                                        float fTargetDistanceSQ) /*override*/
{
    DISTANCE_STATE distanceState = DISTANCE_STATE_MAX;
    if (fTargetDistanceSQ < s_fAttackNearDistanceSQ)
        distanceState = DISTANCE_STATE_NEAR;
    else if (fTargetDistanceSQ < s_fAttackMiddleDistanceSQ)
        distanceState = DISTANCE_STATE_MIDDLE;
    else
        distanceState = DISTANCE_STATE_LONG;

    int32 iStateHandle = CActionTable::GetStateHandle(distanceState);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C074TraximusAI::CActionDispatcher::DispatchCrowdCounterAttack(int32 iTargetNo) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_CROWD_COUNTER);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C074TraximusAI::CActionDispatcher::DispatchDamagedAction(int32 iTargetNo,
                                                         float fTargetDistanceSQ) /*override*/
{

    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_DAMAGED_COUNTER);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool C074TraximusAI::CActionDispatcher::DispatchDefence(void) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(DEFENCE_STATE_GUARRD);
    return DispatchAction(iStateHandle, 0);
};


/*virtual*/ int32 C074TraximusAI::CActionDispatcher::CalcMoveProbability(void) /*override*/
{
    return AI_STATE_MOVE_WALK;
};


/*virtual*/ void C074TraximusAI::CActionDispatcher::OnInitialize(void) /*override*/
{
    /*
     *  probability for DISTANCE
     */
    Utils6034::CProbabilityRecord aProbabilityDistNear[] = { { ACTION_TYPE_BACKSTEP_COUNTER, { 25, 30, 40 } },
                                                             { ACTION_TYPE_ATTACK_B,         { 25, 30, 30 } },
                                                             { ACTION_TYPE_ATTACK_AA,        { 25, 25, 15 } },
                                                             { ACTION_TYPE_ATTACK_THROW,     { 25, 15, 15 } }, };

    Utils6034::CProbabilityRecord aProbabilityDistMiddle[] = { { ACTION_TYPE_ATTACK_AA,      { 30, 30, 30 } },
                                                               { ACTION_TYPE_ATTACK_THROW,   { 30, 30, 30 } },
                                                               { ACTION_TYPE_ATTACK_RUN,     { 20, 20, 20 } },
                                                               { ACTION_TYPE_ATTACK_JUMP,    { 20, 20, 20 } }, };

    Utils6034::CProbabilityRecord aProbabilityDistLong[] = { { ACTION_TYPE_ATTACK_RUN,     { 55, 55, 55 } },
                                                             { ACTION_TYPE_ATTACK_MOVE,    { 45, 45, 45 } }, };


    /*
     *  probability for SPECIAL
     */
    Utils6034::CProbabilityRecord aProbabilityCrowdCounter[] = { { ACTION_TYPE_ATTACK_RUN,  { 55, 55, 55 } },
                                                                 { ACTION_TYPE_ATTACK_MOVE, { 45, 45, 45 } }, };


    Utils6034::CProbabilityRecord aProbabilityAddition[] = { { ACTION_TYPE_BACKSTEP,        { 75, 60, 40 } },
                                                             { ACTION_TYPE_ATTACK_THROW,    { 10, 20, 30 } },
                                                             { ACTION_TYPE_ATTACK_RUN,      { 15, 20, 30 } }, };

    Utils6034::CProbabilityRecord aProbabilityDamagedCounter[] = { { ACTION_TYPE_ATTACK_B,            { 15, 30, 40 } },
                                                                   { ACTION_TYPE_BACKSTEP_COUNTER,    { 55, 70, 60 } },
                                                                   { ACTION_TYPE_NOTHING,             { 30,  0,  0 } }, };

    Utils6034::CProbabilityRecord aProbabilityBackStep[] = { { ACTION_TYPE_ATTACK_AA,       { 10, 20, 20 } },
                                                             { ACTION_TYPE_ATTACK_JUMP,     { 10, 10, 10 } },
                                                             { ACTION_TYPE_ATTACK_B,        {  0, 15, 30 } },
                                                             { ACTION_TYPE_ATTACK_RUN,      {  0, 15, 20 } },
                                                             { ACTION_TYPE_ATTACK_THROW,    { 10, 20, 20 } },
                                                             { ACTION_TYPE_DEFENCE_GUARD,   { 60, 20,  0 } }, };

    /*
     *  probability on DEFENCE
     */
    Utils6034::CProbabilityRecord aProbabilityGuard[] = { { ACTION_TYPE_DEFENCE_GUARD,    { 40, 60, 80 } },
                                                          { ACTION_TYPE_BACKSTEP_COUNTER, {  0,  5, 20 } },
                                                          { ACTION_TYPE_NOTHING,          { 60, 35,  0 } }, };
   
    /*
     *  init probability list
     */
    Utils6034::CProbabilityTable::CListData aListData[] =
    {
        SetListData(DISTANCE_STATE_NEAR,           aProbabilityDistNear),
        SetListData(DISTANCE_STATE_MIDDLE,         aProbabilityDistMiddle),
        SetListData(DISTANCE_STATE_LONG,           aProbabilityDistLong),
        SetListData(SPECIAL_STATE_CROWD_COUNTER,   aProbabilityCrowdCounter),
        SetListData(SPECIAL_STATE_ADDITION,        aProbabilityAddition),
        SetListData(SPECIAL_STATE_DAMAGED_COUNTER, aProbabilityDamagedCounter),
        SetListData(SPECIAL_STATE_BACK_STEP,       aProbabilityBackStep),
        SetListData(DEFENCE_STATE_GUARRD,          aProbabilityGuard),
    };

    Utils6034::CProbabilityTable::CListData* apListData[COUNT_OF(aListData)];
    for (int32 i = 0; i < COUNT_OF(aListData); ++i)
        apListData[i] = &aListData[i];

    m_probabilityTable.CopyListDataArray(apListData, COUNT_OF(apListData));
};


/*virtual*/ bool C074TraximusAI::CActionDispatcher::DispatchStunAdditionAttack(int32 iTargetNo)
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_ADDITION);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool C074TraximusAI::CActionDispatcher::DispatchBackStepAttack(int32 iTargetNo)
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_BACK_STEP);
    return DispatchAction(iStateHandle, iTargetNo);
};


bool C074TraximusAI::CActionDispatcher::DispatchAction(int32 iStateHandle, int32 iTargetNo)
{
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    int8 iActionType = m_probabilityTable.CalcProbability(iStateHandle, difficulty);

    return PushState(iActionType, iTargetNo);
};


bool C074TraximusAI::CActionDispatcher::PushState(int32 iActionType, int32 iTargetNo)
{
    bool bResult = false;

    switch (iActionType)
    {
    case ACTION_TYPE_NOTHING:        
        break;
        
    case ACTION_TYPE_ATTACK_AA:
    case ACTION_TYPE_ATTACK_B:
    case ACTION_TYPE_ATTACK_THROW:
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
        
    case ACTION_TYPE_BACKSTEP:
        {
            Utils6034::CWaitData waitData;
            waitData.fWaitTime = GetWaitTime();

            MessageStackAIStateContext().PushMessage(AI_STATE_WAIT, waitData);
            MessageStackAIStateContext().PushMessage(AI_STATE_BACK_STEP);
            bResult = true;
        }
        break;

    case ACTION_TYPE_BACKSTEP_COUNTER:
        {
            Utils6034::CWaitData waitData;
            waitData.fWaitTime = GetWaitTime();

            MessageStackAIStateContext().PushMessage(AI_STATE_WAIT, waitData);
            MessageStackAIStateContext().PushMessage(AI_STATE_BACK_STEP_COUNTER);
            bResult = true;
        }
        break;
        
    case ACTION_TYPE_ATTACK_RUN:
    case ACTION_TYPE_ATTACK_MOVE:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_RUN_ATTACK);
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


int32 C074TraximusAI::CActionDispatcher::GetAttackType(int32 iActionType) const
{
    switch (iActionType)
    {
    case ACTION_TYPE_ATTACK_AA:
        return ORDERTYPE_ATTACK_AA;

    case ACTION_TYPE_ATTACK_B:
        return ORDERTYPE_ATTACK_B;

    case ACTION_TYPE_ATTACK_THROW:
        return ORDERTYPE_ATTACK_THROW;

    case ACTION_TYPE_ATTACK_JUMP:
        return ORDERTYPE_ATTACK_JUMP;

    default:
        ASSERT(false);
        break;
    };

    return ORDERTYPE_ATTACK_AA;
};


float C074TraximusAI::CActionDispatcher::GetWaitTime(void) const
{
    float afWaitTime[] = { 1.3f, 1.0f, 0.5f };
    static_assert(COUNT_OF(afWaitTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afWaitTime[difficulty];
};


//
// *********************************************************************************
//


C074TraximusAI::CThinkingState::CThinkingState(void)
: Utils6034::CBaseThinkingState(AI_STATE_THINK, AI_STATE_MOVE_MAX)
{
    Utils6034::IAIState* pAIState = nullptr;

    pAIState = new Utils6034::CWalkDispatcher(AI_STATE_MOVE_WALK);
    m_moveStateCtx.RegistAIState(pAIState);

    pAIState = new Utils6034::CRunDispatcher(AI_STATE_MOVE_RUN);
    m_moveStateCtx.RegistAIState(pAIState);

    m_moveStateCtx.SetDefaultState(AI_STATE_MOVE_WALK);
    m_moveStateCtx.StartDefaultState();
};


/*virtual*/ bool C074TraximusAI::CThinkingState::OnAllTargetDown(void) /*override*/
{
    return false;
};


/*virtual*/ bool C074TraximusAI::CThinkingState::OnTargetAttack(int32 iTargetNo) /*override*/
{
    if (!ActionDispatcher().DispatchDefence())
        return true;

    return false;
};


/*virtual*/ bool C074TraximusAI::CThinkingState::OnEndMove(void) /*override*/
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


C074TraximusAI::CWaitState::CWaitState(CActionDispatcher* pActionDispatcher)
: Utils6034::CWaitState(AI_STATE_WAIT)
, m_pActionDispatcher(pActionDispatcher)
{
    ;
};


/*virtual*/ C074TraximusAI::CWaitState::RUNRESULT
C074TraximusAI::CWaitState::OnRun(void) /*override*/
{
    TargetManager().Update();

    if (TargetManager().IsTargetStun())
    {
        int32 iTargetNo = TargetManager().GetTargetNo();
        m_pActionDispatcher->DispatchStunAdditionAttack(iTargetNo);

        return RUNRESULT_END;
    };

    return Utils6034::CWaitState::OnRun();
};


//
// *********************************************************************************
//


C074TraximusAI::CRunAttackState::CRunAttackState(void)
: Utils6034::IBaseAIState(AI_STATE_RUN_ATTACK)
, m_runTimer(0.0f)
, m_fRunTimeTarget(3.0f)
, m_fRunTimeContinue(1.0f)
, m_fRunTargetDistSQ(1.0f)
, m_step(-1)
{
    ;
};


/*virtual*/ C074TraximusAI::CRunAttackState::STARTRESULT
C074TraximusAI::CRunAttackState::OnStart(void) /*override*/
{
    m_step = 0;
    m_runTimer.SetAlarmTime(m_fRunTimeTarget);
    m_runTimer.Start();
    
    return STARTRESULT_END;
};


/*virtual*/ int32 C074TraximusAI::CRunAttackState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C074TraximusAI::CRunAttackState::RUNRESULT
C074TraximusAI::CRunAttackState::OnRun(void) /*override*/
{
    TargetManager().Update();

    m_runTimer.Update();

    switch (m_step)
    {
    case 0:
        {
            int32 iTargetNo = TargetManager().GetTargetNo();
            CharacterController().DispatchRunTarget(iTargetNo);

            float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
            if ((fTargetDistSQ < m_fRunTargetDistSQ) || m_runTimer.IsAlarm())
            {
                m_runTimer.SetAlarmTime(m_fRunTimeContinue);
                m_runTimer.Start();

                ++m_step;
            };
        }
        break;

    case 1:
        {
            CharacterController().DispatchRun();

            if (m_runTimer.IsAlarm())
                return RUNRESULT_END;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return RUNRESULT_CONTINUE;
};


//
// *********************************************************************************
//


C074TraximusAI::CRunMoveState::CRunMoveState(void)
: Utils6034::IBaseAIState(AI_STATE_RUN_MOVE)
{
    ;
};


/*virtual*/ C074TraximusAI::CRunMoveState::STARTRESULT
C074TraximusAI::CRunMoveState::OnStart(void) /*override*/
{
    return STARTRESULT_END;
};


/*virtual*/ int32 C074TraximusAI::CRunMoveState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C074TraximusAI::CRunMoveState::RUNRESULT
C074TraximusAI::CRunMoveState::OnRun(void) /*override*/
{
    return RUNRESULT_END;
};


//
// *********************************************************************************
//


C074TraximusAI::CBackStepJumpAttackState::CBackStepJumpAttackState(void)
: CBackStepJumpAttackState(AI_STATE_BACK_STEP)
{
    ;
};


C074TraximusAI::CBackStepJumpAttackState::CBackStepJumpAttackState(int32 iHandle)
: Utils6034::IBaseAIState(iHandle)
, m_step(-1)
, m_bFlagInvinsibilityChanged(false)
, m_bFlagNoReationChanged(false)
{
    ;
};


/*virtual*/ C074TraximusAI::CBackStepJumpAttackState::STARTRESULT
C074TraximusAI::CBackStepJumpAttackState::OnStart(void) /*override*/
{
    m_step = 0;
    m_bFlagNoReationChanged = false;
    m_bFlagInvinsibilityChanged = false;

    CharacterController().FlagInvincibility().IncCounter();

    return STARTRESULT_END;
};


/*virtual*/ int32 C074TraximusAI::CBackStepJumpAttackState::OnEnd(void) /*override*/
{
    if (!m_bFlagInvinsibilityChanged)
        CharacterController().FlagInvincibility().DecCounter();

    if (m_bFlagNoReationChanged)
        CharacterController().FlagNoReaction().DecCounter();

    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C074TraximusAI::CBackStepJumpAttackState::RUNRESULT
C074TraximusAI::CBackStepJumpAttackState::OnRun(void) /*override*/
{
    TargetManager().Update();

    switch (m_step++)
    {
    case 0:
        {
            int32 iTargetNo = TargetManager().GetTargetNo();
            CharacterController().DispatchMove(ORDERTYPE_MOVE_BACK_STEP, iTargetNo);
        }
        break;

    case 1:
        {
            CharacterController().FlagInvincibility().DecCounter();
            m_bFlagInvinsibilityChanged = true;

            CharacterController().FlagNoReaction().IncCounter();
            m_bFlagNoReationChanged = true;

            int32 iTargetNo = TargetManager().GetTargetNo();
            CharacterController().DispatchAttack(ORDERTYPE_ATTACK_JUMP, iTargetNo);
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


C074TraximusAI::CBackStepCounterState::CBackStepCounterState(CActionDispatcher* pActionDispatcher)
: CBackStepJumpAttackState(AI_STATE_BACK_STEP_COUNTER)
, m_pActionDispatcher(pActionDispatcher)
{
    ;
};


/*virtual*/ C074TraximusAI::CBackStepCounterState::RUNRESULT
C074TraximusAI::CBackStepCounterState::OnRun(void) /*override*/
{
    TargetManager().Update();

    switch (m_step++)
    {
    case 0:
        {
            int32 iTargetNo = TargetManager().GetTargetNo();
            CharacterController().DispatchMove(ORDERTYPE_MOVE_BACK_STEP, iTargetNo);
        }
        break;

    case 1:
        {
            int32 iTargetNo = TargetManager().GetTargetNo();
            m_pActionDispatcher->DispatchBackStepAttack(iTargetNo);

            CharacterController().FlagNoReaction().AddTime(1.0f);
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


/*static*/ CAIModerator* C074TraximusAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C074TraximusAI(pEnemyChr);
};


C074TraximusAI::C074TraximusAI(CEnemyCharacter* pEnemyChr)
: Utils6034::IBaseAIModerator(pEnemyChr, &m_actionDispatcher, AI_STATE_MAX)
, m_actionDispatcher()
{
    ;
};


/*virtual*/ C074TraximusAI::~C074TraximusAI(void)
{
    ;
};


/*virtual*/ void C074TraximusAI::Run(void) /*override*/
{
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


/*virtual*/ void C074TraximusAI::NotifyDamage(int32 iAmount) /*override*/
{
    MessageStackAIStateContext().Cleanup();
    MessageStackAIStateContext().ChangeState(AI_STATE_COUNTER_DAMAGED);
};


/*virtual*/ void C074TraximusAI::NotifyGuard(void) /*override*/
{
    ;
};


/*virtual*/ void C074TraximusAI::OnInitialize(void) /*override*/
{
    /* attach ai state units */
    Utils6034::IAIState* pAIState = nullptr;

    C074TraximusAI::CWaitState* pWaitAIState = new C074TraximusAI::CWaitState(&m_actionDispatcher);
    MessageStackAIStateContext().RegistAIState(pWaitAIState);
    MessageStackAIStateContext().SetReceiver(pWaitAIState);

    Utils6034::CNormalAttackState* pAttackAIState = new Utils6034::CNormalAttackState(AI_STATE_ATTACK);
    MessageStackAIStateContext().RegistAIState(pAttackAIState);
    MessageStackAIStateContext().SetReceiver(pAttackAIState);

    pAIState = new C074TraximusAI::CThinkingState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C074TraximusAI::CRunAttackState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C074TraximusAI::CRunMoveState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseCounterDownState(AI_STATE_COUNTER_DOWN);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseGuardState(AI_STATE_GUARD);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseDamagedCounterState(AI_STATE_COUNTER_DAMAGED);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C074TraximusAI::CBackStepJumpAttackState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C074TraximusAI::CBackStepCounterState(&m_actionDispatcher);
    MessageStackAIStateContext().RegistAIState(pAIState);

    /* start ai state */
    MessageStackAIStateContext().SetDefaultState(AI_STATE_THINK);
    MessageStackAIStateContext().StartDefaultState();
};