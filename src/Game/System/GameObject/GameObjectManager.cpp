#include "GameObjectManager.hpp"
#include "GameObject.hpp"


class CGameObjectContainer
{
private:
    static const int32 MAX_OBJ = 512;

    struct TAG_GARBAGE {};
    struct TAG_TYPE {};

    struct GAMEOBJECTWORK
        : public CListNode<GAMEOBJECTWORK>              // common list link
        , public CListNode<GAMEOBJECTWORK, TAG_GARBAGE> // garbage list link
        , public CListNode<GAMEOBJECTWORK, TAG_TYPE>    // type list link
    {
        enum FLAG
        {
            FLAG_GARBAGE = (1 << 0),
        };

        CList<GAMEOBJECTWORK>   childs;
        CGameObject*            object;
        int32                   generation;
        uint32                  flags;
        int32                   counter;
    
        inline GAMEOBJECTWORK()
            : childs(), object(nullptr), generation(-1), flags(0), counter(0) {};
    };

    union OBJECTHANDLE
    {
        uint32 number;
        struct
        {
            uint32 index      : 9;
            uint32 counter    : 15;
            uint32 type       : 8;
        } parts;

        inline OBJECTHANDLE() : number(0) {};
        inline OBJECTHANDLE(uint32 _number) : number(_number) {};
    };

    static_assert(sizeof(OBJECTHANDLE) == sizeof(uint32), "incorrect size");

    class IEnumerateFunctor
    {
    public:
        virtual ~IEnumerateFunctor() {};
        virtual bool operator()(GAMEOBJECTWORK* pWork) = 0;
    };

    class CObjectCollector final : public IEnumerateFunctor
    {
    public:
        inline CObjectCollector() : m_numObj(0), m_numObjMax(0), m_apObjectwork() {};
        inline virtual ~CObjectCollector() {};
        virtual bool operator()(GAMEOBJECTWORK* pWork) override;

        int32 m_numObj;
        int32 m_numObjMax;
        GAMEOBJECTWORK* m_apObjectwork[MAX_OBJ];
    };

    class COutOfGenerationCollector final : public IEnumerateFunctor
    {
    public:
        inline COutOfGenerationCollector() : m_generation(-1), m_numObj(0), m_numObjMax(0), m_apObjectwork() {};
        inline virtual ~COutOfGenerationCollector() {};
        virtual bool operator()(GAMEOBJECTWORK* pWork) override;

        int32 m_generation;
        int32 m_numObj;
        int32 m_numObjMax;
        GAMEOBJECTWORK* m_apObjectwork[MAX_OBJ];
    };

    class CFindFromName final : public IEnumerateFunctor
    {
    public:
        inline CFindFromName() : m_pResult(nullptr), m_pszName(nullptr) {};
        inline virtual ~CFindFromName() {};
        virtual bool operator()(GAMEOBJECTWORK* pWork) override;
        
        GAMEOBJECTWORK* m_pResult;
        const char* m_pszName;
    };

public:
    CGameObjectContainer(void);    
    ~CGameObjectContainer(void);
    void Period(void);
    uint32 RegistObject(CGameObject* pObject, GAMEOBJECTTYPE::VALUE type, const char* pszParent);
    void RemoveObject(CGameObject* pObject);
    void DeleteObject(CGameObject* pObject);
    CGameObject* GetNext(CGameObject* pObject = nullptr);
    CGameObject* GetNext(GAMEOBJECTTYPE::VALUE type, CGameObject* pObject = nullptr);
    CGameObject* GetObject(uint32 hObject);
    CGameObject* GetObject(const char* pszName);
    void PeriodForList(CList<GAMEOBJECTWORK>& rList, CGameObject* pParent = nullptr);
    void GarbageCollection(void);
    GAMEOBJECTWORK* FindObjectWork(uint32 hObject);
    GAMEOBJECTWORK* FindObjectWork(const char* pszName);
    GAMEOBJECTWORK* AllocObjectWork(void);
    void FreeObjectWork(GAMEOBJECTWORK* pWork);
    uint32 CalcHandle(GAMEOBJECTWORK* pWork);
    void InsertObjectWork(CList<GAMEOBJECTWORK>& rList, GAMEOBJECTWORK* pWork);
    bool EnumerateWork(IEnumerateFunctor* pFunctor);
    bool EnumerateWorkFromList(CList<GAMEOBJECTWORK>& rList, IEnumerateFunctor* pFunctor);

    template<class TAG = CListNodeDefaultTag>
    /*inline*/ CGameObject* GetNextFromList(CList<GAMEOBJECTWORK, TAG>& list, CGameObject* pObject = nullptr);

private:
    GAMEOBJECTWORK                      m_aWorkPool[MAX_OBJ];
    CList<GAMEOBJECTWORK>               m_listWorkAlloc;
    CList<GAMEOBJECTWORK>               m_listWorkFree;
    CList<GAMEOBJECTWORK, TAG_GARBAGE>  m_listWorkGarbage;
    CList<GAMEOBJECTWORK, TAG_TYPE>     m_aListWorkType[GAMEOBJECTTYPE::MAX];
    int32                               m_nNumActiveObj;
    int32                               m_iGeneration;
    CGameObject*                        m_pCurrentParent;
};


//
// *********************************************************************************
//


bool CGameObjectContainer::CObjectCollector::operator()(GAMEOBJECTWORK* pWork)
{
    ASSERT(m_numObj < m_numObjMax);
    m_apObjectwork[m_numObj++] = pWork;

    return true;
};


//
// *********************************************************************************
//


bool CGameObjectContainer::COutOfGenerationCollector::operator()(GAMEOBJECTWORK* pWork)
{
    if (pWork->generation > m_generation)
    {
        ASSERT(m_numObj < m_numObjMax);
        m_apObjectwork[m_numObj++] = pWork;
    };

    return true;
};


//
// *********************************************************************************
//


bool CGameObjectContainer::CFindFromName::operator()(GAMEOBJECTWORK* pWork)
{
    ASSERT(m_pszName);
    ASSERT(pWork->object);

    if (!std::strcmp(pWork->object->GetName(), m_pszName))
    {
        m_pResult = pWork;
        return false;
    };

    return true;
};


//
// *********************************************************************************
//


CGameObjectContainer::CGameObjectContainer(void)
: m_aWorkPool()
, m_listWorkAlloc()
, m_listWorkFree()
, m_listWorkGarbage()
, m_aListWorkType()
, m_nNumActiveObj(0)
, m_iGeneration(0)
, m_pCurrentParent(nullptr)
{
    for (int32 i = 0; i < COUNT_OF(m_aWorkPool); ++i)
    {
        GAMEOBJECTWORK* pWork = &m_aWorkPool[i];

        pWork->childs.clear();
        pWork->object       = nullptr;
        pWork->generation   = -1;
        pWork->flags        = 0;
        pWork->counter      = 0;

        m_listWorkFree.push_back(pWork);
    };
};


CGameObjectContainer::~CGameObjectContainer(void)
{
    CObjectCollector functor;
    functor.m_numObj = 0;
    functor.m_numObjMax = m_nNumActiveObj;

    EnumerateWork(&functor);
    
    for (int32 i = 0; i < functor.m_numObj; ++i)
    {
        ASSERT(functor.m_apObjectwork[i]);
        if (functor.m_apObjectwork[i]->object)
            delete functor.m_apObjectwork[i]->object;
    };
};


void CGameObjectContainer::Period(void)
{
    PeriodForList(m_listWorkAlloc);
    m_pCurrentParent = nullptr;
};


uint32 CGameObjectContainer::RegistObject(CGameObject* pObject, GAMEOBJECTTYPE::VALUE type, const char* pszParent)
{
    GAMEOBJECTWORK* pWork = AllocObjectWork();
    ASSERT(pWork);

    if (!pWork)
        return 0;

    CList<GAMEOBJECTWORK>* pList = &m_listWorkAlloc;

    if (pszParent)
    {
        GAMEOBJECTWORK* pWorkParent = FindObjectWork(pszParent);
        if (pWorkParent)
            pList = &pWorkParent->childs;
    };

    pWork->object        = pObject;
    pWork->generation    = m_iGeneration;
    pWork->flags         = 0;
    ++pWork->counter;

    InsertObjectWork(*pList, pWork);

    /* regist in type list */
    ASSERT(type >= 0);
    ASSERT(type < COUNT_OF(m_aListWorkType));
    m_aListWorkType[type].push_back(pWork);

    return CalcHandle(pWork);
};


void CGameObjectContainer::RemoveObject(CGameObject* pObject)
{
    uint32 hObject = pObject->GetHandle();

    GAMEOBJECTWORK* pWork = FindObjectWork(hObject);
    if (!pWork)
        return;

    /* remove from type list */
    GAMEOBJECTTYPE::VALUE type = pObject->GetType();
    ASSERT(type >= 0);
    ASSERT(type < COUNT_OF(m_aListWorkType));
    
    m_aListWorkType[type].erase(pWork);

    /* move all nodes to common work list if exists */
    auto it = pWork->childs.begin();
    auto itEnd = pWork->childs.end();
    while (it != itEnd)
    {
        GAMEOBJECTWORK* pChildWork = &(*it);
        it = pWork->childs.erase(it);

        m_listWorkAlloc.push_back(pChildWork);
    };

    ASSERT(!m_listWorkAlloc.empty());
    m_listWorkAlloc.erase(pWork);

    pWork->childs.clear();
    pWork->object     = nullptr;
    pWork->generation = -1;
    pWork->flags      = 0;

    FreeObjectWork(pWork);
};


void CGameObjectContainer::DeleteObject(CGameObject* pObject)
{
    uint32 hObject = pObject->GetHandle();

    GAMEOBJECTWORK* pWork = FindObjectWork(hObject);
    if (!pWork)
        return;
    
    ASSERT(!(pWork->flags & GAMEOBJECTWORK::FLAG_GARBAGE), "garbage list already contains this work");

    pWork->flags |= GAMEOBJECTWORK::FLAG_GARBAGE;
    m_listWorkGarbage.push_back(pWork);
};


CGameObject* CGameObjectContainer::GetNext(CGameObject* pObject /*= nullptr*/)
{
    return GetNextFromList(m_listWorkAlloc, pObject);
};


CGameObject* CGameObjectContainer::GetNext(GAMEOBJECTTYPE::VALUE type, CGameObject* pObject /*= nullptr*/)
{
    ASSERT(type >= 0);
    ASSERT(type < COUNT_OF(m_aListWorkType));

    return GetNextFromList(m_aListWorkType[type], pObject);
};


CGameObject* CGameObjectContainer::GetObject(uint32 hObject)
{
    GAMEOBJECTWORK* pWork = FindObjectWork(hObject);    
    if (pWork)
        return pWork->object;

    return nullptr;
};


CGameObject* CGameObjectContainer::GetObject(const char* pszName)
{
    GAMEOBJECTWORK* pWork = FindObjectWork(pszName);
    if (pWork)
        return pWork->object;
    
    return nullptr;
};


void CGameObjectContainer::PeriodForList(CList<GAMEOBJECTWORK>& rList, CGameObject* pParent /*= nullptr*/)
{
    for (GAMEOBJECTWORK& it : rList)
    {
        m_pCurrentParent = pParent;

        if(!(it.flags & GAMEOBJECTWORK::FLAG_GARBAGE))
            it.object->Period();

        if (!it.childs.empty())
            PeriodForList(it.childs, it.object);
    };
};


void CGameObjectContainer::GarbageCollection(void)
{
    auto it = m_listWorkGarbage.begin();
    auto itEnd = m_listWorkGarbage.end();
    while (it != itEnd)
    {
        GAMEOBJECTWORK* pWork = &(*it);
        it = m_listWorkGarbage.erase(it);

        delete pWork->object;
    };
};


CGameObjectContainer::GAMEOBJECTWORK* CGameObjectContainer::FindObjectWork(uint32 hObject)
{
    OBJECTHANDLE handle(hObject);

    int32 index = static_cast<int32>(handle.parts.index);
    
    ASSERT(index >= 0);
    ASSERT(index < COUNT_OF(m_aWorkPool));

    GAMEOBJECTWORK* pWork = &m_aWorkPool[index];

    if (pWork->counter != static_cast<int32>(handle.parts.counter))
        return nullptr;
    
	if (!pWork->object)
		return nullptr;

    ASSERT(pWork->object->GetHandle() == hObject);
	ASSERT(pWork->object->GetType() == static_cast<GAMEOBJECTTYPE::VALUE>(handle.parts.type));

    return pWork;
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
    if (m_listWorkFree.empty())
        return nullptr;

    GAMEOBJECTWORK* pNode = m_listWorkFree.front();
    m_listWorkFree.erase(pNode);

    ++m_nNumActiveObj;
    
    return pNode;
};


void CGameObjectContainer::FreeObjectWork(GAMEOBJECTWORK* pWork)
{
    --m_nNumActiveObj;

    using CListNodeCommon = CListNode<GAMEOBJECTWORK>;
    ASSERT(static_cast<CListNodeCommon*>(pWork)->is_linked() == false, "unlink node from common list before free");
    
    using CListNodeGarbage = CListNode<GAMEOBJECTWORK, TAG_GARBAGE>;
    ASSERT(static_cast<CListNodeGarbage*>(pWork)->is_linked() == false, "unlink node from garbage list before free");

    using CListNodeType = CListNode<GAMEOBJECTWORK, TAG_TYPE>;
    ASSERT(static_cast<CListNodeType*>(pWork)->is_linked() == false, "unlink node from type list before free");

    m_listWorkFree.push_front(pWork);
};


uint32 CGameObjectContainer::CalcHandle(GAMEOBJECTWORK* pWork)
{
    OBJECTHANDLE handle;
    
    handle.parts.type     = static_cast<uint32>(pWork->object->GetType());
    handle.parts.index    = static_cast<uint32>(pWork - &m_aWorkPool[0]);
    handle.parts.counter  = static_cast<uint32>(pWork->counter);

    return handle.number;
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
        auto itEnd = rList.end();
        while (it != itEnd)
        {
            if (pWork->object->GetType() < (*it).object->GetType())
                break;

            ++it;
        };

        if (it)
            rList.insert(it, pWork);
        else
            rList.push_back(pWork);
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

        if (!it.childs.empty())
            bResult = EnumerateWorkFromList(it.childs, pFunctor);

        if (!bResult)
            break;
    };

    return bResult;
};


template<class TAG /*= CListNodeDefaultTag*/>
inline CGameObject* CGameObjectContainer::GetNextFromList(CList<GAMEOBJECTWORK, TAG>& list,
                                                          CGameObject* pObject /*= nullptr*/)
{
    if (!pObject)
    {
        if (list.empty())
            return nullptr;
        else
            return list.front()->object;
    };

    uint32 hObject = pObject->GetHandle();

    GAMEOBJECTWORK* pWork = FindObjectWork(hObject);
    ASSERT(pWork);

    auto it = ++CList<GAMEOBJECTWORK, TAG>::iterator(&list, pWork);
    auto itEnd = list.end();
    if (it != itEnd)
        return (*it).object;

    return nullptr;
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
        s_pGameObjectContainer = new CGameObjectContainer;
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


/*static*/ void CGameObjectManager::SendMessage(CGameObject* pObject, int32 nMessageID, void* pParam /*= nullptr*/)
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


/*static*/ CGameObject* CGameObjectManager::GetNext(CGameObject* pObject /*= nullptr*/)
{
    return GameObjectContainer().GetNext(pObject);
};


/*static*/ CGameObject* CGameObjectManager::GetNext(GAMEOBJECTTYPE::VALUE type, CGameObject* pObject /*= nullptr*/)
{
    return GameObjectContainer().GetNext(type, pObject);
};


/*static*/ CGameObject* CGameObjectManager::GetObject(uint32 hObject)
{
    return GameObjectContainer().GetObject(hObject);
};


/*static*/ CGameObject* CGameObjectManager::GetObject(const char* pszName)
{
    return GameObjectContainer().GetObject(pszName);
};

