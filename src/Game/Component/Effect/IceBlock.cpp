#include "IceBlock.hpp"

#include "Game/System/Particle/Sphere.hpp"
#include "Game/System/Texture/TextureManager.hpp"


static const RwV3d ICE_BLOCK_SCALE = { 1.0f, 2.0f, 1.0f };


class CIceBlock : public CSphere
{
public:
    inline CIceBlock(void) : CSphere(1.0f, 5, 5, 1, 1, { 0xFF, 0xFF, 0xFF, 0xFF }, true) {};
    inline virtual ~CIceBlock(void) {};
};


class CIceBlockList
{
public:
    struct WORK : public CListNode<WORK>
    {
        CIceBlock iceBlock;
    };

public:
    CIceBlockList(void);
    ~CIceBlockList(void);
    void Run(void);
    void Draw(void);
    uint32 Play(const RwV3d* pvPosition);
    void Finish(uint32 hIceBlock);
    void SetPosition(uint32 hIceBlock, const RwV3d* pvPosition);
    void SetScale(uint32 hIceBlock, const RwV3d* pvScaling);
    WORK* WorkAlloc(void);
    void WorkFree(WORK* pWork);
    WORK* WorkFromHandle(uint32 hIceblock) const;

private:
    WORK        m_aWork[10];
    CList<WORK> m_listWorkFree;
    CList<WORK> m_listWorkAlloc;
    RwTexture*  m_pTexture;
};


//
// *********************************************************************************
//


CIceBlockList::CIceBlockList(void)
: m_aWork()
, m_listWorkFree()
, m_listWorkAlloc()
, m_pTexture(nullptr)
{
    for (int32 i = 0; i < COUNT_OF(m_aWork); ++i)
    {
        m_aWork[i].iceBlock.SetScale(&ICE_BLOCK_SCALE);
        m_listWorkFree.push_back(&m_aWork[i]);
    };
};


CIceBlockList::~CIceBlockList(void)
{
    ;
};


void CIceBlockList::Run(void)
{
    if (!m_pTexture)
    {
        CTextureManager::SetCurrentTextureSet("common_ef");

        m_pTexture = CTextureManager::GetRwTexture("frozen");
        ASSERT(m_pTexture);

        for (int32 i = 0; i < COUNT_OF(m_aWork); ++i)
            m_aWork[i].iceBlock.SetTexture(m_pTexture);
    };
};


void CIceBlockList::Draw(void)
{
    for (WORK& it : m_listWorkAlloc)
        it.iceBlock.Draw();
};


uint32 CIceBlockList::Play(const RwV3d* pvPosition)
{
    WORK* pWork = WorkAlloc();
    ASSERT(pWork);

    if (pWork)
        pWork->iceBlock.SetPosition(pvPosition);

    return reinterpret_cast<uint32>(pWork);
};


void CIceBlockList::Finish(uint32 hIceBlock)
{
    WORK* pWork = WorkFromHandle(hIceBlock);
    ASSERT(pWork);

    if (pWork)
    {
        pWork->iceBlock.Reset();
        pWork->iceBlock.SetScale(&ICE_BLOCK_SCALE);

        WorkFree(pWork);
    };
};


void CIceBlockList::SetPosition(uint32 hIceBlock, const RwV3d* pvPosition)
{
    WORK* pWork = WorkFromHandle(hIceBlock);
    ASSERT(pWork);

    if (pWork)
        pWork->iceBlock.SetPosition(pvPosition);
};


void CIceBlockList::SetScale(uint32 hIceBlock, const RwV3d* pvScaling)
{
    WORK* pWork = WorkFromHandle(hIceBlock);
    ASSERT(pWork);
    
    if (pWork)
        pWork->iceBlock.SetScale(pvScaling);
};


CIceBlockList::WORK* CIceBlockList::WorkAlloc(void)
{
    if (m_listWorkFree.empty())
        return nullptr;

    WORK* pWork = m_listWorkFree.front();
    m_listWorkFree.erase(pWork);
    m_listWorkAlloc.push_back(pWork);

    return pWork;
};


void CIceBlockList::WorkFree(WORK* pWork)
{
    ASSERT(pWork);

    m_listWorkAlloc.erase(pWork);
    m_listWorkFree.push_front(pWork);
};


CIceBlockList::WORK* CIceBlockList::WorkFromHandle(uint32 hIceblock) const
{
    return reinterpret_cast<WORK*>(hIceblock);
};


static CIceBlockList* s_pIceBlockList = nullptr;


static CIceBlockList& IceBlockList(void)
{
    ASSERT(s_pIceBlockList);
    return *s_pIceBlockList;
};


/*static*/ void CIceBlockManager::Initialize(void)
{
    if (!s_pIceBlockList)
        s_pIceBlockList = new CIceBlockList;
};


/*static*/ void CIceBlockManager::Terminate(void)
{
    if (s_pIceBlockList)
    {
        delete s_pIceBlockList;
        s_pIceBlockList = nullptr;
    };
};


/*static*/ void CIceBlockManager::Run(void)
{
    IceBlockList().Run();
};


/*static*/ void CIceBlockManager::Draw(void)
{
    IceBlockList().Draw();
};


/*static*/ uint32 CIceBlockManager::Play(const RwV3d* pvPosition)
{
    return IceBlockList().Play(pvPosition);
};


/*static*/ void CIceBlockManager::Finish(uint32 hIceBlock)
{
    IceBlockList().Finish(hIceBlock);
};


/*static*/ void CIceBlockManager::SetPosition(uint32 hIceBlock, const RwV3d* pvPosition)
{
    IceBlockList().SetPosition(hIceBlock, pvPosition);
};


/*static*/ void CIceBlockManager::SetScale(uint32 hIceBlock, const RwV3d* pvScaling)
{
    IceBlockList().SetScale(hIceBlock, pvScaling);
};