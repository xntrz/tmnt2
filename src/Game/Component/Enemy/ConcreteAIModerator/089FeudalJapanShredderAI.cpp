#include "089FeudalJapanShredderAI.hpp"

#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/FenceGimmickManager.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Map/MapCamera.hpp"

#ifdef _DEBUG
#include "Game/System/Misc/DebugShape.hpp"
#endif /* _DEBUG */


static RwV3d s_vecDefenceCircleCenter = { 1.0f, 0.0f, -5.0f };
static float s_f1stDefenceRadiusSQ = 110.25f;
static float s_f2ndDefenceRadiusSQ = 196.0f;
static float s_fAttackNearDistanceSQ = 2.25f;


template<class TState, class TRecords, size_t iRecordNum>
inline static Utils6034::CProbabilityTable::CListData
SetListData(TState state, TRecords(&aRecords)[iRecordNum])
{
    Utils6034::CProbabilityTable::CListData listData;

    listData.iListIndex  = C089FeudalJapanShredderAI::CActionTable::GetStateHandle(state);
    listData.aRecords    = aRecords;
    listData.iRecordsNum = iRecordNum;

    return listData;
};


//
// *********************************************************************************
//


/*static*/ int32
C089FeudalJapanShredderAI::CActionTable::GetListNum(void)
{
    return (DISTANCE_STATE_MAX + SPECIAL_STATE_MAX + DEFENCE_STATE_MAX);
};


/*static*/ int32
C089FeudalJapanShredderAI::CActionTable::GetStateHandle(DISTANCE_STATE state)
{
    if (state >= DISTANCE_STATE_MAX)
        state = DISTANCE_STATE_MIN;

    return (state);
};


/*static*/ int32
C089FeudalJapanShredderAI::CActionTable::GetStateHandle(SPECIAL_STATE state)
{
    if (state >= SPECIAL_STATE_MAX)
        state = SPECIAL_STATE_MIN;

    return (DISTANCE_STATE_MAX + state);
};


/*static*/ int32
C089FeudalJapanShredderAI::CActionTable::GetStateHandle(DEFENCE_STATE state)
{
    if (state >= DEFENCE_STATE_MAX)
        state = DEFENCE_STATE_MIN;

    return (DISTANCE_STATE_MAX + SPECIAL_STATE_MAX + state);
};


//
// *********************************************************************************
//


C089FeudalJapanShredderAI::CActionDispatcher::CActionDispatcher(void)
: Utils6034::IActionDispatcher(CActionTable::GetListNum())
{
    ;
};


/*virtual*/ C089FeudalJapanShredderAI::CActionDispatcher::~CActionDispatcher(void)
{
    ;
};


/*virtual*/ bool
C089FeudalJapanShredderAI::CActionDispatcher::DispatchNormalAttack(int32 iTargetNo,
                                                                   float fTargetDistanceSQ) /*override*/
{
    DISTANCE_STATE distanceState = DISTANCE_STATE_MAX;
    if (fTargetDistanceSQ < s_fAttackNearDistanceSQ)
        distanceState = DISTANCE_STATE_NEAR;
    else
        distanceState = DISTANCE_STATE_MIDDLE;

    int32 iStateHandle = CActionTable::GetStateHandle(distanceState);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C089FeudalJapanShredderAI::CActionDispatcher::DispatchDamagedAction(int32 iTargetNo,
                                                                    float fTargetDistanceSQ) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(SPECIAL_STATE_AVOID);
    return DispatchAction(iStateHandle, iTargetNo);
};


/*virtual*/ bool
C089FeudalJapanShredderAI::CActionDispatcher::DispatchDefence(void) /*override*/
{
    int32 iStateHandle = CActionTable::GetStateHandle(DEFENCE_STATE_GUARD);
    return DispatchAction(iStateHandle, 0);
};


/*virtual*/ int32
C089FeudalJapanShredderAI::CActionDispatcher::CalcMoveProbability(void) /*override*/
{
    return AI_STATE_MOVE_WALK;
};


/*virtual*/ void
C089FeudalJapanShredderAI::CActionDispatcher::OnInitialize(void) /*override*/
{
    /*
     *  probability for DISTANCE
     */
    Utils6034::CProbabilityRecord aProbabilityDistNear[] = { { ACTION_TYPE_ATTACK_THROW,     { 55, 55, 45 } },
                                                             { ACTION_TYPE_ATTACK_TENGU,     { 45, 45, 55 } }, };

    Utils6034::CProbabilityRecord aProbabilityDistMiddle[] = { { ACTION_TYPE_ATTACK_ELEC,    { 60, 60, 50 } },
                                                               { ACTION_TYPE_ATTACK_TENGU,   { 40, 40, 50 } }, };

    /*
     *  probability for SPECIAL
     */
    Utils6034::CProbabilityRecord aProbabilityAvoid[] = { { ACTION_TYPE_DEFENCE_AVOID,  { 50, 90, 100 } },
                                                          { ACTION_TYPE_NOTHING,        { 50,  0,   0 } }, };

    /*
     *  probability for DEFENCE
     */
    Utils6034::CProbabilityRecord aProbabilityGuard[] = { { ACTION_TYPE_DEFENCE_GUARD,    { 40, 40, 40 } },
                                                          { ACTION_TYPE_DEFENCE_AVOID,    { 10, 20, 30 } },
                                                          { ACTION_TYPE_NOTHING,          { 50, 40, 30 } }, };
   
    /*
     *  init probability list
     */
    Utils6034::CProbabilityTable::CListData aListData[] =
    {
        SetListData(DISTANCE_STATE_NEAR,    aProbabilityDistNear),
        SetListData(DISTANCE_STATE_MIDDLE,  aProbabilityDistMiddle),
        SetListData(DEFENCE_STATE_GUARD,    aProbabilityGuard),
        SetListData(SPECIAL_STATE_AVOID,    aProbabilityAvoid),
    };

    Utils6034::CProbabilityTable::CListData* apListData[COUNT_OF(aListData)];
    for (int32 i = 0; i < COUNT_OF(aListData); ++i)
        apListData[i] = &aListData[i];

    m_probabilityTable.CopyListDataArray(apListData, COUNT_OF(apListData));
};


bool
C089FeudalJapanShredderAI::CActionDispatcher::DispatchEscape(void)
{
    return PushState(ACTION_TYPE_ESCAPE, 0);
};


bool
C089FeudalJapanShredderAI::CActionDispatcher::DispatchAction(int32 iStateHandle,
                                                             int32 iTargetNo)
{
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    int8 iActionType = m_probabilityTable.CalcProbability(iStateHandle, difficulty);

    return PushState(iActionType, iTargetNo);
};


bool
C089FeudalJapanShredderAI::CActionDispatcher::PushState(int32 iActionType,
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
        
    case ACTION_TYPE_ESCAPE:
        {
            MessageStackAIStateContext().PushMessage(AI_STATE_ESCAPE);
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
C089FeudalJapanShredderAI::CActionDispatcher::GetAttackType(int32 iActionType) const
{
    switch (iActionType)
    {
    case ACTION_TYPE_ATTACK_TENGU:
        return ORDERTYPE_ATTACK_TENGU;

    case ACTION_TYPE_ATTACK_CHARGE:
        return ORDERTYPE_ATTACK_CHARGE;

    case ACTION_TYPE_ATTACK_ELEC:
        return ORDERTYPE_ATTACK_ELEC;

    case ACTION_TYPE_ATTACK_THROW:
        return ORDERTYPE_ATTACK_THROW;

    default:
        ASSERT(false);
        break;
    };

    return ORDERTYPE_ATTACK_TENGU;
};


float
C089FeudalJapanShredderAI::CActionDispatcher::GetWaitTime(void) const
{
    float afWaitTime[] = { 1.5f, 0.6f, 0.0f };
    static_assert(COUNT_OF(afWaitTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afWaitTime[difficulty];
};


//
// *********************************************************************************
//


C089FeudalJapanShredderAI::CWalkDispatcher::CWalkDispatcher(int32 iStateHandle, C089FeudalJapanShredderAI* pShredderAI)
: Utils6034::CWalkDispatcher(iStateHandle)
, m_pShredderAI(pShredderAI)
{
    ;
};


/*virtual*/ C089FeudalJapanShredderAI::CWalkDispatcher::RUNRESULT
C089FeudalJapanShredderAI::CWalkDispatcher::OnRun(void) /*override*/
{
    TargetManager().Update();

    if (!m_bPauseTimer)
        m_moveTimer.Update();

    m_bPauseTimer = false;

    RwV3d vecTargetPos = TargetManager().GetTargetPos();
    if (!m_pShredderAI->IsOutsideDefenceRadius(&vecTargetPos))
    {
        int32 iTargetNo = TargetManager().GetTargetNo();
        CharacterController().DispatchWalkTarget(iTargetNo);
    };

    if (m_moveTimer.IsAlarm())
        return RUNRESULT_END;

    float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
    if (fTargetDistSQ <= m_fMinTargetDistSQ)
        return RUNRESULT_END;

    return RUNRESULT_CONTINUE;
};


//
// *********************************************************************************
//


C089FeudalJapanShredderAI::CThinkingState::CThinkingState(C089FeudalJapanShredderAI* pShredderAI,
                                                          CActionDispatcher* pActionDispatcher)
: Utils6034::CBaseThinkingState(AI_STATE_THINK, AI_STATE_MOVE_MAX)
, m_pShredderAI(pShredderAI)
, m_timerFenceWait(0.0f)
, m_pActionDispatcher(pActionDispatcher)
{
    /* init move dispatcher */
    Utils6034::IAIState* pAIState = nullptr;

    pAIState = new CWalkDispatcher(AI_STATE_MOVE_WALK, pShredderAI);
    m_moveStateCtx.RegistAIState(pAIState);

    m_moveStateCtx.SetDefaultState(AI_STATE_MOVE_WALK);
    m_moveStateCtx.StartDefaultState();

    /* init fence timer */
    m_timerFenceWait.SetAlarmTime(GetFenceWaitTime());
    m_timerFenceWait.Start();
};


/*virtual*/ C089FeudalJapanShredderAI::CThinkingState::RUNRESULT
C089FeudalJapanShredderAI::CThinkingState::OnRun(void) /*override*/
{
    m_timerFenceWait.Update();

    /*
     *  TODO: Updating target manager here like in retail game but this brokes guard.
     *
     *        Guard will triger only if player starts attack - there is 2 target manager update call
     *        in one frame that broke trigger state in player state watcher - first below, second in
     *        base class Utils6034::CBaseThinkingState::OnRun.
     *
     *        So updating it secondly in base class will not see attack trigger because it updates here.
     */
    TargetManager().Update();

    if (TargetManager().IsTargetSeeMe())
    {
        if (m_pShredderAI->IsOutsideDefenceRadius())
        {
            if (m_pShredderAI->IsEscapeReady())
            {
                m_pActionDispatcher->DispatchEscape();
                return RUNRESULT_END;
            };            
        }
        else
        {
            m_pShredderAI->SetEscapeReady(true);
        };

		RUNRESULT result = Utils6034::CBaseThinkingState::OnRun();
        if ((result == RUNRESULT_CONTINUE) && CheckFenceAttack())
            return RUNRESULT_END;    

        return result;
    };

    if (!m_pShredderAI->IsOutsideDefenceRadius())
    {
        RwV3d vecTargetPos = TargetManager().GetTargetPos();
        CharacterController().DispatchWalkPos(&vecTargetPos);
    };

    return RUNRESULT_END;
};


/*virtual*/ bool
C089FeudalJapanShredderAI::CThinkingState::OnAllTargetDown(void) /*override*/
{
    return false;
};


/*virtual*/ bool
C089FeudalJapanShredderAI::CThinkingState::OnTargetAttack(int32 iTargetNo) /*override*/
{
    if (!ActionDispatcher().DispatchDefence())
        return true;

    return false;
};


/*virtual*/ bool
C089FeudalJapanShredderAI::CThinkingState::OnEndMove(void) /*override*/
{
    int32 iTargetNo = TargetManager().GetTargetNo();
    float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();

    if (!ActionDispatcher().DispatchNormalAttack(iTargetNo, fTargetDistSQ))
        return true;

    return false;
};


bool C089FeudalJapanShredderAI::CThinkingState::CheckFenceAttack(void)
{
    int32 iLastAttackerPlayerNo = CFenceGimmickManager::GetLastAttacker();
    if (iLastAttackerPlayerNo == -1)
        return false;

    const CFenceGimmickManager::ATTACKERINFO* pAttackerInfo = CFenceGimmickManager::GetInfo(iLastAttackerPlayerNo);
    ASSERT(pAttackerInfo != nullptr);

    float fNowTime = CGameProperty::GetTotalElapsedTime();
    float fElapsedTimeSinceLastAttack = (fNowTime - pAttackerInfo->m_fAttackTime);

    if ((fElapsedTimeSinceLastAttack >= 0.2f) || (!m_timerFenceWait.IsAlarm()))
        return false;

    m_timerFenceWait.Reset();

    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(iLastAttackerPlayerNo);
    if (pPlrChr)
    {
        RwV3d vecPlrPos = Math::VECTOR3_ZERO;
        pPlrChr->GetPosition(&vecPlrPos);

        RwV3d vecMyPos = Math::VECTOR3_ZERO;
        AIModerator().EnemyCharacter().Compositor().GetPosition(&vecMyPos);

        RwV3d vecDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDist, &vecMyPos, &vecPlrPos);

        /* TODO this is like in retail game but DispatchNormalAttack accepts only squared dist */
        float fDist = CEnemyUtils::GetDistance(&AIModerator().EnemyCharacter().Compositor(), &vecPlrPos);

        ActionDispatcher().DispatchNormalAttack(iLastAttackerPlayerNo, fDist);
    };

    return true;
};


float C089FeudalJapanShredderAI::CThinkingState::GetFenceWaitTime(void) const
{
    float afFenceWaitTime[] = { 15.0f, 10.0f, 5.0f };
    static_assert(COUNT_OF(afFenceWaitTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afFenceWaitTime[difficulty];
};


//
// *********************************************************************************
//


C089FeudalJapanShredderAI::CAvoidState::CAvoidState(void)
: Utils6034::IBaseAIState(AI_STATE_AVOID)
, m_step(-1)
{
    ;
};


/*virtual*/ C089FeudalJapanShredderAI::CAvoidState::STARTRESULT
C089FeudalJapanShredderAI::CAvoidState::OnStart(void) /*override*/
{
    m_step = 0;
    CharacterController().FlagNoReaction().IncCounter();
    return STARTRESULT_END;
};


/*virtual*/ int32
C089FeudalJapanShredderAI::CAvoidState::OnEnd(void) /*override*/
{
    CharacterController().FlagNoReaction().AddTime(1.0f);
    CharacterController().FlagNoReaction().DecCounter();
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C089FeudalJapanShredderAI::CAvoidState::RUNRESULT
C089FeudalJapanShredderAI::CAvoidState::OnRun(void) /*override*/
{
    switch (m_step++)
    {
    case 0:
        CharacterController().DispatchDashPos(&s_vecDefenceCircleCenter);
        break;

    case 1:
        {
            RwV3d vecTargetPos = TargetManager().GetTargetPos();

            RwV3d vecMyPos = Math::VECTOR3_ZERO;
            AIModerator().EnemyCharacter().Compositor().GetPosition(&vecMyPos);

            RwV3d vec = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vec, &vecTargetPos, &vecMyPos);

            AIModerator().EnemyCharacter().Compositor().SetDirectionFromVector(&vec);

            float fTargetDistSQ = TargetManager().GetTargetDistanceSQ();
            int32 iTargetNo = TargetManager().GetTargetNo();

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


C089FeudalJapanShredderAI::CEscapeState::CEscapeState(C089FeudalJapanShredderAI* pShredderAI)
: Utils6034::IBaseAIState(AI_STATE_ESCAPE)
, m_pShredderAI(pShredderAI)
, m_step(-1)
{
    ;
};


/*virtual*/ C089FeudalJapanShredderAI::CEscapeState::STARTRESULT
C089FeudalJapanShredderAI::CEscapeState::OnStart(void) /*override*/
{
    m_step = 0;    
    CharacterController().FlagNoReaction().IncCounter();
    return STARTRESULT_END;
};


/*virtual*/ int32
C089FeudalJapanShredderAI::CEscapeState::OnEnd(void) /*override*/
{
    CharacterController().FlagNoReaction().DecCounter();
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C089FeudalJapanShredderAI::CEscapeState::RUNRESULT
C089FeudalJapanShredderAI::CEscapeState::OnRun(void) /*override*/
{
    switch (m_step++)
    {
    case 0:
        CharacterController().DispatchAttack(ORDERTYPE_ATTACK_CHARGE, &s_vecDefenceCircleCenter);
        break;

    case 1:
        CharacterController().DispatchDashPos(&s_vecDefenceCircleCenter);
        break;

    case 2:
        {
            int32 iTargetNo = TargetManager().GetTargetNo();
            CharacterController().DispatchAttack(ORDERTYPE_ATTACK_ELEC, iTargetNo);

            if (m_pShredderAI->IsOutsideDefenceRadius())
                m_pShredderAI->SetEscapeReady(false);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return (m_step > 2 ? RUNRESULT_END : RUNRESULT_CONTINUE);
};


//
// *********************************************************************************
//


C089FeudalJapanShredderAI::CElecState::CElecState(Utils6034::CFixTimer* pElecTimer)
: Utils6034::IBaseAIState(AI_STATE_ELEC)
, m_pElecTimer(pElecTimer)
{
    ;
};


/*virtual*/ C089FeudalJapanShredderAI::CElecState::STARTRESULT
C089FeudalJapanShredderAI::CElecState::OnStart(void) /*override*/
{
    return STARTRESULT_END;
};


/*virtual*/ int32 C089FeudalJapanShredderAI::CElecState::OnEnd(void) /*override*/
{
    return FROMSTACK_STATE_HANDLE;
};


/*virtual*/ C089FeudalJapanShredderAI::CElecState::RUNRESULT
C089FeudalJapanShredderAI::CElecState::OnRun(void) /*override*/
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
        CharacterController().DispatchAttack(ORDERTYPE_ATTACK_TENGU, iTargetNo);
    };

    return RUNRESULT_END;
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* C089FeudalJapanShredderAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C089FeudalJapanShredderAI(pEnemyChr);
};


C089FeudalJapanShredderAI::C089FeudalJapanShredderAI(CEnemyCharacter* pEnemyChr)
: Utils6034::IBaseAIModerator(pEnemyChr, &m_actionDispatcher, AI_STATE_MAX)
, m_actionDispatcher()
, m_timerDefenceRadiusAdvance((60.0f * 5.0f)) // 5 min
, m_fCurrentDefenceRadiusSQ(s_f1stDefenceRadiusSQ)
, m_timerElecAttackWait(GetElecWaitTime())
, m_bEscapeReady(true)
{
    ;
};


/*virtual*/ C089FeudalJapanShredderAI::~C089FeudalJapanShredderAI(void)
{
    ;
};


/*virtual*/ void C089FeudalJapanShredderAI::Run(void) /*override*/
{
    m_timerElecAttackWait.Update();
    m_timerDefenceRadiusAdvance.Update();

    if (m_timerDefenceRadiusAdvance.IsAlarm())
    {
        m_fCurrentDefenceRadiusSQ = s_f2ndDefenceRadiusSQ;
        m_timerDefenceRadiusAdvance.Reset();
    };

    if (Utils6034::IsDownState(EnemyCharacter().GetStatus()))
        MessageStackAIStateContext().ChangeState(AI_STATE_COUNTER_DOWN);

    if (CharacterController().IsEnableChangeAction())
    {
        MessageStackAIStateContext().RunState();
        CharacterController().Update();
    };
};


/*virtual*/ void C089FeudalJapanShredderAI::NotifyDamage(int32 iAmount) /*override*/
{
    MessageStackAIStateContext().Cleanup();
    MessageStackAIStateContext().ChangeState(AI_STATE_COUNTER_DAMAGED);
};


/*virtual*/ void C089FeudalJapanShredderAI::NotifyGuard(void) /*override*/
{
    ;
};


/*virtual*/ void C089FeudalJapanShredderAI::OnInitialize(void) /*override*/
{
    /* attach ai state units */
    Utils6034::IAIState* pAIState = nullptr;

    Utils6034::CWaitState* pWaitAIState = new Utils6034::CWaitState(AI_STATE_WAIT);
    MessageStackAIStateContext().RegistAIState(pWaitAIState);
    MessageStackAIStateContext().SetReceiver(pWaitAIState);

    Utils6034::CNormalAttackState* pAttackAIState = new Utils6034::CNormalAttackState(AI_STATE_ATTACK);
    MessageStackAIStateContext().RegistAIState(pAttackAIState);
    MessageStackAIStateContext().SetReceiver(pAttackAIState);

    pAIState = new C089FeudalJapanShredderAI::CThinkingState(this, &m_actionDispatcher);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C089FeudalJapanShredderAI::CAvoidState;
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C089FeudalJapanShredderAI::CEscapeState(this);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseCounterDownState(AI_STATE_COUNTER_DOWN);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseDamagedCounterState(AI_STATE_COUNTER_DAMAGED);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new Utils6034::CBaseGuardState(AI_STATE_GUARD);
    MessageStackAIStateContext().RegistAIState(pAIState);

    pAIState = new C089FeudalJapanShredderAI::CElecState(&m_timerElecAttackWait);
    MessageStackAIStateContext().RegistAIState(pAIState);

    /* start ai state */
    MessageStackAIStateContext().SetDefaultState(AI_STATE_THINK);
    MessageStackAIStateContext().StartDefaultState();

    /* start timers */
    m_timerElecAttackWait.Start();
    m_timerDefenceRadiusAdvance.Start();
};


bool C089FeudalJapanShredderAI::IsOutsideDefenceRadius(void) const
{
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetPosition(&vecMyPos);

    return IsOutsideDefenceRadius(&vecMyPos);
};


bool C089FeudalJapanShredderAI::IsOutsideDefenceRadius(const RwV3d* pvecPos) const
{
    RwV3d vecToDefenceCircleCenterDist = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecToDefenceCircleCenterDist, &s_vecDefenceCircleCenter, pvecPos);

    float fDistSQ = Math::Vec3_Dot(&vecToDefenceCircleCenterDist,
                                   &vecToDefenceCircleCenterDist);

    if (fDistSQ >= m_fCurrentDefenceRadiusSQ)
        return true;

    return false;
};


bool C089FeudalJapanShredderAI::IsEscapeReady(void) const
{
    return m_bEscapeReady;
};


void C089FeudalJapanShredderAI::SetEscapeReady(bool bInside)
{
    m_bEscapeReady = bInside;
};


float C089FeudalJapanShredderAI::GetElecWaitTime(void) const
{
    float afElecWaitTime[] = { 15.0f, 10.0f, 5.0f };
    static_assert(COUNT_OF(afElecWaitTime) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return afElecWaitTime[difficulty];
};