#include "GameGimmickContainer.hpp"
#include "GameGimmick.hpp"
#include "GameTypes.hpp"


CGameGimmickContainer::CGameGimmickContainer(void)
: m_pGameGimmickTable(nullptr)
, m_nGimmickNum(0)
{
    m_pGameGimmickTable = new CGameGimmick[GAMETYPES::GIMMICK_MAX];
    ASSERT(m_pGameGimmickTable);
};


CGameGimmickContainer::~CGameGimmickContainer(void)
{
    if (m_pGameGimmickTable)
    {
        delete[] m_pGameGimmickTable;
        m_pGameGimmickTable = nullptr;
    };

    m_nGimmickNum = 0;
};


CGameGimmick& CGameGimmickContainer::GameGimmick(int32 nIndex) const
{
	return ((nIndex >= 0 && nIndex < GAMETYPES::GIMMICK_MAX) ? m_pGameGimmickTable[nIndex] : *CGameGimmick::Dummy());
};


void CGameGimmickContainer::Regist(CGimmick* pGimmick)
{
    CGameGimmick* pNode = findFreeNode();
    ASSERT(pNode);
    if (pNode)
    {
        pNode->Attach(pGimmick);
        ++m_nGimmickNum;
    };
};


void CGameGimmickContainer::Remove(CGimmick* pGimmick)
{
    CGameGimmick* pNode = findNode(pGimmick);
    ASSERT(pNode);
    if (pNode)
    {
        ASSERT(m_nGimmickNum > 0);
        --m_nGimmickNum;
        pNode->Detach();
    };
};


int32 CGameGimmickContainer::GameGimmickNum(void) const
{
    return m_nGimmickNum;
};


int32 CGameGimmickContainer::GameGimmickMax(void) const
{
    return GAMETYPES::GIMMICK_MAX;
};


CGameGimmick* CGameGimmickContainer::findNode(CGimmick* pGimmick)
{
    for (int32 i = 0; i < GAMETYPES::GIMMICK_MAX; ++i)
    {
        if (m_pGameGimmickTable[i].GetGimmick() == pGimmick)
            return &m_pGameGimmickTable[i];
    };

    return nullptr;
};


CGameGimmick* CGameGimmickContainer::findFreeNode(void)
{
    for (int32 i = 0; i < GAMETYPES::GIMMICK_MAX; ++i)
    {
        if (!m_pGameGimmickTable[i].GetGimmick())
            return &m_pGameGimmickTable[i];
    };

    return nullptr;
};