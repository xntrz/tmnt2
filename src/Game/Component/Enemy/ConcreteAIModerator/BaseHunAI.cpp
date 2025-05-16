#include "BaseHunAI.hpp"

#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseHunChr.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


CBaseHunAI::CDecisionUnitMove::CDecisionUnitMove(CEnemyCharacter* pEnemyChr)
: m_pBaseHunChr(static_cast<CBaseHunChr*>(pEnemyChr))
{
    ;
};


/*virtual*/ CBaseHunAI::CDecisionUnitMove::RESULT
CBaseHunAI::CDecisionUnitMove::Update(void) /*override*/
{
    RESULT result = CBaseAI6045::CDecisionUnitMoveBoss::Update();
    if (result == RESULT_STOP)
        return result;

    if (EnemyChr().AIThinkOrder().OrderMove().m_iMoveType != BASEAI6045::ORDERTYPE_MOVE_RUN_NO)
        return result;

    if (m_pBaseHunChr->IsInBurstMode())
        return result;

    if (!m_pAIDecisionUnitCommonParameter->IsAttack())
        return result;

    int32 orderTargetNo = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(orderTargetNo);
    if (!pPlrChr)
        return result;

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecFootPosPlr);

    RwV3d vecDist = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDist, &vecFootPosPlr, &vecFootPosMe);

    float fMyDir = EnemyChr().Compositor().GetDirection();
    float fDirToPlr = std::atan2(vecDist.x, vecDist.z);
    float fDirDiff = fDirToPlr - fMyDir;
    fDirDiff = std::fabs(fDirDiff);

    if (fDirDiff <= MATH_DEG2RAD(7.2f))
    {
        float fDist = Math::Vec3_Length(&vecDist);

        float fJumpSpeedXZ = 11.0f; /* should be in sync with
                                       CBaseHunChr::CAttackFConnectFromRunStatusStatusObserver::SetJumpSetting */

        float fGravityFTS = 32.0f; /* feet per second */

        float fGravity = CGameProperty::GetGravity();
        float fDistJump = (fDist - std::fabs(fGravityFTS / (fGravity * 4.0f)) * fJumpSpeedXZ);
        fDistJump = std::fabs(fDistJump);

        if (fDistJump <= 0.2f)
        {
            if (IsSatisfyFrequency(m_frequencyTableNo))
            {
                m_pAIDecisionUnitCommonParameter->ChangeDicisionUnit(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_F_CONNECT_FROM_RUN);
                return RESULT_CONTINUE;
            };
        };
    };

    return result;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackA::CDecisionUnitAttackA(void)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_A)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_A;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackB::CDecisionUnitAttackB(CEnemyCharacter* pEnemyChr)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_B)
, m_pBaseHunChr(static_cast<CBaseHunChr*>(pEnemyChr))
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_B;
};


/*virtual*/ bool CBaseHunAI::CDecisionUnitAttackB::CheckTerm(void) /*override*/
{
    if (m_pBaseHunChr->IsInBurstMode())
        return false;

    if (!m_pAIDecisionUnitCommonParameter->IsAttack())
        return false;

    int32 numViewData = m_pAIDecisionUnitCommonParameter->GetViewDataNum();
    for (int32 i = 0; i < numViewData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = m_pAIDecisionUnitCommonParameter->GetViewData(i);

        bool bIsSatisfyDist = (nearerPlayerData.distance <= m_fDistanceCondition);
        bool bIsSatisfyAngle = IsSatisfyAngleCondition(&nearerPlayerData);

        if (bIsSatisfyDist && bIsSatisfyAngle)
        {
            if (!IsSatisfyFrequency(m_frequencyTableNo))
                return false;

            m_orderTargetNo = nearerPlayerData.no;
            return true;
        };
    };

    return false;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackC::CDecisionUnitAttackC(void)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_C)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_C;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackD::CDecisionUnitAttackD(void)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_D)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_D;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackDForEscape::CDecisionUnitAttackDForEscape(void)
: CBaseAI6045::CDecisionUnitAttackCounter(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_D_FOR_ESCAPE)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_D;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackF::CDecisionUnitAttackF(CEnemyCharacter* pEnemyChr)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_F)
, m_pBaseHunChr(static_cast<CBaseHunChr*>(pEnemyChr))
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_F;
};


/*virtual*/ bool CBaseHunAI::CDecisionUnitAttackF::CheckTerm(void) /*override*/
{
    if (m_pBaseHunChr->IsInBurstMode())
        return false;

    if (m_pBaseHunChr->GetStatus() == ENEMYTYPES::STATUS_RUN)
        return false;

    if (!m_pAIDecisionUnitCommonParameter->IsAttack())
        return false;

    int32 numViewData = m_pAIDecisionUnitCommonParameter->GetViewDataNum();
    for (int32 i = 0; i < numViewData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = m_pAIDecisionUnitCommonParameter->GetViewData(i);

        bool bIsSatisfyDistCondition = (nearerPlayerData.distance <= m_fDistanceCondition);
        bool bIsSatisfyDistSuitable = (nearerPlayerData.distance > EnemyChr().AICharacteristic().m_fDistanceOfSuitable);
        bool bIsSatisfyAngle = IsSatisfyAngleCondition(&nearerPlayerData);

        bool bIsObstacleBetweenUs = CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(),
                                                                                    nearerPlayerData.no,
                                                                                    false);
        if (bIsSatisfyAngle &&
            bIsSatisfyDistCondition &&
            bIsSatisfyDistSuitable &&            
            !bIsObstacleBetweenUs)
        {
            if (!IsSatisfyFrequency(m_frequencyTableNo))
                return false;

            m_orderTargetNo = nearerPlayerData.no;
            return true;
        };
    };

    return false;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackFConnectFromRun::CDecisionUnitAttackFConnectFromRun(void)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_F_CONNECT_FROM_RUN)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_F_RUN;
};


/*virtual*/ bool CBaseHunAI::CDecisionUnitAttackFConnectFromRun::CheckTerm(void) /*override*/
{
    /* make it enable only by manual decision unit change (see CBaseHunAI::CDecisionUnitMove::Update) */
    return false;
};


/*virtual*/ void CBaseHunAI::CDecisionUnitAttackFConnectFromRun::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackFOnTallObject::CDecisionUnitAttackFOnTallObject(void)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_F_ON_TALL_OBJ)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_F_TALL;
};


/*virtual*/ bool CBaseHunAI::CDecisionUnitAttackFOnTallObject::CheckTerm(void) /*override*/
{
    if (!m_pAIDecisionUnitCommonParameter->IsAttack())
        return false;

    ENEMYTYPES::STATUS statusNow = EnemyChr().GetStatus();
    bool bIsStatusValid =  (statusNow == ENEMYTYPES::STATUS_IDLE)
                        || (statusNow == ENEMYTYPES::STATUS_TURN)
                        || (statusNow == ENEMYTYPES::STATUS_WALK)
                        || (statusNow == ENEMYTYPES::STATUS_WALK_RIGHT)
                        || (statusNow == ENEMYTYPES::STATUS_WALK_LEFT)
                        || (statusNow == ENEMYTYPES::STATUS_WALK_BACK)
                        || (statusNow == ENEMYTYPES::STATUS_RUN)
                        || (statusNow == ENEMYTYPES::STATUS_DASH)
                        || (statusNow == ENEMYTYPES::STATUS_GETUP);

    if (!bIsStatusValid)
        return false;

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    int32 numEnableData = m_pAIDecisionUnitCommonParameter->GetEnableDataNum();
    for (int32 i = 0; i < numEnableData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = m_pAIDecisionUnitCommonParameter->GetEnableData(i);
        if (nearerPlayerData.distance > m_fDistanceCondition)
            continue;
        
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(nearerPlayerData.no);

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        float fHeightDiff = (vecFootPosMe.y - vecFootPosPlr.y);
        if (fHeightDiff > 0.5f)
        {
            if (!IsSatisfyFrequency(m_frequencyTableNo))
                return false;

            m_orderTargetNo = nearerPlayerData.no;
            return true;
        };
    };

    return false;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackRevenge::CDecisionUnitAttackRevenge(CEnemyCharacter* pEnemyChr)
: CBaseAI6045::CDecisionUnitAttackNormal(BASEHUN_AI::AIDECISIONUNITNAME::ATTACK_REVENGE)
, m_pBaseHunChr(static_cast<CBaseHunChr*>(pEnemyChr))
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_F_RUN;
};


/*virtual*/ bool CBaseHunAI::CDecisionUnitAttackRevenge::CheckTerm(void) /*override*/
{
    if (!m_pBaseHunChr->IsInRevengeMode())
        return false;

    if (m_pAIDecisionUnitCommonParameter->GetEnableDataNum() <= 0)
        return false;

    if (m_pAIDecisionUnitCommonParameter->PlayerWatcher().IsAllPlayerDownState())
        return false;

    const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = m_pAIDecisionUnitCommonParameter->GetEnableData(0);
    m_orderType = nearerPlayerData.no;

    if (nearerPlayerData.distance >= EnemyChr().AICharacteristic().m_fDistanceOfSuitable)
    {
        if (nearerPlayerData.distance >= 5.0f)
            SetAttackTypeFarRange();
        else
            SetAttackTypeMiddleRange();
    }
    else
    {
        SetAttackTypeNearRange();
    };

    return true;
};


/*virtual*/ void CBaseHunAI::CDecisionUnitAttackRevenge::SetAttackTypeNearRange(void)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_D;
};


/*virtual*/ void CBaseHunAI::CDecisionUnitAttackRevenge::SetAttackTypeMiddleRange(void)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_C;
};


/*virtual*/ void CBaseHunAI::CDecisionUnitAttackRevenge::SetAttackTypeFarRange(void)
{
    m_orderType = BASEHUN_AI::ORDERTYPE_ATTACK_B;
};


//
// *********************************************************************************
//


CBaseHunAI::CDecisionUnitAttackCounter::CDecisionUnitAttackCounter(void)
: CBaseAI6045::CDecisionUnitAttackCounter(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER)
{
    ;
};


/*virtual*/ bool CBaseHunAI::CDecisionUnitAttackCounter::CheckTerm(void) /*override*/
{
    /* make it enable only by manual decision unit change (see CBaseHunAI::CatchTriggerActionOrder) */
    return false;
};


//
// *********************************************************************************
//

CBaseHunAI::CBaseHunAI(CEnemyCharacter* pEnemyChr)
: CBaseAI6045(pEnemyChr)
{
    ;
};


/*virtual*/ CBaseHunAI::~CBaseHunAI(void)
{
    ;
};


/*virtual*/ void CBaseHunAI::CatchTriggerActionOrder(void) /*override*/
{
    if (m_catchTriggerAction & BASEHUN_AI::CATCHTRIGGER_COUNTER)
    {
        DecisionUnitCommonParameter().ChangeDicisionUnit(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);

        CEnemyAIDecisionUnit* pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER);
        if (pUnit)
            static_cast<CBaseAI6045::CDecisionUnitAttackCounter*>(pUnit)->ResetTarget();
    };
};