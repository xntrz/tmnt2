#include "CharacterManager.hpp"
#include "Character.hpp"


class CCharacterContainer final
{
private:
    struct CHARACTERWORK : public CListNode<CHARACTERWORK>
    {
        CCharacter* m_pCharacter;
    };

    class IEnumerateFunctor
    {
    public:
        virtual bool operator()(CHARACTERWORK* pWork) = 0;
    };

    class CSearchPtrFunctor : public IEnumerateFunctor
    {
    public:
        virtual bool operator()(CHARACTERWORK* pWork) override;

        CCharacter* m_pTarget;
        CHARACTERWORK* m_pResult;
    };
    
    class CDrawFunctor : public IEnumerateFunctor
    {
    public:
        virtual bool operator()(CHARACTERWORK* pWork) override;
    };
    
public:
    CCharacterContainer(void);
    ~CCharacterContainer(void);
    void Regist(CCharacter* pCharacter);
    void Remove(CCharacter* pCharacter);
    void Draw(void);
    CHARACTERWORK* SearchWork(CCharacter* pCharacter);
    CHARACTERWORK* AllocWork(void);
    void FreeWork(CHARACTERWORK* pWork);
    void EnumerateWork(IEnumerateFunctor* pFunctor);

private:
    CHARACTERWORK m_aWorkPool[32];
    CList<CHARACTERWORK> m_listPool;
    CList<CHARACTERWORK> m_listAlloc;
    int32 m_nNumWorkActive;
};


//
// *********************************************************************************
//


bool CCharacterContainer::CSearchPtrFunctor::operator()(CHARACTERWORK* pWork)
{
    ASSERT(m_pTarget);
    ASSERT(pWork->m_pCharacter);
    
	if (pWork->m_pCharacter == m_pTarget)
	{
		m_pResult = pWork;
        return false;
	};

    return true;
};


//
// *********************************************************************************
//


bool CCharacterContainer::CDrawFunctor::operator()(CHARACTERWORK* pWork)
{
    ASSERT(pWork->m_pCharacter);

    pWork->m_pCharacter->Draw();
    
    return true;
};


//
// *********************************************************************************
//


CCharacterContainer::CCharacterContainer(void)
: m_nNumWorkActive(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aWorkPool); ++i)
    {
        m_aWorkPool[i].m_pCharacter = nullptr;
        m_listPool.push_back(&m_aWorkPool[i]);
    };
};


CCharacterContainer::~CCharacterContainer(void)
{
    ;
};


void CCharacterContainer::Regist(CCharacter* pCharacter)
{
    CHARACTERWORK* pWork = AllocWork();
    ASSERT(pWork);

    if (pWork)
        pWork->m_pCharacter = pCharacter;
};


void CCharacterContainer::Remove(CCharacter* pCharacter)
{
    CHARACTERWORK* pWork = SearchWork(pCharacter);
    ASSERT(pWork);

    if (pWork)
    {
        pWork->m_pCharacter = nullptr;
        FreeWork(pWork);
    };
};


void CCharacterContainer::Draw(void)
{
    CDrawFunctor functor;
    
    EnumerateWork(&functor);
};


CCharacterContainer::CHARACTERWORK* CCharacterContainer::SearchWork(CCharacter* pCharacter)
{
    CSearchPtrFunctor functor;
    functor.m_pTarget = pCharacter;
    functor.m_pResult = nullptr;

    EnumerateWork(&functor);

    return functor.m_pResult;    
};


CCharacterContainer::CHARACTERWORK* CCharacterContainer::AllocWork(void)
{
    if (m_listPool.empty())
        return nullptr;
    
    CHARACTERWORK* pNode = m_listPool.front();
    m_listPool.erase(pNode);
    m_listAlloc.push_back(pNode);

    return pNode;
};


void CCharacterContainer::FreeWork(CHARACTERWORK* pWork)
{
    ASSERT(pWork);
    ASSERT(!m_listAlloc.empty());
    
    m_listAlloc.erase(pWork);
    m_listPool.push_front(pWork);
};


void CCharacterContainer::EnumerateWork(IEnumerateFunctor* pFunctor)
{
    ASSERT(pFunctor);
    
    for (CHARACTERWORK& it : m_listAlloc)
    {
        if (!(*pFunctor)(&it))
            break;
    };
};


static CCharacterContainer* s_pCharacterContainer = nullptr;


static inline CCharacterContainer& CharacterContainer(void)
{
    ASSERT(s_pCharacterContainer);
    return *s_pCharacterContainer;
};


/*static*/ void CCharacterManager::Initialize(void)
{
    if (!s_pCharacterContainer)
        s_pCharacterContainer = new CCharacterContainer;
};


/*static*/ void CCharacterManager::Terminate(void)
{
    if (s_pCharacterContainer)
    {
        delete s_pCharacterContainer;
        s_pCharacterContainer = nullptr;
    };
};


/*static*/ void CCharacterManager::Regist(CCharacter* pCharacter)
{
    CharacterContainer().Regist(pCharacter);
};


/*static*/ void CCharacterManager::Remove(CCharacter* pCharacter)
{
    CharacterContainer().Remove(pCharacter);
};


/*static*/ void CCharacterManager::Draw(void)
{
    CharacterContainer().Draw();
};