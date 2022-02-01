#include "GameEnemyContainer.hpp"
#include "GameEnemy.hpp"
#include "GameTypes.hpp"


CGameEnemyContainer::CGameEnemyContainer(void)
: m_pGameEnemyTable(nullptr)
, m_nEnemyNum(0)
{
    m_pGameEnemyTable = new CGameEnemy[GAMETYPES::ENEMY_MAX];
    ASSERT(m_pGameEnemyTable);
};


CGameEnemyContainer::~CGameEnemyContainer(void)
{
    if (m_pGameEnemyTable)
    {
        delete [] m_pGameEnemyTable;
        m_pGameEnemyTable = nullptr;
    };

    m_nEnemyNum = 0;
};


CGameEnemy& CGameEnemyContainer::GameEnemy(int32 nIndex) const
{
    if (nIndex >= 0 && nIndex < GAMETYPES::ENEMY_MAX)
    {
        return m_pGameEnemyTable[ nIndex ];
    }
    else
    {
        return CGameEnemy::Dummy();
    };
};


void CGameEnemyContainer::Regist(CEnemy* pEnemy)
{
    CGameEnemy* pNode = findFreeNode();
    ASSERT(pNode);
    if (pNode)
    {
        pNode->Attach(pEnemy);
        ++m_nEnemyNum;
    };
};


void CGameEnemyContainer::Remove(CEnemy* pEnemy)
{
    CGameEnemy* pNode = findNode(pEnemy);
    ASSERT(pNode);
    if (pNode)
    {
        ASSERT(m_nEnemyNum > 0);
        --m_nEnemyNum;
        pNode->Detach();
    };
};


int32 CGameEnemyContainer::GameEnemyNum(void) const
{
    return m_nEnemyNum;
};


int32 CGameEnemyContainer::GameEnemyMax(void) const
{
    return GAMETYPES::ENEMY_MAX;
};


CGameEnemy* CGameEnemyContainer::findNode(CEnemy* pEnemy)
{
    for (int32 i = 0; i < GAMETYPES::ENEMY_MAX; ++i)
    {
        if (m_pGameEnemyTable[i].GetEnemy() == pEnemy)
            return &m_pGameEnemyTable[i];
    };

    return nullptr;
};


CGameEnemy* CGameEnemyContainer::findFreeNode(void)
{
    for (int32 i = 0; i < GAMETYPES::ENEMY_MAX; ++i)
    {
        if (!m_pGameEnemyTable[i].GetEnemy())
            return &m_pGameEnemyTable[i];
    };

    return nullptr;
};