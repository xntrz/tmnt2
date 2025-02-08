#include "EnemyWatcher.hpp"
#include "EnemyCharacter.hpp"
#include "CharacterCompositor.hpp"
#include "Enemy.hpp"
#include "EnemyUtils.hpp"
#include "AIUtils.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


/*static*/ CEnemyContainer* CEnemyContainer::m_pInstance = nullptr;


/*static*/ CEnemyContainer& CEnemyContainer::Instance(void)
{
    if (!m_pInstance)
    {
        /**
         *  Since this is game object it will be automatically freed when
         *  game object manager terminates and pointer will be set to NULL in destructor.
         */
        m_pInstance = new CEnemyContainer;
    };

    return *m_pInstance;
};


CEnemyContainer::CServeEnemy::CServeEnemy(void)
: m_hEnemy(0)
, m_eID(ENEMYID::ID_DUMMY)
, m_bBusy(false)
{
    ;
};


CEnemyContainer::CServeEnemy::~CServeEnemy(void)
{
    ;
};


void CEnemyContainer::CServeEnemy::Set(CEnemyCharacter* pEnemyChr)
{
    ASSERT(m_bBusy == false);

    m_hEnemy = pEnemyChr->GetOwner();
    m_eID    = pEnemyChr->GetID();
    m_bBusy  = true;
};


void CEnemyContainer::CServeEnemy::Clean(void)
{
    ASSERT(m_bBusy == true);

    m_bBusy  = false;
    m_eID    = ENEMYID::ID_DUMMY;
    m_hEnemy = 0;
};


CEnemyCharacter* CEnemyContainer::CServeEnemy::GetCharacter(void) const
{
    CGameObject* pObj = CGameObjectManager::GetObject(m_hEnemy);
    ASSERT(pObj);
    ASSERT(pObj->GetType() == GAMEOBJECTTYPE::ENEMY);

    CEnemyCharacter* pEnemyChr = &static_cast<CEnemy*>(pObj)->Character();
    ASSERT(pEnemyChr);

    return pEnemyChr;
};


void CEnemyContainer::CServeEnemy::GetPosition(RwV3d* pvPosition) const
{
    CEnemyCharacter* pEnemyChr = GetCharacter();
    if (pEnemyChr)
        pEnemyChr->Compositor().GetFootPosition(pvPosition);
};


bool CEnemyContainer::CServeEnemy::IsBusy(void) const
{
    return m_bBusy;
};


bool CEnemyContainer::CServeEnemy::IsValid(void) const
{
    ASSERT(m_bBusy == true);

    CGameObject* pObj = CGameObjectManager::GetObject(m_hEnemy);
    if (pObj == nullptr)
        return false;

    if (pObj->GetType() != GAMEOBJECTTYPE::ENEMY)
        return false;

    return true;
};


ENEMYID::VALUE CEnemyContainer::CServeEnemy::GetID(void) const
{
    return m_eID;
};


uint32 CEnemyContainer::CServeEnemy::GetOwner(void) const
{
    return m_hEnemy;
};


//
// *********************************************************************************
//


CEnemyContainer::CEnemyContainer(void)
: CGameObject("EnemyContainer", GAMEOBJECTTYPE::MANIPULATOR)
, m_eIDFilter(ENEMYID::ID_DUMMY)
, m_aServeEnemy()
, m_iEnemyFind(0)
{
    ASSERT(m_pInstance == this);
};


CEnemyContainer::~CEnemyContainer(void)
{
    ASSERT(m_pInstance == this);
    m_pInstance = nullptr;
};


void CEnemyContainer::Run(void)
{
    ;
};


CEnemyContainer::CServeEnemy* CEnemyContainer::FindFirst(void)
{
    m_iEnemyFind = 0;
    return FindNext();
};


CEnemyContainer::CServeEnemy* CEnemyContainer::FindNext(void)
{
    for (int32 i = m_iEnemyFind; i < COUNT_OF(m_aServeEnemy); ++i)
    {
        if (m_aServeEnemy[i].IsBusy())
        {
            if (!m_aServeEnemy[i].IsValid())
            {
                m_aServeEnemy[i].Clean();
            }
            else
            {
                if (m_eIDFilter && (m_eIDFilter == m_aServeEnemy[i].GetID()))
                {
                    m_iEnemyFind = (i + 1);
                    return &m_aServeEnemy[i];
                };
            };
        };
    };

    return nullptr;
};


void CEnemyContainer::Attach(CEnemyCharacter* pEnemyChr)
{
    CServeEnemy* pServeEnemy = nullptr;

    for (int32 i = 0; i < COUNT_OF(m_aServeEnemy); ++i)
    {
        if (m_aServeEnemy[i].IsBusy())              // check if record busy
        {
            if (!m_aServeEnemy[i].IsValid())        // but is not valid
            {
                m_aServeEnemy[i].Clean();           // clean it
                pServeEnemy = &m_aServeEnemy[i];    // and return
            };
        }
        else
        {
            pServeEnemy = &m_aServeEnemy[i];        // record not busy - return it
        };

        if (pServeEnemy)
            break;
    };

    ASSERT(pServeEnemy != nullptr, "no free records");

    if (pServeEnemy)
        pServeEnemy->Set(pEnemyChr);
};


void CEnemyContainer::SetEnemyIDFilter(ENEMYID::VALUE eID)
{
    m_eIDFilter = eID;
};


//
// *********************************************************************************
//


CEnemyWatcher::CEnemyWatcher(void)
: m_pEnemyChr(nullptr)
, m_bEnable(false)
{
    ;
};


CEnemyWatcher::~CEnemyWatcher(void)
{
    ;
};


void CEnemyWatcher::Initialize(CEnemyCharacter* pEnemyChr, bool bEnable)
{
    m_pEnemyChr = pEnemyChr;
    m_bEnable = bEnable;
};


bool CEnemyWatcher::IsFriendAttack(void)
{
    SetEnemyIDFilter();

    ASSERT(Search(m_pEnemyChr->GetOwner()));
    
    CEnemyContainer::CServeEnemy* pServeEnemy = CEnemyContainer::Instance().FindFirst();
    while (pServeEnemy)
    {
        if (pServeEnemy->GetOwner() != m_pEnemyChr->GetOwner())
        {
            CEnemyCharacter* pEnemyChr = pServeEnemy->GetCharacter();

            if (CEnemyUtils::IsAttackState(pEnemyChr->GetStatus()))
                return true;
        };

        pServeEnemy = CEnemyContainer::Instance().FindNext();
    };

    return false;
};


bool CEnemyWatcher::IsFriendStayFront(float fDistance)
{
    SetEnemyIDFilter();

    ASSERT(Search(m_pEnemyChr->GetOwner()));

    RwV3d vMyPosition = Math::VECTOR3_ZERO;
    m_pEnemyChr->Compositor().GetFootPosition(&vMyPosition);

    CEnemyContainer::CServeEnemy* pServeEnemy = CEnemyContainer::Instance().FindFirst();
    while (pServeEnemy)
    {
        if (pServeEnemy->GetOwner() != m_pEnemyChr->GetOwner())
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            pServeEnemy->GetPosition(&vPosition);

            if (CEnemyUtils::GetDistance(&vPosition, &vMyPosition) < fDistance)
            {
                float fDirection = CEnemyUtils::GetDirection(&vPosition, &vMyPosition);

                float fRad = (fDirection - m_pEnemyChr->Compositor().GetDirection());

                fRad = CEnemyUtils::RadianCorrect(fRad);
                fRad = std::fabs(fRad);

                if (fRad > MATH_DEG2RAD(160.0f))
                    return true;
            };
        };

        pServeEnemy = CEnemyContainer::Instance().FindNext();
    };

    return false;
};


int32 CEnemyWatcher::GetFreePlayerNo(int32 iPlayerPurposeMax /*= -1*/)
{
    SetEnemyIDFilter();

    ASSERT(Search(m_pEnemyChr->GetOwner()));

    int32 aPlayerPurposeNum[GAMETYPES::PLAYERS_MAX] = { 0 };

    CEnemyContainer::CServeEnemy* pServeEnemy = CEnemyContainer::Instance().FindFirst();
    while (pServeEnemy)
    {
        if (pServeEnemy->GetOwner() != m_pEnemyChr->GetOwner())
        {
            int32 playerNo = GetPurposePlayerNo(pServeEnemy->GetOwner());
            if (playerNo > -1)
                ++aPlayerPurposeNum[playerNo];                
        };

        pServeEnemy = CEnemyContainer::Instance().FindNext();
    };

    float fDistMin = static_cast<float>(TYPEDEF::UINT16_MAX);
    int32 iPlayerPurposeNumMin = static_cast<int32>(TYPEDEF::UINT16_MAX);
    int32 iPlayerNo = -1;

    int32 iPlayerNum = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < iPlayerNum; ++i)
    {
        if (iPlayerPurposeMax == -1)
        {
            if (aPlayerPurposeNum[i] < iPlayerPurposeNumMin)
            {
                iPlayerPurposeNumMin = aPlayerPurposeNum[i];
                iPlayerNo = i;
            };
        }
        else if (aPlayerPurposeNum[i] <= iPlayerPurposeMax)
        {
            RwV3d vMyPosition = Math::VECTOR3_ZERO;
            m_pEnemyChr->Compositor().GetFootPosition(&vMyPosition);

            float fDistance = CEnemyUtils::GetDistanceFromPlayer(i, &vMyPosition);
            if (fDistance < fDistMin)
            {
                fDistMin = fDistance;
                iPlayerNo = i;
            };
        };
    };

    return iPlayerNo;
};


CEnemyCharacter* CEnemyWatcher::Search(uint32 hEnemy)
{
    CEnemyContainer::CServeEnemy* pServeEnemy = CEnemyContainer::Instance().FindFirst();
    while (pServeEnemy)
    {
        if (pServeEnemy->GetOwner() == hEnemy)
            return pServeEnemy->GetCharacter();

        pServeEnemy = CEnemyContainer::Instance().FindNext();
    };

    return nullptr;
};


int32 CEnemyWatcher::GetPurposePlayerNo(uint32 hEnemy)
{
    int32 iPlayerNo = -1;

    CEnemyCharacter* pEnemyChr = Search(hEnemy);

    CAIThinkOrder::ORDER orderType = pEnemyChr->AIThinkOrder().GetOrder();
    switch (orderType)
    {
    case CAIThinkOrder::ORDER_ATTACK:
        iPlayerNo = pEnemyChr->AIThinkOrder().OrderAttack().m_iPlayerNo;
        break;

    case CAIThinkOrder::ORDER_MOVE:
        iPlayerNo = pEnemyChr->AIThinkOrder().OrderMove().m_iPlayerNo;
        break;

    case CAIThinkOrder::ORDER_WAIT:
        iPlayerNo = pEnemyChr->AIThinkOrder().OrderWait().m_iPlayerNo;
        break;

    default:
        break;
    };

    /**
     *  TODO:   review and test this later - in IDB there '>' not '>='
     *          but player indexing starts from 0
     */
    if (iPlayerNo >= CAIUtils::GetPlayerNum())
        iPlayerNo = -1;

    return iPlayerNo;
};


void CEnemyWatcher::SetEnemyIDFilter(void)
{
    if (m_bEnable)
    {
        ASSERT(m_pEnemyChr);
        CEnemyContainer::Instance().SetEnemyIDFilter(m_pEnemyChr->GetID());
    }
    else
    {
        CEnemyContainer::Instance().SetEnemyIDFilter(ENEMYID::ID_DUMMY);
    };
};