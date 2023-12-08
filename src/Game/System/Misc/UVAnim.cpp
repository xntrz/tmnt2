#include "UVAnim.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"

#include "rtdict.h"
#include "rtanim.h"
#include "rpuvanim.h"


class CUVAnimManager
{
private:
    struct UVDICTINFO
    {
        RtDict* m_pUVAnimDict;
        char m_szName[32];
    };

    struct MATERIALCALLBACKWORK
    {
        RpMaterialCallBack m_pCallback;
        void* m_pData;
    };

public:
    static RpMaterial* AddAnimatedMaterialToList(RpMaterial* pMaterial, void* pData);
    static RpAtomic* AtomicForAllMaterials(RpAtomic* pAtomic, void* pData);
    static RpClump* ClumpForAllMaterials(RpClump* pClump, void* pData);
    
    CUVAnimManager(void);
    virtual ~CUVAnimManager(void);
    bool RegistUVAnimDict(RtDict* pDict, const char* pszName);
    bool SetUVAnimDict(const char* pszName);
    bool IsExistUVAnimDict(const char* pszName);
    RwSList* CreateAnimatedMaterialsList(RpClump* pClump);
    RwSList* CreateAnimatedMaterialsList(RpWorld* pWorld);
    void MaterialsAnimApply(RwSList* pAnimatedMaterials);
    void MaterialsInterpolatorAddAnimTime(RwSList* pAnimatedMaterials, float dt);

private:
    int32 m_nNumUVDictInfo;
    UVDICTINFO m_aUVDictInfo[32];
};


/*static*/ RpMaterial* CUVAnimManager::AddAnimatedMaterialToList(RpMaterial* pMaterial, void* pData)
{
    RwSList* pList = (RwSList*)pData;
    
    ASSERT(pList);

    if (RpMaterialUVAnimExists(pMaterial))
    {
        RpMaterial** ppCurrent = (RpMaterial**)rwSListGetBegin(pList);

        while (ppCurrent != rwSListGetEnd(pList))
        {
            if (pMaterial == *ppCurrent)
                return pMaterial;

            ++ppCurrent;
        };

        *(RpMaterial**)rwSListGetNewEntry(pList, rwID_NAOBJECT) = pMaterial;
    };
    
    return pMaterial;
};


/*static*/ RpAtomic* CUVAnimManager::AtomicForAllMaterials(RpAtomic* pAtomic, void* pData)
{
    MATERIALCALLBACKWORK* pWork = (MATERIALCALLBACKWORK*)pData;

    ASSERT(pWork);
    
    RpGeometryForAllMaterials(
        RpAtomicGetGeometryMacro(pAtomic),
        pWork->m_pCallback,
        pWork->m_pData
    );
    
    return pAtomic;
};


/*static*/ RpClump* CUVAnimManager::ClumpForAllMaterials(RpClump* pClump, void* pData)
{
    RpClumpForAllAtomics(pClump, AtomicForAllMaterials, pData);
    
    return pClump;
};


CUVAnimManager::CUVAnimManager(void)
: m_nNumUVDictInfo(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aUVDictInfo); ++i)
    {
        m_aUVDictInfo[i].m_pUVAnimDict = nullptr;
        m_aUVDictInfo[i].m_szName[0] = '\0';
    };
};


CUVAnimManager::~CUVAnimManager(void)
{
    for (int32 i = 0; i < m_nNumUVDictInfo; ++i)
    {
        ASSERT(m_aUVDictInfo[i].m_pUVAnimDict);

        RtDictDestroy(m_aUVDictInfo[i].m_pUVAnimDict);
        m_aUVDictInfo[i].m_pUVAnimDict = nullptr;
    };

    m_nNumUVDictInfo = 0;
};


bool CUVAnimManager::RegistUVAnimDict(RtDict* pDict, const char* pszName)
{
    ASSERT(pDict);
    ASSERT(pszName);
    ASSERT(m_nNumUVDictInfo < COUNT_OF(m_aUVDictInfo));
    ASSERT(std::strlen(pszName) < COUNT_OF(UVDICTINFO::m_szName));

    if (m_nNumUVDictInfo >= COUNT_OF(m_aUVDictInfo))
        return false;    

    m_aUVDictInfo[m_nNumUVDictInfo].m_pUVAnimDict = pDict;
    std::strcpy(m_aUVDictInfo[m_nNumUVDictInfo].m_szName, pszName);

    ++m_nNumUVDictInfo;

    return true;
};


bool CUVAnimManager::SetUVAnimDict(const char* pszName)
{
    for (int32 i = 0; i < m_nNumUVDictInfo; ++i)
    {
        if (!std::strcmp(m_aUVDictInfo[i].m_szName, pszName))
        {
            ASSERT(m_aUVDictInfo[i].m_pUVAnimDict);

            RtDictSchemaSetCurrentDict(RpUVAnimGetDictSchema(), m_aUVDictInfo[i].m_pUVAnimDict);

            return true;
        };
    };

    return false;
};


bool CUVAnimManager::IsExistUVAnimDict(const char* pszName)
{
    for (int32 i = 0; i < m_nNumUVDictInfo; ++i)
    {
        if (!std::strcmp(m_aUVDictInfo[i].m_szName, pszName))
        {
            ASSERT(m_aUVDictInfo[i].m_pUVAnimDict);

            return true;
        };
    };

    return false;
};


RwSList* CUVAnimManager::CreateAnimatedMaterialsList(RpClump* pClump)
{
    RwSList* pList = rwSListCreate(sizeof(RpMaterial*), rwID_NAOBJECT);
    
    if (pList)
    {
        MATERIALCALLBACKWORK work;
        work.m_pCallback = AddAnimatedMaterialToList;
        work.m_pData = pList;

        RpClumpForAllAtomics(pClump, AtomicForAllMaterials, &work);
    };

    return pList;
};


RwSList* CUVAnimManager::CreateAnimatedMaterialsList(RpWorld* pWorld)
{
    RwSList* pList = rwSListCreate(sizeof(RpMaterial*), rwID_NAOBJECT);

    if (pList)
    {
        MATERIALCALLBACKWORK work;
        work.m_pCallback = AddAnimatedMaterialToList;
        work.m_pData = pList;

        RpWorldForAllMaterials(pWorld, work.m_pCallback, work.m_pData);
        RpWorldForAllClumps(pWorld, ClumpForAllMaterials, &work);
    };

    return pList;
};


void CUVAnimManager::MaterialsAnimApply(RwSList* pAnimatedMaterials)
{
    RpMaterial** ppMaterial = (RpMaterial**)rwSListGetBegin(pAnimatedMaterials);

    while (ppMaterial != rwSListGetEnd(pAnimatedMaterials))
    {
        RpMaterialUVAnimApplyUpdate(*ppMaterial);
        ++ppMaterial;
    };
};


void CUVAnimManager::MaterialsInterpolatorAddAnimTime(RwSList* pAnimatedMaterials, float dt)
{
    RpMaterial** ppMaterial = (RpMaterial**)rwSListGetBegin(pAnimatedMaterials);

    while (ppMaterial != rwSListGetEnd(pAnimatedMaterials))
    {
        RpMaterialUVAnimAddAnimTime(*ppMaterial, dt);
        ++ppMaterial;
    };
};


static CUVAnimManager* s_pUVAnimManager = nullptr;
static int32 s_nUVAnimAllocated = 0;


static inline CUVAnimManager& UVAnimManager(void)
{
    ASSERT(s_pUVAnimManager);
    return *s_pUVAnimManager;
};


/*static*/ void CUVAnim::Initialize(void)
{
    if (!s_pUVAnimManager)
    {
        s_pUVAnimManager = new CUVAnimManager;
    };
};


/*static*/ void CUVAnim::Terminate(void)
{
    if (s_pUVAnimManager)
    {
        ASSERT(s_nUVAnimAllocated == 0);
        
        delete s_pUVAnimManager;
        s_pUVAnimManager = nullptr;
    };
};


/*static*/ void CUVAnim::ReadUVAnim(const char* pszName, const void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(pszName);
    ASSERT(uBufferSize > 0);

    RwMemory MemoryStream;
    MemoryStream.start = (RwUInt8*)pBuffer;
    MemoryStream.length = uBufferSize;

    RtDict* pRtDict = nullptr;
    
    RwStream* pRwStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &MemoryStream);
    ASSERT(pRwStream);
    if (pRwStream)
    {
        if (RwStreamFindChunk(pRwStream, rwID_UVANIMDICT, nullptr, nullptr))
        {
            pRtDict = RtDictSchemaStreamReadDict(RpUVAnimGetDictSchema(), pRwStream);
            ASSERT(pRtDict);
            
            if (std::strcmp(pszName, ""))
            {
                UVAnimManager().RegistUVAnimDict(pRtDict, pszName);
                UVAnimManager().SetUVAnimDict(pszName);
            };
        };
        
        RwStreamClose(pRwStream, nullptr);
    };
};


/*static*/ void CUVAnim::UpdateUVAnim(void* pAnimatedMaterials, float dt)
{
    UVAnimManager().MaterialsInterpolatorAddAnimTime((RwSList*)pAnimatedMaterials, dt);
    UVAnimManager().MaterialsAnimApply((RwSList*)pAnimatedMaterials);
};


/*static*/ void CUVAnim::InitUVAnim(void* hList)
{
    for (int32 i = 0; i < 3600; ++i)
        CUVAnim::UpdateUVAnim(hList, CGameProperty::GetElapsedTime() * 4.0f);
};


/*static*/ void* CUVAnim::CreateAnimatedMaterialsList(RpClump* pClump)
{
    RwSList* pList = UVAnimManager().CreateAnimatedMaterialsList(pClump);
    if(pList)
        ++s_nUVAnimAllocated;

    return pList;
};


/*static*/ void* CUVAnim::CreateAnimatedMaterialsList(RpWorld* pWorld)
{
    RwSList* pList = UVAnimManager().CreateAnimatedMaterialsList(pWorld);
    if (pList)
        ++s_nUVAnimAllocated;

    return pList;
};


/*static*/ void CUVAnim::DestroyAnimatedMaterialsList(void* hList)
{
    ASSERT(hList);

    RwSList* pList = (RwSList*)hList;
    rwSListDestroy(pList);
    
    ASSERT(s_nUVAnimAllocated > 0);
    --s_nUVAnimAllocated;
};


/*static*/ void CUVAnim::SetUVAnimDict(const char* pszName)
{
    UVAnimManager().SetUVAnimDict(pszName);
};


/*static*/ bool CUVAnim::IsExistsUVAnimDict(const char* pszName)
{
    return UVAnimManager().IsExistUVAnimDict(pszName);
};