#include "GameObject.hpp"
#include "GameObjectManager.hpp"


/*static*/ CGameObject* CGameObject::m_pRunningObject = nullptr;


CGameObject::CGameObject(const char* pszName, GAMEOBJECTTYPE::VALUE type, const char* pszParentName /*= nullptr*/)
: m_objtype(type)
, m_oflag(GAMEOBJECTTYPES::FLAG_DEFAULT)
, m_hObject(0)
{
    ASSERT(type >= 0);
    ASSERT(type < GAMEOBJECTTYPE::MAX);

    ASSERT(std::strlen(pszName) < COUNT_OF(m_szObjName));
    std::strcpy(m_szObjName, pszName);

    m_hObject = CGameObjectManager::RegistObject(this, m_objtype, pszParentName);
    ASSERT(m_hObject != 0);
};


CGameObject::~CGameObject(void)
{
    CGameObjectManager::RemoveObject(this);
};


void CGameObject::Run(void)
{
    ;
};


void CGameObject::MessageProc(int32 nMessageID, void* pParam)
{
    switch (nMessageID)
    {
    case GAMEOBJECTTYPES::MESSAGEID_SLEEP:
        SetObjectFlag(GAMEOBJECTTYPES::FLAG_SLEEP);
        break;
        
    case GAMEOBJECTTYPES::MESSAGEID_AWAKE:
        ClearObjectFlag(GAMEOBJECTTYPES::FLAG_SLEEP);
        break;

    default:
        break;
    };
};


void CGameObject::Period(void)
{
    if (!TestObjectFlag(GAMEOBJECTTYPES::FLAG_SLEEP))
    {
        m_pRunningObject = this;
        Run();
        m_pRunningObject = nullptr;
    };
};


const char* CGameObject::GetName(void) const
{
    return m_szObjName;
};


GAMEOBJECTTYPE::VALUE CGameObject::GetType(void) const
{
    return m_objtype;
};


uint32 CGameObject::GetHandle(void) const
{
    return m_hObject;
};


void CGameObject::SetObjectFlag(GAMEOBJECTTYPES::FLAG flag)
{
    m_oflag |= flag;
};


void CGameObject::ClearObjectFlag(GAMEOBJECTTYPES::FLAG flag)
{
    m_oflag &= (~flag);
};


bool CGameObject::TestObjectFlag(GAMEOBJECTTYPES::FLAG flag) const
{
    return ((m_oflag & flag) == flag);
};


bool CGameObject::CheckObjectFlag(GAMEOBJECTTYPES::FLAG flag) const
{
    return ((m_oflag & flag) != 0);
};