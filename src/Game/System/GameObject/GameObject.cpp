#include "GameObject.hpp"
#include "GameObjectManager.hpp"


/*static*/ CGameObject* CGameObject::m_pRunningObject = nullptr;


CGameObject::CGameObject(const char* pszName, GAMEOBJECTTYPE::VALUE type, const char* pszParentName)
: m_objtype(type)
, m_objflag(0)
, m_hObject(0)
{
    ASSERT(std::strlen(pszName) < COUNT_OF(m_szObjName));

    std::strcpy(m_szObjName, pszName);
    m_hObject = CGameObjectManager::RegistObject(this, m_objtype, pszParentName);
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
        SetObjectFlag(GAMEOBJECTTYPES::FLAG_SLEEP, true);
        break;
        
    case GAMEOBJECTTYPES::MESSAGEID_AWAKE:
        SetObjectFlag(GAMEOBJECTTYPES::FLAG_SLEEP, false);
        break;
    };
};


void CGameObject::Period(void)
{
    if (!IsObjectFlagSet(GAMEOBJECTTYPES::FLAG_SLEEP))
    {
        m_pRunningObject = this;
        Run();
        m_pRunningObject = nullptr;
    };
};


void CGameObject::SetObjectFlag(GAMEOBJECTTYPES::FLAG flag, bool bSet)
{
    if (bSet)
        FLAG_SET(m_objflag, flag);
    else
        FLAG_CLEAR(m_objflag, flag);
};


bool CGameObject::IsObjectFlagSet(GAMEOBJECTTYPES::FLAG flag) const
{
    return FLAG_TEST(m_objflag, flag);
};


const char* CGameObject::GetName(void) const
{
    return m_szObjName;
};


GAMEOBJECTTYPE::VALUE CGameObject::GetType(void) const
{
    return m_objtype;
};


GAMEOBJECTTYPES::FLAG CGameObject::GetObjectFlag(void) const
{
    return GAMEOBJECTTYPES::FLAG(m_objflag);
};


uint32 CGameObject::GetHandle(void) const
{
    return m_hObject;
};