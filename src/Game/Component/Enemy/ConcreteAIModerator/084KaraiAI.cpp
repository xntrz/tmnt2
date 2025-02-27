#include "084KaraiAI.hpp"

#include "Game/Component/Enemy/ConcreteEnemyCharacter/084Karai.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


C084KaraiAI::CDecisionUnitMoveEscape::CDecisionUnitMoveEscape(CBaseKaraiChr* pBaseKaraiChr)
: CEnemyAIDecisionUnit(KARAI_AI::AIDECISIONUNITNAME::MOVE_ESCAPE, TYPE_MOVE)
, m_pBaseKaraiChr(pBaseKaraiChr)
{
    ASSERT(m_pBaseKaraiChr != nullptr);
};


/*virtual*/ bool C084KaraiAI::CDecisionUnitMoveEscape::CheckTerm(void) /*override*/
{
    if (!m_pBaseKaraiChr->IsModeWarning())
        return false;

    if (DecisionUnitCommonParameter().CheckAllTypeHistory(this) >= KARAI_AI::MOVE_ESCAPE_PERMIT_INALLTYPEHISTORYNUM)
        return false;

    if (!DecisionUnitCommonParameter().IsMove())
        return false;

    int32 numEnablePlayer = DecisionUnitCommonParameter().GetEnableDataNum();
    if (GetDistanceConditionEnablePlayerNum() < numEnablePlayer)
        return false;

    if (!IsSatisfyFrequency(m_frequencyTableNo))
        return false;

    RwV3d vecAllPlayerPos = Math::VECTOR3_ZERO;
    if (!CAIUtils6045::CalcGroundPointAllPlayer(&vecAllPlayerPos))
        return false;

    return ProcEscapeSetting(&vecAllPlayerPos);
};


/*virtual*/ void C084KaraiAI::CDecisionUnitMoveEscape::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
    EnemyChr().AIThinkOrder().OrderMove().m_iMoveType = m_orderType;
    EnemyChr().AIThinkOrder().OrderMove().m_vAt       = m_vecOrderPos;
};


bool C084KaraiAI::CDecisionUnitMoveEscape::ProcEscapeSetting(const RwV3d* pvecPlayerPos)
{
    RwV3d vecEnemyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecEnemyPos);
    
    RwV3d vecDir = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDir, &vecEnemyPos, pvecPlayerPos);
    vecDir.y = 0.0f;

    if (Math::Vec3_IsEqual(&vecEnemyPos, &Math::VECTOR3_ZERO))
    {
        m_orderType = BASEAI6045::ORDERTYPE_MOVE_SPECIAL; // backaway

        vecDir = { 0.0f, 0.0f, -3.0f };
        EnemyChr().Compositor().RotateVectorByDirection(&vecDir, &vecDir);

        Math::Vec3_Add(&m_vecOrderPos, &vecEnemyPos, &vecDir);
    }
    else
    {
        Math::Vec3_Normalize(&vecDir, &vecDir);
        Math::Vec3_Scale(&vecDir, &vecDir, 2.25f);

        RwV3d vecStart = vecEnemyPos;
        vecStart.y += 0.2f;

        RwV3d vecEnd = vecEnemyPos;
        Math::Vec3_Add(&vecEnd, &vecEnd, &vecDir);
        vecEnd.y = vecStart.y;

        if (CAIUtils::CheckStep(&vecStart, &vecEnd))
        {
            m_orderType = BASEAI6045::ORDERTYPE_MOVE_JUMP_ESCAPE; // jump escape

            Math::Vec3_Sub(&vecDir, pvecPlayerPos, &vecEnemyPos);
            vecDir.y = 0.0f;

            Math::Vec3_Normalize(&vecDir, &vecDir);
            Math::Vec3_Scale(&vecDir, &vecDir, 8.0f);
        }
        else
        {
            m_orderType = BASEAI6045::ORDERTYPE_MOVE_SPECIAL; // backaway

            Math::Vec3_Sub(&vecDir, &vecEnemyPos, pvecPlayerPos);
            vecDir.y = 0.0f;

            Math::Vec3_Normalize(&vecDir, &vecDir);
            Math::Vec3_Scale(&vecDir, &vecDir, 3.0f);
        };

        Math::Vec3_Add(&m_vecOrderPos, &vecEnemyPos, &vecDir);
    };

    return true;
};


bool C084KaraiAI::CDecisionUnitMoveEscape::CheckTermForGuardReaction(void)
{
    if (!m_pBaseKaraiChr->IsModeWarning())
        return false;

    if (!DecisionUnitCommonParameter().IsMoveTime())
        return false;

    if (!DecisionUnitCommonParameter().IsEnableDataValid())
        return false;
    
    if (!IsSatisfyFrequency(m_frequencyTableNo))
        return false;

    const CAIUtils::NEARER_PLAYERDATA* pNearerPlayerData = CAIUtils::GetNearerPlayerData(0);
    if (!pNearerPlayerData)
        return false;

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(pNearerPlayerData->no);
    if (!pPlayerChr)
        return false;

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPos);

    return ProcEscapeSetting(&vecFootPos);
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackSpecial::CDecisionUnitAttackSpecial(CBaseKaraiChr* pBaseKaraiChr)
: CBaseAI6045::CDecisionUnitAttackNormal(KARAI_AI::AIDECISIONUNITNAME::ATTACK_SPECIAL)
, m_pBaseKaraiChr(pBaseKaraiChr)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_SPECIAL;
};


bool C084KaraiAI::CDecisionUnitAttackSpecial::CheckTermForGuardReaction(void)
{
    if (!m_pBaseKaraiChr->IsModeFinal())
        return false;

    const CAIUtils::NEARER_PLAYERDATA* pNearerPlayerData = CAIUtils::GetNearerPlayerData(0);
    if (!pNearerPlayerData)
        return false;

    if (pNearerPlayerData->distance > m_fDistanceCondition)
        return false;

    m_orderTargetNo = pNearerPlayerData->no;
    return true;
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackSpecialForUnusualStatus::CDecisionUnitAttackSpecialForUnusualStatus(void)
: CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus(KARAI_AI::AIDECISIONUNITNAME::ATTACK_SPECIAL_FORUNUSUALSTATUS)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_SPECIAL;
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackF::CDecisionUnitAttackF(void)
: CBaseAI6045::CDecisionUnitAttackNormal(KARAI_AI::AIDECISIONUNITNAME::ATTACK_F)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_F;
};


bool C084KaraiAI::CDecisionUnitAttackF::CheckTermForGuardReaction(void)
{
    const CAIUtils::NEARER_PLAYERDATA* pNearerPlayerData = CAIUtils::GetNearerPlayerData(0);
    if (!pNearerPlayerData)
        return false;

    if (pNearerPlayerData->distance > m_fDistanceCondition)
        return false;

    m_orderTargetNo = pNearerPlayerData->no;
    return true;
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackD::CDecisionUnitAttackD(void)
: CBaseAI6045::CDecisionUnitAttackOverInterval(KARAI_AI::AIDECISIONUNITNAME::ATTACK_D)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_D;
};


/*virtual*/ bool C084KaraiAI::CDecisionUnitAttackD::CheckTerm(void) /*override*/
{
    ENEMYTYPES::STATUS enemyStatus = EnemyChr().GetStatus();
    if (CEnemyUtils::IsFlyawayState(enemyStatus) ||
        CEnemyUtils::IsDownState(enemyStatus))
        return false;

    return CBaseAI6045::CDecisionUnitAttackOverInterval::CheckTerm();
};


/*virtual*/ void C084KaraiAI::CDecisionUnitAttackD::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
    EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = m_orderType;
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = m_orderTargetNo;
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackCounter::CDecisionUnitAttackCounter(void)
: CBaseAI6045::CDecisionUnitAttackCounter(BASEAI6045::AIDECISIONUNITNAME::ATTACK_COUNTER)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_COUNTER;
};


/*virtual*/ bool C084KaraiAI::CDecisionUnitAttackCounter::CheckTerm(void) /*override*/
{
    return false;
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackE::CDecisionUnitAttackE(void)
: CBaseAI6045::CDecisionUnitAttackCounter(KARAI_AI::AIDECISIONUNITNAME::ATTACK_E)
{
    m_fDistanceCondition    = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_orderType             = BASEAI6045::ORDERTYPE_ATTACK_E;
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackC::CDecisionUnitAttackC(void)
: CBaseAI6045::CDecisionUnitAttackOverInterval(KARAI_AI::AIDECISIONUNITNAME::ATTACK_C)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_C1;
};


/*virtual*/ bool C084KaraiAI::CDecisionUnitAttackC::CheckTerm(void) /*override*/
{
    ENEMYTYPES::STATUS enemyStatus = EnemyChr().GetStatus();
    if (CEnemyUtils::IsFlyawayState(enemyStatus) ||
        CEnemyUtils::IsDownState(enemyStatus))
        return false;

    if (DecisionUnitCommonParameter().CheckAttackHistory(this) >= KARAI_AI::ATTACK_C_PERMIT_INATTACKHISTORYNUM)
        return false;

    return CBaseAI6045::CDecisionUnitAttackOverInterval::CheckTerm();
};


/*virtual*/ void C084KaraiAI::CDecisionUnitAttackC::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackOverInterval::OnStart();

    int32 numEnablePlayer = DecisionUnitCommonParameter().GetEnableDataNum();
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = (Math::Rand() % numEnablePlayer);
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackC2::CDecisionUnitAttackC2(void)
: CBaseAI6045::CDecisionUnitAttackNormal(KARAI_AI::AIDECISIONUNITNAME::ATTACK_C2)
{
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_C2;
};


/*virtual*/ bool C084KaraiAI::CDecisionUnitAttackC2::CheckTerm(void) /*override*/
{
    ENEMYTYPES::STATUS enemyStatus = EnemyChr().GetStatus();

    if (CEnemyUtils::IsFlyawayState(enemyStatus))
        return false;
    
    if (CEnemyUtils::IsDownState(enemyStatus))
        return false;

    if (!DecisionUnitCommonParameter().IsAttack())
        return false;

    if (!DecisionUnitCommonParameter().IsEnableDataValid())
        return false;

    RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosEnemy);

    int32 checkCount = 0;

    int32 numEnabledPlayer = DecisionUnitCommonParameter().GetEnableDataNum();
    for (int32 i = 0; i < numEnabledPlayer; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA* pNearerPlayerData = CAIUtils::GetNearerPlayerData(i);

        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(pNearerPlayerData->no);
        if (!pPlayerChr)
            continue;

        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);

        if ((vecFootPosEnemy.y + 0.3f) < vecFootPosPlayer.y)
        {
            PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();

            bool bIsStatusBasic = ((playerStatus >= PLAYERTYPES::STATUS_IDLE) &&
                                   (playerStatus <= PLAYERTYPES::STATUS_DASH_FINISH));
            
            bool bIsStatusAttack = ((playerStatus >= PLAYERTYPES::STATUS_GUARD_READY) &&
                                    (playerStatus <= PLAYERTYPES::STATUS_ATTACK_RUN));
            
            bool bIsStatusThrow = ((playerStatus >= PLAYERTYPES::STATUS_LIFT_CHALLENGE) &&
                                   (playerStatus <= PLAYERTYPES::STATUS_THROWN_MISS));

            bool bIsStatusKnife = (playerStatus == PLAYERTYPES::STATUS_ATTACK_KNIFE);

            if (bIsStatusBasic || bIsStatusAttack || bIsStatusThrow || bIsStatusKnife)
                ++checkCount;
        };
    };

    if (checkCount != numEnabledPlayer)
        return false;

    if (!IsSatisfyFrequency(m_frequencyTableNo))
        return false;

    const CAIUtils::NEARER_PLAYERDATA* pNearerPlayerData = CAIUtils::GetNearerPlayerData(0);
    ASSERT(pNearerPlayerData != nullptr);

    m_orderTargetNo = pNearerPlayerData->no;
    return true;
};


/*virtual*/ void C084KaraiAI::CDecisionUnitAttackC2::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();

    int32 numEnablePlayer = DecisionUnitCommonParameter().GetEnableDataNum();
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = (Math::Rand() % numEnablePlayer);
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackB::CDecisionUnitAttackB(CBaseKaraiChr* pBaseKaraiChr)
: CBaseAI6045::CDecisionUnitAttackNormal(KARAI_AI::AIDECISIONUNITNAME::ATTACK_B)
, m_pBaseKaraiChr(pBaseKaraiChr)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
};


/*virtual*/ void C084KaraiAI::CDecisionUnitAttackB::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();

    bool bWarningMode = m_pBaseKaraiChr->IsModeWarning();
    int32 freqTableBB = (bWarningMode ? KARAI_AI::FREQUENCY_ATTACK_BB_W : KARAI_AI::FREQUENCY_ATTACK_BB);

    C084KaraiAI::SetAttackTypeB(*this, freqTableBB);
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackBForUnusualStatus::CDecisionUnitAttackBForUnusualStatus(CBaseKaraiChr* pBaseKaraiChr)
: CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus(KARAI_AI::AIDECISIONUNITNAME::ATTACK_B_FORUNUSUALSTATUS)
, m_pBaseKaraiChr(pBaseKaraiChr)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
};


/*virtual*/ void C084KaraiAI::CDecisionUnitAttackBForUnusualStatus::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus::OnStart();

    bool bWarningMode = m_pBaseKaraiChr->IsModeWarning();
    int32 freqTableBB = (bWarningMode ? KARAI_AI::FREQUENCY_ATTACK_BB_W : KARAI_AI::FREQUENCY_ATTACK_BB);

    C084KaraiAI::SetAttackTypeB(*this, freqTableBB);
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackA::CDecisionUnitAttackA(CBaseKaraiChr* pBaseKaraiChr)
: CBaseAI6045::CDecisionUnitAttackNormal(KARAI_AI::AIDECISIONUNITNAME::ATTACK_A)
, m_pBaseKaraiChr(pBaseKaraiChr)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
};


/*virtual*/ void C084KaraiAI::CDecisionUnitAttackA::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormal::OnStart();

    bool bWarningMode  = m_pBaseKaraiChr->IsModeWarning();
    int32 freqTableAA  = (bWarningMode ? KARAI_AI::FREQUENCY_ATTACK_AA_W : KARAI_AI::FREQUENCY_ATTACK_AA);
    int32 freqTableAAA = (bWarningMode ? KARAI_AI::FREQUENCY_ATTACK_AAA_W : KARAI_AI::FREQUENCY_ATTACK_AAA);

    C084KaraiAI::SetAttackTypeA(*this, freqTableAA, freqTableAAA);
};


//
// *********************************************************************************
//


C084KaraiAI::CDecisionUnitAttackAForUnusualStatus::CDecisionUnitAttackAForUnusualStatus(CBaseKaraiChr* pBaseKaraiChr)
: CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus(KARAI_AI::AIDECISIONUNITNAME::ATTACK_A_FORUNUSUALSTATUS)
, m_pBaseKaraiChr(pBaseKaraiChr)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
};


/*virtual*/ void C084KaraiAI::CDecisionUnitAttackAForUnusualStatus::OnStart(void) /*override*/
{
    CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus::OnStart();

    bool bWarningMode  = m_pBaseKaraiChr->IsModeWarning();
    int32 freqTableAA  = (bWarningMode ? KARAI_AI::FREQUENCY_ATTACK_AA_W : KARAI_AI::FREQUENCY_ATTACK_AA);
    int32 freqTableAAA = (bWarningMode ? KARAI_AI::FREQUENCY_ATTACK_AAA_W : KARAI_AI::FREQUENCY_ATTACK_AAA);

    C084KaraiAI::SetAttackTypeA(*this, freqTableAA, freqTableAAA);
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* C084KaraiAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C084KaraiAI(pEnemyChr);
};


/*static*/ void C084KaraiAI::SetAttackTypeA(CEnemyAIDecisionUnit& rDecisionUnit, int32 freqTableAA, int32 freqTableAAA)
{
    int32 attackType = BASEAI6045::ORDERTYPE_ATTACK_A;

    if (rDecisionUnit.IsSatisfyFrequency(freqTableAA))
    {
        if (rDecisionUnit.IsSatisfyFrequency(freqTableAAA))
        {
            float fRand = Math::RandFloat();

            float fChanceAAC = ((1.0f / 3.0f) * 2.0f);
            float fChanceAAB = ((1.0f / 3.0f) * 1.0f);

            if (fRand >= fChanceAAC)
                attackType = BASEAI6045::ORDERTYPE_ATTACK_AAC;
            else if (fRand >= fChanceAAB)
                attackType = BASEAI6045::ORDERTYPE_ATTACK_AAB;
            else
                attackType = BASEAI6045::ORDERTYPE_ATTACK_AAA;
        }
        else
        {
            attackType = BASEAI6045::ORDERTYPE_ATTACK_AA;
        };
    }
    else
    {
        attackType = BASEAI6045::ORDERTYPE_ATTACK_A;
    };

    rDecisionUnit.EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = attackType;
};


/*static*/ void C084KaraiAI::SetAttackTypeB(CEnemyAIDecisionUnit& rDecisionUnit, int32 freqTableBB)
{
    if (rDecisionUnit.IsSatisfyFrequency(freqTableBB))
        rDecisionUnit.EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = BASEAI6045::ORDERTYPE_ATTACK_B;
    else
        rDecisionUnit.EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = BASEAI6045::ORDERTYPE_ATTACK_BB;
};


C084KaraiAI::C084KaraiAI(CEnemyCharacter* pEnemyChr)
: CBaseAI6045(pEnemyChr)
{
    CBaseKaraiChr* pBaseKaraiChr = static_cast<CBaseKaraiChr*>(pEnemyChr);

    /* init common param */
    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();

    DecisionUnitCommonParameter().SetAttackInterval(KARAI_AI::ATTACK_INTERVAL[difficulty]);
    DecisionUnitCommonParameter().SetMoveInterval(KARAI_AI::MOVE_INTERVAL_BASE);
    DecisionUnitCommonParameter().SetWaitTimerAllPlayerDown(KARAI_AI::ATTACK_WAITTIME_ALLPLAYERDOWN[difficulty]);

    /* init decision units (attach order is important!) */
    CEnemyAIDecisionUnit* pUnit = nullptr;

    bool bIsTournamentMap = false;

    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
        bIsTournamentMap = true;

    pUnit = new CBaseAI6045::CDecisionUnitGuard;
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_GUARD_B);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitAttackToAir;
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitMoveEscape(pBaseKaraiChr);
    pUnit->SetConditionDistance(KARAI_AI::MOVE_ESCAPE_CONDITION_DISTANCE);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_MOVE_ESCAPE);
    Attach(pUnit);

    if (!bIsTournamentMap)
    {
        pUnit = new C084KaraiAI::CDecisionUnitAttackC2;
        Attach(pUnit);
    };

    pUnit = new C084KaraiAI::CDecisionUnitAttackAForUnusualStatus(pBaseKaraiChr);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_A_UNUSUAL);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackBForUnusualStatus(pBaseKaraiChr);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_B_UNUSUAL);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackSpecialForUnusualStatus;
    pUnit->SetConditionDistance(KARAI_AI::ATTACK_SPECIAL_UNUSUAL_CONDITION_DISTANCE);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_SPECIAL_UNUSUAL);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackA(pBaseKaraiChr);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_A);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackB(pBaseKaraiChr);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_B);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackC;
    pUnit->SetConditionDistance(KARAI_AI::ATTACK_C_CONDITION_DISTANCE);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_C);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackD;
    pUnit->SetConditionDistance(KARAI_AI::ATTACK_D_CONDITION_DISTANCE);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_D);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackE;
    pUnit->SetConditionDistance(KARAI_AI::ATTACK_E_CONDITION_DISTANCE);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_E);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackF;
    pUnit->SetConditionDistance(KARAI_AI::ATTACK_F_CONDITION_DISTANCE);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_F);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackSpecial(pBaseKaraiChr);
    pUnit->SetConditionDistance(KARAI_AI::ATTACK_SPECIAL_CONDITION_DISTANCE);
    pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_SPECIAL);
    Attach(pUnit);

    pUnit = new C084KaraiAI::CDecisionUnitAttackCounter;
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitMoveBoss;
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetMoveTime(KARAI_AI::MOVE_TIME);
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetConditionDistance(KARAI_AI::MOVE_CONDITION_DISTANCE);
    static_cast<CBaseAI6045::CDecisionUnitMoveBoss*>(pUnit)->SetMoveStopDistance(KARAI_AI::MOVE_STOP_DISTANCE);
    Attach(pUnit);

    pUnit = new CBaseAI6045::CDecisionUnitIdle;
    Attach(pUnit);
};


/*virtual*/ C084KaraiAI::~C084KaraiAI(void)
{
    ;
};


/*virtual*/ void C084KaraiAI::CatchTriggerActionOrder(void) /*override*/
{
    if (m_catchTriggerAction & KARAI_AI::CATCHTRIGGER_COUNTER)
    {
        DecisionUnitCommonParameter().UpdatePlayerPositionData();

        const char* pszChangeDicisionUnit = nullptr;

        CEnemyAIDecisionUnit* pUnit = nullptr;

        pUnit = GetDicisionUnitForName(KARAI_AI::AIDECISIONUNITNAME::MOVE_ESCAPE);
        ASSERT(pUnit != nullptr);
        C084KaraiAI::CDecisionUnitMoveEscape* pUnitMoveEscape = static_cast<C084KaraiAI::CDecisionUnitMoveEscape*>(pUnit);

        pUnit = GetDicisionUnitForName(KARAI_AI::AIDECISIONUNITNAME::ATTACK_SPECIAL);
        ASSERT(pUnit != nullptr);
        C084KaraiAI::CDecisionUnitAttackSpecial* pUnitAttackSpecial = static_cast<C084KaraiAI::CDecisionUnitAttackSpecial*>(pUnit);

        pUnit = GetDicisionUnitForName(KARAI_AI::AIDECISIONUNITNAME::ATTACK_F);
        ASSERT(pUnit != nullptr);
        C084KaraiAI::CDecisionUnitAttackF* pUnitAttackF = static_cast<C084KaraiAI::CDecisionUnitAttackF*>(pUnit);

        if (pUnitMoveEscape && pUnitMoveEscape->CheckTermForGuardReaction())
        {
            pszChangeDicisionUnit = KARAI_AI::AIDECISIONUNITNAME::MOVE_ESCAPE;
        }
        else if (pUnitAttackSpecial && pUnitAttackSpecial->CheckTermForGuardReaction())
        {
            pszChangeDicisionUnit = KARAI_AI::AIDECISIONUNITNAME::ATTACK_SPECIAL;
        }
        else if (pUnitAttackF && pUnitAttackF->CheckTermForGuardReaction() && (Math::RandFloat() < 0.5f))
        {
            pszChangeDicisionUnit = KARAI_AI::AIDECISIONUNITNAME::ATTACK_F;
        }
        else
        {
            pszChangeDicisionUnit = KARAI_AI::AIDECISIONUNITNAME::ATTACK_E;
        };

        DecisionUnitCommonParameter().ChangeDicisionUnit(pszChangeDicisionUnit);
    };

    if (m_catchTriggerAction & KARAI_AI::CATCHTRIGGER_MODE_WARNING)
    {
        DecisionUnitCommonParameter().SetMoveInterval(KARAI_AI::MOVE_INTERVAL_WARNING);
        DecisionUnitCommonParameter().SetAttackInterval(KARAI_AI::ATTACK_INTERVAL_FORWARNINGMODE[CGameProperty::GetDifficulty()]);

        CEnemyAIDecisionUnit* pUnit = nullptr;

        pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::GUARD);
        ASSERT(pUnit != nullptr);
        pUnit->SetFreqTable(KARAI_AI::FREQUENCY_GUARD_W);

        pUnit = GetDicisionUnitForName(KARAI_AI::AIDECISIONUNITNAME::ATTACK_SPECIAL);
        ASSERT(pUnit != nullptr);
        pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_SPECIAL_W);

        pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::MOVE);
        ASSERT(pUnit != nullptr);
        static_cast<CBaseAI6045::CDecisionUnitMove*>(pUnit)->SetMoveStopDistance(KARAI_AI::MOVE_STOP_DISTANCE_WARNING);
    };

    if (m_catchTriggerAction & KARAI_AI::CATCHTRIGGER_MODE_FINAL)
    {
        CEnemyAIDecisionUnit* pUnit = nullptr;

        pUnit = GetDicisionUnitForName(BASEAI6045::AIDECISIONUNITNAME::GUARD);
        ASSERT(pUnit != nullptr);
        pUnit->SetFreqTable(KARAI_AI::FREQUENCY_GUARD_F);

        pUnit = GetDicisionUnitForName(KARAI_AI::AIDECISIONUNITNAME::ATTACK_SPECIAL);
        ASSERT(pUnit != nullptr);
        pUnit->SetFreqTable(KARAI_AI::FREQUENCY_ATTACK_SPECIAL_F);
    };
};