#include "GameGimmick.hpp"

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


/*static*/ CGameGimmick& CGameGimmick::Dummy(void)
{
    static CGameGimmick s_DummyGameGimmick;
    
    return s_DummyGameGimmick;
};


CGameGimmick::CGameGimmick(void)
: m_hObj(0)
, m_pGimmick(nullptr)
, m_vPosition(Math::VECTOR3_ZERO)
{
    ;
};


CGameGimmick::~CGameGimmick(void)
{
    ;
};


void CGameGimmick::Attach(CGimmick* pGimmick)
{
    ASSERT(pGimmick);

    m_pGimmick = pGimmick;
    m_hObj = pGimmick->GetHandle();
};


void CGameGimmick::Detach(void)
{
    m_hObj = 0;
    m_pGimmick = nullptr;
};


bool CGameGimmick::IsAlive(void) const
{
    CGameObject* pObject = CGameObjectManager::GetObject(m_hObj);
    
    if (!pObject)
        return false;

    ASSERT(pObject == m_pGimmick);
    
    return true;
};


CGimmick* CGameGimmick::GetGimmick(void)
{
    return m_pGimmick;
};


void CGameGimmick::GetPosition(RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    if (IsAlive())
    {
        Gimmick().GetPosition(&m_vPosition);
        *pvPosition = m_vPosition;        
    }
    else
    {
        *pvPosition = Math::VECTOR3_ZERO;
    };
};


void CGameGimmick::Sleep(void)
{
    if (IsAlive())
    {
        CGameObjectManager::SendMessage(m_pGimmick, GAMEOBJECTTYPES::MESSAGEID_SLEEP);
    };
};


bool CGameGimmick::IsSleep(void) const
{
    if (IsAlive())
    {
        return Gimmick().IsSleep();
    }
    else
    {
        return true;
    };
};


bool CGameGimmick::IsAbleToSleep(void) const
{
    return IS_FLAG_SET(Gimmick().GetFeatures(), GAMEOBJECTTYPES::FLAG_SLEEP);
};


void CGameGimmick::Resume(void)
{
    if (IsAlive())
    {
        CGameObjectManager::SendMessage(m_pGimmick, GAMEOBJECTTYPES::MESSAGEID_AWAKE);
    };
};


CGimmick& CGameGimmick::Gimmick(void) const
{
    ASSERT(m_pGimmick);
    return *m_pGimmick;
};