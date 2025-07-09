#include "088ShredderAI.hpp"

#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"

#ifdef _DEBUG
#include "Game/System/Misc/DebugShape.hpp"
#endif /* _DEBUG */


static const float s_fLineX = 4.6f;
static RwV3d s_vecMoveLineStart = { -(s_fLineX * 0.5f), 0.0f, -3.0f };
static RwV3d s_vecMoveLineCenter = { 0.0f, 0.0f, -5.0f };
static RwV3d s_vecMoveLineEnd = { (s_fLineX * 0.5f), 0.0f, -3.0f };


template<class TState, class TRecords, size_t iRecordNum>
inline static Utils6034::CProbabilityTable::CListData
SetListData(TState state, TRecords(&aRecords)[iRecordNum])
{
    Utils6034::CProbabilityTable::CListData listData;

    listData.iListIndex = C088ShredderAI::CActionTable::GetStateHandle(state);
    listData.aRecords = aRecords;
    listData.iRecordsNum = iRecordNum;

    return listData;
};


//
// *********************************************************************************
//


/*static*/ int32 C088ShredderAI::CActionTable::GetListNum(void)
{
    return (ATTACK_STATE_MAX + DEFENCE_STATE_MAX);
};


/*static*/ int32 C088ShredderAI::CActionTable::GetStateHandle(ATTACK_STATE state)
{
    if (state >= ATTACK_STATE_MAX)
        state = ATTACK_STATE_MIN;

    return state;
};


/*static*/ int32 C088ShredderAI::CActionTable::GetStateHandle(DEFENCE_STATE state)
{
    if (state >= DEFENCE_STATE_MAX)
        state = DEFENCE_STATE_MIN;

    return (ATTACK_STATE_MAX + state);
};



//
// *********************************************************************************
//


C088ShredderAI::CActionDispatcher::CActionDispatcher(void)
: Utils6034::IActionDispatcher(CActionTable::GetListNum())
{
    ;
};


/*virtual*/ C088ShredderAI::CActionDispatcher::~CActionDispatcher(void)
{
    ;
};


/*virtual*/ bool
C088ShredderAI::CActionDispatcher::DispatchNormalAttack(int32 iTargetNo, float fTargetDistanceSQ) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(ATTACK_STATE_ELEC);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C088ShredderAI::CActionDispatcher::DispatchDefence(void) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(DEFENCE_STATE_GUARRD);
    return DispatchAction(iStateHandle, 0);
};


/*virtual*/ int32 C088ShredderAI::CActionDispatcher::CalcMoveProbability(void) /*override*/
{
    return AI_STATE_MOVE_WALK;
};


/*virtual*/ void C088ShredderAI::CActionDispatcher::OnInitialize(void) /*override*/
{
    /*
     *  probability for ATTACK
     */
    Utils6034::CProbabilityRecord aProbabilityAttackElec[] = { { ACTION_TYPE_ATTACK_ELEC, { 100, 100, 100 } },
                                                               { ACTION_TYPE_NOTHING,     {   0,   0,   0 } }, };

    /*
     *  probability on DEFENCE
     */
    Utils6034::CProbabilityRecord aProbabilityDefenceGuard[] = { { ACTION_TYPE_DEFENCE_GUARD,    { 40, 50, 60 } },
                                                                 { ACTION_TYPE_NOTHING,          { 60, 50, 40 } }, };
   
    /*
     *  init probability list
     */
    Utils6034::CProbabilityTable::CListData aListData[] =
    {
        SetListData(ATTACK_STATE_ELEC,    aProbabilityAttackElec),
        SetListData(DEFENCE_STATE_GUARRD, aProbabilityDefenceGuard),
    };

    Utils6034::CProbabilityTable::CListData* apListData[COUNT_OF(aListData)];
    for (int32 i = 0; i < COUNT_OF(aListData); ++i)
        apListData[i] = &aListData[i];

    m_probabilityTable.CopyListDataArray(apListData, COUNT_OF(apListData));
};


bool C088ShredderAI::CActionDispatcher::DispatchAction(int32 iStateHandle, int32 iTargetNo)
{
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    int8 iActionType = m_probabilityTable.CalcProbability(iStateHandle, difficulty);

    return PushState(iActionType, iTargetNo);
};


bool C088ShredderAI::CActionDispatcher::PushState(int32 iActionType, int32 iTargetNo)
{
    bool bResult = false;

    switch (iActionType)
    {
    case ACTION_TYPE_NOTHING:
        break;

    case ACTION_TYPE_ATTACK_ELEC:
        {
            Utils6034::CNormalAttackData attackData;
            attackData.iAttackType = 0;
            attackData.iTargetNo = iTargetNo;

            MessageStackAIStateContext().PushMessage(AI_STATE_ATTACK, attackData);
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


//
// *********************************************************************************
//


C088ShredderAI::CThinkingState::CThinkingState(void)
: Utils6034::IBaseAIState(AI_STATE_THINK)
, m_timer(0.0f)
, m_step(-1)
, m_vecMovePos(Math::VECTOR3_ZERO)
{
    ;
};


/*virtual*/ C088ShredderAI::CThinkingState::STARTRESULT
C088ShredderAI::CThinkingState::OnStart(void) /*override*/
{    
    m_timer.SetAlarmTime(GetBeforeWaitTime());
    m_timer.Start();

    m_step = STEP_WAIT;

    return STARTRESULT_END;
};


/*virtual*/ int32 C088ShredderAI::CThinkingState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C088ShredderAI::CThinkingState::RUNRESULT
C088ShredderAI::CThinkingState::OnRun(void) /*override*/
{
    TargetManager().Update();

    if (TargetManager().PlayerStateWatcher().IsAllPlayerDownState())
    {
        m_timer.Pause();
        return RUNRESULT_CONTINUE;
    };

    m_timer.UnPause();

    if (DispatchMove())
        return RUNRESULT_CONTINUE;

    float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
    int32 iTargetNo = TargetManager().GetTargetNo();
    ActionDispatcher().DispatchNormalAttack(iTargetNo, fTargetDistSQ);

    return RUNRESULT_END;
};


bool C088ShredderAI::CThinkingState::DispatchMove(void)
{
    m_timer.Update();

    switch (m_step)
    {
    case STEP_WAIT:
        {
            if (!m_timer.IsAlarm())
                break;

            RwV3d vecTargetPos = TargetManager().GetTargetPos();

            float t = GetPositionParamT(vecTargetPos.x);
            t += Math::RandFloatRange(-0.1f, 0.1f);

            GetMovePosition(&m_vecMovePos, t);

            RwV3d vecMyPos = Math::VECTOR3_ZERO;
            AIModerator().EnemyCharacter().Compositor().GetPosition(&vecMyPos);

            float fDistSQ = GetToMovePosDistSQ(&vecMyPos);
            if (fDistSQ >= 1.0f)
            {
                m_timer.SetAlarmTime(2.0f);
                m_timer.Start();
                m_step = STEP_MOVE;
            }
            else
            {
                m_timer.SetAlarmTime(2.5f);
                m_timer.Start();
                m_step = STEP_MOVE_ENDED;
            };
        }
        break;

    case STEP_MOVE:
        {
            if (m_timer.IsAlarm())
            {
                m_timer.SetAlarmTime(0.5f);
                m_timer.Reset();
                m_step = STEP_MOVE_ENDED;
                break;
            };

            RwV3d vecMyPos = Math::VECTOR3_ZERO;
            AIModerator().EnemyCharacter().Compositor().GetPosition(&vecMyPos);

            float fDistSQ = GetToMovePosDistSQ(&vecMyPos);
            if (fDistSQ <= 0.1f)
            {
                int32 iTargetNo = TargetManager().GetTargetNo();
                CharacterController().DispatchTurnTarget(iTargetNo);
                break;
            };

            CharacterController().DispatchWalkPos(&m_vecMovePos);
        }
        break;

    case STEP_MOVE_ENDED:
        {
            if (m_timer.IsAlarm())
            {
                m_timer.SetAlarmTime(GetBeforeWaitTime());
                m_timer.Reset();
                return false;
            };

            int32 iTargetNo = TargetManager().GetTargetNo();
            CharacterController().DispatchTurnTarget(iTargetNo);
        }
        break;

    default:
        break;
    };

    return true;
};


float C088ShredderAI::CThinkingState::GetPositionParamT(float fTargetPosX) const
{
    float x = -fTargetPosX;

    if (x < s_vecMoveLineStart.x)
        x = s_vecMoveLineStart.x;
    else if (x > s_vecMoveLineEnd.x)
        x = s_vecMoveLineEnd.x;

    return ((x + (s_fLineX * 0.5f)) / s_fLineX);
};


void C088ShredderAI::CThinkingState::GetMovePosition(RwV3d* pvecMovePos, float t) const
{
    CEnemyUtils::BezierCalc(pvecMovePos,
                            &s_vecMoveLineStart,
                            &s_vecMoveLineCenter,
                            &s_vecMoveLineEnd,
                            Clamp(t, 0.0f, 1.0f));
};


float C088ShredderAI::CThinkingState::GetToMovePosDistSQ(const RwV3d* pvecPos) const
{
    RwV3d vecDist = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDist, &m_vecMovePos, pvecPos);

    float fDistSQ = Math::Vec3_Dot(&vecDist, &vecDist);
    return fDistSQ;
};


float C088ShredderAI::CThinkingState::GetBeforeWaitTime(void) const
{
    float afBeforeWaitTime[] = { 3.0f, 2.5f, 2.0f };
    static_assert(COUNT_OF(afBeforeWaitTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afBeforeWaitTime[difficulty];
};


//
// *********************************************************************************
//


C088ShredderAI::CDownState::CDownState(void)
: Utils6034::IBaseAIState(AI_STATE_DOWN)
{
    ;
};


/*virtual*/ C088ShredderAI::CDownState::STARTRESULT
C088ShredderAI::CDownState::OnStart(void) /*override*/
{
    MessageStackAIStateContext().Cleanup();
    return STARTRESULT_END;
};


/*virtual*/ int32 C088ShredderAI::CDownState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C088ShredderAI::CDownState::RUNRESULT
C088ShredderAI::CDownState::OnRun(void) /*override*/
{
    if (Utils6034::IsDownState(AIModerator().EnemyCharacter().GetStatus()))
        return RUNRESULT_CONTINUE;

    return RUNRESULT_END;
};


//
// *********************************************************************************
//


C088ShredderAI::CAttackState::CAttackState(void)
: Utils6034::CNormalAttackState(AI_STATE_ATTACK)
{
    ;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* C088ShredderAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C088ShredderAI(pEnemyChr);
};


C088ShredderAI::C088ShredderAI(CEnemyCharacter* pEnemyChr)
: Utils6034::IBaseAIModerator(pEnemyChr, &m_actionDispatcher, AI_STATE_MAX)
, m_actionDispatcher()
{

};


/*virtual*/ C088ShredderAI::~C088ShredderAI(void)
{
    ;
};


/*virtual*/ void C088ShredderAI::Run(void) /*override*/
{
    if (Utils6034::IsDownState(EnemyCharacter().GetStatus()))
        MessageStackAIStateContext().ChangeState(AI_STATE_DOWN);

    if (CharacterController().IsEnableChangeAction())
    {
        MessageStackAIStateContext().RunState();
        CharacterController().Update();
    };
};


/*virtual*/ void C088ShredderAI::NotifyDamage(int32 iAmount) /*override*/
{
    MessageStackAIStateContext().Cleanup();
};


/*virtual*/ void C088ShredderAI::NotifyGuard(void) /*override*/
{
    ;
};


/*virtual*/ void C088ShredderAI::OnInitialize(void) /*override*/
{
    /* attach ai state units */
    Utils6034::IAIState* pAIState = nullptr;

    C088ShredderAI::CAttackState* pAttackAIState = new C088ShredderAI::CAttackState;
    MessageStackAIStateContext().RegistAIState(pAttackAIState);
    MessageStackAIStateContext().SetReceiver(pAttackAIState);

    pAIState = new C088ShredderAI::CThinkingState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C088ShredderAI::CDownState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseGuardState(AI_STATE_GUARD); // NOTE: guard state is not used but have motions
    MessageStackAIStateContext().RegistAIState(pAIState);

    /* start ai state */
    MessageStackAIStateContext().SetDefaultState(AI_STATE_THINK);
    MessageStackAIStateContext().StartDefaultState();
};