#include "TextureManager.hpp"

#include "rtpitexd.h"


#ifdef _DEBUG
#define TEXTURE_SET_NUM (256)
#else
#define TEXTURE_SET_NUM (32)
#endif


class CTextureContainer
{
private:
    class CTextureSet : public CListNode<CTextureSet>
    {
    private:
        static const int32 TEX_NAME_MAX = 16;
        
    public:
        static RwTexture* SetLinearFilterCallback(RwTexture* pRwTexture, void* pData);
        static RwTexture* TexDictionaryAddTextureCallback(RwTexture* pRwTexture, void* pData);

        CTextureSet(void);
        virtual ~CTextureSet(void);
        void Initialize(const char* pszName, void* pBuffer, uint32 uBufferSize, int32 iGeneration);
        void Initialize(const char* pszName, RwTexDictionary* pTexDict, int32 iGeneration);
        void Terminate(void);
        void SetCurrent(void);
        void AddSet(void* pBuffer, uint32 uBufferSize);        
        RwTexDictionary* ReadRwTexDictionary(void* pBuffer, uint32 uBufferSize);
        RwTexture* Texture(const char* pszName) const;
        RwTexDictionary* TextureDictionary(void) const;
        const char* Name(void) const;
        int32 Generation(void) const;

    protected:
        char m_szName[TEX_NAME_MAX];
        RwTexDictionary* m_pRwTexDictionary;
        int32 m_iGeneration;
    };

    class CNullTextureSet : public CTextureSet
    {
    public:
        static const char NAME[];
        
    public:
        CNullTextureSet(void);
        virtual ~CNullTextureSet(void);
    };
    
public:
    CTextureContainer(void);
    virtual ~CTextureContainer(void);
    void GenerationInc(void);
    void GenerationDec(void);
    void AddTextDict(const char* pszName, RwTexDictionary* pTexDict);
    void AllocTextureSet(const char* pszName, void* pBuffer, uint32 uBufferSize);
    void FreeTextureSet(CTextureSet* pTextureSet);
    void AddTextureSet(const char* pszName, void* pBuffer, uint32 uBufferSize);
    void SetCurrentSet(const char* pszName);
    void SetCurrentSet(CTextureSet* pTextureSet);
    RwTexture* GetRwTexture(const char* pszName);
    RwTexDictionary* GetRwTextureDictionary(const char* pszName);
    CTextureSet* FindTextureSet(const char* pszName);
    void GarbageCollection(void);

private:
    CTextureSet m_aTextureSet[TEXTURE_SET_NUM];
    CList<CTextureSet> m_listFree;
    CList<CTextureSet> m_listUsing;
    CTextureSet* m_pCurrentSet;
    CNullTextureSet m_NullSet;
    int32 m_iGeneration;
};


/*static*/ RwTexture* CTextureContainer::CTextureSet::SetLinearFilterCallback(RwTexture* pRwTexture, void* pData)
{
    ASSERT(pRwTexture);    
    RwTextureSetFilterMode(pRwTexture, rwFILTERLINEAR);

    return pRwTexture;
};


/*static*/ RwTexture* CTextureContainer::CTextureSet::TexDictionaryAddTextureCallback(RwTexture* pRwTexture, void* pData)
{
    RwTexDictionary* pRwTexDictionary = static_cast<RwTexDictionary*>(pData);
    ASSERT(pRwTexDictionary);
    
    ASSERT(pRwTexture);
    RwTexDictionaryAddTexture(pRwTexDictionary, pRwTexture);

    return pRwTexture;
};


CTextureContainer::CTextureSet::CTextureSet(void)
: m_pRwTexDictionary(nullptr)
, m_iGeneration(-1)
{
    m_szName[0] = '\0';
};


CTextureContainer::CTextureSet::~CTextureSet(void)
{
    ASSERT(!m_pRwTexDictionary);
};


void CTextureContainer::CTextureSet::Initialize(const char* pszName, void* pBuffer, uint32 uBufferSize, int32 iGeneration)
{
    ASSERT(!m_pRwTexDictionary);
    ASSERT(strlen(pszName) < COUNT_OF(m_szName));

    std::strcpy(m_szName, pszName);
    m_iGeneration = iGeneration;

    if (pBuffer)
        m_pRwTexDictionary = ReadRwTexDictionary(pBuffer, uBufferSize);
    else
        m_pRwTexDictionary = RwTexDictionaryCreate();

    ASSERT(m_pRwTexDictionary);
};


void CTextureContainer::CTextureSet::Initialize(const char* pszName, RwTexDictionary* pTexDict, int32 iGeneration)
{
    ASSERT(pszName);
    ASSERT(pTexDict);

    ASSERT(!m_pRwTexDictionary);
    ASSERT(strlen(pszName) < COUNT_OF(m_szName));

    std::strcpy(m_szName, pszName);
    m_iGeneration = iGeneration;
    m_pRwTexDictionary = pTexDict;
};


void CTextureContainer::CTextureSet::Terminate(void)
{
    if (m_pRwTexDictionary)
    {
        RwTexDictionaryDestroy(m_pRwTexDictionary);
        m_pRwTexDictionary = nullptr;
    };

    m_szName[0] = '\0';
    m_iGeneration = -1;
};


void CTextureContainer::CTextureSet::SetCurrent(void)
{
    RwTexDictionarySetCurrent(m_pRwTexDictionary);
};


void CTextureContainer::CTextureSet::AddSet(void* pBuffer, uint32 uBufferSize)
{
    ASSERT(m_pRwTexDictionary);

    RwTexDictionary* pRwTexDictionary = ReadRwTexDictionary(pBuffer, uBufferSize);
    ASSERT(pRwTexDictionary);

    RwTexDictionaryForAllTextures(pRwTexDictionary, TexDictionaryAddTextureCallback, m_pRwTexDictionary);
    RwTexDictionaryDestroy(pRwTexDictionary);
};


RwTexDictionary* CTextureContainer::CTextureSet::ReadRwTexDictionary(void* pBuffer, uint32 uBufferSize)
{
    RwTexDictionary* pResult = nullptr;
    
    RwMemory memStream;
    memStream.start  = static_cast<RwUInt8*>(pBuffer);
    memStream.length = uBufferSize;

    RwStream* pRwStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &memStream);
    ASSERT(pRwStream);
    
    if (pRwStream)
    {
        RwChunkHeaderInfo ChunkHeaderInfo;
        if (RwStreamReadChunkHeaderInfo(pRwStream, &ChunkHeaderInfo))
        {
            switch (ChunkHeaderInfo.type)
            {
            case rwID_TEXDICTIONARY:
                pResult = RwTexDictionaryStreamRead(pRwStream);
                break;

            case rwID_PITEXDICTIONARY:
                pResult = RtPITexDictionaryStreamRead(pRwStream);
                break;

            default:
                ASSERT(false);
                break;
            };

            if (pResult)
                RwTexDictionaryForAllTextures(pResult, SetLinearFilterCallback, nullptr);
        };

        RwStreamClose(pRwStream, nullptr);
        pRwStream = nullptr;
    };

    return pResult;
};


RwTexture* CTextureContainer::CTextureSet::Texture(const char* pszName) const
{
    return RwTexDictionaryFindNamedTexture(m_pRwTexDictionary, pszName);
};


RwTexDictionary* CTextureContainer::CTextureSet::TextureDictionary(void) const
{
    return m_pRwTexDictionary;
};


const char* CTextureContainer::CTextureSet::Name(void) const
{
    return m_szName;
};


int32 CTextureContainer::CTextureSet::Generation(void) const
{
    return m_iGeneration;
};


//
// *********************************************************************************
//


/*static*/ const char CTextureContainer::CNullTextureSet::NAME [] = "NULL";


CTextureContainer::CNullTextureSet::CNullTextureSet(void)
{
    std::strcpy(m_szName, NAME);
};


CTextureContainer::CNullTextureSet::~CNullTextureSet(void)
{
    ;
};


//
// *********************************************************************************
//


CTextureContainer::CTextureContainer(void)
: m_pCurrentSet(nullptr)
, m_iGeneration(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aTextureSet); ++i)
        m_listFree.push_back(&m_aTextureSet[i]);
};


CTextureContainer::~CTextureContainer(void) 
{
    for (auto& it : m_listUsing)
        it.Terminate();
};


void CTextureContainer::GenerationInc(void)
{
    ++m_iGeneration;
};


void CTextureContainer::GenerationDec(void)
{
    ASSERT(m_iGeneration > 0);
    --m_iGeneration;
};


void CTextureContainer::AddTextDict(const char* pszName, RwTexDictionary* pTexDict)
{
    ASSERT(!m_listFree.empty());

    CTextureSet* pTextureSet = m_listFree.front();
    m_listFree.erase(pTextureSet);
    m_listUsing.push_back(pTextureSet);

    pTextureSet->Initialize(pszName, pTexDict, m_iGeneration);

    SetCurrentSet(pTextureSet);
};


void CTextureContainer::AllocTextureSet(const char* pszName, void* pBuffer, uint32 uBufferSize) 
{
    ASSERT(!m_listFree.empty());

    CTextureSet* pTextureSet = m_listFree.front();
    m_listFree.erase(pTextureSet);
    m_listUsing.push_back(pTextureSet);

    pTextureSet->Initialize(pszName, pBuffer, uBufferSize, m_iGeneration);
    
    SetCurrentSet(pTextureSet);
};


void CTextureContainer::FreeTextureSet(CTextureSet* pTextureSet) 
{
    ASSERT(pTextureSet);
    ASSERT(m_listUsing.contains(pTextureSet));

    pTextureSet->Terminate();

    m_listUsing.erase(pTextureSet);
    m_listFree.push_back(pTextureSet);
};


void CTextureContainer::AddTextureSet(const char* pszName, void* pBuffer, uint32 uBufferSize) 
{
    CTextureSet* pTextureSet = FindTextureSet(pszName);
    if (!pTextureSet)
    {
        AllocTextureSet(pszName, pBuffer, uBufferSize);
    }
    else
    {
        pTextureSet->AddSet(pBuffer, uBufferSize);
        SetCurrentSet(pTextureSet);
    };
};


void CTextureContainer::SetCurrentSet(const char* pszName) 
{
    CTextureSet* pTextureSet = FindTextureSet(pszName);
    SetCurrentSet(pTextureSet);
};


void CTextureContainer::SetCurrentSet(CTextureSet* pTextureSet) 
{
    if (m_pCurrentSet != pTextureSet)
    {
        m_pCurrentSet = pTextureSet;
        if (m_pCurrentSet)
            m_pCurrentSet->SetCurrent();
    };
};


RwTexture* CTextureContainer::GetRwTexture(const char* pszName) 
{
    ASSERT(pszName);
    
    if (m_pCurrentSet)
        return m_pCurrentSet->Texture(pszName);

    return nullptr;
};


RwTexDictionary* CTextureContainer::GetRwTextureDictionary(const char* pszName)
{
    CTextureSet* pTextureSet = FindTextureSet(pszName);
    if (pTextureSet)
        return pTextureSet->TextureDictionary();
    
    return nullptr;
};


CTextureContainer::CTextureSet* CTextureContainer::FindTextureSet(const char* pszName)
{
    for (CTextureSet& it : m_listUsing)
    {
        if (!std::strcmp(it.Name(), pszName))
            return &it;
    };

    return nullptr;
};


void CTextureContainer::GarbageCollection(void) 
{
    auto it = m_listUsing.begin();
    auto itEnd = m_listUsing.end();
    while (it != itEnd)
    {
        if ((*it).Generation() > m_iGeneration)
        {
            CTextureSet* pTextureSet = &*it++;

            if (m_pCurrentSet == pTextureSet)
                m_pCurrentSet = nullptr;

            FreeTextureSet(pTextureSet);
        }
        else
        {
            ++it;
        };
    };
};


static CTextureContainer* s_pTextureContainer = nullptr;


static inline CTextureContainer& TextureContainer(void)
{
    ASSERT(s_pTextureContainer);
    return *s_pTextureContainer;
};


/*static*/ void CTextureManager::Initialize(void)
{
    if (!s_pTextureContainer)
        s_pTextureContainer = new CTextureContainer;
};


/*static*/ void CTextureManager::Terminate(void)
{
    if (s_pTextureContainer)
    {
        delete s_pTextureContainer;
        s_pTextureContainer = nullptr;
    };
};


/*static*/ void CTextureManager::GenerationInc(void)
{
    TextureContainer().GenerationInc();
};


/*static*/ void CTextureManager::GenerationDec(void)
{
    TextureContainer().GenerationDec();
    TextureContainer().GarbageCollection();
};


/*static*/ void CTextureManager::AddDict(const char* pszName, RwTexDictionary* pTexDict)
{
    TextureContainer().AddTextDict(pszName, pTexDict);
};


/*static*/ void CTextureManager::AddSet(const char* pszName, void* pBuffer, uint32 uBufferSize)
{
    TextureContainer().AddTextureSet(pszName, pBuffer, uBufferSize);
};


/*static*/ void CTextureManager::ReadSet(const char* pszName, void* pBuffer, uint32 uBufferSize)
{
    TextureContainer().AllocTextureSet(pszName, pBuffer, uBufferSize);
};


/*static*/ RwTexture* CTextureManager::GetRwTexture(const char* pszName)
{
    return TextureContainer().GetRwTexture(pszName);
};


/*static*/ RwTexDictionary* CTextureManager::GetRwTextureDictionary(const char* pszName)
{
    return TextureContainer().GetRwTextureDictionary(pszName);
};


/*static*/ void CTextureManager::SetCurrentTextureSet(const char* pszName)
{
    TextureContainer().SetCurrentSet(pszName);
};

