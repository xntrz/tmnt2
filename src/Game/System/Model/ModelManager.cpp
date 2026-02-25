#include "ModelManager.hpp"
#include "Model.hpp"
#include "ModelToon.hpp"

#include "Game/System/Texture/TextureManager.hpp"


#ifdef _DEBUG
#define MODEL_CLUMP_NUM (256)
#else
#define MODEL_CLUMP_NUM (80)
#endif


class CClumpContainer
{
public:
    class CClump : public CListNode<CClump>
    {
    public:
        CClump(void);
        virtual ~CClump(void);
        void Read(const char* pszName, CModelManager::MODELTYPE modeltype, int32 pattern, void* pBuffer, uint32 uBufferSize, int32 iGeneration);
        void Cleanup(void);
        RpClump* Clone(void);
        RpClump* ReadRpClump(void* pBuffer, uint32 uBufferSize);
        const char* GetName(void) const;
        CModelManager::MODELTYPE GetType(void) const;
        CToonManager::PATTERN GetPattern(void) const;

    private:
        char m_szName[16];
        RpClump* m_pClump;
        CModelManager::MODELTYPE m_type;
        CToonManager::PATTERN m_toonpattern;
        int32 m_iGeneration;
    };
    
public:
    CClumpContainer(void);
    virtual ~CClumpContainer(void);
    void ReadClump(const char* pszName, CModelManager::MODELTYPE modeltype, int32 pattern, void* pBuffer, uint32 uBufferSize);
    CClump* GetClump(const char* pszName);
    CClump* AllocClump(void);
    CClump* FindClump(const char* pszName);
    
private:
    CClump m_aClump[MODEL_CLUMP_NUM];
    CList<CClump> m_listFree;
    CList<CClump> m_listUse;
    int32 m_iGeneration;
};


//
// *********************************************************************************
//


CClumpContainer::CClump::CClump(void)
: m_pClump(nullptr)
, m_type(CModelManager::MODELTYPE_INVALID)
, m_toonpattern(CToonManager::PATTERN_MAX)
, m_iGeneration(-1)
{
    m_szName[0] = '\0';
};


CClumpContainer::CClump::~CClump(void)
{
    ASSERT(!m_pClump);
};


void CClumpContainer::CClump::Read(const char* pszName,
                                   CModelManager::MODELTYPE modeltype,
                                   int32 pattern,
                                   void* pBuffer,
                                   uint32 uBufferSize,
                                   int32 iGeneration)
{
    ASSERT(!m_pClump);
    ASSERT((modeltype == CModelManager::MODELTYPE_TOON) ||
           (modeltype == CModelManager::MODELTYPE_NORMAL));

    ASSERT(std::strlen(pszName) < COUNT_OF(m_szName));
    std::strcpy(m_szName, pszName);

    m_type        = modeltype;
    m_iGeneration = iGeneration;
    
    if (modeltype == CModelManager::MODELTYPE_TOON)
    {
        ASSERT(pattern >= CToonManager::PATTERN_START);
        ASSERT(pattern <  CToonManager::PATTERN_MAX);

        m_toonpattern = CToonManager::PATTERN(pattern);
    }
    else
    {
        m_toonpattern = CToonManager::PATTERN_MAX;
    };

    m_pClump = ReadRpClump(pBuffer, uBufferSize);
    ASSERT(m_pClump);
};


void CClumpContainer::CClump::Cleanup(void)
{
    if (m_pClump)
    {
        RpClumpDestroy(m_pClump);
        m_pClump = nullptr;
    };

    m_szName[0]     = '\0';
    m_type          = CModelManager::MODELTYPE_INVALID;
    m_iGeneration   = -1;
    m_toonpattern   = CToonManager::PATTERN_MAX;
};


RpClump* CClumpContainer::CClump::Clone(void)
{
    return RpClumpClone(m_pClump);
};


RpClump* CClumpContainer::CClump::ReadRpClump(void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);

    RpClump* pResult = nullptr;

    RwMemory MemoryStream;
    MemoryStream.start  = static_cast<RwUInt8*>(pBuffer);
    MemoryStream.length = uBufferSize;

    RwStream* pRwStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &MemoryStream);
    ASSERT(pRwStream);

    if (pRwStream)
    {
        if (RwStreamFindChunk(pRwStream, rwID_CLUMP, nullptr, nullptr))
        {
            RpClump* pClump = RpClumpStreamRead(pRwStream);
            if (pClump)
            {
                RwMatrix matrix;
                RwMatrixSetIdentity(&matrix);
                RwFrameTransform(RpClumpGetFrame(pClump), &matrix, rwCOMBINEREPLACE);
                
                pResult = pClump;
            };
        };

        RwStreamClose(pRwStream, nullptr);
        pRwStream = nullptr;
    };

    return pResult;
};


const char* CClumpContainer::CClump::GetName(void) const
{
    return m_szName;
};


CModelManager::MODELTYPE CClumpContainer::CClump::GetType(void) const
{
    return m_type;
};


CToonManager::PATTERN CClumpContainer::CClump::GetPattern(void) const
{
    return m_toonpattern;
};


//
// *********************************************************************************
//


CClumpContainer::CClumpContainer(void)
: m_iGeneration(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aClump); ++i)
        m_listFree.push_back(&m_aClump[i]);
};


CClumpContainer::~CClumpContainer(void)
{
    for (CClump& it : m_listUse)
        it.Cleanup();
};


void CClumpContainer::ReadClump(const char* pszName,
                                CModelManager::MODELTYPE modeltype,
                                int32 pattern,
                                void* pBuffer,
                                uint32 uBufferSize)
{
    CClump* pClump = AllocClump();
    ASSERT(pClump);
    
    pClump->Read(pszName, modeltype, pattern, pBuffer, uBufferSize, m_iGeneration);    
};


CClumpContainer::CClump* CClumpContainer::GetClump(const char* pszName)
{
    return FindClump(pszName);
};


CClumpContainer::CClump* CClumpContainer::AllocClump(void)
{
    ASSERT(!m_listFree.empty());

    CClump* pNode = m_listFree.front();
    m_listFree.erase(pNode);
    m_listUse.push_back(pNode);

    return pNode;
};


CClumpContainer::CClump* CClumpContainer::FindClump(const char* pszName)
{
    for (CClump& it : m_listUse)
    {
        if (!std::strcmp(it.GetName(), pszName))
            return &it;
    };

    return nullptr;
};


static CClumpContainer* s_pClumpContainer = nullptr;
static int32 s_nNumAllocatedModels = 0;


static inline CClumpContainer& ClumpContainer(void)
{
    ASSERT(s_pClumpContainer);
    return *s_pClumpContainer;
};


/*static*/ void CModelManager::Initialize(void)
{
    if (!s_pClumpContainer)
        s_pClumpContainer = new CClumpContainer;
};


/*static*/ void CModelManager::Terminate(void)
{
    ASSERT(s_nNumAllocatedModels == 0);

    if (s_pClumpContainer)
    {        
        delete s_pClumpContainer;
        s_pClumpContainer = nullptr;
    };
};


/*static*/ CModel* CModelManager::CreateModel(const char* pszName)
{
    ASSERT(pszName);

    CClumpContainer::CClump* pClump = ClumpContainer().GetClump(pszName);
    if (pClump)
    {
        CModel* pModel = new CModel(pClump->GetName(), pClump->Clone());
        ASSERT(pModel);
        
        ++s_nNumAllocatedModels;
        
        return pModel;
    };

    return nullptr;
};


/*static*/ CModel* CModelManager::CreateModelToon(const char* pszName)
{
    ASSERT(pszName);

    CClumpContainer::CClump* pClump = ClumpContainer().GetClump(pszName);
    ASSERT(pClump);
    ASSERT(pClump->GetType() == MODELTYPE_TOON);
    
    if (pClump)
    {
        CModelToon* pModel = new CModelToon(pClump->GetName(), pClump->Clone(), pClump->GetPattern());
        ASSERT(pModel);

        ++s_nNumAllocatedModels;

        return pModel;
    };

    return nullptr;
};


/*static*/ void CModelManager::DestroyModel(CModel* pModel)
{
    ASSERT(pModel);
    ASSERT(s_nNumAllocatedModels > 0);
    
    delete pModel;
    --s_nNumAllocatedModels;
};


/*static*/ void CModelManager::Read(const char* pszName,
                                    MODELTYPE modeltype,
                                    void* pBuffer,
                                    uint32 uBufferSize,
                                    const char* pszTextureSetName,
                                    int32 pattern)
{
    ASSERT(pszTextureSetName);
    ASSERT(pszName);
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);

    CTextureManager::SetCurrentTextureSet(pszTextureSetName);
    ClumpContainer().ReadClump(pszName, modeltype, pattern, pBuffer, uBufferSize);
};


/*static*/ void CModelManager::ReadNormal(const char* pszName,
                                          void* pBuffer,
                                          uint32 uBufferSize,
                                          const char* pszTextureSetName)
{
    ASSERT(pszTextureSetName);
    ASSERT(pszName);
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);

    Read(pszName, MODELTYPE_NORMAL, pBuffer, uBufferSize, pszTextureSetName, CToonManager::PATTERN_MAX);
};


/*static*/ void CModelManager::ReadToon(const char* pszName,
                                        void* pBuffer,
                                        uint32 uBufferSize,
                                        const char* pszTextureSetName,
                                        int32 pattern)
{
    ASSERT(pszTextureSetName);
    ASSERT(pszName);
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);

    Read(pszName, MODELTYPE_TOON, pBuffer, uBufferSize, pszTextureSetName, pattern);
};

