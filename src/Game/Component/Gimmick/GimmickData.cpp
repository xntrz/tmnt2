#include "GimmickData.hpp"
#include "GimmickManager.hpp"


CGimmickData::CGimmickData(void)
: m_pMGDBuffer(nullptr)
{
    ;
};


CGimmickData::~CGimmickData(void)
{
    Cleanup();
};


void CGimmickData::Read(const void* pBuffer, uint32 uBufferSize)
{
    m_pMGDBuffer = new char[uBufferSize];
    std::memcpy(m_pMGDBuffer, pBuffer, uBufferSize);
    
    PostRead();
};


void CGimmickData::PostRead(void)
{
    ;
};


void CGimmickData::Cleanup(void)
{
    if (m_pMGDBuffer)
    {
        delete[] m_pMGDBuffer;
        m_pMGDBuffer = nullptr;
    };
};


void CGimmickData::CreateGimmicks(void)
{
    for (int32 i = 0; i < GetGimmickDataNum(); ++i)
    {
        GIMMICKDATA* pGimmickData = GetGimmickData(i);
        GIMMICKID::VALUE id = static_cast<GIMMICKID::VALUE>(pGimmickData->m_id);

        CGimmickManager::Create(id, pGimmickData->m_subid, pGimmickData);
    };
};


CGimmickData::MGD_HEADER* CGimmickData::GetMGDHeader(void) const
{
    if (m_pMGDBuffer)
        return reinterpret_cast<MGD_HEADER*>(m_pMGDBuffer);

    ASSERT(m_pMGDBuffer);
    return nullptr;
};


CGimmickData::MAPGIMMICK_DATA_LIST* CGimmickData::GetGimmickDataList(void) const
{
    ASSERT(m_pMGDBuffer);
    return reinterpret_cast<MAPGIMMICK_DATA_LIST*>(m_pMGDBuffer + GetMGDHeader()->m_nOffsetDataPos);
};


CGimmickData::GIMMICKDATA* CGimmickData::GetGimmickData(int32 no) const
{
    MAPGIMMICK_DATA_LIST* pGimmickDataList = GetGimmickDataList();
    GIMMICKDATA* pResult = reinterpret_cast<GIMMICKDATA*>(pGimmickDataList->m_aGimmickData);

    for (int32 i = 0; i < no; ++i)
        pResult = reinterpret_cast<GIMMICKDATA*>(reinterpret_cast<char*>(pResult) + pResult->m_size);

    return pResult;
};


int32 CGimmickData::GetGimmickDataNum(void) const
{
	if (m_pMGDBuffer)
        return GetGimmickDataList()->m_nGimmickDataNum;
    
    return 0;
};


CGimmickData::ENEMYSET_DATA_LIST* CGimmickData::GetEnemySetDataList(void) const
{
    ASSERT(m_pMGDBuffer);
    return reinterpret_cast<ENEMYSET_DATA_LIST*>(m_pMGDBuffer + GetMGDHeader()->m_nOffsetEnemySetDataPos);
};


CGimmickData::ENEMYSET_DATA* CGimmickData::GetEnemySetData(int32 no) const
{
    return &GetEnemySetDataList()->m_aEnemySadData[no];
};


static CGimmickData* s_pGimmickData = nullptr;
static CGimmickData* s_pEffectGimmickData = nullptr;


static inline CGimmickData& NormalGimmickData(void)
{
    ASSERT(s_pGimmickData);
    return *s_pGimmickData;
};


static inline CGimmickData& EffectGimmickData(void)
{
    ASSERT(s_pEffectGimmickData);
    return *s_pEffectGimmickData;
};


/*static*/ void CGimmickDataManager::Initialize(void)
{
    if (!s_pGimmickData)
        s_pGimmickData = new CGimmickData;

    if (!s_pEffectGimmickData)
        s_pEffectGimmickData = new CGimmickData;
};


/*static*/ void CGimmickDataManager::Terminate(void)
{
    if (s_pEffectGimmickData)
    {
        delete s_pEffectGimmickData;
        s_pEffectGimmickData = nullptr;
    };

    if (s_pGimmickData)
    {
        delete s_pGimmickData;
        s_pGimmickData = nullptr;
    };
};


/*static*/ void CGimmickDataManager::Read(const char* pszName, const void* pBuffer, uint32 uBufferSize)
{
    if (!std::strcmp(pszName, "effect"))
        EffectGimmickData().Read(pBuffer, uBufferSize);
    else
        NormalGimmickData().Read(pBuffer, uBufferSize);
};


/*static*/ void CGimmickDataManager::CreateGimmicks(MGD_DATA_KIND kind)
{
    switch (kind)
    {
    case KIND_NORMAL:
        NormalGimmickData().CreateGimmicks();
        break;

    case KIND_EFFECT:
        EffectGimmickData().CreateGimmicks();
        break;

    default:
        ASSERT(false);
        break;    
    };
};


/*static*/ const CGimmickData::ENEMYSET_DATA* CGimmickDataManager::GetEnemySetData(int32 no, MGD_DATA_KIND kind)
{
    switch (kind)
    {
    case KIND_NORMAL:
        return NormalGimmickData().GetEnemySetData(no);

    case KIND_EFFECT:
        return EffectGimmickData().GetEnemySetData(no);

    default:
        ASSERT(false);
        return nullptr;
    };
};