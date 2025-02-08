#include "IceBlock.hpp"

#include "Game/System/Particle/Sphere.hpp"
#include "Game/System/Texture/TextureManager.hpp"


class CIceBlock : public CListNode<CIceBlock>
{
public:
    CIceBlock(void);
    ~CIceBlock(void);
    void Draw(void);
    void SetPosition(const RwV3d* pvPosition);
    void SetScaling(const RwV3d* pvScaling);
    void SetTexture(RwTexture* pTexture);

private:
    RwV3d m_vScaling;
    RwV3d m_vPosition;
    CSphere m_shape;
};


class CIceBlockList
{
public:
    CIceBlockList(void);
    ~CIceBlockList(void);
    void Run(void);
    void Draw(void);
    uint32 Play(const RwV3d* pvPosition);
    void Finish(uint32 hIceBlock);
    void SetPosition(uint32 hIceBlock, const RwV3d* pvPosition);
    void SetScale(uint32 hIceBlock, const RwV3d* pvScaling);

private:
    CIceBlock* workAlloc(void);
    void workFree(CIceBlock* pWork);
    CIceBlock* workFromHandle(uint32 hIceblock) const;

private:
    CIceBlock m_aWork[10];
    CList<CIceBlock> m_listWorkPool;
    CList<CIceBlock> m_listWorkAlloc;
    RwTexture* m_pTexture;
};


//
// *********************************************************************************
//


CIceBlock::CIceBlock(void)
: m_shape(1.0f, 5, 5, 1, 1, { 0xFF, 0xFF, 0xFF, 0xFF }, true)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vScaling({ 1.0f, 2.0f, 1.0f })
{
    SetPosition(&m_vPosition);
    SetScaling(&m_vScaling);
};


CIceBlock::~CIceBlock(void)
{
    ;
};


void CIceBlock::Draw(void)
{
    m_shape.Draw();
};


void CIceBlock::SetPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    m_shape.SetPosition(pvPosition);
};


void CIceBlock::SetScaling(const RwV3d* pvScaling)
{
    ASSERT(pvScaling);

    m_shape.SetScale(pvScaling);
};


void CIceBlock::SetTexture(RwTexture* pTexture)
{
    m_shape.SetTexture(pTexture);
};


//
// *********************************************************************************
//


CIceBlockList::CIceBlockList(void)
: m_pTexture(nullptr)
{
    for (int32 i = 0; i < COUNT_OF(m_aWork); ++i)
        m_listWorkPool.push_back(&m_aWork[i]);
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
            m_aWork[i].SetTexture(m_pTexture);
    };
};


void CIceBlockList::Draw(void)
{
    for (CIceBlock& it : m_listWorkAlloc)
        it.Draw();
};


uint32 CIceBlockList::Play(const RwV3d* pvPosition)
{
    CIceBlock* pWork = workAlloc();
    ASSERT(pWork);

    if (pWork)
        pWork->SetPosition(pvPosition);

    return uint32(pWork);
};


void CIceBlockList::Finish(uint32 hIceBlock)
{
    CIceBlock* pWork = workFromHandle(hIceBlock);
    ASSERT(pWork);
    if (pWork)
        workFree(pWork);
};


void CIceBlockList::SetPosition(uint32 hIceBlock, const RwV3d* pvPosition)
{
    CIceBlock* pWork = workFromHandle(hIceBlock);
    ASSERT(pWork);

    if (pWork)
        pWork->SetPosition(pvPosition);
};


void CIceBlockList::SetScale(uint32 hIceBlock, const RwV3d* pvScaling)
{
    CIceBlock* pWork = workFromHandle(hIceBlock);
    ASSERT(pWork);
    
    if (pWork)
        pWork->SetScaling(pvScaling);
};


CIceBlock* CIceBlockList::workAlloc(void)
{
    ASSERT(!m_listWorkPool.empty());

    if (m_listWorkPool.empty())
        return nullptr;

    CIceBlock* pNode = m_listWorkPool.front();
    m_listWorkPool.erase(pNode);
    m_listWorkAlloc.push_back(pNode);

    return pNode;
};


void CIceBlockList::workFree(CIceBlock* pWork)
{
    ASSERT(pWork);

    m_listWorkAlloc.erase(pWork);
    m_listWorkPool.push_back(pWork);
};


CIceBlock* CIceBlockList::workFromHandle(uint32 hIceblock) const
{
    ASSERT(hIceblock);
    return reinterpret_cast<CIceBlock*>(hIceblock);
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
    {
        s_pIceBlockList = new CIceBlockList;
    };
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