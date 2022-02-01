#pragma once


class CGameGimmick;
class CGimmick;


class CGameGimmickContainer
{
public:
    CGameGimmickContainer(void);
    virtual ~CGameGimmickContainer(void);
    CGameGimmick& GameGimmick(int32 nIndex) const;
    void Regist(CGimmick* pGimmick);
    void Remove(CGimmick* pGimmick);
    int32 GameGimmickNum(void) const;
    int32 GameGimmickMax(void) const;

private:
    CGameGimmick* findNode(CGimmick* pGimmick);
    CGameGimmick* findFreeNode(void);

private:
    CGameGimmick* m_pGameGimmickTable;
    int32 m_nGimmickNum;
};