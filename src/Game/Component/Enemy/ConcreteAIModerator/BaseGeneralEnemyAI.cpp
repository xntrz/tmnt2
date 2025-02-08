#include "BaseGeneralEnemyAI.hpp"

#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseGeneralEnemyChr.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"

#ifdef _DEBUG
#include "Game/Component/Enemy/EnemyDebug.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#endif /* _DEBUG */


namespace AIOT
{
    int32 Set(int32 idx, int32 target)
    {
        return (idx | target);
    };


    int32 Get(int32 type)
    {
        static_assert(~AIOT::TargetNo == 0x7FFFFFFF, "checkout");

        return (type & (~AIOT::TargetNo));
    };


    int32 GetTarget(int32 type)
    {
        static_assert(AIOT::TargetNo == 0x80000000, "checkout");

        return (type & AIOT::TargetNo);
    };


    void SetThinkOrder(CAIThinkOrder& rAIThinkOrder,
                       CAIThinkOrder::ORDER eOrder,
                       int32 eTypeIdx,
                       float fTimer,
                       RwV3d* pvVec /*= nullptr*/)
    {
        RwV3d vAt = Math::VECTOR3_ZERO;

        if (pvVec)
            vAt = *pvVec;

        rAIThinkOrder.SetOrder(eOrder);
        rAIThinkOrder.SetAnswer(CAIThinkOrder::RESULT_WAITING);

        switch (eOrder)
        {
        case CAIThinkOrder::ORDER_WAIT:
            {
                CAIThinkOrder::ORDERWAIT& wait = rAIThinkOrder.OrderWait();
                wait.m_vAt          = vAt;
                wait.m_fWaitTimer   = fTimer;
                wait.m_iPlayerNo    = -1;
                wait.m_iWaitType    = Set(eTypeIdx, 0);
            }
            break;
            
        case CAIThinkOrder::ORDER_MOVE:
            {
                ASSERT(eTypeIdx >= AIOT::MoveTurn);
                ASSERT(eTypeIdx <= AIOT::MoveJump);

                CAIThinkOrder::ORDERMOVE& move = rAIThinkOrder.OrderMove();
                move.m_vAt          = vAt;
                move.m_fMoveTimer   = fTimer;
                move.m_iPlayerNo    = -1;
                move.m_iMoveType    = Set(eTypeIdx, 0);
            }
            break;

        case CAIThinkOrder::ORDER_RUN:
            {
                CAIThinkOrder::ORDERRUN& run = rAIThinkOrder.OrderRun();
                run.m_vAt = vAt;
            }
            break;

        case CAIThinkOrder::ORDER_ATTACK:
            {
                ASSERT(eTypeIdx >= AIOT::AttackA);
                ASSERT(eTypeIdx <= AIOT::FireRepeatC);

                CAIThinkOrder::ORDERATTACK& attack = rAIThinkOrder.OrderAttack();
                attack.m_vAt            = vAt;
                attack.m_iPlayerNo      = -1;
                attack.m_iAttackType    = Set(eTypeIdx, 0);
            }
            break;

        default:
            break;
        };
    };


    void SetThinkOrder(CAIThinkOrder& rAIThinkOrder,
                       CAIThinkOrder::ORDER eOrder,
                       int32 eTypeIdx,
                       float fTimer,
                       int32 iNumber)
    {
        rAIThinkOrder.SetOrder(eOrder);
        rAIThinkOrder.SetAnswer(CAIThinkOrder::RESULT_WAITING);

        switch (eOrder)
        {
        case CAIThinkOrder::ORDER_WAIT:
            {
                CAIThinkOrder::ORDERWAIT& wait = rAIThinkOrder.OrderWait();
                wait.m_vAt          = Math::VECTOR3_ZERO;
                wait.m_fWaitTimer   = fTimer;
                wait.m_iPlayerNo    = iNumber;
                wait.m_iWaitType    = Set(eTypeIdx, AIOT::TargetNo);
            }
            break;
            
        case CAIThinkOrder::ORDER_MOVE:
            {
                ASSERT(eTypeIdx >= AIOT::MoveTurn);
                ASSERT(eTypeIdx <= AIOT::MoveJump);

                CAIThinkOrder::ORDERMOVE& move = rAIThinkOrder.OrderMove();
                move.m_vAt          = Math::VECTOR3_ZERO;
                move.m_fMoveTimer   = fTimer;
                move.m_iPlayerNo    = iNumber;
                move.m_iMoveType    = Set(eTypeIdx, AIOT::TargetNo);
            }
            break;

        case CAIThinkOrder::ORDER_RUN:
            {
                CAIThinkOrder::ORDERRUN& run = rAIThinkOrder.OrderRun();
                run.m_vAt = Math::VECTOR3_ZERO;
            }
            break;

        case CAIThinkOrder::ORDER_ATTACK:
            {
                ASSERT(eTypeIdx >= AIOT::AttackA);
                ASSERT(eTypeIdx <= AIOT::Special1);

                CAIThinkOrder::ORDERATTACK& attack = rAIThinkOrder.OrderAttack();
                attack.m_vAt            = Math::VECTOR3_ZERO;
                attack.m_iPlayerNo      = iNumber;
                attack.m_iAttackType    = Set(eTypeIdx, AIOT::TargetNo);
            }
            break;

        default:
            break;
        };
    };


    void SetWaitOrder(CAIThinkOrder& rAIThinkOrder, float fTimer, RwV3d* pvVec /*= nullptr*/)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_WAIT, AIOT::Null, fTimer, pvVec);
    };


    void SetMoveOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, float fTimer, RwV3d* pvVec /*= nullptr*/)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_MOVE, typeIdx, fTimer, pvVec);
    };


    void SetMoveOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, float fTimer, int32 iNumber)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_MOVE, typeIdx, fTimer, iNumber);
    };


    void SetAttackOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, int32 iNumber)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_ATTACK, typeIdx, 0.0f, iNumber);
    };


    void SetFireOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, int32 iNumber, RwV3d* pvVec /*= nullptr*/)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_ATTACK, typeIdx, 0.0f, iNumber);
		if (pvVec)
			rAIThinkOrder.OrderAttack().m_vAt = *pvVec;
    };


    void SetDefenceOrder(CAIThinkOrder& rAIThinkOrder)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_DEFENCE, 0, 0.0f, nullptr);
    };


    void SetNothingOrder(CAIThinkOrder& rAIThinkOrder)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_NOTHING, 0, 0.0f, nullptr);
    };

    
    void SetRunOrder(CAIThinkOrder& rAIThinkOrder, RwV3d* pvAt)
    {
        ASSERT(pvAt != nullptr);
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_RUN, AIOT::Null, 0.0f, pvAt);
    };
}; /* namespace AIOT */


//
// *********************************************************************************
//


/*static*/ const float CTarget::HateMax = 1.0f;


/*virtual*/ float CTarget::ICalcHateOfGeometry::Importance(void) /*override*/
{
    return HateMax;
};


/*virtual*/ float CTarget::ICalcHateOfGeometry::Get(int32 iNo, ReferenceInfo& rReferenceInfo) /*override*/
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerChr)
        return 0.0f;

    RwV3d vPlayerPos = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vPlayerPos);

    if (!CAIUtils::IsObjectViewArea(rReferenceInfo.m_pEnemyCharacter, &vPlayerPos))
        return 0.0f;

    CEnemyCharacter* pEnemyChr = rReferenceInfo.m_pEnemyCharacter;
    CCharacterCompositor* pChrCompositor = &pEnemyChr->Compositor();

    RwV3d vEnemyPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetPosition(&vEnemyPos);

    if (!CAIUtils::IsInsidePatrolArea(&vEnemyPos, &pEnemyChr->Feature()))
        return 0.0f;

    float fDistance = CEnemyUtils::GetDistance(pChrCompositor, &vPlayerPos);
    float fRadiusOfAction = pEnemyChr->AICharacteristic().m_fRadiusOfAction;
    if (fDistance > fRadiusOfAction)
        return 0.0f;

    ASSERT(fRadiusOfAction > 0.0f);

    return (1.0f - (fDistance / fRadiusOfAction));
};


//
// *********************************************************************************
//


/*virtual*/ float CTarget::ICalcHateOfCollision::Importance(void) /*override*/
{
    return HateMax;
};


/*virtual*/ float CTarget::ICalcHateOfCollision::Get(int32 iNo, ReferenceInfo& rReferenceInfo) /*override*/
{
    CPlayerCharacter* pPlayerCharacter = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerCharacter)
        return 0.0f;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    pPlayerCharacter->GetFootPosition(&vPosition);

    CBaseGeneralEnemyChr* pBaseGeneralEnemyChr = static_cast<CBaseGeneralEnemyChr*>(rReferenceInfo.m_pEnemyCharacter);
    CBaseGeneralEnemyChr::MOVABLETYPE movableType = pBaseGeneralEnemyChr->GetMovableType(&vPosition);
    switch (movableType)
    {
    case CBaseGeneralEnemyChr::MOVABLETYPE_JUMP:
    case CBaseGeneralEnemyChr::MOVABLETYPE_POSSIBLE:
        return 1.0f;

    default:
        break;
    };

    return 0.0f;
};


//
// *********************************************************************************
//


/*virtual*/ float CTarget::ICalcHateOfTargetDamaged::Importance(void) /*override*/
{
    return (HateMax * 0.5f);
};


/*virtual*/ float CTarget::ICalcHateOfTargetDamaged::Get(int32 iNo, ReferenceInfo& rReferenceInfo) /*override*/
{
    CPlayerCharacter* pPlayerCharacter = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerCharacter)
        return 0.0f;

    int32 totalDamage = 1;

    int32 playerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        IGamePlayer* pPlayer = CGameProperty::Player(i);
        if (pPlayer->IsAlive())
            totalDamage += pPlayer->GetDamage();
    };

    return static_cast<float>(CGameProperty::GetGamePlayer(iNo)->GetDamage() + 1) / static_cast<float>(totalDamage);
};


//
// *********************************************************************************
//


/*virtual*/ float CTarget::ICalcHateOfDamageAccumulation::Importance(void) /*override*/
{
    return (HateMax * 0.5f);
};


/*virtual*/ float CTarget::ICalcHateOfDamageAccumulation::Get(int32 iNo, ReferenceInfo& rReferenceInfo) /*override*/
{
    CBaseGeneralEnemyChr* pBaseGeneralEnemyChr = static_cast<CBaseGeneralEnemyChr*>(rReferenceInfo.m_pEnemyCharacter);

    int32 totalDamage = 1;

    int32 playerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
        totalDamage += pBaseGeneralEnemyChr->GetDamageAccumulation(i);
    
    return static_cast<float>(pBaseGeneralEnemyChr->GetDamageAccumulation(iNo) + 1) / static_cast<float>(totalDamage);
};


//
// *********************************************************************************
//


/*virtual*/ float CTarget::ICalcHateOfTurtleCharacteristic::Importance(void) /*override*/
{
    return (HateMax * 0.3f);
};


/*virtual*/ float CTarget::ICalcHateOfTurtleCharacteristic::Get(int32 iNo, ReferenceInfo& rReferenceInfo) /*override*/
{
    CPlayerCharacter* pPlayerCharacter = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerCharacter)
        return 0.0f;

    PLAYERID::VALUE idPlayer = pPlayerCharacter->GetID();
    switch (idPlayer)
    {
    case PLAYERID::ID_LEO:
        return 0.7f;

    case PLAYERID::ID_RAP:
        return 0.8f;

    case PLAYERID::ID_MIC:
        return 0.9f;

    case PLAYERID::ID_DON:
        return 0.5f;

    case PLAYERID::ID_SLA:
        return 0.1f;

    case PLAYERID::ID_CAS:
        return 0.9f;

    case PLAYERID::ID_KAR:
        return 0.9f;

    case PLAYERID::ID_SPL:
        return 1.0f;

    default:
        ASSERT(false);
        break;
    };

    return 1.0f;
};


//
// *********************************************************************************
//


/*virtual*/ float CTarget::ICalcHateOfTurtleStatus::Importance(void) /*override*/
{
    return (HateMax * 0.3f);
};


/*virtual*/ float CTarget::ICalcHateOfTurtleStatus::Get(int32 iNo, ReferenceInfo& rReferenceInfo) /*override*/
{
    CPlayerCharacter* pPlayerCharacter = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerCharacter)
        return 0.0f;

    PLAYERTYPES::STATUS eStatus = pPlayerCharacter->GetStatus();
    if (eStatus >= PLAYERTYPES::STATUS_IDLE)
    {
        if (eStatus <= PLAYERTYPES::STATUS_WALK)
            return 0.2f;

        if (eStatus == PLAYERTYPES::STATUS_RUN)
            return 1.0f;
    };

    CAIUtils::PLAYER_STATE_FLAG stateFlag = CAIUtils::PLAYER_STATE_ATTACK
                                          | CAIUtils::PLAYER_STATE_AERIAL
                                          | CAIUtils::PLAYER_STATE_DOWN
                                          | CAIUtils::PLAYER_STATE_UNUSUAL
                                          | CAIUtils::PLAYER_STATE_KNIFE;

    ASSERT(stateFlag == 0x7C, "update me");

    if (!(CAIUtils::GetPlayerStateFlag(eStatus) & stateFlag))
        return 1.0f;

    return 0.8f;
};


//
// *********************************************************************************
//


/*virtual*/ float CTarget::ICalcHateOfRandom::Importance(void) /*override*/
{
    return (HateMax * 0.3f);
};


/*virtual*/ float CTarget::ICalcHateOfRandom::Get(int32 iNo, ReferenceInfo& rReferenceInfo) /*override*/
{
    return Math::RandFloat();
};


//
// *********************************************************************************
//


CTarget::CTarget(int32 iNo)
: m_listRoot()
, m_iNumber(iNo)
, m_fHate(0.0f)
{
    ;
};


/*virtual*/ CTarget::~CTarget(void)
{
    auto it = m_listRoot.begin();
    auto itEnd = m_listRoot.end();
    while (it != itEnd)
    {
        ICalcHate* pCalcHate = &(*it);
        ++it;

        delete pCalcHate;
    };
};


/*virtual*/ void CTarget::Update(ICalcHate::ReferenceInfo& rReferenceInfo)
{
    float fHateMax = 0.0f;
    float fHate = 0.0f;

    for (ICalcHate& it : m_listRoot)
    {
        float fHateOne = it.Get(m_iNumber, rReferenceInfo) * it.Importance();

        if (Math::FEqual(fHateOne, 0.0f))
        {
            m_fHate = 0.0f;
            return;
        };

        fHate += fHateOne;
        fHateMax += 1.0f;
    };

    m_fHate = (fHate / fHateMax);
};


/*virtual*/ void CTarget::Regist(ICalcHate* pCalcHate)
{
    ASSERT(pCalcHate);
    ASSERT(m_listRoot.contains(pCalcHate) == false);

    m_listRoot.push_back(pCalcHate);
};


/*virtual*/ void CTarget::Remove(ICalcHate* pCalcHate)
{
    ASSERT(pCalcHate);
    ASSERT(m_listRoot.contains(pCalcHate) == true);

    m_listRoot.erase(pCalcHate);
};


/*virtual*/ int32 CTarget::GetNumber(void) const
{
    return m_iNumber;
};


/*virtual*/ float CTarget::GetHate(void) const
{
    return m_fHate;
};


//
// *********************************************************************************
//


CTargetCoordinator::CTargetCoordinator(void)
: m_bPermission(true)
, m_pBeforeTarget(nullptr)
, m_pCurrentTarget(nullptr)
, m_listRoot()
, m_referenceInfo()
{
    int32 playerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        CTarget* pTarget = new CTarget(i);
        m_listRoot.push_back(pTarget);
    };
};


/*virtual*/ CTargetCoordinator::~CTargetCoordinator(void)
{
    auto it = m_listRoot.begin();
    auto itEnd = m_listRoot.end();
    while (it != itEnd)
    {
        CTarget* pTarget = &(*it);
        ++it;

        delete pTarget;
    };
};


/*virtual*/ void CTargetCoordinator::SetReferenceInfo(CTarget::ICalcHate::ReferenceInfo& rReferenceInfo)
{
    m_referenceInfo = rReferenceInfo;
};


/*virtual*/ void CTargetCoordinator::Update(void)
{
    if (!CanTheTargetBeChanged())
        return;

    m_pBeforeTarget  = m_pCurrentTarget;
    m_pCurrentTarget = nullptr;

    float fHate = 0.0f;

    for (CTarget& it : m_listRoot)
    {
        it.Update(m_referenceInfo);
        if (it.GetHate() > fHate)
        {
            fHate = it.GetHate();
            m_pCurrentTarget = &it;
        };
    };
};


bool CTargetCoordinator::RegistForHateCalculator(int32 iTarget, CTarget::ICalcHate* pCalcHate)
{
    ASSERT(iTarget < MaxTarget());
    ASSERT(pCalcHate);

    for (CTarget& it : m_listRoot)
    {
        if (it.GetNumber() == iTarget)
        {
            it.Regist(pCalcHate);
            return true;
        };
    };

    return false;
};


int32 CTargetCoordinator::MaxTarget(void) const
{
    return CGameProperty::GetPlayerNum();
};


CTarget* CTargetCoordinator::CurrentTarget() const
{
    return m_pCurrentTarget;
};


bool CTargetCoordinator::CanTheTargetBeChanged(void) const
{
    return m_bPermission;
};


void CTargetCoordinator::SetPermission(bool bState)
{
    m_bPermission = bState;
};


//
// *********************************************************************************
//


CTargetInfo::CTargetInfo(CEnemyCharacter* pEnemyChr)
: m_pEnemyChr(pEnemyChr)
, m_bExist(false)
, m_iNo(0)
, m_fDistance(0.0f)
, m_fAngle(0.0f)
, m_eState(CAIUtils::PLAYER_STATE_NULL)
{
    ;
};


/*virtual*/ CTargetInfo::~CTargetInfo(void)
{
    ;
};


/*virtual*/ void CTargetInfo::Update(void)
{
    if (m_bExist)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_iNo);
        if (!pPlayerChr)
            return;

        RwV3d vPlayerPos = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vPlayerPos);
        vPlayerPos.y = 0.0f;

        RwV3d vEnemyPos = Math::VECTOR3_ZERO;
        m_pEnemyChr->Compositor().GetFootPosition(&vEnemyPos);
        vEnemyPos.y = 0.0f;

        if (Math::FEqual(vEnemyPos.x, vPlayerPos.x) &&
            Math::FEqual(vEnemyPos.z, vPlayerPos.z))
            m_fAngle = (m_pEnemyChr->Compositor().GetDirection());
        else
            m_fAngle = CEnemyUtils::GetDirection(&vEnemyPos, &vPlayerPos);

        m_fDistance = CEnemyUtils::GetDistance(&vEnemyPos, &vPlayerPos);
        m_eState = CAIUtils::GetPlayerStateFlag(pPlayerChr->GetStatus());
    }
    else
    {
        m_fAngle = 0.0f;
        m_fDistance = 0.0f;
        m_eState = CAIUtils::PLAYER_STATE_NULL;
    };
};


/*virtual*/ void CTargetInfo::Set(int32 iNo)
{
    if (CAIUtils::GetActivePlayer(iNo))
    {
        m_iNo = iNo;
        m_bExist = true;
    }
    else
    {
        m_iNo = 0;
        m_bExist = false;
    };

    Update();
};


/*virtual*/ void CTargetInfo::Reset(void)
{
    m_iNo = 0;
    m_bExist = false;

    Update();
};


/*virtual*/ int32 CTargetInfo::Get(void) const
{
    if (!m_bExist)
        return -1;

    if (CAIUtils::GetActivePlayer(m_iNo))
        return m_iNo;

    m_iNo = 0;
    m_bExist = false;
    return -1;
};


/*virtual*/ float CTargetInfo::GetDistance(void) const
{
    return m_fDistance;
};


/*virtual*/ float CTargetInfo::GetAngle(void) const
{
    return m_fAngle;
};


/*virtual*/ CAIUtils::PLAYER_STATE_FLAG CTargetInfo::GetState(void) const
{
    return m_eState;
};


/*virtual*/ bool CTargetInfo::TestState(CAIUtils::PLAYER_STATE_FLAG eFlag) const
{
    return ((m_eState & eFlag) != 0);
};


//
// *********************************************************************************
//


CSpecialityConsider::CSpecialityConsider(void)
: m_pEnemyChr(nullptr)
, m_state(0)
{
    ;
};


/*virtual*/ CSpecialityConsider::~CSpecialityConsider(void)
{
    ;
};


/*virtual*/ void CSpecialityConsider::Update(void)
{
    m_state = 0;

    if (!m_pEnemyChr)
        return;

    ENEMYTYPES::STATUS status = m_pEnemyChr->GetStatus();

    if ((status == ENEMYTYPES::STATUS_GETUP) ||
        ((status > ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK) && (status <= ENEMYTYPES::STATUS_DEATH)))
    {
        m_state |= STATE_FLAG_DOWN;
    };

    CAIThinkOrder& AIThinkOrder = m_pEnemyChr->AIThinkOrder();
    if (AIThinkOrder.GetOrder() == CAIThinkOrder::ORDER_MOVE)
    {
        int32 moveType = AIThinkOrder.OrderMove().m_iMoveType;
        int32 moveId = AIOT::Get(moveType);

        if (moveId == AIOT::MoveRun)
            m_state |= STATE_FLAG_RUN;
        else if(moveId == AIOT::MoveWalk)
            m_state |= STATE_FLAG_WALK;
    };
};


/*virtual*/ void CSpecialityConsider::SetEnemyCharacter(const CEnemyCharacter* pEnemyChr)
{
    m_pEnemyChr = pEnemyChr;
    Update();
};


/*virtual*/ bool CSpecialityConsider::TestState(uint32 state)
{
    return (m_state & state) != 0;
};


//
// *********************************************************************************
//


/*static*/ CBaseGeneralEnemyAIModerator::SUITABLEDISTANCE
CBaseGeneralEnemyAIModerator::GetDistanceOfSuitable(float fDistance, float fDistanceOfSuitable)
{
    if (fDistance < (fDistanceOfSuitable - 1.0f))
        return SUITABLEDISTANCE_INNER;

    if (fDistance > (fDistanceOfSuitable + 1.0f))
        return SUITABLEDISTANCE_OUTER;

    return SUITABLEDISTANCE_EQUAL;
};


CBaseGeneralEnemyAIModerator::CBaseGeneralEnemyAIModerator(CEnemyCharacter* pEnemyChr)
: CAIModerator(pEnemyChr)
, m_targetCoordinator()
, m_targetInfo(pEnemyChr)
, m_pSpecialityConsider(reinterpret_cast<CSpecialityConsider*>(pEnemyChr->SharedData()))
, m_fThinkElapseTime(0.0f)
, m_fThinkBeforeTime(0.0f)
, m_bActionRun(false)
{
    CTarget::ICalcHate::ReferenceInfo refInfo;
    refInfo.m_pEnemyCharacter = pEnemyChr;
    refInfo.m_pAIModerator = this;

    m_targetCoordinator.SetReferenceInfo(refInfo);

    ThinkOrder().SetOrder(CAIThinkOrder::ORDER_NOTHING);
    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);

    if (m_pSpecialityConsider)
        m_pSpecialityConsider->SetEnemyCharacter(pEnemyChr);

    float fElapsedFrames = (1.0f / CGameProperty::GetElapsedTime());
    m_fThinkElapseTime = 1.0f / (fElapsedFrames * Characteristic().m_fThinkingFrequency);
};


CBaseGeneralEnemyAIModerator::~CBaseGeneralEnemyAIModerator(void)
{
    ;
};


void CBaseGeneralEnemyAIModerator::Run(void)
{
    m_fTimer += CGameProperty::GetElapsedTime();

    m_targetInfo.Update();

    if (m_pSpecialityConsider)
    {
        m_pSpecialityConsider->Update();
        if (m_pSpecialityConsider->TestState(CSpecialityConsider::STATE_FLAG_DOWN))
            m_targetInfo.Reset();
    };

    CAIThinkOrder::RESULT answer = ThinkOrder().GetAnswer();
    switch (answer)
    {
    case CAIThinkOrder::RESULT_WAITING:
        if (EnableThinking())
        {
            UNDERACTION underAction = UnderThinking();
            if (underAction == UNDERACTIONS_THINKOVER)
                Thinking();
        };
        break;

    case CAIThinkOrder::RESULT_ACCEPT:
        Thinking();
        break;

    case CAIThinkOrder::RESULT_REFUSE:
        m_targetInfo.Reset();
        Thinking();
        break;

    default:
        ASSERT(false);
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
        break;
    };
};


void CBaseGeneralEnemyAIModerator::Draw(void)
{
    CAIModerator::Draw();
#ifdef _DEBUG
    DebugDrawPatrolArea();
    DebugDrawSuitableArea();
    DebugDrawInfo();
    DebugDrawViewArea();
#endif /* _DEBUG */
};


CBaseGeneralEnemyAIModerator::UNDERACTION CBaseGeneralEnemyAIModerator::OnUnderMove(void)
{
    UNDERACTION underAction = OnUnderWait();
    if (underAction)
        return underAction;

    ENEMYTYPES::STATUS status = EnemyCharacter().GetStatus();
    if ((status < ENEMYTYPES::STATUS_TURN) ||
        (status > ENEMYTYPES::STATUS_DASH))
        return UNDERACTIONS_CONTINUOUS;

    int32 moveType   = ThinkOrder().OrderMove().m_iMoveType;
    int32 moveId     = AIOT::Get(moveType);
    
    if (!((moveId == AIOT::MoveRun)      ||
          (moveId == AIOT::MoveWalk)     ||
          (moveId == AIOT::MoveWalkLeft) ||
          (moveId == AIOT::MoveWalkRight)))
    {
        return UNDERACTIONS_CONTINUOUS;
    };

    int32 moveTarget = AIOT::GetTarget(moveType);

    if ((moveId == AIOT::MoveRun) ||
        (moveId == AIOT::MoveWalk))
    {
#ifdef _DEBUG
        DebugDrawMovePoint();
#endif /* _DEBUG */
        
        if (moveTarget)
        {
            if (!IsTakeMove())
            {
                ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
                return UNDERACTIONS_THINKOVER;
            };

            if (!IsMoveEndForTargetNumber())
            {
                RwV3d vecPos = Math::VECTOR3_ZERO;
                EnemyCharacter().Compositor().GetFootPosition(&vecPos);
                vecPos.y = 0.0f;

                RwV3d vecAt = Math::VECTOR3_ZERO;
                CBaseGeneralEnemyChr::GetMoveAt(ThinkOrder(), &vecAt);
                vecAt.y = 0.0f;

                float fDist = CEnemyUtils::GetDistance(&vecPos, &vecAt);
                float fRadiusOfAction = EnemyCharacter().AICharacteristic().m_fRadiusOfAction;

                if ((fRadiusOfAction * 0.5f) <= fDist)
                {
                    if (moveId != AIOT::MoveRun)
                        ThinkOrder().OrderMove().m_iMoveType = AIOT::Set(AIOT::MoveRun, AIOT::TargetNo);
                };
            }
            else
            {
                ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                return UNDERACTIONS_THINKOVER;
            };
        }
        else
        {
            if (IsMoveEndForTargetPosition())
            {
                ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                return UNDERACTIONS_THINKOVER;
            };
        };
    };

    float fCollRadius = EnemyCharacter().Compositor().GetCollisionParameter().m_fRadius;

    /* init pos */
    RwV3d vecChrPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecChrPos);

    RwV3d vecChrVel = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetVelocity(&vecChrVel);
    vecChrVel.y = 0.0f;

    Math::Vec3_Normalize(&vecChrVel, &vecChrVel);
    Math::Vec3_Scale(&vecChrVel, &vecChrVel, fCollRadius * 4.0f);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vecPos, &vecChrPos, &vecChrVel);

    /* init at */
    RwV3d vecAt = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(ThinkOrder(), &vecAt);

    Math::Vec3_Sub(&vecAt, &vecAt, &vecChrPos);
    vecAt.y = 0.0f;

    Math::Vec3_Normalize(&vecAt, &vecAt);
    Math::Vec3_Scale(&vecAt, &vecAt, fCollRadius * 4.0f);
    Math::Vec3_Add(&vecAt, &vecAt, &vecChrPos);

    CBaseGeneralEnemyChr::MOVABLETYPE movableType = static_cast<CBaseGeneralEnemyChr&>(EnemyCharacter()).GetMovableType(&vecPos);
    if (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_POSSIBLE)
    {
        return UNDERACTIONS_CONTINUOUS;
    }
    else if (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_IMPOSSIBLE)
    {
        movableType = static_cast<CBaseGeneralEnemyChr&>(EnemyCharacter()).GetMovableType(&vecAt);
        if (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_POSSIBLE)
            return UNDERACTIONS_CONTINUOUS;

        if (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_IMPOSSIBLE)
        {
            ThinkOrder().OrderMove().m_iMoveType = AIOT::Set(AIOT::MoveTurn, moveTarget);            
            return UNDERACTIONS_CONTINUOUS;
        };
    }
    else if (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_JUMP)
    {
        ThinkOrder().OrderMove().m_iMoveType = AIOT::Set(AIOT::MoveJump, moveTarget);
        return UNDERACTIONS_CONTINUOUS;
    };

    ASSERT((movableType == CBaseGeneralEnemyChr::MOVABLETYPE_FALLDOWN)    ||
           (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_HOLE)        ||
           (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_BODILYCRUSH) ||
           (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_IMPOSSIBLE)  ||
		   (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_JUMP));

    movableType = static_cast<CBaseGeneralEnemyChr&>(EnemyCharacter()).GetMovableType(&vecAt);
    if (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_POSSIBLE)
    {
        ThinkOrder().OrderMove().m_iMoveType = AIOT::Set(AIOT::MoveTurn, moveTarget);
        return UNDERACTIONS_CONTINUOUS;
    };

    if (movableType == CBaseGeneralEnemyChr::MOVABLETYPE_IMPOSSIBLE)
        return UNDERACTIONS_CONTINUOUS;

    ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
    if (m_bActionRun)
    {
        float fPatrolRadius = EnemyCharacter().Feature().m_fPatrolRadius;
        EnemyCharacter().SetPatrolArea(&vecChrPos, fPatrolRadius);
    };

    return UNDERACTIONS_THINKOVER;
};


bool CBaseGeneralEnemyAIModerator::IsOutsidePatrolArea(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetPosition(&vPosition);

    return (!CAIUtils::IsInsidePatrolArea(&vPosition, &EnemyCharacter().Feature()));
};


bool CBaseGeneralEnemyAIModerator::IsTakeMove(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetPosition(&vPosition);

    if (CAIUtils::IsInsidePatrolArea(&vPosition, &EnemyCharacter().Feature()))
        return (m_targetInfo.Get() != -1);

    return false;
};


AIOT::TYPE CBaseGeneralEnemyAIModerator::IsTakeSideWalk(void)
{
    int32 no = m_targetInfo.Get();

    if (!CAIUtils::GetActivePlayer(no))
        return AIOT::Null;

    if (!EnemyCharacter().IsAttachedStatusObserver(ENEMYTYPES::STATUS_WALK_LEFT))
        return AIOT::Null;

    if (!EnemyCharacter().IsAttachedStatusObserver(ENEMYTYPES::STATUS_WALK_RIGHT))
        return AIOT::Null;

    float fDistanceOfSuitable = (Characteristic().m_fDistanceOfSuitable * 1.5f);    
    if (m_targetInfo.GetDistance() > fDistanceOfSuitable)
        return AIOT::Null;

    float fDir = EnemyCharacter().Compositor().GetDirection();
    float fDiff = m_targetInfo.GetAngle() - fDir;
    float fDiffAbs = std::fabs(fDiff);

    float fRatioOfFrontView = Characteristic().m_fRatioOfFrontView;
    float fFrontView = fRatioOfFrontView * MATH_DEG2RAD(54.0f);

    if (fFrontView < fDiffAbs)
        return AIOT::Null;

    if ((fDiff < 0.0f) ||
        (fDiff > MATH_PI))
        return AIOT::MoveWalkLeft;

    return AIOT::MoveWalkRight;
};


CBaseGeneralEnemyAIModerator::UNDERACTION CBaseGeneralEnemyAIModerator::UnderThinking(void)
{
    UNDERACTION underAction = OnUnderAllActions();
    if (underAction != UNDERACTIONS_CONTINUOUS)
        return underAction;

    CAIThinkOrder::ORDER order = ThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_NOTHING:
    case CAIThinkOrder::ORDER_WAIT:
        underAction = OnUnderWait();
        break;

    case CAIThinkOrder::ORDER_MOVE:
        underAction = OnUnderMove();
        break;

    case CAIThinkOrder::ORDER_RUN:
        underAction = OnUnderRun();
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        underAction = OnUnderAttack();
        break;

    case CAIThinkOrder::ORDER_DEFENCE:
        underAction = OnUnderDefence();
        break;

    default:
        ASSERT(false);
        break;
    };

    return underAction;
};


void CBaseGeneralEnemyAIModerator::Thinking(void)
{
    int32 playerNo = SearchTarget();

    CAIThinkOrder::ORDER order = ActionSelector(playerNo);

    m_bActionRun = false;

    while (true)
    {
        switch (order)
        {
        case CAIThinkOrder::ORDER_NOTHING:
        case CAIThinkOrder::ORDER_WAIT:
            {
                if (OnActionOfWait(playerNo))
                    return;
                
                ASSERT(false);
            }
            break;

        case CAIThinkOrder::ORDER_MOVE:
            {
                if (OnActionOfMove(playerNo))
                    return;
                
                order = CAIThinkOrder::ORDER_WAIT;
            }
            break;

        case CAIThinkOrder::ORDER_RUN:
            {
                if (OnActionOfRun(playerNo))
                {
                    m_bActionRun = true;
                    return;
                };

                order = CAIThinkOrder::ORDER_WAIT;
            }
            break;

        case CAIThinkOrder::ORDER_ATTACK:
            {
                if (OnActionOfAttack(playerNo))
                    return;

                order = CAIThinkOrder::ORDER_WAIT;
            }
            break;

        case CAIThinkOrder::ORDER_DEFENCE:
            {
                if (OnActionOfDefence(playerNo))
                    return;

                order = CAIThinkOrder::ORDER_WAIT;
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


bool CBaseGeneralEnemyAIModerator::EnableThinking(void)
{
    ENEMYTYPES::STATUS status = EnemyCharacter().GetStatus();

    if (status == ENEMYTYPES::STATUS_THINKING)
        return false;

    if ((status >= ENEMYTYPES::STATUS_KNOCK) &&
        (status <= ENEMYTYPES::STATUS_THROWN_BACK))
        return false;

    if ((ThinkOrder().GetOrder()  == CAIThinkOrder::ORDER_NOTHING) &&
        (ThinkOrder().GetAnswer() == CAIThinkOrder::RESULT_WAITING))
        return false;

    if ((m_fTimer - m_fThinkBeforeTime) < m_fThinkElapseTime)
        return false;

    m_fThinkBeforeTime = m_fTimer;

    return true;
};


bool CBaseGeneralEnemyAIModerator::IsSatifyFrequency(int32 no) const
{
    uint8 freq = AIFreqParam().GetFrequency(no);
    uint8 rnd = static_cast<uint8>(Math::RandRange(0, 100));

    return (freq > rnd);
};


float CBaseGeneralEnemyAIModerator::GetFrequency(int32 no) const
{
    return static_cast<float>(AIFreqParam().GetFrequency(no));
};


int32 CBaseGeneralEnemyAIModerator::SearchTarget(void)
{
    int32 beforeTarget = -1;
    int32 newTarget = -1;

    if (m_targetInfo.Get() >= 0)
    {
        int32 no = m_targetInfo.Get();
        if (CAIUtils::GetActivePlayer(no))
            beforeTarget = no;
    };

    m_targetCoordinator.Update();

    CTarget* pTarget = m_targetCoordinator.CurrentTarget();
    if (pTarget)
    {
        int32 no = pTarget->GetNumber();

#ifdef _DEBUG        
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(no);
        ASSERT(pPlayerChr != nullptr);
#endif /* _DEBUG */        

        float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
        float fRand = Math::RandFloat();

        if (fRand < fRatioOfActivity)
            newTarget = no;
    }
    else if (beforeTarget >= 0)
    {
        m_targetInfo.Set(-1);
        return -1;
    };
	
    if ((beforeTarget < 0) || (newTarget < 0))
    {
        if ((beforeTarget >= 0) && (newTarget == -1))
            newTarget = beforeTarget;
    }
    else
    {
        float fRatioOfImpulsiveness = Characteristic().m_fRatioOfImpulsiveness;
        float fRand = Math::RandFloat();

        if (fRatioOfImpulsiveness < fRand)
            newTarget = beforeTarget;
    };
	
    m_targetInfo.Set(newTarget);

    return newTarget;
};


void CBaseGeneralEnemyAIModerator::DebugDrawMovePoint(void)
{
#ifdef _DEBUG
    if (!CEnemyDebug::SHOW_AI_MOVE_POINT)
        return;

    /* draw order destination point */
    RwV3d vecOrderPos = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(ThinkOrder(), &vecOrderPos);

    RwSphere sphere;
    sphere.center = vecOrderPos;
    sphere.radius = 0.5f;

    RwRGBA color = { 0xFF, 0xFF, 0x30, 0xFF };

    CDebugShape::m_fDuration = m_fThinkElapseTime;

    CDebugShape::ShowSphere(&sphere, color);

    /* draw name of enemy */
    const char* pszObjName = EnemyCharacter().GetName();
    ASSERT(pszObjName);

    CDebugShape::m_fLabelHeight = 12.0f;
    CDebugShape::m_fLabelOffsetY = -14.0f;

    CDebugShape::ShowLabel(&vecOrderPos, pszObjName, color);

    CDebugShape::m_fDuration = 0.0f;
#endif /* _DEBUG */
};


void CBaseGeneralEnemyAIModerator::DebugDrawPatrolArea(void)
{
#ifdef _DEBUG
    if (!CEnemyDebug::SHOW_AI_PATROL_AREA)
        return;

    RwV3d vecPatrolOrigin = EnemyCharacter().Feature().m_vPatrolOrigin;
    float fPatrolRadius = EnemyCharacter().Feature().m_fPatrolRadius;

    RwSphere sphere;
    sphere.center = vecPatrolOrigin;
    sphere.radius = fPatrolRadius;

    RwRGBA color = { 0xFF, 0x00, 0x00, 0x40 };
    CDebugShape::ShowSphere(&sphere, color);
#endif /* _DEBUG */
};


void CBaseGeneralEnemyAIModerator::DebugDrawInfo(void)
{
#ifdef _DEBUG
    if (!CEnemyDebug::SHOW_AI_INFO)
        return;

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecMyPos);

    float fHeight = EnemyCharacter().Compositor().GetCollisionParameter().m_fHeight;

    vecMyPos.y += (fHeight * 1.5f);

    enum
    {
        INFOFLAG_NAME       = (1 << 0),
        INFOFLAG_ORDER      = (1 << 2),
        INFOFLAG_TARGET_NO  = (1 << 3),
        INFOFLAG_TARGET_HATE= (1 << 4),
    };

    uint32 infoFlag = 0;

    char szName[128];
    szName[0] = '\0';

    char szInfoOrder[128];
    szInfoOrder[0] = '\0';

    char szInfoTarget[128];
    szInfoTarget[0] = '\0';

    char szInfoHate[128];
    szInfoHate[0] = '\0';

    /* get name */
    const char* pszObjName = EnemyCharacter().GetName();
    ASSERT(pszObjName);

    std::sprintf(szName, "%s", pszObjName);
    infoFlag |= INFOFLAG_NAME;

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

    std::sprintf(szInfoOrder, "ORDER: %s", s_apszOrderName[ThinkOrder().GetOrder()]);
    infoFlag |= INFOFLAG_ORDER;

    /* get target info */
    CTarget* pTarget = m_targetCoordinator.CurrentTarget();
    if (pTarget)
    {
        std::sprintf(szInfoTarget, "TARGET: %" PRId32, pTarget->GetNumber());
        infoFlag |= INFOFLAG_TARGET_NO;

        std::sprintf(szInfoHate, "HATE: %.02f", pTarget->GetHate());
        infoFlag |= INFOFLAG_TARGET_HATE;
    }
    else
    {
        std::sprintf(szInfoTarget, "NO TARGET");
        infoFlag |= INFOFLAG_TARGET_NO;
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

    if (infoFlag & INFOFLAG_TARGET_NO)
    {
        CDebugShape::ShowLabel(&vecMyPos, szInfoTarget, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    if (infoFlag & INFOFLAG_TARGET_HATE)
    {
        CDebugShape::ShowLabel(&vecMyPos, szInfoHate, color);
        CDebugShape::m_fLabelOffsetY += CDebugShape::m_fLabelHeight;
    };

    CDebugShape::m_fLabelOffsetY = 0.0f;
#endif /* _DEBUG */
};


void CBaseGeneralEnemyAIModerator::DebugDrawSuitableArea(void)
{
#ifdef _DEBUG
    if (!CEnemyDebug::SHOW_AI_SUITABLE_AREA)
        return;

    float fDistOfSuit = Characteristic().m_fDistanceOfSuitable;
    float fRadiusOfAction = Characteristic().m_fRadiusOfAction;

    RwV3d vecPosition = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetFootPosition(&vecPosition);

    RwSphere sphere;
    sphere.center = vecPosition;
    sphere.radius = (fRadiusOfAction - fDistOfSuit);

    RwRGBA color = { 0x00, 0xFF, 0x00, 0x40 };
    CDebugShape::ShowSphere(&sphere, color);
#endif /* _DEBUG */
};


void CBaseGeneralEnemyAIModerator::DebugDrawViewArea(void)
{

};