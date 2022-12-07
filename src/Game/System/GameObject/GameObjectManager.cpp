#include "GameObjectManager.hpp"
#include "GameObject.hpp"


class CGameObjectContainer
{
private:
    static const int32 MAX_OBJ = 512;

    struct GAMEOBJECTWORK : public CListNode<GAMEOBJECTWORK>
    {
        enum FLAG
        {
            FLAG_NONE = 0x0,
            FLAG_DELETE_REQUEST,

            FLAG_DEFAULT = FLAG_NONE,
        };

        CList<GAMEOBJECTWORK> m_listChild;
        CGameObject* m_pObject;
        int32 m_iGeneration;
        uint32 m_flag;
        int32 m_iUseCounter;
    };

    struct OBJECTBOX
    {
        CGameObject* m_apObject[MAX_OBJ];
    };

    union OBJECTHANDLE
    {
        uint32 m_number;
        struct
        {
            uint32 m_index      : 9;
            uint32 m_counter    : 15;
            uint32 m_type       : 8;
        } m_parts;
    };

    CHECK_SIZE(OBJECTHANDLE, 4);

    class IEnumerateFunctor
    {
    public:
        virtual bool operator()(GAMEOBJECTWORK* pWork) = 0;
    };

    class CObjectCollector : public IEnumerateFunctor
    {
    public:
        virtual bool operator()(GAMEOBJECTWORK* pWork) override;

        int32 m_nNumObj;
        int32 m_nNumMaxObj;
        GAMEOBJECTWORK* m_apObjectwork[MAX_OBJ];
    };

    class CGarbageCollector : public IEnumerateFunctor
    {
    public:
        virtual bool operator()(GAMEOBJECTWORK* pWork) override;

        int32 m_iGeneration;
        int32 m_nNumObj;
        int32 m_nNumMaxObj;
        GAMEOBJECTWORK* m_apObjectwork[MAX_OBJ];
    };

    class CFindFromName : public IEnumerateFunctor
    {
    public:
        virtual bool operator()(GAMEOBJECTWORK* pWork) override;
        
        GAMEOBJECTWORK* m_pResult;
        const char* m_pszName;
    };

    class CFindFromPtr : public IEnumerateFunctor
    {
    public:
        virtual bool operator()(GAMEOBJECTWORK* pWork) override;
        
        GAMEOBJECTWORK* m_pResult;
        CGameObject* m_pObject;
    };

public:
    CGameObjectContainer(void);    
    ~CGameObjectContainer(void);
    void Period(void);
    uint32 RegistObject(CGameObject* pObject, GAMEOBJECTTYPE::VALUE type, const char* pszParent);
    void RemoveObject(CGameObject* pObject);
    void DeleteObject(CGameObject* pObject);
    CGameObject* GetNext(CGameObject* pObject);
    CGameObject* GetObject(uint32 hObject);
    CGameObject* GetObject(const char* pszName);
    void PeriodForList(CList<GAMEOBJECTWORK>& rList, CGameObject* pParent = nullptr);
    void GarbageCollection(void);
    GAMEOBJECTWORK* FindObjectWork(uint32 hObject);
    GAMEOBJECTWORK* FindObjectWork(CGameObject* pObject);
    GAMEOBJECTWORK* FindObjectWork(const char* pszName);
    GAMEOBJECTWORK* AllocObjectWork(void);
    void FreeObjectWork(GAMEOBJECTWORK* pWork);
    uint32 CalcHandle(GAMEOBJECTWORK* pWork);
    void InsertObjectWork(CList<GAMEOBJECTWORK>& rList, GAMEOBJECTWORK* pWork);
    bool EnumerateWork(IEnumerateFunctor* pFunctor);
    bool EnumerateWorkFromList(CList<GAMEOBJECTWORK>& rList, IEnumerateFunctor* pFunctor);
    
private:
    GAMEOBJECTWORK m_aWorkPool[MAX_OBJ];
    CList<GAMEOBJECTWORK> m_listWorkAlloc;
    CList<GAMEOBJECTWORK> m_listWorkFree;
    int32 m_nNumActiveObj;
    int32 m_iGeneration;
    CGameObject* m_pCurrentParent;
};


bool CGameObjectContainer::CObjectCollector::operator()(GAMEOBJECTWORK* pWork)
{
    ASSERT(m_nNumObj < m_nNumMaxObj);
    
    m_apObjectwork[m_nNumObj++] = pWork;

    return true;
};


bool CGameObjectContainer::CGarbageCollector::operator()(GAMEOBJECTWORK* pWork)
{
    if (FLAG_TEST(pWork->m_flag, GAMEOBJECTWORK::FLAG_DELETE_REQUEST) ||
        pWork->m_iGeneration > m_iGeneration)
    {
        ASSERT(m_nNumObj < m_nNumMaxObj);

        m_apObjectwork[m_nNumObj++] = pWork;
    };

    return true;
};


bool CGameObjectContainer::CFindFromName::operator()(GAMEOBJECTWORK* pWork)
{
    ASSERT(m_pszName);
    ASSERT(pWork->m_pObject);

    if (!std::strcmp(pWork->m_pObject->GetName(), m_pszName))
    {
        m_pResult = pWork;
        return false;
    };

    return true;
};


bool CGameObjectContainer::CFindFromPtr::operator()(GAMEOBJECTWORK* pWork)
{
    ASSERT(m_pObject);
    ASSERT(pWork->m_pObject);

    if(pWork->m_pObject == m_pObject)
    {
        ASSERT(pWork->m_pObject->GetHandle() == m_pObject->GetHandle());
        
        m_pResult = pWork;
        return false;
    };

    return true;
};


CGameObjectContainer::CGameObjectContainer(void)
: m_nNumActiveObj(0)
, m_iGeneration(0)
, m_pCurrentParent(nullptr)
{
    for (int32 i = 0; i < COUNT_OF(m_aWorkPool); ++i)
    {
        GAMEOBJECTWORK* pWork = &m_aWorkPool[i];

        pWork->m_listChild.clear();
        pWork->m_pObject        = nullptr;
        pWork->m_iGeneration    = -1;
        pWork->m_flag           = GAMEOBJECTWORK::FLAG_DEFAULT;
        pWork->m_iUseCounter    = 0;

        m_listWorkFree.push_back(pWork);
    };
};


CGameObjectContainer::~CGameObjectContainer(void)
{
    CObjectCollector functor;
    functor.m_nNumObj = 0;
    functor.m_nNumMaxObj = m_nNumActiveObj;

    EnumerateWork(&functor);
    
    for (int32 i = 0; i < functor.m_nNumObj; ++i)
    {
        ASSERT(functor.m_apObjectwork[i]);
        if (functor.m_apObjectwork[i]->m_pObject)
            delete functor.m_apObjectwork[i]->m_pObject;
    };
};


void CGameObjectContainer::Period(void)
{
    PeriodForList(m_listWorkAlloc);
    m_pCurrentParent = nullptr;
};


uint32 CGameObjectContainer::RegistObject(CGameObject* pObject, GAMEOBJECTTYPE::VALUE type, const char* pszParent)
{
    CList<GAMEOBJECTWORK>& rList = m_listWorkAlloc;

    if (pszParent)
    {
        GAMEOBJECTWORK* pWorkParent = FindObjectWork(pszParent);
        if (pWorkParent)
        {
            rList = pWorkParent->m_listChild;
        };
    };

    GAMEOBJECTWORK* pWork = AllocObjectWork();
    ASSERT(pWork);

    pWork->m_pObject        = pObject;
    pWork->m_iGeneration    = m_iGeneration;
    pWork->m_flag           = GAMEOBJECTWORK::FLAG_DEFAULT;
    ++pWork->m_iUseCounter;

    InsertObjectWork(rList, pWork);
    
    return CalcHandle(pWork);
};


void CGameObjectContainer::RemoveObject(CGameObject* pObject)
{
    GAMEOBJECTWORK* pWork = FindObjectWork(pObject);

    if (pWork)
    {
        if (!pWork->m_listChild.empty())
        {
            //
            //  Move all nodes to commond work list if exists
            //
            auto it = pWork->m_listChild.begin();
            while (it)
            {
                GAMEOBJECTWORK* pChildWork = &(*it);
                it = pWork->m_listChild.erase(it);

                m_listWorkAlloc.push_back(pChildWork);
            };
        };

		ASSERT(!m_listWorkAlloc.empty());
        m_listWorkAlloc.erase(pWork);

        pWork->m_listChild.clear();
        pWork->m_pObject        = nullptr;
        pWork->m_iGeneration    = -1;
        pWork->m_flag           = GAMEOBJECTWORK::FLAG_DEFAULT;
        pWork->m_iUseCounter    = 0;

        FreeObjectWork(pWork);
    };
};


void CGameObjectContainer::DeleteObject(CGameObject* pObject)
{
    GAMEOBJECTWORK* pWork = FindObjectWork(pObject);

    if (pWork)
    {
        FLAG_SET(pWork->m_flag, GAMEOBJECTWORK::FLAG_DELETE_REQUEST);
    };
};


CGameObject* CGameObjectContainer::GetNext(CGameObject* pObject)
{
    if (!pObject)
    {
        if (m_listWorkAlloc.empty())
            return nullptr;
        else
            return m_listWorkAlloc.front()->m_pObject;
    };

    auto it = m_listWorkAlloc.begin();
    while (it)
    {
        if ((*it).m_pObject == pObject)
        {
            //
            //  Return iterator to an next object
            //
            ++it;
            break;
        };

        ++it;
    };

    //
    //  If not end of list
    //
    if (it)
    {
        return (*it).m_pObject;
    };

    return nullptr;
};


CGameObject* CGameObjectContainer::GetObject(uint32 hObject)
{
    GAMEOBJECTWORK* pWork = FindObjectWork(hObject);
    
    if (pWork)
    {
        return pWork->m_pObject;
    }
    else
    {
        return nullptr;
    };
};


CGameObject* CGameObjectContainer::GetObject(const char* pszName)
{
    GAMEOBJECTWORK* pWork = FindObjectWork(pszName);
    
    if (pWork)
    {
        return pWork->m_pObject;
    }
    else
    {
        return nullptr;
    };
};


void CGameObjectContainer::PeriodForList(CList<GAMEOBJECTWORK>& rList, CGameObject* pParent)
{
    for (GAMEOBJECTWORK& it : rList)
    {
        m_pCurrentParent = pParent;

		//RwV3d* p = (RwV3d*)0x1138CC0C;
		//OUTPUT("%s -> %f %f %f\n", it.m_pObject->GetName(), p->x, p->y, p->z);
		//if (it.m_pObject->GetType() == GAMEOBJECTTYPE::CHARACTER)
			//OUTPUT("WTF\n");

        if (!FLAG_TEST(it.m_flag, GAMEOBJECTWORK::FLAG_DELETE_REQUEST))
            it.m_pObject->Period();

        if (!it.m_listChild.empty())
            PeriodForList(it.m_listChild, it.m_pObject);
    };
};


void CGameObjectContainer::GarbageCollection(void)
{
    CGarbageCollector functor;    
    functor.m_iGeneration   = m_iGeneration;
    functor.m_nNumObj       = 0;
    functor.m_nNumMaxObj    = m_nNumActiveObj;
    
    EnumerateWork(&functor);

    ASSERT(functor.m_nNumObj <= m_nNumActiveObj);

    for (int32 i = 0; i < functor.m_nNumObj; ++i)
    {
        ASSERT(functor.m_apObjectwork[i]);
        ASSERT(functor.m_apObjectwork[i]->m_pObject);

        if (functor.m_apObjectwork[i]->m_pObject)
            delete functor.m_apObjectwork[i]->m_pObject;
    };
};


CGameObjectContainer::GAMEOBJECTWORK* CGameObjectContainer::FindObjectWork(uint32 hObject)
{
    OBJECTHANDLE ObjectHandle = { hObject };

    GAMEOBJECTWORK* pWork = &m_aWorkPool[ObjectHandle.m_parts.m_index];
    if (pWork->m_iUseCounter != ObjectHandle.m_parts.m_counter)
        return nullptr;
    
	if (!pWork->m_pObject)
		return nullptr;

    ASSERT(pWork->m_pObject->GetHandle() == hObject);
    ASSERT(pWork->m_pObject->GetType() == ObjectHandle.m_parts.m_type);

    return pWork;
};


CGameObjectContainer::GAMEOBJECTWORK* CGameObjectContainer::FindObjectWork(CGameObject* pObject)
{
    ASSERT(pObject);
    
    CFindFromPtr functor;
    functor.m_pObject = pObject;
    functor.m_pResult = nullptr;

    EnumerateWork(&functor);

    return functor.m_pResult;
};


CGameObjectContainer::GAMEOBJECTWORK* CGameObjectContainer::FindObjectWork(const char* pszName)
{
    ASSERT(pszName);
    
    CFindFromName functor;
    functor.m_pszName = pszName;
    functor.m_pResult = nullptr;

    EnumerateWork(&functor);

    return functor.m_pResult;
};


CGameObjectContainer::GAMEOBJECTWORK* CGameObjectContainer::AllocObjectWork(void)
{
    ASSERT(!m_listWorkFree.empty());

    GAMEOBJECTWORK* pNode = m_listWorkFree.front();
    m_listWorkFree.erase(pNode);

    ++m_nNumActiveObj;
    
    return pNode;
};


void CGameObjectContainer::FreeObjectWork(GAMEOBJECTWORK* pWork)
{
    --m_nNumActiveObj;
    
    m_listWorkFree.push_back(pWork);
};


uint32 CGameObjectContainer::CalcHandle(GAMEOBJECTWORK* pWork)
{
    OBJECTHANDLE ObjectHandle;
    
    ObjectHandle.m_parts.m_type     = pWork->m_pObject->GetType();
    ObjectHandle.m_parts.m_index    = ((uint8*)pWork - (uint8*)&m_aWorkPool[0]) / sizeof(GAMEOBJECTWORK);
    ObjectHandle.m_parts.m_counter  = pWork->m_iUseCounter;

    return ObjectHandle.m_number;
};


void CGameObjectContainer::InsertObjectWork(CList<GAMEOBJECTWORK>& rList, GAMEOBJECTWORK* pWork)
{
    ASSERT(pWork);
    
    if (rList.empty())
    {
        rList.push_back(pWork);
    }
    else
    {
        auto it = rList.begin();
        while (it)
        {
            if(pWork->m_pObject->GetType() < (*it).m_pObject->GetType())
                break;

            ++it;
        };

        if (it)
        {
            rList.insert(it, pWork);
        }
        else
        {
            rList.push_back(pWork);
        };
    };
};


bool CGameObjectContainer::EnumerateWork(IEnumerateFunctor* pFunctor)
{
    return EnumerateWorkFromList(m_listWorkAlloc, pFunctor);
};


bool CGameObjectContainer::EnumerateWorkFromList(CList<GAMEOBJECTWORK>& rList, IEnumerateFunctor* pFunctor)
{
    ASSERT(pFunctor);

    bool bResult = true;

    for (GAMEOBJECTWORK& it : rList)
    {
        bResult = (*pFunctor)(&it);
        if (!bResult)
            break;

        if (!it.m_listChild.empty())
            bResult = EnumerateWorkFromList(it.m_listChild, pFunctor);

        if (!bResult)
            break;
    };

    return bResult;
};


static CGameObjectContainer* s_pGameObjectContainer = nullptr;


static inline CGameObjectContainer& GameObjectContainer(void)
{
    ASSERT(s_pGameObjectContainer);
    return *s_pGameObjectContainer;
};


/*static*/ void CGameObjectManager::Initialize(void)
{
    if (!s_pGameObjectContainer)
    {
        s_pGameObjectContainer = new CGameObjectContainer;
    };
};


/*static*/ void CGameObjectManager::Terminate(void)
{
    if (s_pGameObjectContainer)
    {
        delete s_pGameObjectContainer;
        s_pGameObjectContainer = nullptr;
    };
};


/*static*/ void CGameObjectManager::Period(void)
{
    GameObjectContainer().Period();
    GameObjectContainer().GarbageCollection();
};


/*static*/ void CGameObjectManager::SendMessage(CGameObject* pObject, int32 nMessageID, void* pParam)
{
    ASSERT(pObject);
    
    pObject->MessageProc(nMessageID, pParam);
};


/*static*/ uint32 CGameObjectManager::RegistObject(CGameObject* pObject, GAMEOBJECTTYPE::VALUE type, const char* pszParent)
{
    return GameObjectContainer().RegistObject(pObject, type, pszParent);
};


/*static*/ void CGameObjectManager::RemoveObject(CGameObject* pObject)
{
    GameObjectContainer().RemoveObject(pObject);
};


/*static*/ void CGameObjectManager::DeleteObject(CGameObject* pObject)
{
    GameObjectContainer().DeleteObject(pObject);
};


/*static*/ CGameObject* CGameObjectManager::GetNext(CGameObject* pObject)
{
    return GameObjectContainer().GetNext(pObject);
};


/*static*/ CGameObject* CGameObjectManager::GetObject(uint32 hObject)
{
    return GameObjectContainer().GetObject(hObject);
};


/*static*/ CGameObject* CGameObjectManager::GetObject(const char* pszName)
{
    return GameObjectContainer().GetObject(pszName);
};

