#include "MotionManager.hpp"
#include "Motion.hpp"


class CHAnimationContainer
{
public:
    class CHAnimation : public CListNode<CHAnimation>
    {
    public:
        CHAnimation(void);
        ~CHAnimation(void);
        void Read(const char* pszName, const void* pBuffer, uint32 uBufferSize);
        void Cleanup(void);
        RtAnimAnimation* ReadRpHAnimatin(const void* pBuffer, uint32 uBufferSize);
        const char* GetName(void) const;
        RtAnimAnimation* GetAnimation(void) const;

    private:
        char m_szName[16];
        RtAnimAnimation* m_pHAnimation;
    };

    class CHAnimationSet : public CListNode<CHAnimationSet>
    {
    public:
        CHAnimationSet(void);
        ~CHAnimationSet(void);
        void Initialize(const char* pszName, CHAnimationSet* pParentSet);
        void Cleanup(CHAnimationContainer* pContainer);
        void AddHAnimation(CHAnimation* pHAnimation);
        CHAnimation* GetHAnimation(const char* pszName);        
        const char* GetName(void) const;
        int32 GetGeneration(void) const;
        CHAnimationSet* GetParent(void) const;
        
    private:
        char m_szName[16];
        CList<CHAnimation> m_listHAnimation;
        CHAnimationSet* m_pParentSet;
        int32 m_iGeneration;
    };

public:
    CHAnimationContainer(void);
    ~CHAnimationContainer(void);
    void AllocHAnimationSet(const char* pszSetName, const char* pszParentSetName);
    void SetCurrentHAnimationSet(const char* pszSetName);
    void SetCurrentHAnimationSet(CHAnimationSet* pHAnimationSet);
    void Read(const char* pszName, const void* pBuffer, uint32 uBufferSize);
    CHAnimation* GetHAnimation(const char* pszName);
    CHAnimationSet* FindHAnmationSet(const char* pszName);
    CHAnimation* AllocHAnimation(void);
    void FreeHAnimation(CHAnimation* pHAnimation);

private:
    CHAnimationSet m_aHAnimationSet[16];
    CHAnimation m_aHAnimation[384];
    CList<CHAnimationSet> m_listFreeSet;
    CList<CHAnimationSet> m_listAllocSet;
    CList<CHAnimation> m_listHAnimationPool;
    CHAnimationSet m_GlobalSet;
    CHAnimationSet* m_pCurrentSet;
    int32 m_iGeneration;
};


CHAnimationContainer::CHAnimation::CHAnimation(void)
: m_pHAnimation(nullptr)
{
    m_szName[0] = '\0';
};


CHAnimationContainer::CHAnimation::~CHAnimation(void)
{
    ASSERT(!m_pHAnimation);
};


void CHAnimationContainer::CHAnimation::Read(const char* pszName, const void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(uBufferSize);
    ASSERT(std::strlen(pszName) < COUNT_OF(m_szName));

    std::strcpy(m_szName, pszName);
    m_pHAnimation = ReadRpHAnimatin(pBuffer, uBufferSize);
    ASSERT(m_pHAnimation);
};


void CHAnimationContainer::CHAnimation::Cleanup(void)
{
    ASSERT(m_pHAnimation);
    
    if (m_pHAnimation)
    {
        RtAnimAnimationDestroy(m_pHAnimation);
        m_pHAnimation = nullptr;
    };

    m_szName[0] = '\0';
};


RtAnimAnimation* CHAnimationContainer::CHAnimation::ReadRpHAnimatin(const void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(uBufferSize);

    RtAnimAnimation* pResult = nullptr;
    RwMemory MemoryStream;
    
    MemoryStream.start = (RwUInt8*)pBuffer;
    MemoryStream.length = uBufferSize;

    RwStream* pRwStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &MemoryStream);
    ASSERT(pRwStream);
    if (pRwStream)
    {
        if (RwStreamFindChunk(pRwStream, rwID_ANIMANIMATION, nullptr, nullptr))
            pResult = RtAnimAnimationStreamRead(pRwStream);

        RwStreamClose(pRwStream, nullptr);
    };

    return pResult;
};


const char* CHAnimationContainer::CHAnimation::GetName(void) const
{
    return m_szName;
};


RtAnimAnimation* CHAnimationContainer::CHAnimation::GetAnimation(void) const
{
    return m_pHAnimation;
};


CHAnimationContainer::CHAnimationSet::CHAnimationSet(void)
: m_pParentSet(nullptr)
, m_iGeneration(-1)
{
    m_szName[0] = '\0';    
};


CHAnimationContainer::CHAnimationSet::~CHAnimationSet(void)
{
    ASSERT(m_listHAnimation.empty());    
};


void CHAnimationContainer::CHAnimationSet::Initialize(const char* pszName, CHAnimationSet* pParentSet)
{
    ASSERT(pszName);
    ASSERT(std::strlen(pszName) < COUNT_OF(m_szName));

    std::strcpy(m_szName, pszName);
    m_pParentSet = pParentSet;
};


void CHAnimationContainer::CHAnimationSet::Cleanup(CHAnimationContainer* pContainer)
{
    auto it = m_listHAnimation.begin();
    while (it)
    {
        CHAnimation* pHAnimation = &(*it);
        it = m_listHAnimation.erase(it);

        pHAnimation->Cleanup();
        pContainer->FreeHAnimation(pHAnimation);
    };
};


void CHAnimationContainer::CHAnimationSet::AddHAnimation(CHAnimation* pHAnimation)
{
    m_listHAnimation.push_back(pHAnimation);
};


CHAnimationContainer::CHAnimation* CHAnimationContainer::CHAnimationSet::GetHAnimation(const char* pszName)
{
    ASSERT(pszName);

    for (CHAnimation& it : m_listHAnimation)
    {
        if (!std::strcmp(it.GetName(), pszName))
            return &it;
    };

    return nullptr;
};


const char* CHAnimationContainer::CHAnimationSet::GetName(void) const
{
    return m_szName;
};


int32 CHAnimationContainer::CHAnimationSet::GetGeneration(void) const
{
    return m_iGeneration;
};


CHAnimationContainer::CHAnimationSet* CHAnimationContainer::CHAnimationSet::GetParent(void) const
{
    return m_pParentSet;
};


CHAnimationContainer::CHAnimationContainer(void)
: m_pCurrentSet(nullptr)
, m_iGeneration(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aHAnimation); ++i)
        m_listHAnimationPool.push_back(&m_aHAnimation[i]);

    for (int32 i = 0; i < COUNT_OF(m_aHAnimationSet); ++i)
        m_listFreeSet.push_back(&m_aHAnimationSet[i]);

    m_GlobalSet.Initialize(CMotionManager::GLOBAL_SET, nullptr);
    m_pCurrentSet = &m_GlobalSet;
};


CHAnimationContainer::~CHAnimationContainer(void)
{
    for (CHAnimationSet& it : m_listAllocSet)
        it.Cleanup(this);

    m_GlobalSet.Cleanup(this);
};


void CHAnimationContainer::AllocHAnimationSet(const char* pszSetName, const char* pszParentSetName)
{
    ASSERT(!m_listFreeSet.empty());

    CHAnimationSet* pHAnimationSet = m_listFreeSet.front();
    m_listFreeSet.erase(pHAnimationSet);
	m_listAllocSet.push_back(pHAnimationSet);

    CHAnimationSet* pHAnimationSetParent = nullptr;
    if (pszParentSetName)
        pHAnimationSetParent = FindHAnmationSet(pszParentSetName);

    if (!pHAnimationSetParent)
        pHAnimationSetParent = &m_GlobalSet;

    pHAnimationSet->Initialize(pszSetName, pHAnimationSetParent);
    
    SetCurrentHAnimationSet(pHAnimationSet);
};


void CHAnimationContainer::SetCurrentHAnimationSet(const char* pszSetName)
{
    if (pszSetName == CMotionManager::GLOBAL_SET)
    {
        SetCurrentHAnimationSet(&m_GlobalSet);
    }
    else
    {
        CHAnimationSet* pHAnimationSet = FindHAnmationSet(pszSetName);
        ASSERT(pHAnimationSet);
        SetCurrentHAnimationSet(pHAnimationSet);
    };
};


void CHAnimationContainer::SetCurrentHAnimationSet(CHAnimationSet* pHAnimationSet)
{
    ASSERT(pHAnimationSet);

    m_pCurrentSet = pHAnimationSet;
};


void CHAnimationContainer::Read(const char* pszName, const void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pszName);
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);
    ASSERT(m_pCurrentSet);

    CHAnimation* pHAnimation = AllocHAnimation();
    ASSERT(pHAnimation);

    pHAnimation->Read(pszName, pBuffer, uBufferSize);
    m_pCurrentSet->AddHAnimation(pHAnimation);
};


CHAnimationContainer::CHAnimation* CHAnimationContainer::GetHAnimation(const char* pszName)
{
    ASSERT(pszName);
    ASSERT(m_pCurrentSet);

    CHAnimation* pHAnimation = nullptr;
    CHAnimationSet* pHAnimationSet = m_pCurrentSet;
    
    while (pHAnimationSet)
    {
        pHAnimation = pHAnimationSet->GetHAnimation(pszName);
        if (pHAnimation)
            break;

        pHAnimationSet = pHAnimationSet->GetParent();
        ASSERT(pHAnimationSet);
    };

    return pHAnimation;
};


CHAnimationContainer::CHAnimationSet* CHAnimationContainer::FindHAnmationSet(const char* pszName)
{
    ASSERT(pszName);

    for (CHAnimationSet& it : m_listAllocSet)
    {
        if (!std::strcmp(it.GetName(), pszName))
            return &it;
    };

    return nullptr;
};


CHAnimationContainer::CHAnimation* CHAnimationContainer::AllocHAnimation(void)
{
    ASSERT(!m_listHAnimationPool.empty());

    CHAnimation* pNode = m_listHAnimationPool.front();
    m_listHAnimationPool.erase(pNode);

    return pNode;
};


void CHAnimationContainer::FreeHAnimation(CHAnimation* pHAnimation)
{
    ASSERT(!pHAnimation->is_linked());
    
    m_listHAnimationPool.push_back(pHAnimation);
};


static CHAnimationContainer* s_pHAnimationContainer = nullptr;


static inline CHAnimationContainer& AnimationContainer(void)
{
    ASSERT(s_pHAnimationContainer);
    return *s_pHAnimationContainer;
};


/*static*/ const char* CMotionManager::GLOBAL_SET = "__global__";


/*static*/ void CMotionManager::Initialize(void)
{
    if (!s_pHAnimationContainer)
    {
        s_pHAnimationContainer = new CHAnimationContainer;
    };
};


/*static*/ void CMotionManager::Terminate(void)
{
    if (s_pHAnimationContainer)
    {
        delete s_pHAnimationContainer;
        s_pHAnimationContainer = nullptr;
    };
};


/*static*/ void CMotionManager::CreateMotionSet(const char* pszSetName, const char* pszParentSetName)
{
    AnimationContainer().AllocHAnimationSet(pszSetName, pszParentSetName);
};


/*static*/ void CMotionManager::SetCurrentMotionSet(const char* pszSetName)
{
    AnimationContainer().SetCurrentHAnimationSet(pszSetName);
};


/*static*/ void CMotionManager::Read(const char* pszName, const void* pBuffer, uint32 uBufferSize)
{
    AnimationContainer().Read(pszName, pBuffer, uBufferSize);
};


/*static*/ CMotion* CMotionManager::CreateMotion(const char* pszName)
{
    CHAnimationContainer::CHAnimation* pHAnimation = AnimationContainer().GetHAnimation(pszName);
    ASSERT(pHAnimation);
    if (pHAnimation)
    {
        CMotion* pMotion = new CMotion(pHAnimation->GetName(), pHAnimation->GetAnimation());
        ASSERT(pMotion);
        return pMotion;
    };

    return nullptr;
};

