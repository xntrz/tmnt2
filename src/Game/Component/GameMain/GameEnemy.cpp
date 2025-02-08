#include "GameEnemy.hpp"

#include "Game/Component/Enemy/Enemy.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


/*static*/ CGameEnemy CGameEnemy::m_dummy;


/*static*/ CGameEnemy* CGameEnemy::Dummy(void)
{
    return &m_dummy;
};


CGameEnemy::CGameEnemy(void)
: m_hObj(0)
, m_pEnemy(nullptr)
, m_vPosition(Math::VECTOR3_ZERO)
, m_bActivate(false)
{
    ;
};


CGameEnemy::~CGameEnemy(void)
{
    ;
};


void CGameEnemy::Attach(CEnemy* pEnemy)
{
    ASSERT(pEnemy);

    m_pEnemy = pEnemy;
    m_hObj = pEnemy->GetHandle();
	m_bActivate = true;
};


void CGameEnemy::Detach(void)
{
    m_pEnemy = nullptr;
    m_hObj = 0;
};


void CGameEnemy::InvokeDeathFloor(void)
{
    if (IsAlive())
    {
        CCharacterCompositor* pChrCompositor = &Enemy().Character().Compositor();
        CGameObjectManager::SendMessage(pChrCompositor, CHARACTERTYPES::MESSAGEID_DEATHFLOOR);
    };
};


void CGameEnemy::Activate(void)
{
    if (IsAlive())
    {
        m_bActivate = true;
        Enemy().StartAI();
    };
};


void CGameEnemy::Inactivate(void)
{
    if (IsAlive())
    {
        m_bActivate = false;
        Enemy().StopAI();
    };
};


bool CGameEnemy::IsActive(void) const
{
    if (IsAlive())
        return m_bActivate;
    
    return false;
};


CEnemy* CGameEnemy::GetEnemy(void)
{
    return m_pEnemy;
};


void CGameEnemy::GetPosition(RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    if (IsAlive())
    {
        Enemy().GetPosition(&m_vPosition);
        *pvPosition = m_vPosition;
    }
    else
    {
        *pvPosition = Math::VECTOR3_ZERO;
    };
};


bool CGameEnemy::IsAlive(void) const
{
    CGameObject* pObject = CGameObjectManager::GetObject(m_hObj);
    if (!pObject)
        return false;

    ASSERT(pObject == m_pEnemy);

    return true;
};


CEnemy& CGameEnemy::Enemy(void) const
{
    ASSERT(m_pEnemy);
    
    return *m_pEnemy;
};