#include "EffectManager.hpp"
#include "MagicManager.hpp"
#include "MagicParameter.hpp"
#include "Tracer.hpp"
#include "Effect.hpp"
#include "MagicSetup.hpp"
#include "Magic.hpp"
#include "Light.hpp"
#include "Snow.hpp"
#include "IceBlock.hpp"
#include "WaterSplash.hpp"
#include "ScreenEffectRain.hpp"
#include "ScreenEffectFlash.hpp"
#include "ScreenEffectAccel.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Module/LocusModule.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Module/BarrierModule.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


#if defined(_DEBUG)
#define EFFECT_COMMON_MAX_NUM   (256)
#define EFFECT_ATTACHED_MAX_NUM (256)
#define EFFECT_DISPLAY_MAX_NUM  (256)
#elif defined(NDEBUG)
#define EFFECT_COMMON_MAX_NUM   (100)
#define EFFECT_ATTACHED_MAX_NUM (30)
#define EFFECT_DISPLAY_MAX_NUM  (100)
#endif


class CEffectList
{
private:
    struct EFFECTWORK : public CListNode<EFFECTWORK>
    {
        CEffect* m_pEffect;
        float m_fSortZ;
    };
    
public:
    CEffectList(int32 nNum);
    ~CEffectList(void);
    void Cleanup(void);
    bool IsEnableRegist(void) const;
    void RegistEffect(CEffect* pEffect);
    void RemoveEffect(CEffect* pEffect);
    CEffect* GetEffect(const char* pszName);
    void GarbageCollection(void);
    void Draw(RwCamera* pCamera);
    EFFECTWORK* SearchEffectWork(CEffect* pEffect);
    EFFECTWORK* SearchEffectWork(const char* pszName);

private:
    EFFECTWORK*       m_paEffectWork;
    int32             m_nEffectListNum;
    CList<EFFECTWORK> m_listWorkPool;
    CList<EFFECTWORK> m_listWorkAlloc;
    int32             m_nActiveEffectNum;
};

class CEffectContainer
{
private:
    enum POOLTYPE
    {
        POOLTYPE_COMMON = 0,
        POOLTYPE_ATTACHED,
        
        POOLTYPEMAX,
    };
    
public:
    CEffectContainer(void);
    ~CEffectContainer(void);
    void Cleanup(void);
    void Run(void);
    void Draw(RwCamera* pCamera);
    void Regist(CEffect* pEffect);
    void Remove(CEffect* pEffect);
    void Finish(CEffect* pEffect);
    void Play(CEffect* pEffect);
    CEffect* Search(const char* pszName);
    CEffect* SearchCurrent(const char* pszName);
    bool IsEnableRegistPool(void);
    bool IsEnableRegistDisplay(void);
    bool IsExist(CEffect* pEffect);

private:
    CEffectList* m_pCurrentEffectPool;
    CEffectList* m_pListCommonEffectPool;
    CEffectList* m_pListAttachedEffectPool;
    CEffectList* m_pListEffectDisplay;
    POOLTYPE     m_pooltype;
};


CEffectList::CEffectList(int32 nNum)
: m_paEffectWork(nullptr)
, m_nEffectListNum(nNum)
, m_nActiveEffectNum(0)
{
    ASSERT(m_nEffectListNum > 0);

    m_paEffectWork = new EFFECTWORK[m_nEffectListNum];

    for (int32 i = 0; i < m_nEffectListNum; ++i)
    {
        m_paEffectWork[i].m_pEffect = nullptr;
        m_paEffectWork[i].m_fSortZ = 0.0f;
        m_listWorkPool.push_back(&m_paEffectWork[i]);
    };
};


CEffectList::~CEffectList(void)
{
    Cleanup();
    
    if (m_paEffectWork)
    {
        ASSERT(m_nEffectListNum > 0);

        delete[] m_paEffectWork;
        m_paEffectWork = nullptr;
        
        m_nEffectListNum = 0;
    };

    m_nActiveEffectNum = 0;
};


void CEffectList::Cleanup(void)
{
    auto it = m_listWorkAlloc.begin();
    auto itEnd = m_listWorkAlloc.end();
    while (it != itEnd)
    {
        EFFECTWORK* pNode = &(*it);
        pNode->m_pEffect = nullptr;
        pNode->m_fSortZ = 0.0f;

        it = m_listWorkAlloc.erase(it);
        m_listWorkPool.push_back(pNode);
    };

    m_nActiveEffectNum = 0;
};


bool CEffectList::IsEnableRegist(void) const
{
    return (m_nActiveEffectNum < m_nEffectListNum);
};


void CEffectList::RegistEffect(CEffect* pEffect)
{
    ASSERT(pEffect);
    ASSERT(!m_listWorkPool.empty());

    EFFECTWORK* pWork = m_listWorkPool.front();
    m_listWorkPool.erase(pWork);
    m_listWorkAlloc.push_back(pWork);

    pWork->m_fSortZ = 0.0f;
    pWork->m_pEffect = pEffect;

    ++m_nActiveEffectNum;
};


void CEffectList::RemoveEffect(CEffect* pEffect)
{
    EFFECTWORK* pWork = SearchEffectWork(pEffect);
    ASSERT(pWork);

    if (pWork)
    {
        --m_nActiveEffectNum;
        ASSERT(pWork->m_pEffect == pEffect);

        delete pWork->m_pEffect;
        pWork->m_pEffect = nullptr;
        pWork->m_fSortZ = 0.0f;

        m_listWorkAlloc.erase(pWork);
        m_listWorkPool.push_back(pWork);
    };
};


CEffect* CEffectList::GetEffect(const char* pszName)
{
    EFFECTWORK* pWork = SearchEffectWork(pszName);
    if (pWork)
        return pWork->m_pEffect;
    
    return nullptr;
};


void CEffectList::GarbageCollection(void)
{
    auto it = m_listWorkAlloc.begin();
    auto itEnd = m_listWorkAlloc.end();
    while (it != itEnd)
    {
        EFFECTWORK* pWork = &(*it);

        ASSERT(pWork->m_pEffect);

        if (pWork->m_pEffect->IsEnd())
        {
            delete pWork->m_pEffect;
            pWork->m_pEffect = nullptr;
            pWork->m_fSortZ = 0.0f;
            
            it = m_listWorkAlloc.erase(it);
            m_listWorkPool.push_back(pWork);

            --m_nActiveEffectNum;
        }
        else
        {
            ++it;
        };
    };
};


void CEffectList::Draw(RwCamera* pCamera)
{
    for (EFFECTWORK& it : m_listWorkAlloc)
    {
        CEffect* pEffect = it.m_pEffect;
        ASSERT(pEffect);

        pEffect->Draw(pCamera);
    };
};


CEffectList::EFFECTWORK* CEffectList::SearchEffectWork(CEffect* pEffect)
{
    for (EFFECTWORK& it : m_listWorkAlloc)
    {
        ASSERT(it.m_pEffect);
        
        if ((it.m_pEffect == pEffect) && !std::strcmp(it.m_pEffect->GetName(), pEffect->GetName()))
            return &it;
    };

    return nullptr;
};


CEffectList::EFFECTWORK* CEffectList::SearchEffectWork(const char* pszName)
{
    for (EFFECTWORK& it : m_listWorkAlloc)
    {
        ASSERT(it.m_pEffect);
        
        if (!std::strcmp(it.m_pEffect->GetName(), pszName))
            return &it;
    };

    return nullptr;
};


//
// *********************************************************************************
//


CEffectContainer::CEffectContainer(void)
: m_pCurrentEffectPool(nullptr)
, m_pListCommonEffectPool(nullptr)
, m_pListAttachedEffectPool(nullptr)
, m_pListEffectDisplay(nullptr)
, m_pooltype(POOLTYPE_COMMON)
{
    m_pListCommonEffectPool   = new CEffectList(EFFECT_COMMON_MAX_NUM);
    m_pListAttachedEffectPool = new CEffectList(EFFECT_ATTACHED_MAX_NUM);
    m_pListEffectDisplay      = new CEffectList(EFFECT_DISPLAY_MAX_NUM);

    m_pCurrentEffectPool = m_pListCommonEffectPool;
};


CEffectContainer::~CEffectContainer(void)
{
    if (m_pListEffectDisplay)
    {
        delete m_pListEffectDisplay;
        m_pListEffectDisplay = nullptr;
    };

    if (m_pListAttachedEffectPool)
    {
        delete m_pListAttachedEffectPool;
        m_pListAttachedEffectPool = nullptr;
    };

    if (m_pListCommonEffectPool)
    {
        delete m_pListCommonEffectPool;
        m_pListCommonEffectPool = nullptr;
    };
};


void CEffectContainer::Cleanup(void)
{
    m_pListEffectDisplay->Cleanup();
    m_pListAttachedEffectPool->Cleanup();
    m_pListCommonEffectPool->Cleanup();
};


void CEffectContainer::Run(void)
{
    m_pListEffectDisplay->GarbageCollection();
};


void CEffectContainer::Draw(RwCamera* pCamera)
{
    m_pListEffectDisplay->Draw(pCamera);
};


void CEffectContainer::Regist(CEffect* pEffect)
{
    m_pCurrentEffectPool->RegistEffect(pEffect);
};


void CEffectContainer::Remove(CEffect* pEffect)
{
    m_pCurrentEffectPool->RemoveEffect(pEffect);
};


void CEffectContainer::Finish(CEffect* pEffect)
{
    m_pListEffectDisplay->RemoveEffect(pEffect);
};


void CEffectContainer::Play(CEffect* pEffect)
{
    m_pListEffectDisplay->RegistEffect(pEffect);
    pEffect->Start();
};


CEffect* CEffectContainer::Search(const char* pszName)
{
    CEffect* pEffect = m_pListCommonEffectPool->GetEffect(pszName);
    if (!pEffect)
    {
        pEffect = m_pListAttachedEffectPool->GetEffect(pszName);
    };
    
    return pEffect;
};


CEffect* CEffectContainer::SearchCurrent(const char* pszName)
{
    return m_pCurrentEffectPool->GetEffect(pszName);
};


bool CEffectContainer::IsEnableRegistPool(void)
{
    return m_pCurrentEffectPool->IsEnableRegist();
};


bool CEffectContainer::IsEnableRegistDisplay(void)
{
    return m_pListEffectDisplay->IsEnableRegist();
};


bool CEffectContainer::IsExist(CEffect* pEffect)
{
    return (m_pListEffectDisplay->GetEffect(pEffect->GetName()) != nullptr);
};


static CEffectContainer* s_pEffectContainer = nullptr;


static inline CEffectContainer& EffectContainer(void)
{
    ASSERT(s_pEffectContainer);
    return *s_pEffectContainer;
};


static inline CEffect* EffectFromHandle(uint32 hEffect)
{
    CGameObject* pGameObject = CGameObjectManager::GetObject(hEffect);
    if (pGameObject)
    {
        ASSERT(pGameObject->GetType() == GAMEOBJECTTYPE::EFFECT);
        ASSERT(pGameObject->GetHandle() == hEffect);
        
        return static_cast<CEffect*>(pGameObject);
    };

    return nullptr;
};


/*static*/ void CEffectManager::Initialize(void)
{
    if (!s_pEffectContainer)
        s_pEffectContainer = new CEffectContainer;

    CMagicManager::Initialize();
    CIceBlockManager::Initialize();
    CScreenEffectRain::Initialize();
    CWaterSplashManager::Initialize();
    CSnowManager::Initialize();
    CScreenEffectFlash::Initialize();
    CScreenEffectAccel::Initialize();    
    CLocusModuleManager::Initialize();
    CEffectLightManager::Initialize();
};


/*static*/ void CEffectManager::Terminate(void)
{    
    CEffectLightManager::Terminate();
    CLocusModuleManager::Terminate();
    CScreenEffectAccel::Terminate();
    CScreenEffectFlash::Terminate();
    CSnowManager::Terminate();
    CScreenEffectRain::Terminate();
    CWaterSplashManager::Terminate();
    CIceBlockManager::Terminate();
    CMagicManager::Terminate();

    if (s_pEffectContainer)
    {
        s_pEffectContainer->Cleanup();

        delete s_pEffectContainer;
        s_pEffectContainer = nullptr;
    };
};


/*static*/ void CEffectManager::Period(void)
{
    EffectContainer().Run();
    CMagicManager::Run();
    CIceBlockManager::Run();
    CScreenEffectAccel::Run();
    CSnowManager::Run();
    CScreenEffectRain::Run();
    CWaterSplashManager::Run();
    CScreenEffectFlash::Run();    
    CLocusModuleManager::Run();
    CEffectLightManager::Run();
};


/*static*/ void CEffectManager::Draw(RwCamera* pRwCamera)
{
    CCircleShadowModule::DrawAll();
    CAccumulateModule::DrawAllAccumulate();
    CIceBlockManager::Draw();
    EffectContainer().Draw(pRwCamera);
    CMagicManager::Draw(pRwCamera);
    CBarrierModule::DrawAll();
    CLocusModuleManager::Draw();
    CWaterSplashManager::Draw();
    CSnowManager::Draw();
    CScreenEffectRain::Draw();
    CScreenEffectAccel::Draw();
    CScreenEffectFlash::Draw();
    CEffectLightManager::Draw();
};


/*static*/ void CEffectManager::Read(const char* pszName, void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);

    if (!EffectContainer().IsEnableRegistPool())
        return;

    if (EffectContainer().Search(pszName))
        return;

    CEffect* pEffect = new CEffect(pszName);
    if (pEffect)
    {
        pEffect->ReadEffect(pBuffer, uBufferSize);
        pEffect->SetStringEffectOn(CGameData::Option().Display().IsEnabledFontEffect());

        EffectContainer().Regist(pEffect);

        MAGIC_SETUP::CreateMagic(pszName);
    };    
};


/*static*/ CEffect* CEffectManager::GetEffectFromPool(const char* pszName)
{
    return EffectContainer().Search(pszName);
};


/*static*/ uint32 CEffectManager::Play(EFFECTID::VALUE idEffect, const RwV3d* pvPosition, bool bPlaySound)
{
    return Play(EFFECTID::GetNameFromID(idEffect), pvPosition, bPlaySound);
};


/*static*/ uint32 CEffectManager::Play(EFFECTID::VALUE idEffect, const RwV3d* pvPosition, float fDirection, bool bPlaySound)
{
    return Play(EFFECTID::GetNameFromID(idEffect), pvPosition, fDirection, bPlaySound);
};


/*static*/ uint32 CEffectManager::Play(const char* pszName, const RwV3d* pvPosition, bool bPlaySound)
{
    ASSERT(pszName);
    ASSERT(pvPosition);

	if (!EffectContainer().IsEnableRegistDisplay())
	{
        ASSERT(false, "effect display pool empty");
        FATAL("effect display pool empty");
        return 0;
	};

    CEffect* pEffect = EffectContainer().Search(pszName);
    if (!pEffect)
    {
        return 0;
    };

	CEffect* pEffectPlay = pEffect->Clone();
    ASSERT(pEffectPlay);

    pEffectPlay->SetPosition(pvPosition);
    pEffectPlay->SetSoundPlay(bPlaySound);

    EffectContainer().Play(pEffectPlay);

    return pEffectPlay->GetHandle();
};


/*static*/ uint32 CEffectManager::Play(const char* pszName, const RwV3d* pvPosition, float fDirection, bool bPlaySound)
{
    ASSERT(pszName);
    ASSERT(pvPosition);

	if (!EffectContainer().IsEnableRegistDisplay())
	{
        ASSERT(false, "effect display pool empty");
        FATAL("effect display pool empty");
        return 0;
	};

    CEffect* pEffect = EffectContainer().Search(pszName);
    if (!pEffect)
        return 0;

    CEffect* pEffectPlay = pEffect->Clone();
    ASSERT(pEffectPlay);

    pEffectPlay->SetDirection(fDirection);
    pEffectPlay->SetPosition(pvPosition);
    pEffectPlay->SetSoundPlay(bPlaySound);

    EffectContainer().Play(pEffectPlay);

    return pEffectPlay->GetHandle();
};


/*static*/ uint32 CEffectManager::PlayTrace(EFFECTID::VALUE idEffect, CTracer* pTracer, const RwV3d* pvOffset, bool bPlaySound)
{
    return PlayTrace(EFFECTID::GetNameFromID(idEffect), pTracer, pvOffset, bPlaySound);    
};


/*static*/ uint32 CEffectManager::PlayTrace(EFFECTID::VALUE idEffect, CTracer* pTracer, const RwV3d* pvOffset, float fDirection, bool bPlaySound)
{
    return PlayTrace(EFFECTID::GetNameFromID(idEffect), pTracer, pvOffset, fDirection, bPlaySound);
};


/*static*/ uint32 CEffectManager::PlayTrace(const char* pszName, CTracer* pTracer, const RwV3d* pvOffset, bool bPlaySound)
{
    ASSERT(pszName);
    ASSERT(pTracer);
    ASSERT(pvOffset);

    if (!EffectContainer().IsEnableRegistDisplay())
    {
        ASSERT(false, "effect display pool empty");
        FATAL("effect display pool empty");
        delete pTracer;
        return 0;
    };

    CEffect* pEffect = EffectContainer().Search(pszName);
    if (!pEffect)
    {
        delete pTracer;
        return 0;
    };

    CEffect* pEffectPlay = pEffect->Clone();
    ASSERT(pEffectPlay);

    pEffectPlay->SetTracer(pTracer, pvOffset, 0.0f);
    pEffectPlay->SetDirectionTraceFlag(true);
    pEffectPlay->SetSoundPlay(bPlaySound);

    EffectContainer().Play(pEffectPlay);

    return pEffectPlay->GetHandle();
};


/*static*/ uint32 CEffectManager::PlayTrace(const char* pszName, CTracer* pTracer, const RwV3d* pvOffset, float fDirection, bool bPlaySound)
{
    ASSERT(pszName);
    ASSERT(pTracer);
    ASSERT(pvOffset);

    if (!EffectContainer().IsEnableRegistDisplay())
    {
        ASSERT(false, "effect display pool empty");
        FATAL("effect display pool empty");
        delete pTracer;
        return 0;
    };

    CEffect* pEffect = EffectContainer().Search(pszName);
    if (!pEffect)
    {
        delete pTracer;
        return 0;
    };

    CEffect* pEffectPlay = pEffect->Clone();
    ASSERT(pEffectPlay);

    pEffectPlay->SetDirection(fDirection);
    pEffectPlay->SetTracer(pTracer, pvOffset, fDirection);
    pEffectPlay->SetDirectionTraceFlag(false);
    pEffectPlay->SetSoundPlay(bPlaySound);

    EffectContainer().Play(pEffectPlay);

    return pEffectPlay->GetHandle();
};


/*static*/ void CEffectManager::ConvertWithDestroy(const char* pszName, CMagicParameter* pMagicParameter)
{
    CEffect* pEffect = EffectContainer().SearchCurrent(pMagicParameter->GetBaseEffectName());
    if (pEffect)
    {
        CMagic* pMagic = new CMagic(pszName);

        pMagic->CreateSubstance(pMagicParameter->GetBaseEffectName());
        pMagic->SetParameter(pMagicParameter);
        pMagic->SetStringEffectOn(CGameData::Option().Display().IsEnabledFontEffect());

        CMagicManager::RegistMagic(pMagic);

        EffectContainer().Remove(pEffect);
    };
};


/*static*/ void CEffectManager::Convert(const char* pszName, CMagicParameter* pMagicParameter)
{
    CEffect* pEffect = EffectContainer().SearchCurrent(pMagicParameter->GetBaseEffectName());
    if (pEffect)
    {
        CMagic* pMagic = new CMagic(pszName);

        pMagic->CreateSubstance(pMagicParameter->GetBaseEffectName());
        pMagic->SetParameter(pMagicParameter);
        pMagic->SetStringEffectOn(CGameData::Option().Display().IsEnabledFontEffect());

        CMagicManager::RegistMagic(pMagic);
    };
};


/*static*/ void CEffectManager::SetActive(uint32 hEffect, bool bActive)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
    {
        if (EffectContainer().IsExist(pEffect))
            pEffect->SetActive(bActive);
    };
};


/*static*/ void CEffectManager::Finish(uint32 hEffect)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
        EffectContainer().Finish(pEffect);
};


/*static*/ void CEffectManager::End(uint32 hEffect)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
    {
        if (EffectContainer().IsExist(pEffect))
            pEffect->End();
    };
};


/*static*/ bool CEffectManager::IsEnd(uint32 hEffect)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
    {
        if (!EffectContainer().IsExist(pEffect))
            return true;
    };

    return false;
};


/*static*/ void CEffectManager::SetPosition(uint32 hEffect, const RwV3d* pvPosition)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
    {
        if (EffectContainer().IsExist(pEffect))
            pEffect->SetPosition(pvPosition);
    };
};


/*static*/ void CEffectManager::SetScale(uint32 hEffect, float fScale)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
    {
        if (EffectContainer().IsExist(pEffect))
            pEffect->SetScale(fScale);
    };
};


/*static*/ void CEffectManager::SetDirection(uint32 hEffect, RwMatrix* pMatrix)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
    {
        if (EffectContainer().IsExist(pEffect))
            pEffect->SetDirection(pMatrix);
    };
};


/*static*/ void CEffectManager::SetDirection(uint32 hEffect, const RwV3d* pvDirection)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
    {
        if (EffectContainer().IsExist(pEffect))
            pEffect->SetDirection(pvDirection);
    };
};


/*static*/ void CEffectManager::SetDirection(uint32 hEffect, float fDirection)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
    {
        if (EffectContainer().IsExist(pEffect))
            pEffect->SetDirection(fDirection);
    };
};


/*static*/ EFFECTID::VALUE CEffectManager::GetEffectID(uint32 hEffect)
{
    CEffect* pEffect = EffectFromHandle(hEffect);
    if (pEffect)
        return pEffect->GetID();
    
    return EFFECTID::ID_UNKNOWN;
};