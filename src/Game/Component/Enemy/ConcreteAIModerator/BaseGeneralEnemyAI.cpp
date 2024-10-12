#include "BaseGeneralEnemyAI.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseGeneralEnemyChr.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


namespace AIOT
{
    int32 Set(int32 idx, int32 target)
    {
        return (idx | target);
    };


    int32 Get(int32 type)
    {
        return (type & 0x7FFFFFFF);
    };


    int32 GetTarget(int32 type)
    {
        return (type & 0x80000000);
    };


    void SetThinkOrder(CAIThinkOrder& rAIThinkOrder, CAIThinkOrder::ORDER eOrder, int32 eTypeIdx, float fTimer, RwV3d* pvVec)
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
                ASSERT((eTypeIdx >= AIOT::TYPE::MoveTurn) && (eTypeIdx <= AIOT::TYPE::MoveJump));

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
                ASSERT((eTypeIdx >= AIOT::TYPE::AttackA) && (eTypeIdx <= AIOT::TYPE::FireRepeatC));

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


    void SetThinkOrder(CAIThinkOrder& rAIThinkOrder, CAIThinkOrder::ORDER eOrder, int32 eTypeIdx, float fTimer, int32 iNumber)
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
                wait.m_iWaitType    = Set(eTypeIdx, 0x80000000);
            }
            break;
            
        case CAIThinkOrder::ORDER_MOVE:
            {
                ASSERT((eTypeIdx >= AIOT::TYPE::MoveTurn) && (eTypeIdx <= AIOT::TYPE::MoveJump));

                CAIThinkOrder::ORDERMOVE& move = rAIThinkOrder.OrderMove();
                move.m_vAt          = Math::VECTOR3_ZERO;
                move.m_fMoveTimer   = fTimer;
                move.m_iPlayerNo    = iNumber;
                move.m_iMoveType    = Set(eTypeIdx, 0x80000000);
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
                ASSERT((eTypeIdx >= AIOT::TYPE::AttackA) && (eTypeIdx <= AIOT::TYPE::FireRepeatC));

                CAIThinkOrder::ORDERATTACK& attack = rAIThinkOrder.OrderAttack();
                attack.m_vAt            = Math::VECTOR3_ZERO;
                attack.m_iPlayerNo      = iNumber;
                attack.m_iAttackType    = Set(eTypeIdx, 0x80000000);
            }
            break;

        default:
            break;
        };
    };


    void SetWaitOrder(CAIThinkOrder& rAIThinkOrder, float fTimer, RwV3d* pvVec)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_WAIT, 0, fTimer, pvVec);
    };


    void SetMoveOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, float fTimer, RwV3d* pvVec)
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


    void SetFireOrder(CAIThinkOrder& rAIThinkOrder, int32 typeIdx, int32 iNumber, RwV3d* pvVec)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_ATTACK, typeIdx, 0.0f, pvVec);
    };


    void SetDefenceOrder(CAIThinkOrder& rAIThinkOrder)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_DEFENCE, 0, 0.0f, nullptr);
    };


    void SetNothingOrder(CAIThinkOrder& rAIThinkOrder)
    {
        SetThinkOrder(rAIThinkOrder, CAIThinkOrder::ORDER_NOTHING, 0, 0.0f, nullptr);
    };
}; /* namespace AIOT */


/*static*/ const float CTarget::HateMax = 1.0f;


float CTarget::ICalcHateOfGeometry::Importance(void)
{
    return HateMax;
};


float CTarget::ICalcHateOfGeometry::Get(int32 iNo, ReferenceInfo& rReferenceInfo)
{
    CPlayerCharacter* pPlayerCharacter = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerCharacter)
        return 0.0f;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    pPlayerCharacter->GetFootPosition(&vPosition);

    CBaseGeneralEnemyChr* pBaseGeneralEnemyChr = static_cast<CBaseGeneralEnemyChr*>(rReferenceInfo.m_pEnemyCharacter);
    CCharacterCompositor& rCharaterCompositor = pBaseGeneralEnemyChr->CharacterCompositor();

    /* TODO CEnemyUtils */
    /* TODO CAIUtils */

    return 0.0f;
};


float CTarget::ICalcHateOfCollision::Importance(void)
{
    return HateMax;
};


float CTarget::ICalcHateOfCollision::Get(int32 iNo, ReferenceInfo& rReferenceInfo)
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
    };

    return 0.0f;
};


float CTarget::ICalcHateOfTargetDamaged::Importance(void)
{
    return (HateMax * 0.5f);
};


float CTarget::ICalcHateOfTargetDamaged::Get(int32 iNo, ReferenceInfo& rReferenceInfo)
{
    CPlayerCharacter* pPlayerCharacter = CAIUtils::GetActivePlayer(iNo);
    if (!pPlayerCharacter)
        return 0.0f;

    int32 playerNum = CGameProperty::GetPlayerNum();
    int32 totalDamage = 0;

    for (int32 i = 1; i < playerNum; ++i)
    {
        IGamePlayer* pPlayer = CGameProperty::Player(i);
        if (pPlayer->IsAlive())
            totalDamage += pPlayer->GetDamage();
    };

    /* TODO test it */
    return static_cast<float>((CGameProperty::GetGamePlayer(iNo)->GetDamage() + 1) / totalDamage);
};


float CTarget::ICalcHateOfDamageAccumulation::Importance(void)
{
    return (HateMax * 0.5f);
};


float CTarget::ICalcHateOfDamageAccumulation::Get(int32 iNo, ReferenceInfo& rReferenceInfo)
{
    CBaseGeneralEnemyChr* pBaseGeneralEnemyChr = static_cast<CBaseGeneralEnemyChr*>(rReferenceInfo.m_pEnemyCharacter);

    int32 totalDamage = 0;
    int32 playerNum = CGameProperty::GetPlayerNum();

    for (int32 i = 0; i < playerNum; ++i)
        totalDamage += pBaseGeneralEnemyChr->GetDamageAccumulation(i);

    /* TODO test it */
    return static_cast<float>((pBaseGeneralEnemyChr->GetDamageAccumulation(iNo) + 1) / totalDamage);
};


float CTarget::ICalcHateOfTurtleCharacteristic::Importance(void)
{
    return (HateMax * 0.3f);
};


float CTarget::ICalcHateOfTurtleCharacteristic::Get(int32 iNo, ReferenceInfo& rReferenceInfo)
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
    case PLAYERID::ID_CAS:
    case PLAYERID::ID_KAR:
        return 0.9f;

    case PLAYERID::ID_DON:
        return 0.5f;

    case PLAYERID::ID_SLA:
        return 0.1f;

    default:
        break;
    };

    return 1.0f;
};


float CTarget::ICalcHateOfTurtleStatus::Importance(void)
{
    return (HateMax * 0.3f);
};


float CTarget::ICalcHateOfTurtleStatus::Get(int32 iNo, ReferenceInfo& rReferenceInfo)
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

    const uint32 stateFlag =  CAIUtils::PLAYER_STATE_ATTACK
                            | CAIUtils::PLAYER_STATE_AERIAL
                            | CAIUtils::PLAYER_STATE_DOWN
                            | CAIUtils::PLAYER_STATE_UNUSUAL
                            | CAIUtils::PLAYER_STATE_KNIFE;

    static_assert(stateFlag == 0x7C, "update me");

    if (!(CAIUtils::GetPlayerStateFlag(eStatus) & stateFlag))
        return 1.0f;

    return 0.8f;
};


float CTarget::ICalcHateOfRandom::Importance(void)
{
    return (HateMax * 0.3f);
};


float CTarget::ICalcHateOfRandom::Get(int32 iNo, ReferenceInfo& rReferenceInfo)
{
    return Math::RandFloat();
};


CTarget::CTarget(int32 iNo)
: m_listRoot()
, m_iNumber(iNo)
, m_fHate(0.0f)
{
    ;
};


CTarget::~CTarget(void)
{
    auto it = m_listRoot.begin();
    while (it != m_listRoot.end())
    {
        ICalcHate* pCalcHate = &(*it);
        ++it;

        delete pCalcHate;
    };
};


void CTarget::Update(ICalcHate::ReferenceInfo& rReferenceInfo)
{
    float fHateMax = 0.0f;
    float fHate = 0.0f;

    for (auto& it : m_listRoot)
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


void CTarget::Regist(ICalcHate* pCalcHate)
{
    ASSERT(pCalcHate);
    ASSERT(m_listRoot.contains(pCalcHate) == false);

    m_listRoot.push_back(pCalcHate);
};


void CTarget::Remove(ICalcHate* pCalcHate)
{
    ASSERT(pCalcHate);
    ASSERT(m_listRoot.contains(pCalcHate) == true);

    m_listRoot.erase(pCalcHate);
};


int32 CTarget::GetNumber(void) const
{
    return m_iNumber;
};


float CTarget::GetHate(void) const
{
    return m_fHate;
};


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


CTargetCoordinator::~CTargetCoordinator(void)
{
    auto it = m_listRoot.begin();
    while (it != m_listRoot.end())
    {
        CTarget* pTarget = &(*it);
        ++it;

        delete pTarget;
    };
};


void CTargetCoordinator::SetReferenceInfo(CTarget::ICalcHate::ReferenceInfo& rReferenceInfo)
{
    m_referenceInfo = rReferenceInfo;
};


void CTargetCoordinator::Update(void)
{
    if (!m_bPermission)
        return;

    m_pBeforeTarget = m_pCurrentTarget;
    m_pCurrentTarget = nullptr;

    float fHate = 0.0f;

    for (auto& it : m_listRoot)
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

    for (auto& it : m_listRoot)
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


CTargetInfo::CTargetInfo(CEnemyCharacter& rEnemyCharacter)
: m_rEnemyCharacter(rEnemyCharacter)
, m_bExist(false)
, m_iNo(0)
, m_fDistance(0.0f)
, m_fAngle(0.0f)
, m_eState(CAIUtils::PLAYER_STATE_NULL)
{
    ;
};


CTargetInfo::~CTargetInfo(void)
{
    ;
};


void CTargetInfo::Update(void)
{
    if (m_bExist)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_iNo);
        if (!pPlayerChr)
            return;

        RwV3d vPosition = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vPosition);

        vPosition.y = 0.0f;

        /* TODO CEnemyUtils */
    }
    else
    {
        m_fAngle = 0.0f;
        m_fDistance = 0.0f;
        m_eState = CAIUtils::PLAYER_STATE_NULL;
    };
};


void CTargetInfo::Set(int32 iNo)
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


void CTargetInfo::Reset(void)
{
    m_iNo = 0;
    m_bExist = false;

    Update();
};


int32 CTargetInfo::Get(void) const
{
    if (!m_bExist)
        return -1;

    if (CAIUtils::GetActivePlayer(m_iNo))
        return m_iNo;

    m_iNo = 0;
    m_bExist = false;
    return -1;
};


float CTargetInfo::GetDistance(void) const
{
    return m_fDistance;
};


float CTargetInfo::GetAngle(void) const
{
    return m_fAngle;
};


CAIUtils::PLAYER_STATE_FLAG CTargetInfo::GetState(void) const
{
    return m_eState;
};


bool CTargetInfo::TestState(CAIUtils::PLAYER_STATE_FLAG eFlag) const
{
    return (m_eState & eFlag) != 0;
};


CSpecialityConsider::CSpecialityConsider(void)
: m_pEnemyCharacter(nullptr)
, m_state(0)
{
    ;
};


CSpecialityConsider::~CSpecialityConsider(void)
{
    ;
};


void CSpecialityConsider::Update(void)
{

};


void CSpecialityConsider::SetEnemyCharacter(const CEnemyCharacter* pEnemyCharacter)
{
    m_pEnemyCharacter = pEnemyCharacter;
};


bool CSpecialityConsider::TestState(uint32 state)
{
    return (m_state & state) != 0;
};