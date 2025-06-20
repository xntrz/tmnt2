#include "BaseEliteAI.hpp"

#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"

#ifdef _DEBUG
#include "Game/Component/Enemy/EnemyDebug.hpp"
#include "Game/Component/GameMain/GameStageDebug.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#endif /* _DEBUG */


CEliteAIParam::CThinkStack::CThinkStack(void)
: priority(ELITEAI::PRIORITY_NONE)
, thinkOrder()
{
    ;
};


CEliteAIParam::CEliteAIParam(CEnemyCharacter* pEnemyChr)
: m_vecTerriotiryCenter(Math::VECTOR3_ZERO)
, m_fTerritoryRadius(0.0f)
, m_eliteDummyNo(-1)
, m_eliteDummyHandle(0)
, m_enemyStatus(ENEMYTYPES::STATUS_MAX)
, m_enemyStatusOld(ENEMYTYPES::STATUS_MAX)
, m_attackResult(ELITEAI::ATTACK_RESULT_NONE)
, m_bCounter(false)
, m_pszCounterName(nullptr)
, m_fRatioOfActivity(0.0f)
, m_fRatioOfMove(0.0f)
, m_fThinkTimer(0.0f)
, m_bThink(false)
, m_fThinkWaitTimer(0.0f)
, m_fElapsedTime(0.0f)
, m_attackCount(0)
, m_attackCountDamage(0)
, m_pEnemyChr(pEnemyChr)
, m_playerWatcher()
, m_thinkStack()
, m_thinkList()
, m_apfExecTimer()
, m_aszExecTimerName()
{
    for (int32 i = 0; i < COUNT_OF(m_thinkStack); ++i)
    {
        m_thinkStack[i].thinkOrder.SetOrder(CAIThinkOrder::ORDER_NOTHING);
        m_thinkStack[i].priority = ELITEAI::PRIORITY_NONE;
    };

    for (int32 i = 0; i < THINK_MAX; ++i)
    {
        m_apfExecTimer[i] = nullptr;
        m_aszExecTimerName[i][0] = '\0';
    };

    SetOwnTerritory(nullptr, 12.0f);
    ClearCounterAttack();
    ClearAttackCountDamage();
    ClearAllExecTimer();
    ClearEliteDummyInfo();

    PlayerWatcher().Initialize(m_pEnemyChr);
};


CEliteAIParam::~CEliteAIParam(void)
{
    ;
};


void CEliteAIParam::Update(void)
{
    if (EnemyChr().AIThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_ACCEPT)
        ThinkOrderDeleteRequest(ELITEAI::PRIORITY_NORMAL);

    m_playerWatcher.Update();

    m_fRatioOfActivity = static_cast<float>(Math::Rand()) / static_cast<float>(TYPEDEF::SINT32_MAX);
    m_fRatioOfMove = static_cast<float>(Math::Rand()) / static_cast<float>(TYPEDEF::SINT32_MAX);

    m_bThink = Think();

    if (m_fThinkWaitTimer >= 0.0f)
        m_fThinkWaitTimer -= CGameProperty::GetElapsedTime();

    if (m_fThinkWaitTimer > 0.0f)
        m_bThink = false;

    ThinkOrderDeleteFront(ELITEAI::PRIORITY_LOW);
};


void CEliteAIParam::DebugDrawPatrolAreaInfo(void)
{
#ifdef _DEBUG
    RwUInt8 alphaBasis = static_cast<RwUInt8>(128.0f / 4.0f);
    CDebugShape::ShowSphere(m_vecTerriotiryCenter, m_fTerritoryRadius, { 255, 0, 0, alphaBasis });
#endif /* _DEBUG */
};


void CEliteAIParam::DebugDrawInfo(void)
{
#ifdef _DEBUG
    CDebugShape::m_fLabelHeight = 12.0f;

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyPos);

    float fHeight = EnemyChr().Compositor().GetCollisionParameter().m_fHeight;
    vecMyPos.y += (fHeight * 1.5f);

    RwRGBA text = { 0xFF, 0xFF, 0x7F, 0xFF };
    RwRGBA title = { 0xFF, 0x00, 0x00, 0xFF };
    
    CDebugShape::ShowLabel(&vecMyPos, "Timers:", title);
    CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;

    for (int32 i = 0; i < COUNT_OF(m_apfExecTimer); ++i)
    {
        if (!m_apfExecTimer[i])
            continue;

        char szTimerInfo[128];
        szTimerInfo[0] = '\0';

        std::sprintf(szTimerInfo, "%s: %.04f", m_aszExecTimerName[i], *m_apfExecTimer[i]);

        CDebugShape::ShowLabel(&vecMyPos, szTimerInfo, text);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    CDebugShape::ShowLabel(&vecMyPos, "Think stack:", title);
    CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;

    for (CThinkStack& it : m_thinkList)
    {
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

        char szThinkInfo[128];
        szThinkInfo[0] = '\0';

        std::sprintf(szThinkInfo, "ORDER: %s", s_apszOrderName[it.thinkOrder.GetOrder()]);

        CDebugShape::ShowLabel(&vecMyPos, szThinkInfo, text);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };
#endif /* _DEBUG */
};


void CEliteAIParam::UpdateMe(void)
{
    ENEMYTYPES::STATUS enemyStatusNow = EnemyChr().GetStatus();
    if (enemyStatusNow != m_enemyStatus)
    {
        m_enemyStatusOld = m_enemyStatus;
        m_enemyStatus = enemyStatusNow;
    };    
};


bool CEliteAIParam::Think(void)
{
    m_fThinkTimer += CGameProperty::GetElapsedTime();

    float fThinkFreq = (1.0f - EnemyChr().AICharacteristic().m_fThinkingFrequency);
    fThinkFreq = Clamp(fThinkFreq, 0.0f, fThinkFreq);
    
    if (fThinkFreq > m_fThinkTimer)
        return false;

    m_fThinkTimer = 0.0f;

    return true;
};


bool CEliteAIParam::SetThinkOrderStack(CAIThinkOrder thinkOrder, ELITEAI::PRIORITY priority)
{
    for (int32 i = 0; i < COUNT_OF(m_thinkStack); ++i)
    {
        if (m_thinkStack[i].thinkOrder.GetOrder() == CAIThinkOrder::ORDER_NOTHING)
        {
            m_thinkStack[i].thinkOrder = thinkOrder;            
            m_thinkStack[i].priority = priority;

            m_thinkList.push_back(&m_thinkStack[i]);

            return true;
        };
    };

    return false;
};


void CEliteAIParam::ThinkOrderSetFromStack(CAIThinkOrder& thinkOrder)
{
    ASSERT(!m_thinkList.empty());

    CThinkStack* pThink = m_thinkList.front();

    bool bIsPriorityForRun = ((pThink->priority == ELITEAI::PRIORITY_LOW)    ||
                              (pThink->priority == ELITEAI::PRIORITY_NORMAL) ||
                              (pThink->priority == ELITEAI::PRIORITY_HIGH));

    if ((pThink->priority != ELITEAI::PRIORITY_NONE) &&
        (pThink->priority != ELITEAI::PRIORITY_ABSLOW) &&
        bIsPriorityForRun)
    {
        thinkOrder = pThink->thinkOrder;
    };
};


void CEliteAIParam::ThinkOrderDeleteFront(ELITEAI::PRIORITY priority)
{
    if (m_thinkList.empty())
        return;

    CThinkStack* pThink = m_thinkList.front();
    if (pThink->priority == priority)
    {
        m_thinkList.erase(pThink);
        pThink->thinkOrder.SetOrder(CAIThinkOrder::ORDER_NOTHING);
    };
};


void CEliteAIParam::ThinkOrderDeleteRequest(ELITEAI::PRIORITY priority)
{
    if (m_thinkList.empty())
        return;

    CThinkStack* pThink = m_thinkList.front();
    if ((pThink->priority == priority) || (priority == ELITEAI::PRIORITY_NONE))
    {
        if (pThink->thinkOrder.GetOrder() != CAIThinkOrder::ORDER_NOTHING)
            pThink->priority = ELITEAI::PRIORITY_LOW;
    };
};


void CEliteAIParam::ThinkOrderDelete(ELITEAI::PRIORITY priority)
{
    for (int32 i = 0; i < COUNT_OF(m_thinkStack); ++i)
    {
        if (m_thinkStack[i].thinkOrder.GetOrder() == CAIThinkOrder::ORDER_NOTHING)
            continue;
        
        if ((priority == m_thinkStack[i].priority) ||
            (priority == ELITEAI::PRIORITY_ALL))
        {
            m_thinkStack[i].thinkOrder.SetOrder(CAIThinkOrder::ORDER_NOTHING);
            m_thinkStack[i].priority = ELITEAI::PRIORITY_NONE;

            ASSERT(m_thinkStack[i].is_linked() == true, "order is waiting but not linked");
            m_thinkList.erase(&m_thinkStack[i]);
        };
    };
};


bool CEliteAIParam::IsMovePermission(void) const
{
    float fRatioOfMove = EnemyChr().AICharacteristic().m_fRatioOfMove;
    return (fRatioOfMove > m_fRatioOfMove);
};


bool CEliteAIParam::IsActivityPermission(void) const
{
    float fRatioOfActivity = EnemyChr().AICharacteristic().m_fRatioOfActivity;
    return (fRatioOfActivity > m_fRatioOfActivity);
};


void CEliteAIParam::SetActionPermissionTimeout(float fWaitTime)
{
    m_fThinkWaitTimer = fWaitTime;
};


bool CEliteAIParam::IsActionPermission(void) const
{
    return m_bThink;
};


void CEliteAIParam::SetAttackResult(ELITEAI::ATTACK_RESULT attackResult)
{
    m_attackResult = attackResult;
};


ELITEAI::ATTACK_RESULT CEliteAIParam::GetAttackResult(void) const
{
    return m_attackResult;
};


bool CEliteAIParam::RegistExecTimer(float* pfTimer, const char* pszName)
{
    for (int32 i = 0; i < COUNT_OF(m_apfExecTimer); ++i)
    {
        if (m_apfExecTimer[i] == nullptr)
        {
            m_apfExecTimer[i] = pfTimer;
            *m_apfExecTimer[i] = 1.0f;

            std::strcpy(m_aszExecTimerName[i], pszName);

            return true;
        };
    };

    return false;
};


void CEliteAIParam::ClearAllExecTimer(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apfExecTimer); ++i)
    {
        m_apfExecTimer[i] = nullptr;
        m_aszExecTimerName[i][0] = '\0';
    };
};


void CEliteAIParam::ClearElapsedTimer(void)
{
    m_fElapsedTime = 0.0f;
};


void CEliteAIParam::UpdateTimers(void)
{
    m_fElapsedTime += CGameProperty::GetElapsedTime();

    for (int32 i = 0; i < COUNT_OF(m_apfExecTimer); ++i)
    {
        if (!m_apfExecTimer[i])
            continue;

        if (*m_apfExecTimer[i] < 0.0f)
            continue;
        
        *m_apfExecTimer[i] -= CGameProperty::GetElapsedTime();
    };
};


float CEliteAIParam::GetElapsedTime(void) const
{
    return m_fElapsedTime;
};


void CEliteAIParam::SetOwnTerritory(const RwV3d* pvecTerritoryCenter, float fTerritoryRadius)
{
    if (pvecTerritoryCenter)
        m_vecTerriotiryCenter = *pvecTerritoryCenter;
    else
        m_vecTerriotiryCenter = Math::VECTOR3_ZERO;

    m_fTerritoryRadius = fTerritoryRadius;
};


bool CEliteAIParam::IsStatusChanged(void) const
{
    ENEMYTYPES::STATUS statusNow = EnemyChr().GetStatus();
    return (m_enemyStatus != statusNow);
};


ENEMYTYPES::STATUS CEliteAIParam::GetStatus(void) const
{
    return (m_enemyStatus);
};


bool CEliteAIParam::IsThinkStackEmpty(void) const
{
    return m_thinkList.empty();
};


void CEliteAIParam::SetCounterAttack(const char* pszCounterBehaviorName)
{
    m_pszCounterName = pszCounterBehaviorName;
    m_bCounter = true;
};


void CEliteAIParam::ClearCounterAttack(void)
{
    m_bCounter = false;
    m_pszCounterName = nullptr;
};


bool CEliteAIParam::IsCounterAttack(void) const
{
    return m_bCounter;
};


const char* CEliteAIParam::GetCounterBehaviorName(void) const
{
    return m_pszCounterName;
};


void CEliteAIParam::AddAttackCount(void)
{
    ++m_attackCount;
};


void CEliteAIParam::ClearAttackCount(void)
{
    m_attackCount = 0;
};


int32 CEliteAIParam::GetAttackCount(void) const
{
    return m_attackCount;
};


void CEliteAIParam::AddAttackCountDamage(int32 iAmount)
{
    m_attackCountDamage += iAmount;
};


void CEliteAIParam::ClearAttackCountDamage(void)
{
    m_attackCountDamage = 0;
};


int32 CEliteAIParam::GetAttackCountDamage(void) const
{
    return m_attackCountDamage;
};


float CEliteAIParam::GetTerritoryDist(const RwV3d* pvecPos /*= nullptr*/) const
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecPos);

    if (pvecPos != nullptr)
        vecPos = *pvecPos;

    RwV3d vecDist = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDist, &m_vecTerriotiryCenter, &vecPos);

    float fDist = Math::Vec3_Length(&vecDist);
    return fDist;
};


float CEliteAIParam::GetTerritoryRadius(void) const
{
    return m_fTerritoryRadius;
};


void CEliteAIParam::GetTerritoryCenter(RwV3d* pvecResult) const
{
    *pvecResult = m_vecTerriotiryCenter;
};


bool CEliteAIParam::IsInsideTerritory(const RwV3d* pvecPos /*= nullptr*/) const
{
    float fTerritoryDist = GetTerritoryDist(pvecPos);
    float fTerritoryRadius = GetTerritoryRadius();

    return (fTerritoryDist <= fTerritoryRadius);
};


void CEliteAIParam::ClearEliteDummyInfo(void)
{
    m_eliteDummyNo = -1;
    m_eliteDummyHandle = 0;
};


void CEliteAIParam::SetEliteDummyNo(int32 no)
{
    m_eliteDummyNo = no;
};


void CEliteAIParam::SetEliteDummyHandle(int32 handle)
{
    m_eliteDummyHandle = handle;
};


int32 CEliteAIParam::GetEliteDummyNo(void) const
{
    return m_eliteDummyNo;
};


uint32 CEliteAIParam::GetEliteDummyHandle(void) const
{
    return m_eliteDummyHandle;
};


CPlayerWatcher& CEliteAIParam::PlayerWatcher(void)
{
    return m_playerWatcher;
};


const CPlayerWatcher& CEliteAIParam::PlayerWatcher(void) const
{
    return m_playerWatcher;
};


CEnemyCharacter& CEliteAIParam::EnemyChr(void)
{
    return *m_pEnemyChr;
};


const CEnemyCharacter& CEliteAIParam::EnemyChr(void) const
{
    return *m_pEnemyChr;
};


//
// *********************************************************************************
//


/*static*/ CEnemyCharacter* CAIBehavior::m_pInitEnemyChr = nullptr;
/*static*/ CBaseEliteAI* CAIBehavior::m_pInitAIModerator = nullptr;
/*static*/ CEliteAIParam* CAIBehavior::m_pInitAIParam = nullptr;


/*static*/ void CAIBehavior::SetInitParam(CEnemyCharacter* pEnemyChr,
                                          CBaseEliteAI* pAIModerator,
                                          CEliteAIParam* pAIParam)
{
    m_pInitEnemyChr = pEnemyChr;
    m_pInitAIModerator = pAIModerator;
    m_pInitAIParam = pAIParam;
};


CAIBehavior::CAIBehavior(const char* pszName)
: m_bActive(true)
, m_pEnemyChr(m_pInitEnemyChr)
, m_pAIModerator(m_pInitAIModerator)
, m_pszName(pszName)
, m_pAIParam(m_pInitAIParam)
, m_AIThinkOrder()
{
    ;
};


/*virtual*/ CAIBehavior::~CAIBehavior(void)
{
    ;
};


void CAIBehavior::OrderWait(int32 waitType, float fWaitTime /*= 0.0f*/)
{
    m_AIThinkOrder.SetOrder(CAIThinkOrder::ORDER_WAIT);
    m_AIThinkOrder.OrderWait().m_iWaitType = waitType;
    m_AIThinkOrder.OrderWait().m_fWaitTimer = fWaitTime;
    m_AIThinkOrder.OrderWait().m_iPlayerNo = PlayerWatcher().GetTargetPlayerData().no;
    m_AIThinkOrder.OrderWait().m_vAt = PlayerWatcher().GetTargetPlayerData().position;
};


void CAIBehavior::OrderMove(int32 moveType, float fMoveTime /*= 0.0f*/)
{
    m_AIThinkOrder.SetOrder(CAIThinkOrder::ORDER_MOVE);
    m_AIThinkOrder.OrderMove().m_iMoveType = moveType;
    m_AIThinkOrder.OrderMove().m_fMoveTimer = fMoveTime;
    m_AIThinkOrder.OrderMove().m_iPlayerNo = PlayerWatcher().GetTargetPlayerData().no;
    m_AIThinkOrder.OrderMove().m_vAt = PlayerWatcher().GetTargetPlayerData().position;
};


void CAIBehavior::OrderAttack(int32 attackType)
{
    m_AIThinkOrder.SetOrder(CAIThinkOrder::ORDER_ATTACK);
    m_AIThinkOrder.OrderAttack().m_iAttackType = attackType;
    m_AIThinkOrder.OrderAttack().m_iPlayerNo = PlayerWatcher().GetTargetPlayerData().no;
    m_AIThinkOrder.OrderAttack().m_vAt = PlayerWatcher().GetTargetPlayerData().position;
};


void CAIBehavior::OrderDefence(int32 defenceType)
{
    m_AIThinkOrder.SetOrder(CAIThinkOrder::ORDER_DEFENCE);
    m_AIThinkOrder.OrderDefence().m_iDefenceType = defenceType;
};


void CAIBehavior::OrderPush(ELITEAI::PRIORITY priority)
{
    if (m_AIThinkOrder.GetOrder() == CAIThinkOrder::ORDER_WAIT)
    {
        ASSERT(priority == ELITEAI::PRIORITY_LOW);
        priority = ELITEAI::PRIORITY_LOW;
    };

    AIParam().SetThinkOrderStack(m_AIThinkOrder, priority);
};


bool CAIBehavior::IsSatisfyFreq(int32 freqTableNo) const
{
    uint8 freq = AIModerator().AIFreqParam().GetFrequency(freqTableNo);
    uint8 rand = static_cast<uint8>(Math::RandRange(0, 100));

    return (freq > rand);
};


float CAIBehavior::GetFreqRatio(int32 freqTableNo) const
{
    uint8 freq = AIModerator().AIFreqParam().GetFrequency(freqTableNo);
    return (static_cast<float>(freq) / 100.0f);
};


bool CAIBehavior::IsActive(void) const
{
    return m_bActive;
};


void CAIBehavior::SetActive(bool bActive)
{
    m_bActive = bActive;
};


const char* CAIBehavior::GetName(void) const
{
    return m_pszName;
};


CPlayerWatcher& CAIBehavior::PlayerWatcher(void)
{
    return AIParam().PlayerWatcher();
};


const CPlayerWatcher& CAIBehavior::PlayerWatcher(void) const
{
    return AIParam().PlayerWatcher();
};


CEliteAIParam& CAIBehavior::AIParam(void)
{
    ASSERT(m_pAIParam != nullptr);
    return *m_pAIParam;
};


const CEliteAIParam& CAIBehavior::AIParam(void) const
{
    ASSERT(m_pAIParam != nullptr);
    return *m_pAIParam;
};


CBaseEliteAI& CAIBehavior::AIModerator(void)
{
    ASSERT(m_pAIModerator != nullptr);
    return *m_pAIModerator;
};


const CBaseEliteAI& CAIBehavior::AIModerator(void) const
{
    ASSERT(m_pAIModerator != nullptr);
    return *m_pAIModerator;
};


CEnemyCharacter& CAIBehavior::EnemyChr(void)
{
    ASSERT(m_pEnemyChr != nullptr);
    return *m_pEnemyChr;
};


//
// *********************************************************************************
//


CAIBehaviorManager::CAIBehaviorManager(CEliteAIParam* pAIParam)
: m_bAllOff(false)
, m_no(-1)
, m_aBehaviorObj()
, m_listBehavior()
, m_pNowBehaviorObj(nullptr)
, m_pAIParam(pAIParam)
{
    for (int32 i = 0; i < COUNT_OF(m_aBehaviorObj); ++i)
    {
        m_aBehaviorObj[i].behavior = nullptr;
        m_aBehaviorObj[i].priority = ELITEAI::PRIORITY_NONE;
    };
};


CAIBehaviorManager::~CAIBehaviorManager(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aBehaviorObj); ++i)
    {
        if (m_aBehaviorObj[i].behavior)
        {
            delete m_aBehaviorObj[i].behavior;
            m_aBehaviorObj[i].behavior = nullptr;

            m_aBehaviorObj[i].priority = ELITEAI::PRIORITY_NONE;
        };
    };
};


CAIBehavior::RESULT CAIBehaviorManager::Update(void)
{
    if (m_pNowBehaviorObj)
        return m_pNowBehaviorObj->behavior->Update();

    return CAIBehavior::RESULT_END;
};


void CAIBehaviorManager::DebugDrawOrderInfo(void)
{
#ifdef _DEBUG
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    AIParam().EnemyChr().Compositor().GetFootPosition(&vecMyPos);

    float fHeight = AIParam().EnemyChr().Compositor().GetCollisionParameter().m_fHeight;
    vecMyPos.y += (fHeight * 1.5f);

    enum
    {
        INFOFLAG_ORDER    = (1 << 0),
        INFOFLAG_BEHAVIOR = (1 << 1),
        INFOFLAG_DMGUR    = (1 << 2),
    };

    uint32 infoFlag = 0;
    char szInfoOrder[128];
    szInfoOrder[0] = '\0';

    char szBehavior[128];
    szBehavior[0] = '\0';

    char szDmgur[128];
    szDmgur[0] = '\0';

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

    std::sprintf(szInfoOrder, "ORDER: %s", s_apszOrderName[AIParam().EnemyChr().AIThinkOrder().GetOrder()]);
    infoFlag |= INFOFLAG_ORDER;

    /* get behavior current info */
    infoFlag |= INFOFLAG_BEHAVIOR;
    if (m_pNowBehaviorObj)
    {
        const char* pszBehavior = m_pNowBehaviorObj->behavior->GetName();
        std::sprintf(szBehavior, "BEHAVIOR: %s", pszBehavior);
    }
    else
    {
        std::sprintf(szBehavior, "NO BEHAVIOR");
    };

    /* get info about damage heap */
    infoFlag |= INFOFLAG_DMGUR;

    int32 dmgurCount = AIParam().GetAttackCountDamage();
    int32 attackCount = AIParam().GetAttackCount();
    std::sprintf(szDmgur, "DMG/ATTACKS: %" PRId32 "/%" PRId32, dmgurCount, attackCount);

    /* draw by info flag */
    RwRGBA color = { 0xFF, 0xFF, 0x7F, 0xFF }; // yellow

    CDebugShape::m_fLabelHeight = 11.0f;

    if (infoFlag & INFOFLAG_ORDER)
    {
        CDebugShape::ShowLabel(&vecMyPos, szInfoOrder, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_BEHAVIOR)
    {
        CDebugShape::ShowLabel(&vecMyPos, szBehavior, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_DMGUR)
    {
        CDebugShape::ShowLabel(&vecMyPos, szDmgur, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };
#endif /* _DEBUG */
};


void CAIBehaviorManager::Stride(void)
{
    if (CounterAttack())
        return;

    CAIBehaviorObj* pRunnableBehaviorObj = Find();
    Change(pRunnableBehaviorObj);
};


bool CAIBehaviorManager::Attach(CAIBehavior* pAIBehavior, ELITEAI::PRIORITY priority)
{
    ASSERT(pAIBehavior != nullptr);

    for (int32 i = 0; i < COUNT_OF(m_aBehaviorObj); ++i)
    {
        CAIBehaviorObj* pAIBehaviorObj = &m_aBehaviorObj[i];
        if (!pAIBehaviorObj->behavior)
        {
            pAIBehaviorObj->behavior = pAIBehavior;
            pAIBehaviorObj->priority = priority;

            pAIBehavior->Initialize();

            auto it = m_listBehavior.begin();
            auto itEnd = m_listBehavior.end();
            while (it != itEnd)
            {
                if (pAIBehaviorObj->priority < (*it).priority)
                    break;

                ++it;
            };

            if (it != itEnd)
                m_listBehavior.insert(it, pAIBehaviorObj);
            else
                m_listBehavior.push_back(pAIBehaviorObj);

            return true;
        }
        else
        {
#ifdef _DEBUG
            if (!std::strcmp(pAIBehaviorObj->behavior->GetName(), pAIBehavior->GetName()))
                ASSERT(false, "%s behavior unit already registered!", pAIBehavior->GetName());
#endif /* _DEBUG */
        };
    };

    return false;
};


void CAIBehaviorManager::Change(const char* pszName)
{
    CAIBehaviorObj* pNewBehaviorObj = Find(pszName);
    Change(pNewBehaviorObj);
};


void CAIBehaviorManager::Change(CAIBehaviorObj* pNewBehaviorObj)
{
    ASSERT(pNewBehaviorObj != nullptr);

    if (m_pNowBehaviorObj)
        m_pNowBehaviorObj->behavior->OnEnd();

    m_pNowBehaviorObj = pNewBehaviorObj;
    m_pNowBehaviorObj->behavior->OnActive();
};


CAIBehavior* CAIBehaviorManager::GetBehavior(const char* pszName)
{
    CAIBehaviorObj* pAIBehaviorObj = Find(pszName);
    return pAIBehaviorObj->behavior;
};


CAIBehaviorManager::CAIBehaviorObj* CAIBehaviorManager::Find(const char* pszName)
{
    ASSERT(pszName != nullptr);

    for (CAIBehaviorObj& it : m_listBehavior)
    {
        if (!std::strcmp(it.behavior->GetName(), pszName))
            return &it;
    };

    return nullptr;
};


CAIBehaviorManager::CAIBehaviorObj* CAIBehaviorManager::Find(void)
{
    for (CAIBehaviorObj& it : m_listBehavior)
    {
        AIParam().PlayerWatcher().SetTargetType(CPlayerWatcher::TARGET_TYPE_NEAR);

        if (it.behavior->IsActive())
        {
            if (it.behavior->IsRunnable())
                return &it;
        };
    };

    return nullptr;
};


bool CAIBehaviorManager::CounterAttack(void)
{
    if (!AIParam().IsCounterAttack())
        return false;

    const char* pszBehaviorName = AIParam().GetCounterBehaviorName();

    CAIBehaviorObj* pCounterBehaviorObj = Find(pszBehaviorName);
    Change(pCounterBehaviorObj);

    AIParam().ClearCounterAttack();

    return true;
};


CEliteAIParam& CAIBehaviorManager::AIParam(void)
{
    return *m_pAIParam;
};


//
// *********************************************************************************
//


CBaseEliteAI::CAIBehaviorDebug::CAIBehaviorDebug(void)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::DEBUG)
, m_afTimers()
, m_pattern(0)
, m_orderType(-1)
{
    for (int32 i = 0; i < COUNT_OF(m_afTimers); ++i)
        m_afTimers[i] = 0.0f;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorDebug::Initialize(void) /*override*/
{
  
};


/*virtual*/ bool CBaseEliteAI::CAIBehaviorDebug::IsRunnable(void) /*override*/
{
#ifdef _DEBUG
    m_orderType = -1;

    int32 controller = (IGamepad::Max() - 1);
    if (controller < 0)
        controller = 0;

    uint32 digital = IGamepad::GetDigital(controller);
    uint32 digitalRelease = IGamepad::GetDigitalRelease(controller);
    uint32 digitalRepeat = IGamepad::GetDigitalRepeat(controller);
    uint32 digitalTrigger = IGamepad::GetDigitalTrigger(controller);

    if (IGamepad::GetDigitalTrigger(controller, IGamepad::DIGITAL_START))
        return true;

    return (m_orderType != -1);
#else /* _DEBUG */
	return false;
#endif /* _DEBUG */
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorDebug::OnActive(void) /*override*/
{
#ifdef _DEBUG
    m_orderType = CGameStageDebug::COUNTER;
#endif /* _DEBUG */
    AIModerator().AIBehaviorMgr().Change("AttackStab");
};


/*virtual*/ CBaseEliteAI::CAIBehaviorDebug::RESULT
CBaseEliteAI::CAIBehaviorDebug::Update(void) /*override*/
{
    return RESULT_FORCEEND;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorDebug::OnEnd(void) /*override*/
{
    
};


//
// *********************************************************************************
//


CBaseEliteAI::CAIBehaviorIdle::CAIBehaviorIdle(void)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::IDLE)
{
    ;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorIdle::OnActive(void) /*override*/
{
    OrderWait(ELITEAI::ORDERTYPE_WAIT_IDLE, 2.0f);

    if (PlayerWatcher().IsPlayerActionRange(-1, 1.0f))
        OrderWait(ELITEAI::ORDERTYPE_WAIT_TURN);

    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


CBaseEliteAI::CAIBehaviorJump::CAIBehaviorJump(void)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::JUMP)
{
    ;
};


/*virtual*/ bool CBaseEliteAI::CAIBehaviorJump::IsRunnable(void) /*override*/
{
    RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);
    vecMyFootPos.y += 0.2f;

    RwV3d vecStep = { 0.0f, 0.0f, 1.0f };
    EnemyChr().Compositor().RotateVectorByDirection(&vecStep, &vecStep);

    RwV3d vecMyFootPosNew = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vecMyFootPosNew, &vecMyFootPos, &vecStep);

    float fMapHitRadius = CAIUtils::GetMapHitRadius(&EnemyChr().Compositor());
    float fJumpInitSpeed = EnemyChr().Feature().m_fJumpInitializeSpeed;
    int32 checkHoleDivNum = 3;

    CAIUtils::CHECKMOVELINE_RESULT checkMoveLineRet = CAIUtils::CheckMoveLine(&vecMyFootPos,
                                                                              &vecMyFootPosNew,
                                                                              fMapHitRadius,
                                                                              checkHoleDivNum,
                                                                              fJumpInitSpeed);

    if (checkMoveLineRet == CAIUtils::CHECKMOVELINE_RESULT_NEEDJUMP)
        return true;

    return false;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorJump::OnActive(void) /*override*/
{
    OrderMove(ELITEAI::ORDERTYPE_MOVE_JUMP);
    OrderPush(ELITEAI::PRIORITY_LOW);
};


//
// *********************************************************************************
//


CBaseEliteAI::CAIBehaviorMove::CAIBehaviorMove(void)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::MOVE)
, m_fDashTimeout(0.0f)
, m_fReachLimit(10.0f)
, m_bEnablePlayer(false)
, m_bAfraidOfDonBarrier(true)
, m_fDonBarrierTimer(0.0f)
{
    ;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorMove::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fDonBarrierTimer, "Barrier");
};


/*virtual*/ bool CBaseEliteAI::CAIBehaviorMove::IsRunnable(void) /*override*/
{
    m_bEnablePlayer = true;    
    m_fDashTimeout -= AIParam().GetElapsedTime();
    m_fReachLimit -= AIParam().GetElapsedTime();

    if (!PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ENABLE))
    {
        m_bEnablePlayer = false;
        AIParam().SetActionPermissionTimeout(0.5f);

        if (!IsDisablePlayer())
            return false;
    };

    if (PlayerWatcher().IsDonBarrier(4.0f))
    {
        if (m_fDonBarrierTimer <= 0.0f)
            m_bAfraidOfDonBarrier = (Math::RandRange(0, 100) > 30);

        m_fDonBarrierTimer = 1.0f;

        if (m_bAfraidOfDonBarrier)
            return false;
    };

    if (PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(70.0f), 1.5f))
    {
        m_fReachLimit = 10.0f;
        return false;
    };

    CAIUtils::OBSTACLE_TYPE obstacleType = CAIUtils::CheckObstacle(&EnemyChr().Compositor(), 1.0f, 2.0f);
    if (obstacleType == CAIUtils::OBSTACLE_NONE)
        return true;

    return false;
};


bool CBaseEliteAI::CAIBehaviorMove::IsDisablePlayer(void)
{
    if (PlayerWatcher().IsViewArea(CAIUtils::PLAYER_STATE_ALL))
    {
        if (!PlayerWatcher().IsPlayerSuitRange(-1, 1.8f))
            return true;
    };

    return false;
};


//
// *********************************************************************************
//


CBaseEliteAI::CAIBehaviorGuard::CAIBehaviorGuard(bool bAvoidAttack)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::GUARD)
, m_guardType(GUARDTYPE_SHORT_RANGE)
, m_fGuardLimitTime(0.0f)
, m_bAvoidAttack(bAvoidAttack)
, m_fGuardTimeout(0.0f)
{
    ;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorGuard::Initialize(void) /*override*/
{
    AIParam().RegistExecTimer(&m_fGuardTimeout, "Guard");
};


/*virtual*/ bool CBaseEliteAI::CAIBehaviorGuard::IsRunnable(void) /*override*/
{
    if (m_fGuardTimeout > 0.0f)
        return false;

    float fGuardAngle = MATH_DEG2RAD(360.0f);
    if (!EnemyChr().TestFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE))
        fGuardAngle /= 3.0f;

    bool bTrigger = false;

    if (PlayerWatcher().IsAttackTriggerWithinRange(3.6f, fGuardAngle))
    {
        m_guardType = GUARDTYPE_SHORT_RANGE;
        bTrigger = true;
    };

    if (PlayerWatcher().IsShootShurikenTrigger(25.0f))
    {
        m_guardType = GUARDTYPE_LONG_RANGE;
        bTrigger = true;
    };

    if (bTrigger)
    {
        m_fGuardTimeout = 0.8f;

        if (IsSatisfyFreq(ELITEAI::FREQUENCY_GUARD))
            return true;
    };

    return false;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorGuard::OnActive(void) /*override*/
{
    switch (m_guardType)
    {
    case GUARDTYPE_SHORT_RANGE:
        OrderDefence(ELITEAI::ORDERTYPE_DEFENCE_NEAR);
        break;

    case GUARDTYPE_LONG_RANGE:
        {
            if (PlayerWatcher().IsPlayerWithinRange(-1, 9.0f))
                OrderDefence(ELITEAI::ORDERTYPE_DEFENCE_MIDDLE);
            else
                OrderDefence(ELITEAI::ORDERTYPE_DEFENCE_LONG);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    OrderPush(ELITEAI::PRIORITY_NORMAL);
    
    m_fGuardLimitTime = 1.0f;

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ CAIBehavior::RESULT CBaseEliteAI::CAIBehaviorGuard::Update(void) /*override*/
{
    m_fGuardLimitTime -= CGameProperty::GetElapsedTime();

    if (m_AIThinkOrder.OrderDefence().m_iDefenceType != ELITEAI::ORDERTYPE_DEFENCE_NEAR)
    {
        if (PlayerWatcher().IsShootShuriken(16.0f))
            m_fGuardLimitTime = 0.3f;

        if (m_fGuardLimitTime < 0.0f)
            return RESULT_END;

        OrderPush(ELITEAI::PRIORITY_LOW);
        return RESULT_CONTINUE;
    };

    if (m_fGuardLimitTime >= 0.0f)
    {
        if (!PlayerWatcher().IsAttackWithinRange(3.6f, MATH_DEG2RAD(360.0f)))
            return RESULT_END;

        OrderPush(ELITEAI::PRIORITY_LOW);
        return RESULT_CONTINUE;
    };

    if (m_bAvoidAttack)
        AIParam().SetCounterAttack(ELITEAI::AIBEHAVIORNAME::AVOID_ATTACK);

	return RESULT_END;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorGuard::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


void CBaseEliteAI::CAIBehaviorGuard::SetAvoidAttack(bool bAvoidAttack)
{
    m_bAvoidAttack = bAvoidAttack;
};


//
// *********************************************************************************
//


CBaseEliteAI::CAIBehaviorAttack::CAIBehaviorAttack(void)
: CAIBehaviorAttack(ELITEAI::AIBEHAVIORNAME::ATTACK)
{
    ;
};


CBaseEliteAI::CAIBehaviorAttack::CAIBehaviorAttack(const char* pszName)
: CAIBehavior(pszName)
{
    ;
};


/*virtual*/ bool CBaseEliteAI::CAIBehaviorAttack::IsRunnable(void) /*override*/
{
    if (!AIParam().IsActivityPermission())
        return false;

    if (!AIParam().IsActionPermission())
        return false;

    if (!PlayerWatcher().IsSuitableArea(MATH_DEG2RAD(70.0f)))
        return false;

    return true;
};


//
// *********************************************************************************
//


CBaseEliteAI::CAIBehaviorHighAttack::CAIBehaviorHighAttack(void)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::ATTACK_HIGH)
{
    ;
};


/*virtual*/ bool CBaseEliteAI::CAIBehaviorHighAttack::IsRunnable(void) /*override*/
{
    if (!PlayerWatcher().IsHighAttack(-1.0f, 0.2f, 3.0f))
        return false;

    if (!IsSatisfyFreq(ELITEAI::FREQUENCY_ATTACK_HIGH))
        return false;

    return true;
};


//
// *********************************************************************************
//


CBaseEliteAI::CAIBehaviorAvoidEnclose::CAIBehaviorAvoidEnclose(void)
: CAIBehavior(ELITEAI::AIBEHAVIORNAME::AVOID_ENCLOSE)
{
    ;
};


/*virtual*/ bool CBaseEliteAI::CAIBehaviorAvoidEnclose::IsRunnable(void) /*override*/
{
    if (EnemyChr().AIThinkOrder().GetAnswer() != CAIThinkOrder::RESULT_REFUSE)
        return false;

    if (PlayerWatcher().GetPlayerNumThere(3.0f) < 3)
        return false;

    return true;
};


/*virtual*/ void CBaseEliteAI::CAIBehaviorAvoidEnclose::OnActive(void) /*override*/
{
    AIParam().SetCounterAttack(ELITEAI::AIBEHAVIORNAME::AVOID_ATTACK);
};


//
// *********************************************************************************
//


CBaseEliteAI::CBaseEliteAI(CEnemyCharacter* pEnemyChr)
: CAIModerator(pEnemyChr)
, m_pAIBehaviorMgr(nullptr)
, m_pAIParam(nullptr)
, m_fAIStartTime(1.0f)
{
    m_pAIParam = new CEliteAIParam(pEnemyChr);
    m_pAIBehaviorMgr = new CAIBehaviorManager(m_pAIParam);

    m_fAIStartTime = 1.0f;
    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    ThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);

    SetTerritory(pEnemyChr->GetID());

    CAIBehavior::SetInitParam(pEnemyChr, this, m_pAIParam);

    /* attach default behavior units */
    CAIBehavior* pBehavior = nullptr;

    pBehavior = new CBaseEliteAI::CAIBehaviorJump;
    Attach(pBehavior, ELITEAI::PRIORITY_LOW);

    pBehavior = new CBaseEliteAI::CAIBehaviorIdle;
    Attach(pBehavior, ELITEAI::PRIORITY_ABSLOW);

#ifdef _DEBUG
    pBehavior = new CBaseEliteAI::CAIBehaviorDebug;
    Attach(pBehavior, ELITEAI::PRIORITY_HIGH);
#endif /* _DEBUG */    
};


/*virtual*/ CBaseEliteAI::~CBaseEliteAI(void)
{
    if (m_pAIBehaviorMgr)
    {
        delete m_pAIBehaviorMgr;
        m_pAIBehaviorMgr = nullptr;
    };

    if (m_pAIParam)
    {
        delete m_pAIParam;
        m_pAIParam = nullptr;
    };
};


/*virtual*/ void CBaseEliteAI::Run(void) /*override*/
{
    if (!WaitAIStartProc())
        return;

    UpdateStatus();
    AIParam().UpdateTimers();
    UpdateAI();
    AIParam().UpdateMe();
    UpdateChrFlags();
};


/*virtual*/ void CBaseEliteAI::Draw(void) /*override*/
{
    CAIModerator::Draw();
#ifdef _DEBUG
    CDebugShape::m_fLabelOffsetY = 0.0f;

    if (CEnemyDebug::SHOW_AI_INFO)
    {
        AIBehaviorMgr().DebugDrawOrderInfo();
        AIParam().DebugDrawInfo();
    };

    if (CEnemyDebug::SHOW_AI_PATROL_AREA)
        AIParam().DebugDrawPatrolAreaInfo();
    
    CDebugShape::m_fLabelOffsetY = 0.0f;
#endif /* _DEBUG */
};


/*virtual*/ void CBaseEliteAI::AttackCountInc(void)
{
    ;
};


/*virtual*/ void CBaseEliteAI::AttackCountDamage(int32 iAmount)
{
    ;
};


/*virtual*/ void CBaseEliteAI::CatchAttack(uint32 hObj, GAMEOBJECTTYPE::VALUE objType)
{
    ;
};


/*virtual*/ void CBaseEliteAI::OnStatusEnd(ENEMYTYPES::STATUS statusEnd)
{
    ;
};


void CBaseEliteAI::SetTerritory(ENEMYID::VALUE enemyId)
{
    float fTerritoryRadius = 0.0f;
    RwV3d vecTerritoryCenter = Math::VECTOR3_ZERO;

    switch (enemyId)
    {
    case ENEMYID::ID_ELITE_FOOT_A:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_A:
        fTerritoryRadius = 12.0f;
        vecTerritoryCenter = { 0.0f, 0.2f, -14.0f };
        break;

    case ENEMYID::ID_ELITE_FOOT_B:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_B:
        fTerritoryRadius = 8.0f;
        vecTerritoryCenter = { 0.0f, 0.78f, 0.0f };
        break;

    case ENEMYID::ID_ELITE_FOOT_C:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_C:
        fTerritoryRadius = 19.0f;
        vecTerritoryCenter = { 0.0f, 0.0f, 24.0f };
        break;

    case ENEMYID::ID_ELITE_FOOT_D:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_D:
        fTerritoryRadius = 10.0f;
        vecTerritoryCenter = { 0.0f, 0.0f, -6.0f };
        break;

    case ENEMYID::ID_FOOT_MECH_SPLINTER:
        fTerritoryRadius = 10.0f;
        vecTerritoryCenter = { 0.0f, -2.0f, 0.0f };
        break;

    default:
        fTerritoryRadius = 20.0f;
        vecTerritoryCenter = { 0.0f, 0.74f, 0.0f };
        break;
    };

    bool bIsNexusMap = CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                       CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X);
    if (bIsNexusMap)
    {
        fTerritoryRadius = 20.0f;
        vecTerritoryCenter = { 0.0f, 0.74f, 0.0f };
    };

#ifdef _DEBUG
    bool bIsTestMap = CEnemyUtils::CheckNowMapForMapID(MAPID::ID_TEST);
    if (bIsTestMap)
    {
        fTerritoryRadius = 40.0f;
        vecTerritoryCenter = { 0.0f, 2.0f, 0.0f };
    };
#endif /* _DEBUG */

    AIParam().SetOwnTerritory(&vecTerritoryCenter, fTerritoryRadius);
};


bool CBaseEliteAI::WaitAIStartProc(void)
{
    m_fAIStartTime -= CGameProperty::GetElapsedTime();
    if (m_fAIStartTime > 0.0f)
    {
        ThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);
        ThinkOrder().OrderWait().m_iWaitType = ELITEAI::ORDERTYPE_WAIT_TIMEOUT;
        ThinkOrder().OrderWait().m_fWaitTimer = 0.1f;
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

        return false;
    };

    return true;
};


void CBaseEliteAI::UpdateStatus(void)
{
    ENEMYTYPES::STATUS statusEnd = ENEMYTYPES::STATUS_MAX;

    if (AIParam().IsStatusChanged())
        statusEnd = AIParam().GetStatus();

    OnStatusEnd(statusEnd);
};


void CBaseEliteAI::UpdateAI(void)
{
    if (ThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_WAITING)
        return;

    AIParam().Update();

    if (AIParam().IsThinkStackEmpty())
    {
        CAIBehavior::RESULT result = AIBehaviorMgr().Update();
        if (result != CAIBehavior::RESULT_CONTINUE)
        {
            ThinkOrder().SetOrder(CAIThinkOrder::ORDER_WAIT);

            if (result == CAIBehavior::RESULT_FORCEEND)
                AIBehaviorMgr().Change(ELITEAI::AIBEHAVIORNAME::IDLE);
            else
                AIBehaviorMgr().Stride();

            AIParam().ClearElapsedTimer();
        };
    };

    if (!AIParam().IsThinkStackEmpty())
        AIParam().ThinkOrderSetFromStack(ThinkOrder());

    /* TODO: old think order is unused (?) */
    m_AIThinkOrderOld = ThinkOrder();

    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
};


void CBaseEliteAI::UpdateChrFlags(void)
{
    ENEMYTYPES::STATUS statusNow = EnemyChr().GetStatus();
    if (CEnemyUtils::IsStunState(statusNow))
    {
        if (EnemyChr().TestFlag(ENEMYTYPES::FLAG_NOREACTION) ||
            EnemyChr().TestFlag(ENEMYTYPES::FLAG_INVINCIBILITY))
        {
            EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
            EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
        };
    };
};


void CBaseEliteAI::Attach(CAIBehavior* pBehavior, ELITEAI::PRIORITY pri)
{
    AIBehaviorMgr().Attach(pBehavior, pri);
};


CEliteAIParam& CBaseEliteAI::AIParam(void)
{
    return *m_pAIParam;
};


const CEliteAIParam& CBaseEliteAI::AIParam(void) const
{
    return *m_pAIParam;
};


CAIBehaviorManager& CBaseEliteAI::AIBehaviorMgr(void)
{
    return *m_pAIBehaviorMgr;
};


const CAIBehaviorManager& CBaseEliteAI::AIBehaviorMgr(void) const
{
    return *m_pAIBehaviorMgr;
};


CEnemyCharacter& CBaseEliteAI::EnemyChr(void)
{
    return EnemyCharacter();
};


const CEnemyCharacter& CBaseEliteAI::EnemyChr(void) const
{
    return EnemyCharacter();
};