#include "MagicManager.hpp"
#include "Magic.hpp"
#include "Tracer.hpp"

#include "Game/System/GameObject/GameObjectManager.hpp"


#if defined(_DEBUG)
#define MAGIC_COMMON_MAX_NUM   (256)
#define MAGIC_ATTACHED_MAX_NUM (256)
#define MAGIC_DISPLAY_MAX_NUM  (256)
#elif defined(NDEBUG)
#define MAGIC_COMMON_MAX_NUM   (15)
#define MAGIC_ATTACHED_MAX_NUM (30)
#define MAGIC_DISPLAY_MAX_NUM  (30)
#endif


class CMagicList
{
private:
    struct MAGICWORK : public CListNode<MAGICWORK>
    {
        CMagic* m_pMagic;
        float m_fSortZ;
    };

public:
    CMagicList(int32 nNum);
    ~CMagicList(void);
    void Cleanup(void);
    bool IsEnableRegist(void) const;
    void RegistMagic(CMagic* pMagic);
    void RemoveMagic(CMagic* pMagic);
    CMagic* GetMagic(const char* pszName);
    void GarbageCollection(void);
    void Draw(RwCamera* pCamera);
    MAGICWORK* SearchMagicWork(CMagic* pMagic);
    MAGICWORK* SearchMagicWork(const char* pszName);

private:
    MAGICWORK* m_paMagicWork;
    int32 m_nMagicListNum;
    CList<MAGICWORK> m_listWorkPool;
    CList<MAGICWORK> m_listWorkAlloc;
    int32 m_nActiveMagicNum;
};


class CMagicContainer
{
public:
    enum POOLTYPE
    {
        POOLTYPE_COMMON = 0,
        POOLTYPE_ATTACHED,

        POOLTYPEMAX,
    };

public:
    CMagicContainer(void);
    ~CMagicContainer(void);
    void Cleanup(void);
    void Run(void);
    void Draw(RwCamera* pCamera);
    void Regist(CMagic* pMagic);
    void Remove(CMagic* pMagic);
    void Finish(CMagic* pMagic);
    void Play(CMagic* pMagic);
    CMagic* Search(const char* pszName);
    CMagic* SearchCurrent(const char* pszName);
    bool IsEnableRegistPool(void);
    bool IsEnableRegistDisplay(void);
    bool IsExist(CMagic* pMagic);
    void Change(POOLTYPE pooltype);

private:
    CMagicList* m_pCurrentMagicPool;
    CMagicList* m_pListCommonMagicPool;
    CMagicList* m_pListAttachedMagicPool;
    CMagicList* m_pListMagicDisplay;
    POOLTYPE m_pooltype;
};


//
// *********************************************************************************
//


CMagicList::CMagicList(int32 nNum)
: m_paMagicWork(nullptr)
, m_nMagicListNum(nNum)
, m_nActiveMagicNum(0)
{
    ASSERT(m_nMagicListNum > 0);

    m_paMagicWork = new MAGICWORK[m_nMagicListNum];

    for (int32 i = 0; i < m_nMagicListNum; ++i)
    {
        m_paMagicWork[i].m_pMagic = nullptr;
        m_paMagicWork[i].m_fSortZ = 0.0f;
        m_listWorkPool.push_back(&m_paMagicWork[i]);
    };
};


CMagicList::~CMagicList(void)
{
    Cleanup();

    if (m_paMagicWork)
    {
        ASSERT(m_nMagicListNum > 0);

        delete[] m_paMagicWork;
        m_paMagicWork = nullptr;

        m_nMagicListNum = 0;
    };

    m_nActiveMagicNum = 0;
};


void CMagicList::Cleanup(void)
{
    auto it = m_listWorkAlloc.begin();
    auto itEnd = m_listWorkAlloc.end();
    while (it != itEnd)
    {
        MAGICWORK* pNode = &(*it);

        pNode->m_pMagic = nullptr;
        pNode->m_fSortZ = 0.0f;

        it = m_listWorkAlloc.erase(it);
        m_listWorkPool.push_back(pNode);
    };

    m_nActiveMagicNum = 0;
};


bool CMagicList::IsEnableRegist(void) const
{
    return (m_nActiveMagicNum < m_nMagicListNum);
};


void CMagicList::RegistMagic(CMagic* pMagic)
{
    ASSERT(pMagic);
    ASSERT(!m_listWorkPool.empty());

    MAGICWORK* pWork = m_listWorkPool.front();
    m_listWorkPool.erase(pWork);
    m_listWorkAlloc.push_back(pWork);

    pWork->m_fSortZ = 0.0f;
    pWork->m_pMagic = pMagic;

    ++m_nActiveMagicNum;
};


void CMagicList::RemoveMagic(CMagic* pMagic)
{
    MAGICWORK* pWork = SearchMagicWork(pMagic);
    ASSERT(pWork);

    if (pWork)
    {
		--m_nActiveMagicNum;
        ASSERT(pWork->m_pMagic == pMagic);

        delete pWork->m_pMagic;
        pWork->m_pMagic = nullptr;
        pWork->m_fSortZ = 0.0f;

        m_listWorkAlloc.erase(pWork);
        m_listWorkPool.push_back(pWork);
    };
};


CMagic* CMagicList::GetMagic(const char* pszName)
{
    MAGICWORK* pWork = SearchMagicWork(pszName);
    if (pWork)
        return pWork->m_pMagic;
    
    return nullptr;
};


void CMagicList::GarbageCollection(void)
{
    auto it = m_listWorkAlloc.begin();
    auto itEnd = m_listWorkAlloc.end();
    while (it != itEnd)
    {
        MAGICWORK* pWork = &(*it);

        ASSERT(pWork->m_pMagic);

        if (pWork->m_pMagic->IsEnd())
        {
            delete pWork->m_pMagic;
            pWork->m_pMagic = nullptr;
            pWork->m_fSortZ = 0.0f;

            it = m_listWorkAlloc.erase(it);
            m_listWorkPool.push_back(pWork);

            --m_nActiveMagicNum;
        }
        else
        {
            ++it;
        };
    };
};


void CMagicList::Draw(RwCamera* pCamera)
{
    for (MAGICWORK& it : m_listWorkAlloc)
    {
        ASSERT(it.m_pMagic);
        it.m_pMagic->Draw(pCamera);
    };
};


CMagicList::MAGICWORK* CMagicList::SearchMagicWork(CMagic* pMagic)
{
    for (MAGICWORK& it : m_listWorkAlloc)
    {
        ASSERT(it.m_pMagic);

        if ((it.m_pMagic == pMagic) && !std::strcmp(it.m_pMagic->GetName(), pMagic->GetName()))
            return &it;
    };

    return nullptr;
};


CMagicList::MAGICWORK* CMagicList::SearchMagicWork(const char* pszName)
{
    for (MAGICWORK& it : m_listWorkAlloc)
    {
        ASSERT(it.m_pMagic);

        if (!std::strcmp(it.m_pMagic->GetName(), pszName))
            return &it;
    };

    return nullptr;
};


//
// *********************************************************************************
//


CMagicContainer::CMagicContainer(void)
: m_pCurrentMagicPool(nullptr)
, m_pListCommonMagicPool(nullptr)
, m_pListAttachedMagicPool(nullptr)
, m_pListMagicDisplay(nullptr)
, m_pooltype(POOLTYPE_COMMON)
{
    m_pListCommonMagicPool   = new CMagicList(MAGIC_COMMON_MAX_NUM);
    m_pListAttachedMagicPool = new CMagicList(MAGIC_ATTACHED_MAX_NUM);
    m_pListMagicDisplay      = new CMagicList(MAGIC_DISPLAY_MAX_NUM);

    m_pCurrentMagicPool = m_pListCommonMagicPool;
};


CMagicContainer::~CMagicContainer(void)
{
    if (m_pListMagicDisplay)
    {
        delete m_pListMagicDisplay;
        m_pListMagicDisplay = nullptr;
    };

    if (m_pListAttachedMagicPool)
    {
        delete m_pListAttachedMagicPool;
        m_pListAttachedMagicPool = nullptr;
    };

    if (m_pListCommonMagicPool)
    {
        delete m_pListCommonMagicPool;
        m_pListCommonMagicPool = nullptr;
    };
};


void CMagicContainer::Cleanup(void)
{
    m_pListMagicDisplay->Cleanup();
    m_pListAttachedMagicPool->Cleanup();
    m_pListCommonMagicPool->Cleanup();
};


void CMagicContainer::Run(void)
{
    m_pListMagicDisplay->GarbageCollection();
};


void CMagicContainer::Draw(RwCamera* pCamera)
{
    m_pListMagicDisplay->Draw(pCamera);
};


void CMagicContainer::Regist(CMagic* pMagic)
{
    m_pCurrentMagicPool->RegistMagic(pMagic);
};


void CMagicContainer::Remove(CMagic* pMagic)
{
    m_pCurrentMagicPool->RemoveMagic(pMagic);
};


void CMagicContainer::Finish(CMagic* pMagic)
{
    m_pListMagicDisplay->RemoveMagic(pMagic);
};


void CMagicContainer::Play(CMagic* pMagic)
{
    m_pListMagicDisplay->RegistMagic(pMagic);
    pMagic->StartObject();
};


CMagic* CMagicContainer::Search(const char* pszName)
{
    CMagic* pMagic = m_pListCommonMagicPool->GetMagic(pszName);
    if (!pMagic)
        pMagic = m_pListAttachedMagicPool->GetMagic(pszName);

    return pMagic;
};


CMagic* CMagicContainer::SearchCurrent(const char* pszName)
{
    return m_pCurrentMagicPool->GetMagic(pszName);
};


bool CMagicContainer::IsEnableRegistPool(void)
{
    return m_pCurrentMagicPool->IsEnableRegist();
};


bool CMagicContainer::IsEnableRegistDisplay(void)
{
    return m_pListMagicDisplay->IsEnableRegist();
};


bool CMagicContainer::IsExist(CMagic* pMagic)
{
    return (m_pListMagicDisplay->GetMagic(pMagic->GetName()) != nullptr);
};


void CMagicContainer::Change(POOLTYPE pooltype)
{
    switch (pooltype)
    {
    case POOLTYPE_ATTACHED:
        m_pCurrentMagicPool = m_pListAttachedMagicPool;
        m_pooltype = POOLTYPE_ATTACHED;
        break;

    case POOLTYPE_COMMON:
        m_pCurrentMagicPool = m_pListCommonMagicPool;
        m_pooltype = POOLTYPE_COMMON;
        break;

    default:
        ASSERT(false);
        break;
    };
};


static CMagicContainer* s_pMagicContainer = nullptr;


static inline CMagicContainer& MagicContainer(void)
{
    ASSERT(s_pMagicContainer);
    return *s_pMagicContainer;
};


static inline CMagic* MagicFromHandle(uint32 hMagic)
{
    CGameObject* pGameObject = CGameObjectManager::GetObject(hMagic);
    if (pGameObject)
    {
        ASSERT(pGameObject->GetType() == GAMEOBJECTTYPE::EFFECT);
        ASSERT(static_cast<CEffect*>(pGameObject)->GetEffectType() == CEffect::TYPE_WITHHIT);
        
        return static_cast<CMagic*>(pGameObject);
    };

    return nullptr;
};


CMagicManager::CParameter::CParameter(void)
: m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_AXIS_Z)
, m_pParent(nullptr)
, m_pTarget(nullptr)
, m_bPlaySound(true)
{
    ;
};


void CMagicManager::CParameter::SetPositon(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    m_vPosition = *pvPosition;
};


void CMagicManager::CParameter::SetDirection(const RwV3d* pvDirection)
{
    ASSERT(pvDirection);
    m_vDirection = *pvDirection;
};


void CMagicManager::CParameter::SetDirection(float fDirection)
{
    m_vDirection.x = Math::Sin(fDirection);
    m_vDirection.z = Math::Cos(fDirection);
};


void CMagicManager::CParameter::SetObject(CGameObject* pObj)
{
    m_pParent = pObj;
};


void CMagicManager::CParameter::SetTarget(CTracer* pTarget)
{
    m_pTarget = pTarget;
};


void CMagicManager::CParameter::SetTracer(CTracer* pTracer)
{
    SetTarget(pTracer);
};


void CMagicManager::CParameter::SetSoundPlay(bool bSet)
{
    m_bPlaySound = bSet;
};


/*static*/ void CMagicManager::Initialize(void)
{
    if (!s_pMagicContainer)
        s_pMagicContainer = new CMagicContainer;
};


/*static*/ void CMagicManager::Terminate(void)
{
    if (s_pMagicContainer)
    {
        s_pMagicContainer->Cleanup();

        delete s_pMagicContainer;
        s_pMagicContainer = nullptr;
    };
};


/*static*/ void CMagicManager::Run(void)
{
    MagicContainer().Run();
};


/*static*/ void CMagicManager::Draw(RwCamera* pCamera)
{
    MagicContainer().Draw(pCamera);
};


/*static*/ void CMagicManager::ChangeToCommon(void)
{
    MagicContainer().Change(CMagicContainer::POOLTYPE_COMMON);
};


/*static*/ void CMagicManager::ChangeToAttached(void)
{
    MagicContainer().Change(CMagicContainer::POOLTYPE_ATTACHED);
};


/*static*/ uint32 CMagicManager::RegistMagic(CMagic* pMagic)
{
    ASSERT(MagicContainer().IsEnableRegistPool());
    ASSERT(pMagic);
    
    MagicContainer().Regist(pMagic);

    return pMagic->GetHandle();
};


/*static*/ uint32 CMagicManager::Play(MAGICID::VALUE idMagic, const CParameter* pParameter)
{
    return Play(MAGICID::GetNameFromID(idMagic), pParameter);
};


/*static*/ uint32 CMagicManager::Play(const char* pszMagicName, const CParameter* pParameter)
{
    ASSERT(pszMagicName);
    ASSERT(pParameter);

    if (!MagicContainer().IsEnableRegistDisplay())
        return 0;

    CMagic* pMagic = MagicContainer().Search(pszMagicName);
    if (!pMagic)
        return 0;

    CMagic* pMagicPlay = pMagic->Clone();
    ASSERT(pMagicPlay);

    pMagicPlay->SetPosition(&pParameter->m_vPosition);
    pMagicPlay->SetDirection(&pParameter->m_vDirection);
    pMagicPlay->SetParent(pParameter->m_pParent);
    pMagicPlay->SetTarget(pParameter->m_pTarget);
    pMagicPlay->SetSoundPlay(pParameter->m_bPlaySound);

    MagicContainer().Play(pMagicPlay);
    
    return pMagicPlay->GetHandle();
};


/*static*/ uint32 CMagicManager::Play(const char* pszMagicName,
                                      const RwV3d* pvPosition,
                                      const RwV3d* pvDirection /*= nullptr*/,
                                      CGameObject* pParent /*= nullptr*/,
                                      bool bPlaySound /*= true */)
{
    ASSERT(pszMagicName);
    ASSERT(pvPosition);

    if (!MagicContainer().IsEnableRegistDisplay())
        return 0;
    
    CMagic* pMagic = MagicContainer().Search(pszMagicName);
    if (!pMagic)
        return 0;

	CMagic* pMagicPlay = pMagic->Clone();
    ASSERT(pMagicPlay);

    pMagicPlay->SetPosition(pvPosition);
    if (pvDirection)
        pMagicPlay->SetDirection(pvDirection);
    pMagicPlay->SetParent(pParent);
    pMagicPlay->SetSoundPlay(bPlaySound);

    MagicContainer().Play(pMagicPlay);

    return pMagicPlay->GetHandle();
};


/*static*/ uint32 CMagicManager::Play(const char* pszMagicName,
                                      const RwV3d* pvPosition,
                                      const RwV3d* pvDirection,
                                      CGameObject* pParent,
                                      CTracer* pTarget,
                                      bool bPlaySound)
{
    ASSERT(pszMagicName);
    ASSERT(pvPosition);

    if (!MagicContainer().IsEnableRegistDisplay())
    {
        if (pTarget)
            delete pTarget;

        return 0;
    };

    CMagic* pMagic = MagicContainer().Search(pszMagicName);
    if (!pMagic)
    {
        if (pTarget)
            delete pTarget;

        return 0;
    };

    CMagic* pMagicPlay = pMagic->Clone();
    ASSERT(pMagicPlay);

    pMagicPlay->SetPosition(pvPosition);
    if (pvDirection)
        pMagicPlay->SetDirection(pvDirection);    
    pMagicPlay->SetParent(pParent);
    pMagicPlay->SetTarget(pTarget);
    pMagicPlay->SetSoundPlay(bPlaySound);

    MagicContainer().Play(pMagicPlay);

    return pMagicPlay->GetHandle();
};


/*static*/ void CMagicManager::SetPosition(uint32 hMagic, const RwV3d* pvPosition)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetPosition(pvPosition);
    };
};


/*static*/ void CMagicManager::GetPosition(uint32 hMagic, RwV3d* pvPosition)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->GetPosition(pvPosition);
    };
};


/*static*/ void CMagicManager::SetDirection(uint32 hMagic, const RwV3d* pvDirection)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetDirection(pvDirection);
    };
};


/*static*/ void CMagicManager::SetDirection(uint32 hMagic, const RwMatrix* pMatRot)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetDirection(pMatRot);
    };
};


/*static*/ void CMagicManager::SetScale(uint32 hMagic, float fScale)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetScale(fScale);
    };
};


/*static*/ void CMagicManager::SetLive(uint32 hMagic, float fLivetime)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetLive(fLivetime);
    };
};


/*static*/ void CMagicManager::SetDamage(uint32 hMagic, int32 nPower)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetDamage(nPower);
    };
};


/*static*/ void CMagicManager::SetStatusTime(uint32 hMagic, float fTime)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetStatusTime(fTime);
    };
};


/*static*/ void CMagicManager::SetHitAttack(uint32 hMagic, MAGICTYPES::FEATURE feature)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetFeature(feature);
    };
};


/*static*/ void CMagicManager::SetChangeStatus(uint32 hMagic, int32 nStatus)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetChangeStatus(nStatus);
    };
};


/*static*/ void CMagicManager::SetFlyawaySpeed(uint32 hMagic, float x, float y)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->SetFlyawayXY(x, y);
    };    
};


/*static*/ void CMagicManager::SetSpeed(uint32 hMagic, const RwV3d* pvVector)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
        {
            ASSERT(pvVector);
            
            MAGICTYPES::MOVEMENT movement;
            movement.m_vVelocity = *pvVector;
            movement.m_vAcceleration = Math::VECTOR3_ZERO;
            
            pMagic->SetMovement(&movement);
            pMagic->AddFeature(MAGICTYPES::FEATURE_STRAIGHT);
        };
    };
};


/*static*/ void CMagicManager::SetSpeed(uint32 hMagic, float fDirection, float fSpeed)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
        {            
            MAGICTYPES::MOVEMENT movement;
            movement.m_vVelocity.x = Math::Sin(fDirection) * fSpeed;
            movement.m_vVelocity.y = 0.0f;
            movement.m_vVelocity.z = Math::Cos(fDirection) * fSpeed;
            movement.m_vAcceleration = Math::VECTOR3_ZERO;

            pMagic->SetMovement(&movement);
            pMagic->AddFeature(MAGICTYPES::FEATURE_STRAIGHT);
        };
    };
};


/*static*/ void CMagicManager::SetSpeed(uint32 hMagic, float fSpeed)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
        {
            pMagic->SetSpeed(fSpeed);
            pMagic->AddFeature(MAGICTYPES::FEATURE_STRAIGHT);            
        };
    };
};


/*static*/ void CMagicManager::Finish(uint32 hMagic)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
        MagicContainer().Finish(pMagic);
};


/*static*/ void CMagicManager::End(uint32 hMagic)
{
    CMagic* pMagic = MagicFromHandle(hMagic);
    if (pMagic)
    {
        if (MagicContainer().IsExist(pMagic))
            pMagic->End();
    };
};


/*static*/ bool CMagicManager::IsChargeAttack(CMagic* pMagic)
{
    static const char* s_apszSpEffectNameList[] =
    {
        "leo_sp",
        "rap_sp",
        "mic_sp_ball",
        "kar_sp",
        "cas_sp_ball",
        "sla_sp",
    };

    for (int32 i = 0; i < COUNT_OF(s_apszSpEffectNameList); ++i)
    {
        if (!std::strcmp(pMagic->GetName(), s_apszSpEffectNameList[i]))
            return true;
    };

    return false;
};