#include "GimmickManager.hpp"
#include "GimmickFactory.hpp"
#include "GimmickInfo.hpp"
#include "Gimmick.hpp"
#include "GimmickData.hpp"

#include "ConcreteGimmick/SystemGimmick.hpp"

#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"


class CGimmickEventDispatcher
{
private:
    static const int32 EVENT_PARAM_MAX = 32;
    static const int32 EVENT_SEND_DEPTH_MAX = 6;

    struct EVENT_PARAM
    {
        GIMMICKTYPES::EVENT_ARGS m_args;
        char m_szTargetName[GAMEOBJECTTYPES::NAME_MAX];

        void Init(const char* pszTargetName, const char* pszSenderName, GIMMICKTYPES::EVENTTYPE type, void* pParam);
    };

public:
    CGimmickEventDispatcher(void);
    ~CGimmickEventDispatcher(void);
    void Dispatch(void);
    void PostEvent(const char* pszReceiver, const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype, void* pParam);

private:
    EVENT_PARAM* paramAlloc(void);
    void paramFree(EVENT_PARAM* pEventParam);
    bool sendBegin(void);
    void sendEnd(void);
    void send(EVENT_PARAM* pEventParam);

private:
    EVENT_PARAM m_aEventParam[EVENT_PARAM_MAX];
    int32 m_nEventNum;
    bool m_bDispatching;
    int32 m_nSendDepth;
};


class CGimmickContainer
{
private:
    static const int32 DRAWPRIMAX = CGimmickInfo::DRAWPRIMAX + 1;
    static const int32 WORKMAX = DRAWPRIMAX * 64;

    struct GIMMICKWORK : public CListNode<GIMMICKWORK>
    {
        CGimmick* m_pGimmick;
    };

public:
    CGimmickContainer(void);
    ~CGimmickContainer(void);
    void Create(int32 nNodeNum, int32 nPriNum);
    void Destroy(void);
    void Draw(int32 nPriority);
    void Add(int32 nPriority, CGimmick* pGimmick);
    void Remove(CGimmick* pGimmick);
    CGimmick* Find(const char* pszName);
    void Lock(void);
    void Unlock(void);
    bool IsLocked(void) const;

private:
    GIMMICKWORK* workAlloc(void);
    void workFree(GIMMICKWORK* pWork);
    GIMMICKWORK* workFind(CGimmick* pGimmick, int32 nPriority);

private:
    GIMMICKWORK m_aWork[WORKMAX];
    CList<GIMMICKWORK> m_listWorkPool;
    CList<GIMMICKWORK> m_listDraw[DRAWPRIMAX];
    bool m_bLock;
};


void CGimmickEventDispatcher::EVENT_PARAM::Init(const char* pszTargetName, const char* pszSenderName, GIMMICKTYPES::EVENTTYPE type, void* pParam)
{
    ASSERT(pszTargetName);
    ASSERT(pszSenderName);

    std::strcpy(m_szTargetName, pszTargetName);
    std::strcpy(m_args.m_szSender, pszSenderName);
    m_args.m_type = type;
};


CGimmickEventDispatcher::CGimmickEventDispatcher(void)
: m_nEventNum(0)
, m_bDispatching(false)
, m_nSendDepth(0)
{
    std::memset(m_aEventParam, 0x00, sizeof(m_aEventParam));    
};


CGimmickEventDispatcher::~CGimmickEventDispatcher(void)
{
    ;
};


void CGimmickEventDispatcher::Dispatch(void)
{
    ASSERT(!m_bDispatching);
    m_bDispatching = true;
    m_nSendDepth = 0;

    for (int32 i = 0; i < m_nEventNum; ++i)
    {
        send(&m_aEventParam[i]);
        paramFree(&m_aEventParam[i]);
    };

    m_bDispatching = false;
    m_nEventNum = 0;
};


void CGimmickEventDispatcher::PostEvent(const char* pszReceiver, const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype, void* pParam)
{
    if (m_bDispatching)
    {
        EVENT_PARAM param;
        param.Init(pszReceiver, pszSender, eventtype, pParam);

        send(&param);
    }
    else
    {
        EVENT_PARAM* pEventParam = paramAlloc();
        ASSERT(pEventParam);
        if (pEventParam)
            pEventParam->Init(pszReceiver, pszSender, eventtype, pParam);
    };
};


CGimmickEventDispatcher::EVENT_PARAM* CGimmickEventDispatcher::paramAlloc(void)
{
    ASSERT(m_nEventNum < COUNT_OF(m_aEventParam));

    if (m_nEventNum >= COUNT_OF(m_aEventParam))
        return nullptr;

    return &m_aEventParam[m_nEventNum++];
};


void CGimmickEventDispatcher::paramFree(EVENT_PARAM* pEventParam)
{
    ASSERT(pEventParam);
};


bool CGimmickEventDispatcher::sendBegin(void)
{
    ++m_nSendDepth;
    ASSERT(m_nSendDepth < EVENT_SEND_DEPTH_MAX);
    return (m_nSendDepth < EVENT_SEND_DEPTH_MAX);
};


void CGimmickEventDispatcher::sendEnd(void)
{
    ASSERT(m_nSendDepth > 0);
    --m_nSendDepth;
};


void CGimmickEventDispatcher::send(EVENT_PARAM* pEventParam)
{
    ASSERT(pEventParam);
    if (sendBegin())
    {
        CGimmick* pGimmick = CGimmickManager::Find(pEventParam->m_szTargetName);
        if (pGimmick)
        {
            OUTPUT(
                " Sending event: %s to %s (param: %d depth %d)\n",
                pEventParam->m_args.m_szSender,
                pEventParam->m_szTargetName,
                pEventParam->m_args.m_type,
                m_nSendDepth
            );

            CGameObjectManager::SendMessage(
                pGimmick,
                GIMMICKTYPES::MESSAGEID_RECVEVENT,
                &pEventParam->m_args
            );
        };

        sendEnd();
    };
};


CGimmickContainer::CGimmickContainer(void)
: m_bLock(false)
{
    for (int32 i = 0; i < COUNT_OF(m_aWork); ++i)
    {
        m_aWork[i].m_pGimmick = nullptr;
        m_listWorkPool.push_back(&m_aWork[i]);
    };        
};


CGimmickContainer::~CGimmickContainer(void)
{
    Destroy();
};


void CGimmickContainer::Create(int32 nNodeNum, int32 nPriNum)
{
    ;
};


void CGimmickContainer::Destroy(void)
{
    ;
};


void CGimmickContainer::Draw(int32 nPriority)
{
    ASSERT(nPriority >= 0 && nPriority < COUNT_OF(m_listDraw));
    
    Lock();

    CRenderStateManager::SetDefault();

    CList<GIMMICKWORK>& rDrawList = m_listDraw[nPriority];
    for (GIMMICKWORK& it : rDrawList)
    {
        CGimmick* pGimmick = it.m_pGimmick;
        ASSERT(pGimmick);
                
        if (!pGimmick->IsSleep())
            pGimmick->Draw();
    };

    Unlock();
};


void CGimmickContainer::Add(int32 nPriority, CGimmick* pGimmick)
{
    ASSERT(pGimmick);
    ASSERT(nPriority >= 0 && nPriority < COUNT_OF(m_listDraw));
    ASSERT(!IsLocked());

    GIMMICKWORK* pWork = workAlloc();
    ASSERT(pWork);
    if(pWork)
    {
        pWork->m_pGimmick = pGimmick;

        m_listDraw[nPriority].push_back(pWork);
    };
};


void CGimmickContainer::Remove(CGimmick* pGimmick)
{
    ASSERT(pGimmick);
    ASSERT(!IsLocked());

    int32 nPriority = CGimmickInfo::GetDrawPriority(pGimmick->GetID());
    ASSERT(nPriority >= 0 && nPriority < COUNT_OF(m_listDraw));

    GIMMICKWORK* pWork = workFind(pGimmick, nPriority);
    ASSERT(pWork);
    if (pWork)
    {
        m_listDraw[nPriority].erase(pWork);

        workFree(pWork);
    };
};


CGimmick* CGimmickContainer::Find(const char* pszName)
{
    CGameObject* pGameObject = CGameObjectManager::GetObject(pszName);
    if (pGameObject && pGameObject->GetType() == GAMEOBJECTTYPE::GIMMICK)
        return (CGimmick*)pGameObject;
    else
        return nullptr;
};


void CGimmickContainer::Lock(void)
{
    m_bLock = true;
};


void CGimmickContainer::Unlock(void)
{
    m_bLock = false;
};


bool CGimmickContainer::IsLocked(void) const
{
    return m_bLock;
};


CGimmickContainer::GIMMICKWORK* CGimmickContainer::workAlloc(void)
{
    ASSERT(!m_listWorkPool.empty());

    GIMMICKWORK* pNode = m_listWorkPool.front();
    ASSERT(pNode);
    m_listWorkPool.erase(pNode);

    return pNode;
};


void CGimmickContainer::workFree(GIMMICKWORK* pWork)
{
    ASSERT(pWork);
    
    m_listWorkPool.push_back(pWork);
};


CGimmickContainer::GIMMICKWORK* CGimmickContainer::workFind(CGimmick* pGimmick, int32 nPriority)
{
    CList<GIMMICKWORK>& rList = m_listDraw[nPriority];

    for (GIMMICKWORK& it : rList)
    {
        ASSERT(it.m_pGimmick);
        
        if (it.m_pGimmick == pGimmick)
            return &it;
    };

    return nullptr;
};


static CGimmickContainer* s_pGimmickContainer = nullptr;
static CGimmickEventDispatcher* s_pGimmickEventDispatcher = nullptr;
static bool s_bPlayStarted = false;


static inline CGimmickContainer& GimmickContainer(void)
{
    ASSERT(s_pGimmickContainer);
    return *s_pGimmickContainer;
};


static inline CGimmickEventDispatcher& GimmickEventDispatcher(void)
{
    ASSERT(s_pGimmickEventDispatcher);
    return *s_pGimmickEventDispatcher;
};


/*static*/ void CGimmickManager::Initialize(void)
{
    if (!s_pGimmickContainer)
    {
        s_pGimmickContainer = new CGimmickContainer;
    };

    if (!s_pGimmickEventDispatcher)
    {
        s_pGimmickEventDispatcher = new CGimmickEventDispatcher;
    };

    CGimmickDataManager::Initialize();
    s_bPlayStarted = false;
};


/*static*/ void CGimmickManager::Terminate(void)
{
    CGimmickDataManager::Terminate();
    
    if (s_pGimmickEventDispatcher)
    {
        delete s_pGimmickEventDispatcher;
        s_pGimmickEventDispatcher = nullptr;
    };

    if (s_pGimmickContainer)
    {
        delete s_pGimmickContainer;
        s_pGimmickContainer = nullptr;
    };
};


/*static*/ void CGimmickManager::Draw(int32 nPriority)
{
    GimmickContainer().Draw(nPriority);    
};


/*static*/ void CGimmickManager::DispatchEvent(void)
{
    GimmickEventDispatcher().Dispatch();
};


/*static*/ CGimmick* CGimmickManager::Create(GIMMICKID::VALUE idGimmick, int32 subid, void* pParam)
{
    CGimmick* pGimmick = CGimmickFactory::Create(idGimmick, subid, pParam);
    if (pGimmick)
    {
        GimmickContainer().Add(
            CGimmickInfo::GetDrawPriority(idGimmick),
            pGimmick
        );

        CGameEvent::SetGimmickCreated(pGimmick);
    };

    return pGimmick;
};


/*static*/ void CGimmickManager::Release(CGimmick* pGimmick)
{
    ASSERT(pGimmick);
    
    if (pGimmick)
    {
        CGameEvent::SetGimmickDestroyed(pGimmick);

        GimmickContainer().Remove(pGimmick);
        CGameObjectManager::DeleteObject(pGimmick);
    };
};


/*static*/ CGimmick* CGimmickManager::Find(const char* pszName)
{
    return GimmickContainer().Find(pszName);
};


/*static*/ void CGimmickManager::NotifyPlayStarted(void)
{
    s_bPlayStarted = true;
};


/*static*/ void CGimmickManager::CreateSetGimmick(void)
{
	CGimmickDataManager::CreateGimmicks(CGimmickDataManager::KIND_NORMAL);
	CGimmickDataManager::CreateGimmicks(CGimmickDataManager::KIND_EFFECT);
};


/*static*/ bool CGimmickManager::IsPlayStarted(void)
{
    return s_bPlayStarted;
};


/*static*/ void CGimmickManager::SetPlayerStartPosition(int32 nPlayerNo, bool bProtect)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    CReplaceGimmick::SetPlayerStartPosition(nPlayerNo, &vPosition, bProtect);
};


/*static*/ void CGimmickManager::ReplacePlayer(int32 nPlayerNo, const RwV3d* pvPosition, bool bProtect)
{
    CReplaceGimmick::ReplacePlayer(nPlayerNo, pvPosition, bProtect);
};


/*static*/ void CGimmickManager::PostEvent(const char* pszReceiver, const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype, void* pParam)
{
    GimmickEventDispatcher().PostEvent(pszReceiver, pszSender, eventtype, pParam);
};


/*static*/ void CGimmickManager::CreateStageSpecialGimmick(STAGEID::VALUE idStage)
{
    ;
};

