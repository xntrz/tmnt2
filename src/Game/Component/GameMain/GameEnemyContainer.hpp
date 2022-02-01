#pragma once


class CGameEnemy;
class CEnemy;


class CGameEnemyContainer
{
public:
    CGameEnemyContainer(void);
    virtual ~CGameEnemyContainer(void);
    CGameEnemy& GameEnemy(int32 nIndex) const;
    void Regist(CEnemy* pEnemy);
    void Remove(CEnemy* pEnemy);
    int32 GameEnemyNum(void) const;
    int32 GameEnemyMax(void) const;

private:
    CGameEnemy* findNode(CEnemy* pEnemy);
    CGameEnemy* findFreeNode(void);

private:
    CGameEnemy* m_pGameEnemyTable;
    int32 m_nEnemyNum;
};