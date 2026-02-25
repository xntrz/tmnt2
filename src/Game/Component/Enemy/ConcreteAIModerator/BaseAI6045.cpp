#include "BaseAI6045.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"

#ifdef _DEBUG
#include "Game/Component/Enemy/EnemyDebug.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#endif /* _DEBUG */


/*static*/ CEnemyCharacter* CEnemyAIDecisionUnit::m_pInitEnemyChr = nullptr;
/*static*/ CAIModerator* CEnemyAIDecisionUnit::m_pInitAIModerator = nullptr;


/*static*/ void CEnemyAIDecisionUnit::SetInitParam(CEnemyCharacter* pEnemyChr, CAIModerator* pAIModerator)
{
    m_pInitEnemyChr = pEnemyChr;
    m_pInitAIModerator = pAIModerator;
};


CEnemyAIDecisionUnit::CEnemyAIDecisionUnit(const char* pszUnitName, TYPE type)
: m_pEnemyChr(m_pInitEnemyChr)
, m_pAIModerator(m_pInitAIModerator)
, m_bFrequencyEnable(false)
, m_frequencyTableNo(-1)
, m_fDistanceCondition(0.0f)
, m_fAngleCondition(0.0f)
, m_orderType(-1)
, m_orderTargetNo(-1)
, m_vecOrderPos(Math::VECTOR3_ZERO)
, m_szUnitName()
, m_pAIDecisionUnitCommonParameter(nullptr)
, m_type(TYPENUM)
, m_id(-1)
{
    ASSERT(pszUnitName != nullptr);
    std::strcpy(m_szUnitName, pszUnitName);

    ASSERT(type >= TYPE_WAIT);
    ASSERT(type <  TYPENUM);
    m_type = type;
};


bool CEnemyAIDecisionUnit::IsSatisfyFrequency(int32 iTableNo) const
{
    if (!m_bFrequencyEnable)
        return true;

    uint8 freq = EnemyChr().AIModerator().AIFreqParam().GetFrequency(iTableNo);
    uint8 rand = static_cast<uint8>(Math::RandRange(0, 100));

    return (freq > rand);
};


bool CEnemyAIDecisionUnit::IsSatisfyAngleCondition(const CAIUtils::NEARER_PLAYERDATA* pNearerPlayerData) const
{
    float fDirMe = EnemyChr().Compositor().GetDirection();
    float fDirTarget = pNearerPlayerData->angle;

    float fDirDiff = (fDirTarget - fDirMe);
    fDirDiff = std::fabs(fDirDiff);

    if (fDirDiff > MATH_PI)
        fDirDiff = (MATH_PI2 - fDirDiff);

    return (fDirDiff <= m_fAngleCondition);
};


int32 CEnemyAIDecisionUnit::GetDistanceConditionViewAreaPlayerNum(void) const
{
    int32 numDistanceOk = 0;

    int32 numViewAreaData = DecisionUnitCommonParameter().GetViewDataNum();
    for (int32 i = 0; i < numViewAreaData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(i);

        if (nearerPlayerData.distance <= m_fDistanceCondition)
            ++numDistanceOk;
    };

    return numDistanceOk;
};


int32 CEnemyAIDecisionUnit::GetDistanceConditionEnablePlayerNum(void) const
{
    int32 numDistanceOk = 0;

    int32 numEnableData = DecisionUnitCommonParameter().GetEnableDataNum();
    for (int32 i = 0; i < numEnableData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetEnableData(i);

        if (nearerPlayerData.distance <= m_fDistanceCondition)
            ++numDistanceOk;
    };

    return numDistanceOk;
};


void CEnemyAIDecisionUnit::SetFreqTable(int32 tableNo)
{
    m_frequencyTableNo = tableNo;
    m_bFrequencyEnable = true;
};


void CEnemyAIDecisionUnit::SetConditionDistance(float fDist)
{
    m_fDistanceCondition = fDist;
};


void CEnemyAIDecisionUnit::SetConditionAngle(float fAngle)
{
    m_fAngleCondition = fAngle;
};


CEnemyAIDecisionUnit::TYPE CEnemyAIDecisionUnit::GetType(void) const
{
    return m_type;
};


int32 CEnemyAIDecisionUnit::GetID(void) const
{
    return m_id;
};


const char* CEnemyAIDecisionUnit::GetName(void) const
{
    return m_szUnitName;
};


CEnemyCharacter& CEnemyAIDecisionUnit::EnemyChr(void)
{
    ASSERT(m_pEnemyChr != nullptr);
    return *m_pEnemyChr;
};


const CEnemyCharacter& CEnemyAIDecisionUnit::EnemyChr(void) const
{
    ASSERT(m_pEnemyChr != nullptr);
    return *m_pEnemyChr;
};


CEnemyAIDecisionUnitCommonParameter& CEnemyAIDecisionUnit::DecisionUnitCommonParameter(void)
{
    ASSERT(m_pAIDecisionUnitCommonParameter != nullptr);
    return *m_pAIDecisionUnitCommonParameter;
};


const CEnemyAIDecisionUnitCommonParameter& CEnemyAIDecisionUnit::DecisionUnitCommonParameter(void) const
{
    ASSERT(m_pAIDecisionUnitCommonParameter != nullptr);
    return *m_pAIDecisionUnitCommonParameter;
};


//
// *********************************************************************************
//


CEnemyAIDecisionUnitTrigger::CEnemyAIDecisionUnitTrigger(void)
: CEnemyAIDecisionUnit(BASEAI6045::AIDECISIONUNITNAME::TRIGGER, TYPE_ETC)
{
    ;
};


/*virtual*/ bool CEnemyAIDecisionUnitTrigger::CheckTerm(void) /*override*/
{
    /* make it not selectable by decision unit mgr */
    return false;
};


/*virtual*/ void CEnemyAIDecisionUnitTrigger::InitTrigger(void)
{
    if (EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_MOVE))
    {
        DecisionUnitCommonParameter().ClearSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_PERMIT);
        DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_ENABLE);
    };

    if (EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_ATTACK))
    {
        DecisionUnitCommonParameter().ClearSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_PERMIT);
        DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_ENABLE);
    };

    if (EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_REDUCTION_HP))
    {
        DecisionUnitCommonParameter().ClearSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_PERMIT);
        DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_ENABLE);
    };
};


/*virtual*/ void CEnemyAIDecisionUnitTrigger::ProcTrigger(void)
{
    if (!DecisionUnitCommonParameter().IsViewDataValid())
        DecisionUnitCommonParameter().ClearSpecialFlag(BASEAI6045::FLAG_TRIGGER_PERMIT_ALL);

    if (DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_ENABLE) &&
        !DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_PERMIT))
    {
        uint8 freq = EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_MOVE);
        switch (freq)
        {
        case BASEAI6045::FREQUENCY_TRIGGER_ALWAYS:
            {
                DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_PERMIT);
            }
            break;

        case BASEAI6045::FREQUENCY_TRIGGER_DAMAGED:
            {
                if (IsDamaged())
                    DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_PERMIT);
            }
            break;

        case BASEAI6045::FREQUENCY_TRIGGER_SUITDIST:
            {
                if (IsPlayerSuitableDitance())
                    DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_PERMIT);
            }
            break;

        case BASEAI6045::FREQUENCY_TRIGGER_IGNORE:
            break;

        case BASEAI6045::FREQUENCY_TRIGGER_PLRSTATUS:
            {
                if (DecisionUnitCommonParameter().IsViewDataValid())
                {
                    int32 numViewData = DecisionUnitCommonParameter().GetViewDataNum();
                    for (int32 i = 0; i < numViewData; ++i)
                    {
                        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(i);

                        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(nearerPlayerData.no);
                        if (!pPlayerChr)
                            continue;

                        PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
                        if ((playerStatus == PLAYERTYPES::STATUS_RUN) ||
                            (playerStatus == PLAYERTYPES::STATUS_DASH))
                            DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_PERMIT);
                    };
                };
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    if (DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_ENABLE) &&
        !DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_PERMIT))
    {
        uint8 freq = EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_ATTACK);
        switch (freq)
        {
        case BASEAI6045::FREQUENCY_TRIGGER_ALWAYS:
            {
                DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_PERMIT);
            }            
            break;

        case BASEAI6045::FREQUENCY_TRIGGER_DAMAGED:
            {
                if (IsDamaged())
                    DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_PERMIT);
            }
            break;

        case BASEAI6045::FREQUENCY_TRIGGER_SUITDIST:
            {
                if (IsPlayerSuitableDitance())
                    DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_PERMIT);
            }
            break;

        case BASEAI6045::FREQUENCY_TRIGGER_IGNORE:
            break;

        case BASEAI6045::FREQUENCY_TRIGGER_PLRSTATUS:
            {
                if (DecisionUnitCommonParameter().IsViewDataValid())
                {
                    int32 numViewData = DecisionUnitCommonParameter().GetViewDataNum();
                    for (int32 i = 0; i < numViewData; ++i)
                    {
                        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(i);

                        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(nearerPlayerData.no);
                        if (!pPlayerChr)
                            continue;

                        PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
                        if ((playerStatus != PLAYERTYPES::STATUS_WALK) &&
                            !(CAIUtils::GetPlayerStateFlag(playerStatus) & CAIUtils::PLAYER_STATE_GUARD))
                            DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_PERMIT);
                    };
                };
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    if (DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_ENABLE) &&
        !DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_PERMIT))
    {
        float fHpRatio = CEnemyUtils::GetEnemyHPRate(&EnemyChr());
        if (fHpRatio < 0.3f)
            DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_PERMIT);
    };
};


bool CEnemyAIDecisionUnitTrigger::IsPlayerSuitableDitance(void)
{
    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(0);
    if (nearerPlayerData.distance > EnemyChr().AICharacteristic().m_fDistanceOfSuitable)
        return false;

    return true;
};


bool CEnemyAIDecisionUnitTrigger::IsDamaged(void)
{
    int32 hpNow = EnemyChr().Feature().m_iHP;
    int32 hpMax = EnemyChr().Feature().m_iHPMax;

    return (hpNow != hpMax);
};


//
// *********************************************************************************
//


CEnemyAIDecisionUnitCommonParameter::CEnemyAIDecisionUnitCommonParameter(CEnemyCharacter* pEnemyChr)
: m_numPlayerEnable(0)
, m_apDataViewArea()
, m_numDataViewArea(0)
, m_fRatioOfActivity(0.0f)
, m_fRatioOfMove(0.0f)
, m_fAttackTimer(0.0f)
, m_fAttackInterval(0.0f)
, m_fMoveTimer(0.0f)
, m_fMoveInterval(0.0f)
, m_fMoveWaitTimer(0.0f)
, m_fAttackWaitTimer(0.0f)
, m_fAttackWaitTimerAllPlayerDown(0.0f)
, m_specialFlag(BASEAI6045::FLAG_DEFAULT)
, m_pszChangeDecisionUnitName(nullptr)
, m_aAIDecisionUnitAllTypeHistory()
, m_aAIDecisionUnitAttackHistory()
, m_playerWatcher()
, m_pEnemyChr(pEnemyChr)
{
    for (int32 i = 0; i < COUNT_OF(m_apDataViewArea); ++i)
        m_apDataViewArea[i] = nullptr;

    ClearHistoryProcess(m_aAIDecisionUnitAllTypeHistory, COUNT_OF(m_aAIDecisionUnitAllTypeHistory));
    ClearHistoryProcess(m_aAIDecisionUnitAttackHistory, COUNT_OF(m_aAIDecisionUnitAttackHistory));

    m_playerWatcher.Initialize(pEnemyChr);
};


CEnemyAIDecisionUnitCommonParameter::~CEnemyAIDecisionUnitCommonParameter(void)
{
    ;
};


void CEnemyAIDecisionUnitCommonParameter::UpdateEveryTime(void)
{
    if (m_fMoveWaitTimer >= 0.0f)
        m_fMoveWaitTimer -= CGameProperty::GetElapsedTime();

    if (m_fAttackWaitTimer >= 0.0f)
        m_fAttackWaitTimer -= CGameProperty::GetElapsedTime();

    UpdatePlayerPositionData();
};


void CEnemyAIDecisionUnitCommonParameter::UpdatePlayerPositionData(void)
{
    m_numPlayerEnable = CAIUtils::MakeNearerPlayerData(&m_pEnemyChr->Compositor());
    m_numDataViewArea = 0;

    for (int32 i = 0; i < m_numPlayerEnable; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA* pNearedPlayerData = CAIUtils::GetNearerPlayerData(i);
        ASSERT(pNearedPlayerData != nullptr);

        if (CAIUtils::IsObjectViewArea2(m_pEnemyChr, pNearedPlayerData->distance, pNearedPlayerData->angle))
            m_apDataViewArea[m_numDataViewArea++] = pNearedPlayerData;
    };
};


void CEnemyAIDecisionUnitCommonParameter::UpdatePlayerStatusData(void)
{
    m_playerWatcher.Update();

    if (m_playerWatcher.IsAllPlayerDownState())
        m_fAttackWaitTimer = m_fAttackWaitTimerAllPlayerDown;
};


void CEnemyAIDecisionUnitCommonParameter::UpdateMyParameter(void)
{
    m_fRatioOfActivity = Math::RandFloat();
    m_fRatioOfMove = Math::RandFloat();

    if (m_fAttackTimer < m_fAttackInterval)
        m_fAttackTimer += CGameProperty::GetElapsedTime();

    if (m_fMoveTimer < m_fMoveInterval)
        m_fMoveTimer += CGameProperty::GetElapsedTime();
};


void CEnemyAIDecisionUnitCommonParameter::SetHistory(CEnemyAIDecisionUnit* pDecisionUnit)
{
    if (pDecisionUnit->GetType() == CEnemyAIDecisionUnit::TYPE_ATTACK)
        SetAttackHistory(pDecisionUnit);

    SetAllTypeHistory(pDecisionUnit);
};


void CEnemyAIDecisionUnitCommonParameter::ClearHistory(void)
{
    ClearHistoryProcess(m_aAIDecisionUnitAllTypeHistory, COUNT_OF(m_aAIDecisionUnitAllTypeHistory));
    ClearHistoryProcess(m_aAIDecisionUnitAttackHistory, COUNT_OF(m_aAIDecisionUnitAttackHistory));
};


void CEnemyAIDecisionUnitCommonParameter::SetAllTypeHistory(CEnemyAIDecisionUnit* pDecisionUnit)
{
    SetHistoryProcess(pDecisionUnit,
                      m_aAIDecisionUnitAllTypeHistory,
                      COUNT_OF(m_aAIDecisionUnitAllTypeHistory));
};


int32 CEnemyAIDecisionUnitCommonParameter::CheckAllTypeHistory(CEnemyAIDecisionUnit* pDecisionUnit)
{
    return CheckHistoryProcess(pDecisionUnit,
                               m_aAIDecisionUnitAllTypeHistory,
                               COUNT_OF(m_aAIDecisionUnitAllTypeHistory));
};


void CEnemyAIDecisionUnitCommonParameter::SetAttackHistory(CEnemyAIDecisionUnit* pDecisionUnit)
{
    if (pDecisionUnit->GetType() == CEnemyAIDecisionUnit::TYPE_ATTACK)
    {
        SetHistoryProcess(pDecisionUnit,
                          m_aAIDecisionUnitAttackHistory,
                          COUNT_OF(m_aAIDecisionUnitAttackHistory));
    };
};


int32 CEnemyAIDecisionUnitCommonParameter::CheckAttackHistory(CEnemyAIDecisionUnit* pDecisionUnit)
{
    if (pDecisionUnit->GetType() == CEnemyAIDecisionUnit::TYPE_ATTACK)
    {
        return CheckHistoryProcess(pDecisionUnit,
                                   m_aAIDecisionUnitAttackHistory,
                                   COUNT_OF(m_aAIDecisionUnitAttackHistory));
    };

    return 0;
};


bool CEnemyAIDecisionUnitCommonParameter::CalcGroundPointViewAreaPlayer(RwV3d* vecGroundPt) const
{
    ASSERT(vecGroundPt != nullptr);

    RwV3d vecBuff = Math::VECTOR3_ZERO;
    int32 numPos = 0;

    for (int32 i = 0; i < m_numDataViewArea; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_apDataViewArea[i]->no);
        if (!pPlayerChr)
            continue;

        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);

        Math::Vec3_Add(&vecBuff, &vecBuff, &vecFootPosPlayer);

        ++numPos;
    };

    if (numPos)
    {
        Math::Vec3_Scale(vecGroundPt, &vecBuff, (1.0f / static_cast<float>(numPos)));
        return true;
    };

    return false;
};


void
CEnemyAIDecisionUnitCommonParameter::SetHistoryProcess(CEnemyAIDecisionUnit* pDecisionUnit,
                                                       int32* aHistory,
                                                       int32 numHistory)
{
    for (int32 i = numHistory; i > 0; --i)
        aHistory[i] = aHistory[i - 1];

    aHistory[0] = pDecisionUnit->GetID();
};


int32
CEnemyAIDecisionUnitCommonParameter::CheckHistoryProcess(CEnemyAIDecisionUnit* pDecisionUnit,
                                                         int32* aHistory,
                                                         int32 numHistory)
{
    int32 numUnitHistory = 0;

    for (int32 i = 0; i < numHistory; ++i)
    {
        if (aHistory[i] == pDecisionUnit->GetID())
            ++numUnitHistory;
    };

    return numUnitHistory;
};


void CEnemyAIDecisionUnitCommonParameter::ClearHistoryProcess(int32* aHistory, int32 numHistory)
{
    for (int32 i = 0; i < numHistory; ++i)
        aHistory[i] = -1;
};


const char* CEnemyAIDecisionUnitCommonParameter::GetChangeDicisionUnit(void) const
{
    return m_pszChangeDecisionUnitName;
};


void CEnemyAIDecisionUnitCommonParameter::ChangeDicisionUnit(const char* pszUnitName)
{
    ASSERT(pszUnitName != nullptr);

    SetSpecialFlag(BASEAI6045::FLAG_CHANGEDECISION);
    m_pszChangeDecisionUnitName = pszUnitName;
};


void CEnemyAIDecisionUnitCommonParameter::SetSpecialFlag(BASEAI6045::SPECIAL_FLAG flag)
{
    m_specialFlag |= flag;
};


void CEnemyAIDecisionUnitCommonParameter::ClearSpecialFlag(BASEAI6045::SPECIAL_FLAG flag)
{
    m_specialFlag &= (~flag);
};


bool CEnemyAIDecisionUnitCommonParameter::TestSpecialFlag(BASEAI6045::SPECIAL_FLAG flag) const
{
    return ((m_specialFlag & flag) == flag);
};


bool CEnemyAIDecisionUnitCommonParameter::CheckSpecialFlag(BASEAI6045::SPECIAL_FLAG flag) const
{
    return ((m_specialFlag & flag) != 0);
};


void CEnemyAIDecisionUnitCommonParameter::SetAttackInterval(float fInterval)
{
    m_fAttackInterval = fInterval;
};


void CEnemyAIDecisionUnitCommonParameter::SetMoveInterval(float fInterval)
{
    m_fMoveInterval = fInterval;
};


void CEnemyAIDecisionUnitCommonParameter::SetAttackTimer(float fTimer)
{
    m_fAttackTimer = fTimer;
};


void CEnemyAIDecisionUnitCommonParameter::SetMoveTimer(float fTimer)
{
    m_fMoveTimer = fTimer;
};


void CEnemyAIDecisionUnitCommonParameter::SetMoveWaitTime(float fTimer)
{
    m_fMoveWaitTimer = fTimer;
};


void CEnemyAIDecisionUnitCommonParameter::SetAttackWaitTime(float fTimer)
{
    m_fAttackWaitTimer = fTimer;
};


void CEnemyAIDecisionUnitCommonParameter::SetWaitTimerAllPlayerDown(float fTimer)
{
    m_fAttackWaitTimerAllPlayerDown = fTimer;
};


bool CEnemyAIDecisionUnitCommonParameter::IsAttackTime(void) const
{
    return (m_fAttackTimer >= m_fAttackInterval);
};


bool CEnemyAIDecisionUnitCommonParameter::IsAttackPermission(void) const
{
    if (m_pEnemyChr->AICharacteristic().m_fRatioOfActivity <= m_fRatioOfActivity)
        return false;

    if (m_fAttackWaitTimer > 0.0f)
        return false;

    if (TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_ENABLE) &&
        !TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_ATTACK_PERMIT))
        return false;

    if (m_fAttackTimer < m_fAttackInterval)
        return false;

    if (m_playerWatcher.IsAllPlayerFlyawayOrDownState())
        return false;

    return true;
};


bool CEnemyAIDecisionUnitCommonParameter::IsMoveTime(void) const
{
    return (m_fMoveTimer >= m_fMoveInterval);
};


bool CEnemyAIDecisionUnitCommonParameter::IsMovePermission(void) const
{
    if (m_pEnemyChr->AICharacteristic().m_fRatioOfMove <= m_fRatioOfMove)
        return false;

    if (m_fMoveWaitTimer > 0.0f)
        return false;

    if (m_fMoveTimer < m_fMoveInterval)
        return false;
    
    return true;
};


float CEnemyAIDecisionUnitCommonParameter::GetAttackInterval(void) const
{
    return m_fAttackInterval;
};


float CEnemyAIDecisionUnitCommonParameter::GetRatioOfActivity(void) const
{
    return m_fRatioOfActivity;
};


float CEnemyAIDecisionUnitCommonParameter::GetRatioOfMove(void) const
{
    return m_fRatioOfMove;
};


float CEnemyAIDecisionUnitCommonParameter::GetMoveWaitTime(void) const
{
    return m_fMoveWaitTimer;
};


float CEnemyAIDecisionUnitCommonParameter::GetAttackWaitTime(void) const
{
    return m_fAttackWaitTimer;
};


CPlayerWatcher& CEnemyAIDecisionUnitCommonParameter::PlayerWatcher(void)
{
    return m_playerWatcher;
};


bool CEnemyAIDecisionUnitCommonParameter::IsViewDataValid(void) const
{
    return (m_numDataViewArea > 0) &&
           (m_numDataViewArea <= CAIUtils::GetPlayerNum());
};


int32 CEnemyAIDecisionUnitCommonParameter::GetViewDataNum(void) const
{
    return m_numDataViewArea;
};


const CAIUtils::NEARER_PLAYERDATA&
CEnemyAIDecisionUnitCommonParameter::GetViewData(int32 index)
{
    ASSERT(index >= 0);
    ASSERT(index < m_numDataViewArea);

    return *m_apDataViewArea[index];
};


const CAIUtils::NEARER_PLAYERDATA&
CEnemyAIDecisionUnitCommonParameter::GetViewData(int32 index) const
{
    ASSERT(index >= 0);
    ASSERT(index < m_numDataViewArea);

    return *m_apDataViewArea[index];
};


bool CEnemyAIDecisionUnitCommonParameter::IsEnableDataValid(void) const
{
    return ((m_numPlayerEnable > 0) &&
            (m_numPlayerEnable <= CAIUtils::GetPlayerNum()));
};


int32 CEnemyAIDecisionUnitCommonParameter::GetEnableDataNum(void) const
{
    return m_numPlayerEnable;
};


const CAIUtils::NEARER_PLAYERDATA&
CEnemyAIDecisionUnitCommonParameter::GetEnableData(int32 index)
{
    return *CAIUtils::GetNearerPlayerData(index);
};


const CAIUtils::NEARER_PLAYERDATA&
CEnemyAIDecisionUnitCommonParameter::GetEnableData(int32 index) const
{
    return *CAIUtils::GetNearerPlayerData(index);
};


//
// *********************************************************************************
//


CEnemyAIDecisionUnitManager::CEnemyAIDecisionUnitManager(CEnemyCharacter* pEnemyCHr, 
                                                         CEnemyAIDecisionUnitCommonParameter* pAIDecisionUnitCommonParameter)
: m_apAIDecisionUnit()
, m_numAIDicisionUnit(0)
, m_pNowDecisionUnit(nullptr)
, m_pAIDecisionUnitCommonParameter(pAIDecisionUnitCommonParameter)
, m_pEnemyChr(pEnemyCHr)
, m_fThinkTimer(0.0f)
, m_pAIDecisionUnitTrigger(nullptr)
{
    for (int32 i = 0; i < COUNT_OF(m_apAIDecisionUnit); ++i)
        m_apAIDecisionUnit[i] = nullptr;
};


CEnemyAIDecisionUnitManager::~CEnemyAIDecisionUnitManager(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apAIDecisionUnit); ++i)
    {
        if (m_apAIDecisionUnit[i])
        {
            delete m_apAIDecisionUnit[i];
            m_apAIDecisionUnit[i] = nullptr;
        };
    };
};


void CEnemyAIDecisionUnitManager::Run(void)
{
    m_pAIDecisionUnitCommonParameter->UpdateEveryTime();

    if (m_pAIDecisionUnitCommonParameter->CheckSpecialFlag(BASEAI6045::FLAG_TRIGGER_ENABLE_ALL) &&
        m_pAIDecisionUnitTrigger)
    {
        m_pAIDecisionUnitTrigger->ProcTrigger();
    };

    if (m_pAIDecisionUnitCommonParameter->TestSpecialFlag(BASEAI6045::FLAG_CHANGEDECISION))
    {
        if (SetNowDecisionUnit(m_pAIDecisionUnitCommonParameter->GetChangeDicisionUnit()))
        {
            m_pAIDecisionUnitCommonParameter->ClearSpecialFlag(BASEAI6045::FLAG_CHANGEDECISION);
            return;
        };
    };

    if (ProcAIThinkOrderResult())
    {
        m_pAIDecisionUnitCommonParameter->UpdatePlayerStatusData();
        m_pAIDecisionUnitCommonParameter->UpdateMyParameter();

        if (!m_pAIDecisionUnitCommonParameter->IsAttackTime() &&
            (m_pEnemyChr->AIThinkOrder().GetOrder() == CAIThinkOrder::ORDER_ATTACK))
        {
            CEnemyAIDecisionUnit* pDecisionUnit = CheckRunnableUnit();
            if (pDecisionUnit)
                SetNowDecisionUnit(pDecisionUnit);
        };

        if (ProcThinkingFrequency())
            ProcUpdateUnit();
    };
};


void CEnemyAIDecisionUnitManager::Draw(void)
{
#ifdef _DEBUG
    DebugDrawInfo();
    DebugDrawPatrolArea();
    DebugDrawSuitableArea();
#endif /* _DEBUG */
};


void CEnemyAIDecisionUnitManager::Attach(CEnemyAIDecisionUnit* pDecisionUnit)
{
    ASSERT(pDecisionUnit != nullptr);    
    ASSERT(m_numAIDicisionUnit < COUNT_OF(m_apAIDecisionUnit));

#ifdef _DEBUG    
    for (int32 i = 0; i < m_numAIDicisionUnit; ++i)
    {
        if (!std::strcmp(m_apAIDecisionUnit[i]->GetName(), pDecisionUnit->GetName()))
            ASSERT(false, "%s decision unit already registered!", pDecisionUnit->GetName());
    };
#endif /* _DEBUG */

    m_apAIDecisionUnit[m_numAIDicisionUnit++] = pDecisionUnit;

    pDecisionUnit->m_id = (m_numAIDicisionUnit - 1);
    pDecisionUnit->m_pAIDecisionUnitCommonParameter = m_pAIDecisionUnitCommonParameter;
    
    if (!std::strcmp(pDecisionUnit->GetName(), BASEAI6045::AIDECISIONUNITNAME::TRIGGER))
    {
        m_pAIDecisionUnitTrigger = static_cast<CEnemyAIDecisionUnitTrigger*>(pDecisionUnit);
        m_pAIDecisionUnitTrigger->InitTrigger();
    };
};


void CEnemyAIDecisionUnitManager::SetNowDecisionUnit(CEnemyAIDecisionUnit* pDecisionUnit)
{
    ASSERT(pDecisionUnit != nullptr);

    if (m_pNowDecisionUnit)
        m_pNowDecisionUnit->OnEnd();
    
    m_pNowDecisionUnit = pDecisionUnit;
    m_pNowDecisionUnit->OnStart();

    m_pAIDecisionUnitCommonParameter->SetHistory(m_pNowDecisionUnit);
};


bool CEnemyAIDecisionUnitManager::SetNowDecisionUnit(const char* pszDecisionUnitName)
{
    CEnemyAIDecisionUnit* pDecisionUnit = SearchDecisionUnit(pszDecisionUnitName);
    if (pDecisionUnit)
    {
        SetNowDecisionUnit(pDecisionUnit);
        return true;
    };

    return false;
};


CEnemyAIDecisionUnit* CEnemyAIDecisionUnitManager::SearchDecisionUnit(const char* pszDecisionUnitName)
{
    ASSERT(pszDecisionUnitName != nullptr);

    for (int32 i = 0; i < m_numAIDicisionUnit; ++i)
    {
        if (!std::strcmp(m_apAIDecisionUnit[i]->GetName(), pszDecisionUnitName))
            return m_apAIDecisionUnit[i];
    };

    return nullptr;
};


CEnemyAIDecisionUnit* CEnemyAIDecisionUnitManager::CheckRunnableUnit(void)
{
    for (int32 i = 0; i < m_numAIDicisionUnit; ++i)
    {
        if (m_apAIDecisionUnit[i]->CheckTerm())
            return m_apAIDecisionUnit[i];
    };

    return nullptr;
};


bool CEnemyAIDecisionUnitManager::ProcThinkingFrequency(void)
{
    m_fThinkTimer += CGameProperty::GetElapsedTime();

    float fThinkFrequency = m_pEnemyChr->AICharacteristic().m_fThinkingFrequency;
    fThinkFrequency = (1.0f - fThinkFrequency);

    if (fThinkFrequency <= 0.0f)
        fThinkFrequency = 0.0f;

    if (fThinkFrequency <= m_fThinkTimer)
    {
        m_fThinkTimer = 0.0f;
        return true;
    };

    m_pNowDecisionUnit = nullptr;

    if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_WAIT)
    {
        EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
        EnemyChr().AIThinkOrder().OrderWait().m_iWaitType = BASEAI6045::ORDERTYPE_WAIT_IDLE;
        EnemyChr().AIThinkOrder().OrderWait().m_fWaitTimer = 0.0f;
    };

    return false;
};


bool CEnemyAIDecisionUnitManager::ProcAIThinkOrderResult(void)
{
    if (EnemyChr().AIThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_ACCEPT)
        return true;

    if (EnemyChr().AIThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_REFUSE)
    {
        m_pAIDecisionUnitCommonParameter->ClearSpecialFlag(BASEAI6045::FLAG_MOVE_ALL);
        m_pAIDecisionUnitCommonParameter->UpdatePlayerStatusData();

        CEnemyAIDecisionUnit* pDecisionUnit = CheckRunnableUnit();
        if (pDecisionUnit)
            SetNowDecisionUnit(pDecisionUnit);

        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    };

    ENEMYTYPES::STATUS status = EnemyChr().GetStatus();
    if (CEnemyUtils::IsKnockState(status)   ||
        CEnemyUtils::IsFlyawayState(status) ||
        CEnemyUtils::IsDownState(status)    ||
        CEnemyUtils::IsStunState(status)    ||
        CEnemyUtils::IsLiftState(status))
    {
        m_pAIDecisionUnitCommonParameter->UpdateMyParameter();
    };

    return false;
};


void CEnemyAIDecisionUnitManager::ProcUpdateUnit(void)
{
    CEnemyAIDecisionUnit::RESULT result = CEnemyAIDecisionUnit::RESULT_STOP;

    if (m_pNowDecisionUnit)
        result = m_pNowDecisionUnit->Update();

    switch (result)
    {
    case CEnemyAIDecisionUnit::RESULT_END:
        {
            CEnemyAIDecisionUnit* pDecisionUnit = CheckRunnableUnit();
            if (pDecisionUnit && (pDecisionUnit->GetID() != m_pNowDecisionUnit->GetID()))
                SetNowDecisionUnit(pDecisionUnit);
        }
        break;

    case CEnemyAIDecisionUnit::RESULT_STOP:
        {
            CEnemyAIDecisionUnit* pDecisionUnit = CheckRunnableUnit();
            if (pDecisionUnit)
                SetNowDecisionUnit(pDecisionUnit);
        }
        break;

    default:
        break;
    };
};


CEnemyCharacter& CEnemyAIDecisionUnitManager::EnemyChr()
{
    return *m_pEnemyChr;
};


void CEnemyAIDecisionUnitManager::DebugDrawInfo(void)
{
#ifdef _DEBUG
    if (!CEnemyDebug::SHOW_AI_INFO)
        return;

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyPos);

    float fHeight = EnemyChr().Compositor().GetCollisionParameter().m_fHeight;

    vecMyPos.y += (fHeight * 1.5f);

    enum
    {
        INFOFLAG_NAME   = (1 << 0),
        INFOFLAG_ORDER  = (1 << 1),
        INFOFLAG_MOTION = (1 << 2),
        INFOFLAG_UNIT   = (1 << 3),
    };

    uint32 infoFlag = 0;

    char szName[128];
    szName[0] = '\0';

    char szInfoOrder[128];
    szInfoOrder[0] = '\0';

    char szMotion[128];
    szMotion[0] = '\0';

    char szDecisionUnit[128];
    szDecisionUnit[0] = '\0';

    /* get name */
    const char* pszObjName = EnemyChr().GetName();
    ASSERT(pszObjName);

    std::sprintf(szName, "%s", pszObjName);
    infoFlag |= INFOFLAG_NAME;

    /* get order info */
    static const char* s_apszOrderName [] =
    {
        "NOTHING",
        "WAIT",
        "MOVE",
        "RUN",
        "ATTACK",
        "DEFENCE",
    };

    static_assert(COUNT_OF(s_apszOrderName) == CAIThinkOrder::ORDER_MAX, "update table");

    std::sprintf(szInfoOrder, "ORDER: %s", s_apszOrderName[EnemyChr().AIThinkOrder().GetOrder()]);
    infoFlag |= INFOFLAG_ORDER;

    /* get motion info */
    const char* pszMotion = EnemyChr().Compositor().GetMotionName();
    if (pszMotion)
    {
        std::sprintf(szMotion, "MOTION: %s", pszMotion);
        infoFlag |= INFOFLAG_MOTION;
    }
    else
    {
        std::sprintf(szMotion, "NO MOTION");
        infoFlag |= INFOFLAG_MOTION;
    };

    /* get decision unit info */
    if (m_pNowDecisionUnit)
    {
        std::sprintf(szDecisionUnit, "UNIT: %s", m_pNowDecisionUnit->GetName());
        infoFlag |= INFOFLAG_UNIT;
    }
    else
    {
        std::sprintf(szDecisionUnit, "NO UNIT");
        infoFlag |= INFOFLAG_UNIT;
    };

    /* draw by info flag */
    RwRGBA color = { 0xFF, 0xFF, 0x7F, 0xFF }; // yellow

    CDebugShape::m_fLabelHeight = 14.0f;
    CDebugShape::m_fLabelOffsetY = 0.0f;

    if (infoFlag & INFOFLAG_NAME)
    {
        CDebugShape::ShowLabel(&vecMyPos, szName, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_ORDER)
    {
        CDebugShape::ShowLabel(&vecMyPos, szInfoOrder, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_MOTION)
    {
        CDebugShape::ShowLabel(&vecMyPos, szMotion, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_UNIT)
    {
        CDebugShape::ShowLabel(&vecMyPos, szDecisionUnit, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    CDebugShape::m_fLabelOffsetY = 0.0f;
#endif /* _DEBUG */
};


void CEnemyAIDecisionUnitManager::DebugDrawPatrolArea(void)
{
#ifdef _DEBUG
    if (!CEnemyDebug::SHOW_AI_PATROL_AREA)
        return;

    RwV3d vecPatrolOrigin = EnemyChr().Feature().m_vPatrolOrigin;
    float fPatrolRadius = EnemyChr().Feature().m_fPatrolRadius;

    RwSphere sphere;
    sphere.center = vecPatrolOrigin;
    sphere.radius = fPatrolRadius;

    RwRGBA color = { 0xFF, 0x00, 0x00, 0x40 };
    CDebugShape::ShowSphere(&sphere, color);
#endif /* _DEBUG */
};


void CEnemyAIDecisionUnitManager::DebugDrawSuitableArea(void)
{
#ifdef _DEBUG
    if (!CEnemyDebug::SHOW_AI_SUITABLE_AREA)
        return;

    float fDistOfSuit = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    float fRadiusOfAction = EnemyChr().AICharacteristic().m_fRadiusOfAction;

    RwV3d vecPosition = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecPosition);

    RwSphere sphere;
    sphere.center = vecPosition;
    sphere.radius = (fRadiusOfAction - fDistOfSuit);

    RwRGBA color = { 0x00, 0xFF, 0x00, 0x40 };
    CDebugShape::ShowSphere(&sphere, color);
#endif /* _DEBUG */
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitIdle::CDecisionUnitIdle(void)
: CEnemyAIDecisionUnit(BASEAI6045::AIDECISIONUNITNAME::IDLE, TYPE_WAIT)
{
    ;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitIdle::CheckTerm(void) /*override*/
{
    return true;
};


/*virtual*/ CBaseAI6045::CDecisionUnitIdle::RESULT
CBaseAI6045::CDecisionUnitIdle::Update(void) /*override*/
{
    SetIdleAIThinkOrderParameter();
    return RESULT_END;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitIdle::OnStart(void) /*override*/
{
    SetIdleAIThinkOrderParameter();
};


void CBaseAI6045::CDecisionUnitIdle::SetIdleAIThinkOrderParameter(void)
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);

    if (DecisionUnitCommonParameter().IsViewDataValid())
    {
        EnemyChr().AIThinkOrder().OrderWait().m_iPlayerNo = DecisionUnitCommonParameter().GetViewData(0).no;
        EnemyChr().AIThinkOrder().OrderWait().m_iWaitType = BASEAI6045::ORDERTYPE_WAIT_TURN_NO;
        EnemyChr().AIThinkOrder().OrderWait().m_fWaitTimer = 0.0f;
    }
    else
    {
        EnemyChr().AIThinkOrder().OrderWait().m_iWaitType = BASEAI6045::ORDERTYPE_WAIT_IDLE;
        EnemyChr().AIThinkOrder().OrderWait().m_fWaitTimer = 0.0f;
    };
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitMove::CDecisionUnitMove(void)
: CDecisionUnitMove(BASEAI6045::AIDECISIONUNITNAME::MOVE)
{
    ;
};


CBaseAI6045::CDecisionUnitMove::CDecisionUnitMove(const char* pszUnitName)
: CEnemyAIDecisionUnit(pszUnitName, TYPE_MOVE)
, m_fMoveTime(0.0f)
, m_fMoveStopDistance(0.0f)
, m_bPatrolAreaEnable(false)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fRadiusOfAction * 0.5f;
    m_fMoveStopDistance = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMove::CheckTerm(void) /*override*/
{
    if (IsDoing() && DecisionUnitCommonParameter().IsMoveTime())
        return ThinkMoveOrder();

    return false;
};


/*virtual*/ CBaseAI6045::CDecisionUnitMove::RESULT
CBaseAI6045::CDecisionUnitMove::Update(void) /*override*/
{
    if (!ThinkMoveOrder())
        return RESULT_STOP;

    SetMoveOrder();

    if (IsDoing())
        return RESULT_END;

    return RESULT_STOP;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitMove::OnStart(void) /*override*/
{
    SetMoveOrder();
    EnemyChr().AIThinkOrder().OrderMove().m_fMoveTimer = m_fMoveTime;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitMove::OnEnd(void) /*override*/
{
    DecisionUnitCommonParameter().SetMoveTimer(0.0f);
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMove::IsDoing(void)
{
    if (EnemyChr().AICharacteristic().m_fRatioOfMove <= DecisionUnitCommonParameter().GetRatioOfMove())
        return false;

    if (DecisionUnitCommonParameter().GetMoveWaitTime() > 0.0f)
        return false;

    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    if (DecisionUnitCommonParameter().GetViewData(0).distance < m_fMoveStopDistance)
        return false;

    if (m_bPatrolAreaEnable)
    {
        RwV3d vecFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecFootPos);

        if (!CAIUtils::IsInsidePatrolArea(&vecFootPos, &EnemyChr().Feature()))
            return false;
    };

    return true;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMove::ThinkMoveOrder(void)
{
    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    float fNearerView = 99999.0f;
    int32 nearerViewDataNo = -1;

    int32 numViewData = DecisionUnitCommonParameter().GetViewDataNum();
    for (int32 i = 0; i < numViewData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(i);
        float fViewValue = CAIUtils::GetAimTargetValueForNearerPlayerData(&EnemyChr(), &nearerPlayerData, 1.0f);
        if (fViewValue < fNearerView)
        {
            fNearerView = fViewValue;
            nearerViewDataNo = i;
        };        
    };

    ASSERT(nearerViewDataNo != -1);
    if (nearerViewDataNo == -1)
        return false;

    const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(nearerViewDataNo);

    m_orderTargetNo = nearerPlayerData.no;
    m_orderType = (IsMoveToRun(nearerPlayerData.distance) ? BASEAI6045::ORDERTYPE_MOVE_RUN_NO :
                                                            BASEAI6045::ORDERTYPE_MOVE_WALK_NO);

    EnemyChr().AIThinkOrder().OrderMove().m_iMoveType = m_orderType;

    return true;
};


void CBaseAI6045::CDecisionUnitMove::SetMoveOrder(void)
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
    EnemyChr().AIThinkOrder().OrderMove().m_iMoveType = m_orderType;

    if ((m_orderType == BASEAI6045::ORDERTYPE_MOVE_WALK_POS) ||
        (m_orderType == BASEAI6045::ORDERTYPE_MOVE_RUN_POS))
    {
        EnemyChr().AIThinkOrder().OrderMove().m_vAt = m_vecOrderPos;
    }
    else
    {
        EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo = m_orderTargetNo;
    };
};


bool CBaseAI6045::CDecisionUnitMove::IsMoveToRun(float fDistance)
{
    if (DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_ENABLE) &&
        !DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_MOVERUN_PERMIT))
        return false;

    BASEAI6045::SPECIAL_FLAG sflag = BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_ENABLE
                                   | BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_PERMIT;

    if (DecisionUnitCommonParameter().TestSpecialFlag(sflag))
    {
        uint8 freq = EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_REDUCTION_HP);
        if (freq == 3)
            return false;

        if ((freq == 2) ||
            (freq == 1))
            return true;
    };

    float fOffset = 0.0f;

    ENEMYTYPES::STATUS enemyStatus = EnemyChr().GetStatus();
    if (CEnemyUtils::IsMoveState(enemyStatus))
    {
        if (enemyStatus == ENEMYTYPES::STATUS_RUN)
            fOffset = 0.5f;
        else
            fOffset = -0.5f;
    };

    fDistance += fOffset;
    if (fDistance < 0.0f)
        fDistance = 0.0f;

    return (fDistance > m_fDistanceCondition);
};


void CBaseAI6045::CDecisionUnitMove::SetMoveTime(float fTime)
{
    m_fMoveTime = fTime;
};


void CBaseAI6045::CDecisionUnitMove::SetMoveStopDistance(float fDistance)
{
    m_fMoveStopDistance = fDistance;
};


void CBaseAI6045::CDecisionUnitMove::SetPatrolAreaEnable(bool bEnable)
{
    m_bPatrolAreaEnable = bEnable;
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitMove2::CDecisionUnitMove2(void)
: CDecisionUnitMove2(BASEAI6045::AIDECISIONUNITNAME::MOVE)
{
    ;
};



CBaseAI6045::CDecisionUnitMove2::CDecisionUnitMove2(const char* pszUnitName)
: CDecisionUnitMove(pszUnitName)
, m_fCheckObstacleDistane(3.0f)
, m_iCheckObstacleDistanceDivNum(3)
, m_bCheckJump(true)
, m_bHeightCorrection(false)
{
    /* This is debug only var to test all move order patterns like back or turn and so on.
       Retail move code will not work correctly mostly cases because of small height diffs -
       so by this we are correct heights at order start and end position to make it work properly.
       TODO: but anyway required correction of mark point to the wall collision point if behind it

       By default for retail compatibility this set to false and not used in game */
    m_bHeightCorrection = false;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMove2::IsDoing(void) /*override*/
{
    if (EnemyChr().AICharacteristic().m_fRatioOfMove <= DecisionUnitCommonParameter().GetRatioOfMove())
        return false;

    if (DecisionUnitCommonParameter().GetMoveWaitTime() > 0.0f)
        return false;

    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    if (m_bPatrolAreaEnable)
    {
        RwV3d vecFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecFootPos);

        if (!CAIUtils::IsInsidePatrolArea(&vecFootPos, &EnemyChr().Feature()))
            return false;
    };

    return true;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMove2::ThinkMoveOrder(void) /*override*/
{
    BASEAI6045::SPECIAL_FLAG sflag = BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_ENABLE
                                   | BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_PERMIT;

    if (DecisionUnitCommonParameter().TestSpecialFlag(sflag) &&
        (EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_REDUCTION_HP) == 1)) // TODO freq naming
    {
        return ThinkMoveOrderRunaway();
    }
    else
    {        
        uint8 freq = EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_MOVE_APPROACH);
		switch (freq)
        {
        case BASEAI6045::FREQUENCY_MOVE_STRAIGHTLINE: return ThinkMoveOrderStraightLine();
        case BASEAI6045::FREQUENCY_MOVE_TURN:         return ThinkMoveOrderTurn();
        case BASEAI6045::FREQUENCY_MOVE_BACK:         return ThinkMoveOrderBack();
        case BASEAI6045::FREQUENCY_MOVE_HITANDAWAY:   return ThinkMoveOrderHitAndAway();
        default: break;
        };
    };

    return false;
};


bool CBaseAI6045::CDecisionUnitMove2::ThinkMoveOrderBack(void)
{
    if (!DecisionUnitCommonParameter().IsViewDataValid())
    {
        ClearBackSetting();
        return false;
    };

    if (!DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_MOVE_BACK))
    {
        m_orderTargetNo = DecisionUnitCommonParameter().GetViewData(0).no;
        DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_MOVE_BACK);
    };

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
    if (!pPlayerChr)
    {
        ClearBackSetting();
        return false;
    };

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);

    RwV3d vecPlayerToMe = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecPlayerToMe, &vecFootPosMe, &vecFootPosPlayer);

    float fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    float fDist = Math::Vec3_Length(&vecPlayerToMe);
    if (fDist < fDistanceOfSuitable)
    {
        ClearBackSetting();
        return false;
    };

    float fDirDiff = std::atan2(vecPlayerToMe.x, vecPlayerToMe.z) - pPlayerChr->GetDirection();
    fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);

    /* move angle to back */
    float fDirDiffAbs = std::fabs(MATH_PI - std::fabs(fDirDiff));
    if (fDirDiffAbs < (MATH_DEG2RAD(18.0f) * 0.5f))
    {
        /* if we are behind the player order to move closely if its possible */
        m_vecOrderPos = vecFootPosPlayer;
        if (!CheckMapCollisionAndHole(&vecFootPosMe, &m_vecOrderPos))
        {
            fDist = CEnemyUtils::GetDistance(&m_vecOrderPos, &vecFootPosMe);
            m_orderType = (IsMoveToRun(fDist) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS :
                                                BASEAI6045::ORDERTYPE_MOVE_WALK_POS);
            return true;
        };

        ClearBackSetting();
        return false;
    };

    /* move angle to back
       then find average angle
       then move angle back
       then restore sign */
    float fRotDir = MATH_PI - (MATH_PI - std::fabs(fDirDiff)) * 0.5f;
    if (fDirDiff < 0.0f)
        fRotDir *= -1.0f;
    fRotDir += pPlayerChr->GetDirection();
    fRotDir = CEnemyUtils::RadianCorrect(fRotDir);
    
    fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    float fDistanceMin = (fDistanceOfSuitable * 1.25f);
	float fDistanceMax = (fDistanceOfSuitable * 2.0f);
    float fDirDiffRatio = (std::fabs(fDirDiff) * MATH_INV_PI);
    float fDistStep = ((fDistanceMax - fDistanceMin) * (1.0f - fDirDiffRatio)) + fDistanceMin;

    RwMatrix matRotY;
    RwMatrixSetIdentity(&matRotY);
    Math::Matrix_RotateY(&matRotY, fRotDir);

    RwV3d vecStep = Math::VECTOR3_AXIS_Z;
    RwV3dTransformVector(&vecStep, &vecStep, &matRotY);
    Math::Vec3_Scale(&vecStep, &vecStep, fDistStep);

    Math::Vec3_Add(&m_vecOrderPos, &vecFootPosPlayer, &vecStep);

    if (CheckMapCollisionAndHole(&vecFootPosMe, &m_vecOrderPos))
    {
        ClearBackSetting();
        return false;
    };

    m_orderType = (IsMoveToRun(fDistStep) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS :
                                            BASEAI6045::ORDERTYPE_MOVE_WALK_POS);

    return true;
};


bool CBaseAI6045::CDecisionUnitMove2::ThinkMoveOrderHitAndAway(void)
{
    if (!DecisionUnitCommonParameter().IsViewDataValid())
    {
        ClearHitAndAwaySetting();
        return false;
    };

    if (!DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_MOVE_HITANDAWAY))
        return ThinkMoveOrderStraightLine();

    float fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    fDistanceOfSuitable *= 2.0f;

    if (!CalcRunawayPoint(&m_vecOrderPos, fDistanceOfSuitable))
    {
        ClearHitAndAwaySetting();
        return false;
    };

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    if (CEnemyUtils::GetDistance(&vecFootPos, &m_vecOrderPos) < 0.25f)
    {
        ClearHitAndAwaySetting();
        DecisionUnitCommonParameter().SetMoveWaitTime(0.75f);
        return false;
    };

    if (CheckMapCollisionAndHole(&vecFootPos, &m_vecOrderPos))
    {
        ClearHitAndAwaySetting();
        return false;
    };

    m_orderType = (IsMoveToRun(1000.0f) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS :
                                          BASEAI6045::ORDERTYPE_MOVE_WALK_POS);
    return true;
};


bool CBaseAI6045::CDecisionUnitMove2::ThinkMoveOrderRunaway(void)
{
    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    float fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    fDistanceOfSuitable *= 2.0f;

    if (!CalcRunawayPoint(&m_vecOrderPos, fDistanceOfSuitable))
        return false;
    
    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    if (CEnemyUtils::GetDistance(&vecFootPos, &m_vecOrderPos) < 0.25f)
        return false;

    if (CheckMapCollisionAndHole(&vecFootPos, &m_vecOrderPos))
        return false;

    m_orderType = (IsMoveToRun(1000.0f) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS :
                                          BASEAI6045::ORDERTYPE_MOVE_WALK_POS);

    return true;
};


bool CBaseAI6045::CDecisionUnitMove2::ThinkMoveOrderStraightLine(void)
{
   if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    int32 numPlayerInViewArea = DecisionUnitCommonParameter().GetViewDataNum();
    for (int32 i = 0; i < numPlayerInViewArea; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(i);

        if (nearerPlayerData.distance < m_fMoveStopDistance)
        {
            if (!CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(), nearerPlayerData.no, false))
                break;
        };

        RwV3d vecStart = Math::VECTOR3_ZERO;
        RwV3d vecEnd = Math::VECTOR3_ZERO;
        if (GetCheckObstacleLine(nearerPlayerData.no, &vecStart, &vecEnd))
        {
            float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
            float fMapHitRadius = CAIUtils::GetMapHitRadius(&EnemyChr().Compositor());            

            CAIUtils::CHECKMOVELINE_RESULT checkMoveLineResult = CAIUtils::CheckMoveLine(&vecStart,
                                                                                         &vecEnd,
                                                                                         fMapHitRadius,
                                                                                         m_iCheckObstacleDistanceDivNum,
                                                                                         fJumpInitSpeed);

            if (checkMoveLineResult == CAIUtils::CHECKMOVELINE_RESULT_PERMISSION)
            {
                m_orderTargetNo = nearerPlayerData.no;
                m_orderType = (IsMoveToRun(nearerPlayerData.distance) ? BASEAI6045::ORDERTYPE_MOVE_RUN_NO :
                                                                        BASEAI6045::ORDERTYPE_MOVE_WALK_NO);
                return true;
            }
            else if (checkMoveLineResult == CAIUtils::CHECKMOVELINE_RESULT_NEEDJUMP)
            {
                if (m_bCheckJump && (i == 0))
                    return false;
            };
        };
    };

    const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(0);

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(nearerPlayerData.no);
    if (!pPlayerChr)
        return false;

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);

    float fRadiusOfAction = EnemyChr().AICharacteristic().m_fRadiusOfAction;
    RwV3d vecMovePos = Math::VECTOR3_ZERO;
    float fDistance = CAIUtils::CalcMovePointEscapeObstacle(&vecMovePos,
                                                            &EnemyChr(),
                                                            &vecFootPosPlayer,
                                                            1.0f,
                                                            fRadiusOfAction);
    if (fDistance >= 0.0f)
    {
        m_vecOrderPos = vecMovePos;
        m_orderType = (IsMoveToRun(fDistance) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS :
                                                BASEAI6045::ORDERTYPE_MOVE_WALK_POS);

        return true;
    };

    return false;
};


bool CBaseAI6045::CDecisionUnitMove2::ThinkMoveOrderTurn(void)
{
    if (!DecisionUnitCommonParameter().IsViewDataValid())
    {
        ClearTurnSetting();
        return false;
    };

    if (!DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_MOVE_TURN))
    {
        m_orderTargetNo = DecisionUnitCommonParameter().GetViewData(0).no;
        DecisionUnitCommonParameter().SetSpecialFlag(BASEAI6045::FLAG_MOVE_TURN);
    };

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_orderTargetNo);
    if (!pPlayerChr)
    {
        ClearTurnSetting();
        return false;
    };

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);

    RwV3d vecPlayerToMe = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecPlayerToMe, &vecFootPosMe, &vecFootPosPlayer);

    float fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    float fDist = Math::Vec3_Length(&vecPlayerToMe);
    if (fDist < fDistanceOfSuitable)
    {
        ClearTurnSetting();
        return false;
    };

    /* player direction diff to me */
    float fDirDiff = std::atan2(vecPlayerToMe.x, vecPlayerToMe.z) - pPlayerChr->GetDirection();
    fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);

    /* move angle to the side */
    float fDirDiffAbs = std::fabs((MATH_PI * 0.5f) - std::fabs(fDirDiff));
    if (fDirDiffAbs < (MATH_DEG2RAD(18.0f) * 0.5f))
    {
        /* if we are on the side - order to move closely if its possible */
        m_vecOrderPos = vecFootPosPlayer;
        if (!CheckMapCollisionAndHole(&vecFootPosMe, &m_vecOrderPos))
        {
            fDist = CEnemyUtils::GetDistance(&m_vecOrderPos, &vecFootPosMe);
            m_orderType = (IsMoveToRun(fDist) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS : 
                                                BASEAI6045::ORDERTYPE_MOVE_WALK_POS);
            return true;
        };

        ClearTurnSetting();
        return false;
    };

    /* move angle to the side
       then find average angle
       then move angle back
       then restore sign */
    float fRotDir = (MATH_PI * 0.5f) - (((MATH_PI * 0.5f) - std::fabs(fDirDiff)) * 0.5f);
    if (fDirDiff < 0.0f)
        fRotDir *= -1.0f;    
    fRotDir += pPlayerChr->GetDirection();
    fRotDir = CEnemyUtils::RadianCorrect(fRotDir);

    fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    float fDistanceMin = (fDistanceOfSuitable * 1.25f);
    float fDistanceMax = (fDistanceOfSuitable * 2.0f);
    float fDirDiffRatio = (std::fabs(fDirDiff) * (MATH_INV_PI * 2.0f));
    float fDistStep = ((fDistanceMax - fDistanceMin) * (1.0f - fDirDiffRatio)) + fDistanceMin;

    RwMatrix matRotY;
    RwMatrixSetIdentity(&matRotY);
    Math::Matrix_RotateY(&matRotY, fRotDir);

    RwV3d vecStep = Math::VECTOR3_AXIS_Z;
    RwV3dTransformVector(&vecStep, &vecStep, &matRotY);
    Math::Vec3_Scale(&vecStep, &vecStep, fDistStep);

    Math::Vec3_Add(&m_vecOrderPos, &vecFootPosPlayer, &vecStep);

    if (CheckMapCollisionAndHole(&vecFootPosMe, &m_vecOrderPos))
    {
        ClearTurnSetting();
        return false;
    };

    m_orderType = (IsMoveToRun(fDistStep) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS :
                                            BASEAI6045::ORDERTYPE_MOVE_WALK_POS);
    return true;
};


void CBaseAI6045::CDecisionUnitMove2::ClearTurnSetting(void)
{
    DecisionUnitCommonParameter().ClearSpecialFlag(BASEAI6045::FLAG_MOVE_TURN);
};


void CBaseAI6045::CDecisionUnitMove2::ClearHitAndAwaySetting(void)
{
    DecisionUnitCommonParameter().ClearSpecialFlag(BASEAI6045::FLAG_MOVE_HITANDAWAY);
};


void CBaseAI6045::CDecisionUnitMove2::ClearBackSetting(void)
{
    DecisionUnitCommonParameter().ClearSpecialFlag(BASEAI6045::FLAG_MOVE_BACK);
};


bool CBaseAI6045::CDecisionUnitMove2::GetCheckObstacleLine(int32 iPlayerNo, RwV3d* pvecStart, RwV3d* pvecEnd)
{
    ASSERT(pvecStart != nullptr);
    ASSERT(pvecEnd != nullptr);

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerNo);
    if (!pPlayerChr)
        return false;

    RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosEnemy);
    vecFootPosEnemy.y += 0.2f;

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);
    vecFootPosPlayer.y = vecFootPosEnemy.y;

    CAIUtils::GetLineForRange(pvecStart, pvecEnd, &vecFootPosEnemy, &vecFootPosPlayer, m_fCheckObstacleDistane);
    return true;
};


bool CBaseAI6045::CDecisionUnitMove2::CheckMapCollisionAndHole(RwV3d* pvecStart, RwV3d* pvecEnd)
{
    ASSERT(pvecStart != nullptr);
    ASSERT(pvecEnd != nullptr);
    
    RwV3d vecStart = *pvecStart;
    vecStart.y += 0.1f;

    RwV3d vecEnd = *pvecEnd;
    vecEnd.y += 0.1f;

    //CDebugShape::DurationPush(2.0f);
    //CDebugShape::ShowSphere(vecStart, { 255, 0, 0, 255 });
    //CDebugShape::ShowSphere(vecEnd, { 0, 255, 0, 255 });
    //CDebugShape::DurationPop();

    float fMapHitRadius = CAIUtils::GetMapHitRadius(&EnemyChr().Compositor());
    if (CAIUtils::CheckMapCollisionThick(&vecStart, &vecEnd, fMapHitRadius))
    {
        if (m_bHeightCorrection)
        {
            float fHeightDiff = std::fabs(vecStart.y - vecEnd.y);
            if (fHeightDiff > 0.75f)
                return true;
        }
        else
        {
            return true;
        };
    };

    if (CAIUtils::CheckHole(&vecStart, &vecEnd, m_iCheckObstacleDistanceDivNum))
        return true;

    return false;
};


bool CBaseAI6045::CDecisionUnitMove2::CalcRunawayPoint(RwV3d* pvecRunawayPoint, float fRunawayDistance)
{
    ASSERT(pvecRunawayPoint != nullptr);

    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    RwV3d vecAllPlayerPos = Math::VECTOR3_ZERO;
    if (!DecisionUnitCommonParameter().CalcGroundPointViewAreaPlayer(&vecAllPlayerPos))
        return false;

    RwV3d vecEnemyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecEnemyPos);

    RwV3d vecAway = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecAway, &vecEnemyPos, &vecAllPlayerPos);
    Math::Vec3_Normalize(&vecAway, &vecAway);
    Math::Vec3_Scale(&vecAway, &vecAway, fRunawayDistance);

    *pvecRunawayPoint = vecAllPlayerPos;
    Math::Vec3_Add(pvecRunawayPoint, pvecRunawayPoint, &vecAway);

    return true;
};


void CBaseAI6045::CDecisionUnitMove2::SetCheckObstacleDistance(float fDist)
{
    m_fCheckObstacleDistane = fDist;
};


void CBaseAI6045::CDecisionUnitMove2::SetCheckObstacleDistanceDivNum(int32 iDivNum)
{
    m_iCheckObstacleDistanceDivNum = iDivNum;
};


void CBaseAI6045::CDecisionUnitMove2::SetCheckJump(bool bState)
{
    m_bCheckJump = bState;
};


void CBaseAI6045::CDecisionUnitMove2::SetHeightCorrection(bool bHeightCorrection)
{
    m_bHeightCorrection = bHeightCorrection;
};


//
// *********************************************************************************
//


/*virtual*/ bool CBaseAI6045::CDecisionUnitMoveBoss::IsDoing(void) /*override*/
{
    if (EnemyChr().AICharacteristic().m_fRatioOfMove <= DecisionUnitCommonParameter().GetRatioOfMove())
        return false;

    if (DecisionUnitCommonParameter().GetMoveWaitTime() > 0.0f)
        return false;

    if (!DecisionUnitCommonParameter().IsEnableDataValid())
        return false;

    return true;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMoveBoss::ThinkMoveOrder(void) /*override*/
{
    return ThinkMoveOrderStraightLineForBoss();
};


bool CBaseAI6045::CDecisionUnitMoveBoss::ThinkMoveOrderStraightLineForBoss(void)
{
    if (!DecisionUnitCommonParameter().IsEnableDataValid())
        return false;

    RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

    int32 numPlayerEnable = DecisionUnitCommonParameter().GetEnableDataNum();
    for (int32 i = 0; i < numPlayerEnable; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetEnableData(i);

        if (nearerPlayerData.distance < m_fMoveStopDistance)
        {
            if (!CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(), nearerPlayerData.no, false))
                break;
        };

        RwV3d vecStart = Math::VECTOR3_ZERO;
        RwV3d vecEnd = Math::VECTOR3_ZERO;
        if (GetCheckObstacleLine(nearerPlayerData.no, &vecStart, &vecEnd))
        {
            float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
            float fMapHitRadius = CAIUtils::GetMapHitRadius(&EnemyChr().Compositor());            

            CAIUtils::CHECKMOVELINE_RESULT checkMoveLineResult = CAIUtils::CheckMoveLine(&vecStart,
                                                                                         &vecEnd,
                                                                                         fMapHitRadius,
                                                                                         m_iCheckObstacleDistanceDivNum,
                                                                                         fJumpInitSpeed);

            if (checkMoveLineResult == CAIUtils::CHECKMOVELINE_RESULT_PERMISSION)
            {
                m_orderTargetNo = nearerPlayerData.no;
                m_orderType = (IsMoveToRun(nearerPlayerData.distance) ?
                                BASEAI6045::ORDERTYPE_MOVE_RUN_NO :
                                BASEAI6045::ORDERTYPE_MOVE_WALK_NO);

                return true;
            }
            else if (checkMoveLineResult == CAIUtils::CHECKMOVELINE_RESULT_NEEDJUMP)
            {
                if (m_bCheckJump && (i == 0))
                    return false;
            };
        };
    };

    return false;
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitMovePatrolOrigin::CDecisionUnitMovePatrolOrigin(void)
: CDecisionUnitMove2(BASEAI6045::AIDECISIONUNITNAME::MOVE_PATROLORIGIN)
, m_fTimer(0.0f)
, m_fWaitTimer(3.0f)
{
    ;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMovePatrolOrigin::CheckTerm(void) /*override*/
{
    if (IsDoing() && ThinkMoveOrderPatrolOrigin())
    {
        if (m_fTimer <= m_fWaitTimer)
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            return false;
        };

        return true;
    };

    m_fTimer = 0.0f;
    return false;
};


/*virtual*/ CBaseAI6045::CDecisionUnitMovePatrolOrigin::RESULT
CBaseAI6045::CDecisionUnitMovePatrolOrigin::Update(void) /*override*/
{
    if (!IsDoing())
        return RESULT_STOP;

    if (!ThinkMoveOrderPatrolOrigin())
        return RESULT_STOP;

    SetMoveOrder();
    return RESULT_CONTINUE;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitMovePatrolOrigin::OnEnd(void) /*override*/
{
    CDecisionUnitMove2::OnEnd();

    m_fTimer = 0.0f;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMovePatrolOrigin::IsDoing(void) /*override*/
{
    if (IsPlayerPatrolArea())
        return false;

    return !IsReachPatrolOrigin();
};


bool CBaseAI6045::CDecisionUnitMovePatrolOrigin::ThinkMoveOrderPatrolOrigin(void)
{
    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    RwV3d vecPatrolOrg = EnemyChr().Feature().m_vPatrolOrigin;

    RwV3d vecStart = Math::VECTOR3_ZERO;
    RwV3d vecEnd = Math::VECTOR3_ZERO;
    CAIUtils::GetLineForRange(&vecStart, &vecEnd, &vecFootPos, &vecPatrolOrg, m_fCheckObstacleDistane);

    float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
    float fMapHitRadius = CAIUtils::GetMapHitRadius(&EnemyChr().Compositor());
    CAIUtils::CHECKMOVELINE_RESULT checkMoveLineResult = CAIUtils::CheckMoveLine(&vecStart,
                                                                                 &vecEnd,
                                                                                 fMapHitRadius,
                                                                                 m_iCheckObstacleDistanceDivNum,
                                                                                 fJumpInitSpeed);

    if (checkMoveLineResult == CAIUtils::CHECKMOVELINE_RESULT_PERMISSION)
    {
        float fDistance = CEnemyUtils::GetDistance(&vecStart, &vecEnd);

        m_orderType = IsMoveToRun(fDistance) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS :
                                               BASEAI6045::ORDERTYPE_MOVE_WALK_POS;
        m_vecOrderPos = vecEnd;

        return true;
    }
    else if (checkMoveLineResult == CAIUtils::CHECKMOVELINE_RESULT_NEEDJUMP)
    {
        m_orderType = BASEAI6045::ORDERTYPE_MOVE_JUMP;
        m_vecOrderPos = vecEnd;

        return true;
    };

    float fRadiusOfAction = EnemyChr().AICharacteristic().m_fRadiusOfAction;
    RwV3d vecMovePt = Math::VECTOR3_ZERO;
    float fDist = CAIUtils::CalcMovePointEscapeObstacle(&vecMovePt,
                                                        &EnemyChr(),
                                                        &vecPatrolOrg,
                                                        1.0f,
                                                        fRadiusOfAction);
    if (fDist < 0.0f)
        return false;

    m_orderType = IsMoveToRun(fDist) ? BASEAI6045::ORDERTYPE_MOVE_RUN_POS :
                                       BASEAI6045::ORDERTYPE_MOVE_WALK_POS;
    m_vecOrderPos = vecMovePt;

    return true;
};


bool CBaseAI6045::CDecisionUnitMovePatrolOrigin::IsPlayerPatrolArea(void)
{
    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (!pPlayerChr)
            continue;

        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);

        if (CAIUtils::IsInsidePatrolArea(&vecFootPosPlayer, &EnemyChr().Feature()))
            return true;
    };

    return false;
};


bool CBaseAI6045::CDecisionUnitMovePatrolOrigin::IsReachPatrolOrigin(float* pfDist /*= nullptr*/)
{
    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    RwV3d vecPatrolOrg = EnemyChr().Feature().m_vPatrolOrigin;

    RwV3d vecDistance = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDistance, &vecPatrolOrg, &vecFootPos);

    float fDistance = Math::Vec3_Length(&vecDistance);
    if (pfDist)
        *pfDist = fDistance;

    return (fDistance < 0.5f);
};


void CBaseAI6045::CDecisionUnitMovePatrolOrigin::SetWaitTimer(float fTimer)
{
    m_fWaitTimer = fTimer;
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitMoveObstacleJump::CDecisionUnitMoveObstacleJump(void)
: CEnemyAIDecisionUnit(BASEAI6045::AIDECISIONUNITNAME::MOVE_OBSTACLEJUMP, TYPE_MOVE)
, m_fCheckObstacleDistance(3.0f)
{
    ;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitMoveObstacleJump::CheckTerm(void) /*override*/
{
    if (EnemyChr().AICharacteristic().m_fRatioOfMove <= DecisionUnitCommonParameter().GetRatioOfMove())
        return false;

    if (DecisionUnitCommonParameter().GetMoveWaitTime() > 0.0f)
        return false;

    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    float fObstacleHeight = 0.0f;
    if (!CAIUtils::CheckStep(&EnemyChr().Compositor(), m_fCheckObstacleDistance, &fObstacleHeight))
        return false;

    float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    if (Math::FEqual(vecFootPos.y, fObstacleHeight))
        return false;

    float fGravity = CGameProperty::GetGravity();
    float fJumpHeight = CEnemyUtils::GetJumpableHeight(fJumpInitSpeed, fGravity);
    if ((fObstacleHeight - vecFootPos.y) > fJumpHeight)
        return false;

    RwV3d vecDistance = { 0.0f, 0.0f, m_fCheckObstacleDistance };
    EnemyChr().Compositor().RotateVectorByDirection(&vecDistance, &vecDistance);

    RwV3d vecStart = vecFootPos;
    vecStart.y = (fObstacleHeight + 0.1f);

    RwV3d vecEnd = vecStart;
    Math::Vec3_Add(&vecEnd, &vecEnd, &vecDistance);

    if (CWorldMap::CheckCollisionLine(&vecStart, &vecEnd))
        return false;

    return true;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitMoveObstacleJump::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_MOVE);
    EnemyChr().AIThinkOrder().OrderMove().m_iMoveType = BASEAI6045::ORDERTYPE_MOVE_JUMP;
};


void CBaseAI6045::CDecisionUnitMoveObstacleJump::SetCheckObstacleDistance(float fDist)
{
    m_fCheckObstacleDistance = fDist;
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitGuard::CDecisionUnitGuard(void)
: CEnemyAIDecisionUnit(BASEAI6045::AIDECISIONUNITNAME::GUARD, TYPE_GUARD)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(90.0f);
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitGuard::CheckTerm(void) /*override*/
{
    if (!IsDoing())
        return false;

    if ((EnemyChr().GetStatus() != ENEMYTYPES::STATUS_GUARD) && m_bFrequencyEnable)
    {
        float fChance = static_cast<float>(EnemyChr().FrequencyParameter(m_frequencyTableNo));

        if (DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_ENABLE) &&
            DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_PERMIT))
        {
            uint8 freq = EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_REDUCTION_HP);
            if (freq == 2) // TODO freq naming
                fChance *= 0.5f;
            else if (freq == 3)
                fChance *= 1.5f;
        };

        if ((Math::RandFloat() * 100.0f) > fChance)
            return false;
    };

    return true;
};


/*virtual*/ CBaseAI6045::CDecisionUnitGuard::RESULT
CBaseAI6045::CDecisionUnitGuard::Update(void) /*override*/
{
    if (DecisionUnitCommonParameter().PlayerWatcher().IsAttackWithinRange(m_fDistanceCondition, MATH_PI2))
        return RESULT_CONTINUE;

    return RESULT_STOP;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitGuard::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_DEFENCE);
};


/*virtual*/ void CBaseAI6045::CDecisionUnitGuard::OnEnd(void) /*override*/
{
    ;
};


bool CBaseAI6045::CDecisionUnitGuard::IsDoing(void)
{
    float fAngle = MATH_PI2;
    if (!EnemyChr().TestFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE))
        fAngle /= 3.0f;

    if (DecisionUnitCommonParameter().PlayerWatcher().IsShootShurikenTrigger(25.0f))
        return true;

    if (DecisionUnitCommonParameter().PlayerWatcher().IsAttackTriggerWithinRange(m_fDistanceCondition, fAngle))
        return true;

    return false;
};


//
// *********************************************************************************
//


/*virtual*/ bool CBaseAI6045::CDecisionUnitGuard2::CheckTerm(void) /*override*/
{
    float fAngle = MATH_PI2;
    if (!EnemyChr().TestFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE))
        fAngle /= 3.0f;

    float fChance = 1.0f;

    if (DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_ENABLE) &&
        DecisionUnitCommonParameter().TestSpecialFlag(BASEAI6045::FLAG_TRIGGER_REDUCTIONHP_PERMIT))
    {
        uint8 freq = EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_TRIGGER_REDUCTION_HP);
        if (freq == 2) // TODO freq naming
            fChance *= 0.5f;
        else if (freq == 3)
            fChance *= 1.5f;
    };

    int32 numPlayers = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayers; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (!pPlayerChr)
            continue;

        PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
        CAIUtils::PLAYER_STATE_FLAG stateFlag = CAIUtils::GetPlayerStateFlag(playerStatus);

        bool bTrigger = false;

        if (stateFlag & CAIUtils::PLAYER_STATE_KNIFE)
            bTrigger = DecisionUnitCommonParameter().PlayerWatcher().IsPlayerShootShurikenTrigger(i, 25.0f);
        else
            bTrigger = DecisionUnitCommonParameter().PlayerWatcher().IsPlayerAttackTriggerWithinRange(i, m_fDistanceCondition, fAngle);

        if (bTrigger)
        {
            int32 freqTableNo = -1;
            switch (playerStatus)
            {
            case PLAYERTYPES::STATUS_DASH:
            case PLAYERTYPES::STATUS_DASH_FINISH:
            case PLAYERTYPES::STATUS_ATTACK_RUN:
                freqTableNo = CEnemyParameter::FREQUENCY_GUARD_RUN_ATTACK;
                break;

            case PLAYERTYPES::STATUS_ATTACK_A:
            case PLAYERTYPES::STATUS_ATTACK_AA:
            case PLAYERTYPES::STATUS_ATTACK_AAB:
            case PLAYERTYPES::STATUS_ATTACK_AAC:
            case PLAYERTYPES::STATUS_ATTACK_AABB:
            case PLAYERTYPES::STATUS_ATTACK_AABC:
            case PLAYERTYPES::STATUS_ATTACK_AABBB:
            case PLAYERTYPES::STATUS_ATTACK_AABBC:
                freqTableNo = CEnemyParameter::FREQUENCY_GUARD_A;
                break;

            case PLAYERTYPES::STATUS_ATTACK_B_CHARGE:
            case PLAYERTYPES::STATUS_ATTACK_B:
                freqTableNo = CEnemyParameter::FREQUENCY_GUARD_B;
                break;

            case PLAYERTYPES::STATUS_ATTACK_JUMP:
                freqTableNo = CEnemyParameter::FREQUENCY_GUARD_JUMP_ATTACK;
                break;

            case PLAYERTYPES::STATUS_ATTACK_KNIFE:
            case PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP:
            case PLAYERTYPES::STATUS_THROWN_COMBINATION:
            case PLAYERTYPES::STATUS_THROWN_COMBINATION_END:
            case PLAYERTYPES::STATUS_THROWN_MISS:
                freqTableNo = CEnemyParameter::FREQUENCY_GUARD_SHOT;
                break;

            case PLAYERTYPES::STATUS_LIFT_CHALLENGE:
            case PLAYERTYPES::STATUS_THROW:
            case PLAYERTYPES::STATUS_THROW_BACK:
            case PLAYERTYPES::STATUS_THROW_COMBINATION:
                freqTableNo = CEnemyParameter::FREQUENCY_GUARD_THROW;
                break;

            default:
                break;
            };

            if (freqTableNo != -1)
            {
                float fFreq = static_cast<float>(EnemyChr().FrequencyParameter(freqTableNo));
                fFreq *= fChance;

                if ((Math::RandFloat() * 100.0f) <= fFreq)
                    return true;
            };
        };
    };

    return false;
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitAttackNormal::CDecisionUnitAttackNormal(const char* pszUnitName)
: CEnemyAIDecisionUnit(pszUnitName, TYPE_ATTACK)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_NORMAL;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitAttackNormal::CheckTerm(void) /*override*/
{
    if (!DecisionUnitCommonParameter().IsAttackPermission())
        return false;

    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    int32 numViewData = DecisionUnitCommonParameter().GetViewDataNum();
    for (int32 i = 0; i < numViewData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(i);

        bool bIsSatisfyDist = (nearerPlayerData.distance <= m_fDistanceCondition);
        bool bIsSatisfyAngle = IsSatisfyAngleCondition(&nearerPlayerData);

        if (bIsSatisfyDist &&
            bIsSatisfyAngle)
        {
            if (!CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(), nearerPlayerData.no, false))
            {
                if (!IsSatisfyFrequency(m_frequencyTableNo))
                    return false;

                m_orderTargetNo = nearerPlayerData.no;
                return true;
            };
        };
    };

    return false;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitAttackNormal::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
    EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = m_orderType;
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = m_orderTargetNo;

    DecisionUnitCommonParameter().SetAttackTimer(0.0f);
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus::CDecisionUnitAttackNormalForUnusualStatus(const char* pszUnitName)
: CDecisionUnitAttackNormal(pszUnitName)
{
    ;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitAttackNormalForUnusualStatus::CheckTerm(void) /*override*/
{
    if (!DecisionUnitCommonParameter().IsAttackPermission())
        return false;

    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    int32 numViewData = DecisionUnitCommonParameter().GetViewDataNum();
    for (int32 i = 0; i < numViewData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetViewData(i);

        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(nearerPlayerData.no);
        if (!pPlayerChr)
            continue;

        PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
        CAIUtils::PLAYER_STATE_FLAG playerStateFlag = CAIUtils::GetPlayerStateFlag(playerStatus);

        if ((playerStateFlag & CAIUtils::PLAYER_STATE_UNUSUAL) &&
            (nearerPlayerData.distance <= m_fDistanceCondition) &&
            IsSatisfyAngleCondition(&nearerPlayerData))
        {
            if (!CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(), nearerPlayerData.no, false))
            {
                if (!IsSatisfyFrequency(m_frequencyTableNo))
                    return false;
                
                m_orderTargetNo = nearerPlayerData.no;
                return true;
            };
        };
    };

    return false;
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitAttackToAir::CDecisionUnitAttackToAir(void)
: CEnemyAIDecisionUnit(BASEAI6045::AIDECISIONUNITNAME::ATTACK_TOAIR, TYPE_ATTACK)
, m_fHeightLow(0.5f)
, m_fHeightHigh(3.0f)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_TO_AIR;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitAttackToAir::CheckTerm(void) /*override*/
{
    if (!DecisionUnitCommonParameter().IsAttackPermission())
        return false;

    if (!DecisionUnitCommonParameter().PlayerWatcher().IsHighAttack(m_fDistanceCondition, m_fHeightLow, m_fHeightHigh))
        return false;

    if (!DecisionUnitCommonParameter().IsEnableDataValid())
        return false;
    
    const CPlayerWatcher::PLAYERDATA& nearerPlrDataWatch = DecisionUnitCommonParameter().PlayerWatcher().GetPlayerData(CPlayerWatcher::PLAYER_DATA_NEAR);
    
    int32 numEnableData = DecisionUnitCommonParameter().GetEnableDataNum();
    for (int32 i = 0; i < numEnableData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlrDataEnable = DecisionUnitCommonParameter().GetEnableData(i);

        if (nearerPlrDataEnable.no == nearerPlrDataWatch.no)
        {
            if (!IsSatisfyAngleCondition(&nearerPlrDataEnable))
                return false;
            
            if (!IsSatisfyFrequency(m_frequencyTableNo))
                return false;
        
            m_orderTargetNo = nearerPlrDataWatch.no;
            return true;
        };
    };

    return false;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitAttackToAir::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
    EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = m_orderType;
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = m_orderTargetNo;

    DecisionUnitCommonParameter().SetAttackTimer(0.0f);
};


void CBaseAI6045::CDecisionUnitAttackToAir::SetHeightLow(float fHeightLow)
{
    m_fHeightLow = fHeightLow;
};


void CBaseAI6045::CDecisionUnitAttackToAir::SetHeightHigh(float fHeightHigh)
{
    m_fHeightHigh = fHeightHigh;
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitAttackOverInterval::CDecisionUnitAttackOverInterval(const char* pszUnitName)
: CEnemyAIDecisionUnit(pszUnitName, TYPE_ATTACK)
, m_attackPermissionPlayerNum(-1)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_fAngleCondition    = MATH_DEG2RAD(45.0f);
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_NORMAL;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitAttackOverInterval::CheckTerm(void) /*override*/
{
    if (!DecisionUnitCommonParameter().IsAttackPermission())
        return false;

    if (!DecisionUnitCommonParameter().IsEnableDataValid())
        return false;

    int32 numEnableOutOfDist = 0;
    int32 firstOutOfDistEnableNo = -1;

    int32 numPlayerEnable = DecisionUnitCommonParameter().GetEnableDataNum();
    for (int32 i = 0; i < numPlayerEnable; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlayerData = DecisionUnitCommonParameter().GetEnableData(i);
        if (nearerPlayerData.distance > m_fDistanceCondition)
        {
            ++numEnableOutOfDist;
            if (firstOutOfDistEnableNo < 0)
                firstOutOfDistEnableNo = i;
        };
    };

    int32 attackPermissionPlayerNum = m_attackPermissionPlayerNum;
    if (attackPermissionPlayerNum < 0)
        attackPermissionPlayerNum = numPlayerEnable;

    if (numEnableOutOfDist < attackPermissionPlayerNum)
        return false;

    if (!IsSatisfyFrequency(m_frequencyTableNo))
        return false;
    
    m_orderTargetNo = DecisionUnitCommonParameter().GetEnableData(firstOutOfDistEnableNo).no;
    return true;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitAttackOverInterval::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
    EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = m_orderType;
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = m_orderTargetNo;

    DecisionUnitCommonParameter().SetAttackTimer(0.0f);
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitAttackCounter::CDecisionUnitAttackCounter(const char* pszUnitName)
: CEnemyAIDecisionUnit(pszUnitName, TYPE_ATTACK)
, m_attackPermissionPlayerNum(2)
{
    m_fDistanceCondition = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_orderType          = BASEAI6045::ORDERTYPE_ATTACK_COUNTER;
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitAttackCounter::CheckTerm(void) /*override*/
{
    if (!DecisionUnitCommonParameter().IsAttackPermission())
        return false;

    if (!DecisionUnitCommonParameter().IsViewDataValid())
        return false;

    int32 inViewPlayerNum = GetDistanceConditionViewAreaPlayerNum();
    if (inViewPlayerNum < m_attackPermissionPlayerNum)
        return false;

    if (!IsSatisfyFrequency(m_frequencyTableNo))
        return false;

    m_orderTargetNo = DecisionUnitCommonParameter().GetViewData(0).no;
    return true;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitAttackCounter::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
    EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = m_orderType;
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = m_orderTargetNo;

    DecisionUnitCommonParameter().SetAttackTimer(0.0f);
};


void CBaseAI6045::CDecisionUnitAttackCounter::ResetTarget(void)
{
    m_orderTargetNo = -1;
};


//
// *********************************************************************************
//


CBaseAI6045::CDecisionUnitAttackThrow::CDecisionUnitAttackThrow(void)
: CDecisionUnitAttackThrow(BASEAI6045::AIDECISIONUNITNAME::ATTACK_THROW)
{
    ;
};


CBaseAI6045::CDecisionUnitAttackThrow::CDecisionUnitAttackThrow(const char* pszUnitName)
: CEnemyAIDecisionUnit(pszUnitName, TYPE_ATTACK)
{
    m_orderType = BASEAI6045::ORDERTYPE_ATTACK_THROW;
	m_fDistanceCondition = 3.0f;
	m_fAngleCondition = MATH_DEG2RAD(45.0f);
};


/*virtual*/ bool CBaseAI6045::CDecisionUnitAttackThrow::CheckTerm(void) /*override*/
{
    if (!m_pAIDecisionUnitCommonParameter->IsAttackPermission())
        return false;

    int32 numViewData = m_pAIDecisionUnitCommonParameter->GetViewDataNum();
    for (int32 i = 0; i < numViewData; ++i)
    {
        const CAIUtils::NEARER_PLAYERDATA& nearerPlrData = m_pAIDecisionUnitCommonParameter->GetViewData(i);

        /* check distance */
        if (nearerPlrData.distance > m_fDistanceCondition)
            continue;

        /* check angle */
        if (!IsSatisfyAngleCondition(&nearerPlrData))
            continue;

        /* check obstacle between target and me */
        bool bIsObstacleBetween = CAIUtils6045::CheckObstacleBetweenEnemyToPlayer(&EnemyChr().Compositor(),
                                                                                  nearerPlrData.no,
                                                                                  false);

        if (bIsObstacleBetween)
            continue;

        /* obtain target character and check for SP charging */
        CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(nearerPlrData.no);
        ASSERT(pPlrChr != nullptr);

        if (pPlrChr->GetStatus() == PLAYERTYPES::STATUS_ATTACK_B_CHARGE)
            continue;

        /* check for valid height & freq */
        RwV3d vecFootPosMe = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecFootPosMe);

        RwV3d vecFootPosPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetFootPosition(&vecFootPosPlr);

        RwV3d vecVelocityPlr = Math::VECTOR3_ZERO;
        pPlrChr->GetVelocity(&vecVelocityPlr);

        float fHeightDiff = (vecFootPosPlr.y - vecFootPosMe.y);
        fHeightDiff = std::fabs(fHeightDiff);

        bool bIsPlrHeightValid = (fHeightDiff < 0.3f);
        bool bIsPlrFalling = (vecVelocityPlr.y <= 0.0f);
        bool bIsSatisfyFreq = (IsSatisfyFrequency(m_frequencyTableNo));

        if (bIsPlrHeightValid &&
            bIsPlrFalling     &&
            bIsSatisfyFreq)
        {
            m_orderTargetNo = nearerPlrData.no;
            return true;
        };
    };

    return false;
};


/*virtual*/ void CBaseAI6045::CDecisionUnitAttackThrow::OnStart(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetOrder(CAIThinkOrder::ORDER_ATTACK);
    EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType = m_orderType;
    EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo = m_orderTargetNo;

    DecisionUnitCommonParameter().SetAttackTimer(0.0f);
};


//
// *********************************************************************************
//


CBaseAI6045::CBaseAI6045(CEnemyCharacter* pEnemyChr)
: CAIModerator(pEnemyChr)
, m_pAIDecisionUnitManager(nullptr)
, m_pAIDecisionUnitCommonParameter(nullptr)
, m_catchTriggerAction(false)
{
    ThinkOrder().Clear();

    m_pAIDecisionUnitCommonParameter = new CEnemyAIDecisionUnitCommonParameter(pEnemyChr);
    m_pAIDecisionUnitManager = new CEnemyAIDecisionUnitManager(pEnemyChr, m_pAIDecisionUnitCommonParameter);

    DecisionUnitCommonParameter().SetAttackInterval(0.0f);
    DecisionUnitCommonParameter().SetAttackTimer(0.0f);
    DecisionUnitCommonParameter().SetMoveInterval(0.0f);
    DecisionUnitCommonParameter().SetMoveTimer(0.0f);

    CEnemyAIDecisionUnit::SetInitParam(pEnemyChr, this);
};


/*virtual*/ CBaseAI6045::~CBaseAI6045(void)
{
    if (m_pAIDecisionUnitManager)
    {
        delete m_pAIDecisionUnitManager;
        m_pAIDecisionUnitManager = nullptr;
    };

    if (m_pAIDecisionUnitCommonParameter)
    {
        delete m_pAIDecisionUnitCommonParameter;
        m_pAIDecisionUnitCommonParameter = nullptr;
    };
};


/*virtual*/ void CBaseAI6045::Run(void) /*override*/
{
    if (m_catchTriggerAction)
    {
        CatchTriggerActionOrder();
        m_catchTriggerAction = false;
    };

    DecisionUnitManager().Run();
};


/*virtual*/ void CBaseAI6045::Draw(void) /*override*/
{
    CAIModerator::Draw();
#ifdef _DEBUG
    DecisionUnitManager().Draw();
#endif /* _DEBUG */
};


/*virtual*/ void CBaseAI6045::CatchTriggerActionOrder(void)
{
    ;
};


void CBaseAI6045::Attach(CEnemyAIDecisionUnit* pDecisionUnit)
{
    DecisionUnitManager().Attach(pDecisionUnit);
};


CEnemyAIDecisionUnit* CBaseAI6045::GetDicisionUnitForName(const char* pszDecisionUnitName)
{
    return DecisionUnitManager().SearchDecisionUnit(pszDecisionUnitName);
};


void CBaseAI6045::SetCatchTriggerAction(uint32 catchTriggerAction)
{
    m_catchTriggerAction |= catchTriggerAction;
};


CEnemyAIDecisionUnitManager& CBaseAI6045::DecisionUnitManager(void)
{
    ASSERT(m_pAIDecisionUnitManager != nullptr);
    return *m_pAIDecisionUnitManager;
};


const CEnemyAIDecisionUnitManager& CBaseAI6045::DecisionUnitManager(void) const
{
    ASSERT(m_pAIDecisionUnitManager != nullptr);
    return *m_pAIDecisionUnitManager;
};


CEnemyAIDecisionUnitCommonParameter& CBaseAI6045::DecisionUnitCommonParameter(void)
{
    ASSERT(m_pAIDecisionUnitCommonParameter != nullptr);
    return *m_pAIDecisionUnitCommonParameter;
};


const CEnemyAIDecisionUnitCommonParameter& CBaseAI6045::DecisionUnitCommonParameter(void) const
{
    ASSERT(m_pAIDecisionUnitCommonParameter != nullptr);
    return *m_pAIDecisionUnitCommonParameter;
};
